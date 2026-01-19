//支持环境变量设置日志开关和日志等级以及标签过滤

#include <stdio.h>
#include "Log/elog.h"



//通过环境变量配置日志打印
//export ELOG_ENABLE=0          //设置日志打印开关
//export ELOG_LEVEL=DEBUG       //设置打印日志的等级，高于该等级的都会输出
//export ELOG_TAG="Debug.Log1"  //设置打印的标签，可以按照前缀设置
void example_environment()
{
    elog_init(); 
    /*elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG,  ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);*/
    elog_set_text_color_enabled(true);
    elog_start();
    elog_a("Assert.Log","This is assert log\n");
    elog_e("Error.Log","This is error log\n");
    elog_w("Warn.Log","This is warn log\n");
    elog_i("Info.Log","This is info log\n");
    elog_d("Debug.Log","This is debug log\n");
    elog_d("Debug.Log1","This is debug log1\n");
    elog_d("Debug.Log1.Log1","This is debug log1.log1\n");
    elog_d("Debug.Log1.Log2","This is debug log1.log2\n");
    elog_v("Verbose.Log","This is verbose log\n");
}

//程序直接控制打印,需要取消ELOG_ENABLE的设置：unset ELOG_ENABLE
void example_software()
{
    elog_init(); 
    elog_set_text_color_enabled(true);
    elog_start();
    elog_set_filter_lvl(ELOG_LVL_DEBUG);
    elog_set_filter_tag("Error");
    //elog_set_filter_tag("Assert");
    elog_a("Assert.Log","This is assert log\n");
    elog_e("Error.Log","This is error log\n");
    elog_w("Warn.Log","This is warn log\n");
    elog_i("Info.Log","This is info log\n");
    elog_d("Debug.Log","This is debug log\n");
    elog_d("Debug.Log1","This is debug log1\n");
    elog_d("Debug.Log1.Log1","This is debug log1.log1\n");
    elog_d("Debug.Log1.Log2","This is debug log1.log2\n");
    elog_v("Verbose.Log","This is verbose log\n");
}



int main()
{
    //example_environment();
    example_software();
    return 0;
}