#include "TpProcess.h"
#include <TpCore.h>
#include "TpFileInfo.h"
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <fstream>
#include <atomic>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include "TpFile.h"
#include <spawn.h>
#include <sys/wait.h>

class TpProcessImpl
{
public:
    std::atomic<TpProcess::ProcessState> state_{TpProcess::NotRunning};
    pid_t pid_ = -1;
    std::string programName_;
    std::mutex mutex_;

    // 状态变更通知条件变量
    std::condition_variable stateCV_;

    // 执行参数转换
    char **prepareArgs(const TpString &program, const TpVector<TpString> &arguments)
    {
        char **argv = new char *[arguments.size() + 2];
        argv[0] = strdup(program.c_str());

        for (int i = 0; i < arguments.size(); ++i)
        {
            argv[i + 1] = strdup(arguments[i].c_str());
        }

        argv[arguments.size() + 1] = nullptr;
        return argv;
    }

    // 清理参数内存
    void freeArgs(char **argv)
    {
        for (int i = 0; argv[i] != nullptr; ++i)
        {
            free(argv[i]);
        }
        delete[] argv;
    }

    // 获取进程名称
    TpString getProcessName() const
    {
        if (pid_ <= 0)
            return "";

        try
        {
            std::ifstream fs("/proc/" + std::to_string(pid_) + "/comm");
            TpString name;
            std::getline(fs, name);
            return name;
        }
        catch (...)
        {
            return "";
        }
    }

    pid_t findChildPid()
    {
        TpString path = "/proc/" + std::to_string(pid_) + "/task/" +
                        std::to_string(pid_) + "/children";
        try
        {
            std::ifstream fs(path);
            pid_t child;
            if (fs >> child)
                return child;
        }
        catch (...)
        {
        }
        return -1;
    }
};

TpProcess::TpProcess()
{
    data_ = new TpProcessImpl();
}

TpProcess::~TpProcess()
{
    TpProcessImpl *processData = static_cast<TpProcessImpl *>(data_);

    if (processData->state_ != NotRunning)
    {
        waitForFinished(500);
    }
    delete processData;
    processData = nullptr;
    data_ = nullptr;
}

void TpProcess::start(const TpString &program, const TpVector<TpString> &arguments)
{
    TpProcessImpl *processData = static_cast<TpProcessImpl *>(data_);

    std::lock_guard<std::mutex> lock(processData->mutex_);
    if (processData->state_ != NotRunning)
        return;

    processData->state_ = Starting;

#if 1
    pid_t pid = fork();
    if (pid == 0)
    {
        char **argv = processData->prepareArgs(program, arguments);
        int aa = execvp(program.c_str(), argv);
        processData->freeArgs(argv);
        // 执行失败
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        // 父进程
        processData->pid_ = pid;
        processData->programName_ = program.c_str();
        processData->state_ = Running;
    }
    else
    {
        // fork失败
        processData->state_ = NotRunning;
    }
#else
    // 在fork之前创建管道
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        std::cerr << "创建管道失败: " << strerror(errno) << std::endl;
        processData->state_ = NotRunning;
        return;
    }

    // 第一次 fork
    pid_t firstChildPid = fork();
    if (firstChildPid == 0)
    {
        // 第一个子进程
        close(pipefd[0]); // 关闭读取端

        // 创建新的会话，脱离终端控制
        setsid();

        // 第二次 fork
        pid_t secondChildPid = fork();
        if (secondChildPid == 0)
        {
            // 第二个子进程（实际的业务进程）
            close(pipefd[1]); // 关闭写入端

            // 关闭所有继承的文件描述符
            int max_fd = sysconf(_SC_OPEN_MAX);
            for (int fd = 3; fd < max_fd; fd++)
            {
                close(fd);
            }

            // 重置信号处理
            struct sigaction sa;
            sa.sa_handler = SIG_DFL;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = 0;

            for (int sig = 1; sig < NSIG; sig++)
            {
                if (sig != SIGKILL && sig != SIGSTOP)
                {
                    sigaction(sig, &sa, NULL);
                }
            }

            // 准备并执行目标程序
            char **argv = processData->prepareArgs(program, arguments);
            int result = execvp(program.c_str(), argv);
            processData->freeArgs(argv);

            // 如果执行到这里，说明 execvp 失败了
            _exit(EXIT_FAILURE);
        }
        else if (secondChildPid > 0)
        {
            // 将第二个子进程的PID通过管道发送给父进程
            write(pipefd[1], &secondChildPid, sizeof(secondChildPid));
            close(pipefd[1]);

            // 第一个子进程立即退出，让第二个子进程成为孤儿进程
            _exit(EXIT_SUCCESS);
        }
        else
        {
            // 第二次 fork 失败
            _exit(EXIT_FAILURE);
        }
    }
    else if (firstChildPid > 0)
    {
        // 父进程
        close(pipefd[1]); // 关闭写入端

        // 读取第二个子进程的PID
        pid_t secondChildPid = -1;
        ssize_t bytesRead = read(pipefd[0], &secondChildPid, sizeof(secondChildPid));
        close(pipefd[0]);

        // 等待第一个子进程退出
        int status;
        waitpid(firstChildPid, &status, 0);

        // 第二个子进程已经被 init 进程管理
        // 获取第二个子进程的PID

        if (bytesRead == sizeof(secondChildPid) && secondChildPid > 0)
        {
            processData->pid_ = secondChildPid;
            processData->programName_ = program.c_str();
            processData->state_ = Running;
            // std::cout << "进程已启动为系统独立进程，PID: " << secondChildPid << std::endl;
        }
        else
        {
            processData->state_ = NotRunning;
            std::cerr << "无法获取子进程PID" << std::endl;
        }

        // processData->pid_ = -1;
        // processData->programName_ = program.c_str();
        // processData->state_ = Running;

        // std::cout << "进程已启动为系统独立进程" << std::endl;
    }
    else
    {
        // 第一次 fork 失败
        processData->state_ = NotRunning;
        // std::cerr << "第一次 fork 失败: " << strerror(errno) << std::endl;
    }
#endif
}

// 启动进程（命令行模式）
void TpProcess::start(const TpString &command)
{
    TpFile commandFile(command);
    if (!commandFile.exists())
        return;

    TpProcessImpl *processData = static_cast<TpProcessImpl *>(data_);

    std::lock_guard<std::mutex> lock(processData->mutex_);
    if (processData->state_ != NotRunning)
        return;

    processData->state_ = Starting;

    struct stat st;
    stat(command.c_str(), &st);

    mode_t newMode = st.st_mode | S_IXUSR | S_IXGRP | S_IXOTH;
    chmod(command.c_str(), newMode);

    pid_t pid = fork();
    if (pid == 0)
    {
        // 子进程
        execlp("/bin/sh", "sh", "-c", command.c_str(), nullptr);
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        // 父进程
        processData->pid_ = pid;
        processData->programName_ = "sh";
        processData->state_ = Running;

        // 等待Shell启动子进程
        usleep(50000); // 延迟50ms等待子进程生成
        pid_t targetPid = processData->findChildPid();

        processData->pid_ = targetPid;
    }
    else
    {
        processData->state_ = NotRunning;
    }
}

TpProcess::ProcessState TpProcess::state() const
{
    TpProcessImpl *processData = static_cast<TpProcessImpl *>(data_);
    return processData->state_;
}

bool TpProcess::waitForFinished(int msecs)
{
    TpProcessImpl *processData = static_cast<TpProcessImpl *>(data_);

    if (processData->pid_ <= 0)
        return false;

    int status;
    const auto pid = waitpid(processData->pid_, &status, WNOHANG);

    if (pid == processData->pid_)
    {
        processData->state_ = NotRunning;
        return true;
    }

    // 超时等待逻辑
    using namespace std::chrono;
    auto start = steady_clock::now();

    while (true)
    {
        if (waitpid(processData->pid_, &status, WNOHANG) == processData->pid_)
        {
            processData->state_ = NotRunning;
            return true;
        }

        if (duration_cast<milliseconds>(steady_clock::now() - start).count() > msecs)
        {
            return false;
        }
        usleep(10000); // 10ms间隔检查
    }

    return true;
}

bool TpProcess::waitForStarted(int msecs)
{
    TpProcessImpl *processData = static_cast<TpProcessImpl *>(data_);

    using namespace std::chrono;
    auto start = steady_clock::now();

    while (true)
    {
        {
            std::lock_guard<std::mutex> lock(processData->mutex_);
            if (processData->state_ == Running)
                return true;
            if (processData->state_ == NotRunning)
                return false;
        }

        if (duration_cast<milliseconds>(steady_clock::now() - start).count() > msecs)
        {
            return false;
        }
        usleep(10000); // 10ms间隔检查
    }

    return true;
}

int32_t TpProcess::launchProcessID()
{
    TpProcessImpl *processData = static_cast<TpProcessImpl *>(data_);
    // std::lock_guard<std::mutex> lock(processData->mutex_);  // 加锁
    return static_cast<int32_t>(processData->pid_);
}

pid_t TpProcess::lauanchProcessPID()
{
    TpProcessImpl *processData = static_cast<TpProcessImpl *>(data_);
    // std::lock_guard<std::mutex> lock(processData->mutex_);  // 加锁
    return processData->pid_;
}

TpString TpProcess::launchProcessName()
{
    TpProcessImpl *processData = static_cast<TpProcessImpl *>(data_);
    return processData->getProcessName().empty() ? processData->programName_ : processData->getProcessName();
}
