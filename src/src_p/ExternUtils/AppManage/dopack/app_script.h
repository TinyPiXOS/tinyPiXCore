/*///------------------------------------------------------------------------------------------------------------------------//
		启动脚本执行内容
说 明 : 
日 期 : 2024.11.20

/*///------------------------------------------------------------------------------------------------------------------------//

#ifndef _APP_SCRIPT_H_
#define _APP_SCRIPT_H_


//检查应用程序是否存在
#define CHECK_APP_OK	"if [ ! -f \"$APP_PATH\" ]; then\n\techo \"应用程序不存在，无法启动。\"\n\texit 1\nfi"

//检查依赖库


//检查日志文件
#define CHECK_LOG_OK	"if [ ! -d \"$MYAPP_LOG_DIR\" ]; then\
        echo \"Creating log directory $MYAPP_LOG_DIR\"\
        mkdir -p $MYAPP_LOG_DIR\
    fi"

//
















#endif
