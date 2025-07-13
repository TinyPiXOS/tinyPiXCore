#include "tpProcess.h"
#include "tpUtils.h"
#include "tpFileInfo.h"
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <fstream>
#include <atomic>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include "tpFile.h"

class tpProcessImpl
{
public:
    std::atomic<tpProcess::ProcessState> state_{tpProcess::NotRunning};
    pid_t pid_ = -1;
    std::string programName_;
    std::mutex mutex_;

    // 状态变更通知条件变量
    std::condition_variable stateCV_;

    // 执行参数转换
    char **prepareArgs(const tpString &program, const tpVector<tpString> &arguments)
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
    tpString getProcessName() const
    {
        if (pid_ <= 0)
            return "";

        try
        {
            std::ifstream fs("/proc/" + std::to_string(pid_) + "/comm");
            tpString name;
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
        tpString path = "/proc/" + std::to_string(pid_) + "/task/" +
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

tpProcess::tpProcess()
{
    data_ = new tpProcessImpl();
}

tpProcess::~tpProcess()
{
    tpProcessImpl *processData = static_cast<tpProcessImpl *>(data_);

    if (processData->state_ != NotRunning)
    {
        waitForFinished(500);
    }
    delete processData;
    processData = nullptr;
    data_ = nullptr;
}

void tpProcess::start(const tpString &program, const tpVector<tpString> &arguments)
{
    tpProcessImpl *processData = static_cast<tpProcessImpl *>(data_);

    std::lock_guard<std::mutex> lock(processData->mutex_);
    if (processData->state_ != NotRunning)
        return;

    processData->state_ = Starting;

    pid_t pid = fork();
    if (pid == 0)
    {
        // 子进程
        char **argv = processData->prepareArgs(program, arguments);
        execvp(program.c_str(), argv);
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
}

// 启动进程（命令行模式）
void tpProcess::start(const tpString &command)
{
    tpFile commandFile(command);
    if (!commandFile.exists())
        return;

    tpProcessImpl *processData = static_cast<tpProcessImpl *>(data_);

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

tpProcess::ProcessState tpProcess::state() const
{
    tpProcessImpl *processData = static_cast<tpProcessImpl *>(data_);
    return processData->state_;
}

bool tpProcess::waitForFinished(int msecs)
{
    tpProcessImpl *processData = static_cast<tpProcessImpl *>(data_);

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

bool tpProcess::waitForStarted(int msecs)
{
    tpProcessImpl *processData = static_cast<tpProcessImpl *>(data_);

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

int32_t tpProcess::launchProcessID()
{
    tpProcessImpl *processData = static_cast<tpProcessImpl *>(data_);
    // std::lock_guard<std::mutex> lock(processData->mutex_);  // 加锁
    return static_cast<int32_t>(processData->pid_);
}

pid_t tpProcess::lauanchProcessPID()
{
    tpProcessImpl *processData = static_cast<tpProcessImpl *>(data_);
    // std::lock_guard<std::mutex> lock(processData->mutex_);  // 加锁
    return processData->pid_;
}

tpString tpProcess::launchProcessName()
{
    tpProcessImpl *processData = static_cast<tpProcessImpl *>(data_);
    return processData->getProcessName().empty() ? processData->programName_ : processData->getProcessName();
}
