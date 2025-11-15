#include "TpApp.h"
#include "TpString.h"
#include "TpConfig.h"

int32_t main(int32_t argc, char *argv[])
{
    TpConfig conf("/System/conf/tinyPiX.conf");

    int32_t width = conf.value("display-setting/width").toInt();
    int32_t height = conf.value("display-setting/height").toInt();
    int32_t format = conf.value("display-setting/format").toInt();
    std::cout << "[ display-setting] :  width : " << width << "  height:  " << height << "  format:  " << format << std::endl;

    TpString daemon = conf.value("attribute-setting/daemon");
    TpString acclerate = conf.value("attribute-setting/acclerate");
    TpString brightness = conf.value("attribute-setting/brightness");
    TpString sharemem = conf.value("attribute-setting/sharemem");
    TpString shareone = conf.value("attribute-setting/shareone");
    std::cout << "[ attribute-setting] :  daemon : " << daemon << "  acclerate: " << acclerate
              << "  brightness:  " << brightness << "  sharemem:  " << sharemem << "  shareone:  " << shareone << std::endl;

    TpString simulator = conf.value("system-setting/simulator");
    TpString quitwait = conf.value("system-setting/quitwait");
    std::cout << "[ system-setting] :  simulator : " << simulator << "  quitwait:  " << quitwait << std::endl;

    TpString startdir = conf.value("mode-setting/startdir");
    TpString startapp = conf.value("mode-setting/startapp");
    std::cout << "[ mode-setting] :  startdir : " << startdir << "  startapp:  " << startapp << std::endl;

    return 0;
}
