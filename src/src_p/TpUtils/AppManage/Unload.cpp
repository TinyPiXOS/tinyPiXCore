/*///------------------------------------------------------------------------------------------------------------------------//
        应用卸载程序
说 明 :
日 期 : 2024.8.21

/*/
//------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "Install.h"
#include "Unload.h"
#include "AppManage/AppmUtils.h"
#include "Purview.h"
#include "ConfJson.h"
#include "typesDef.h"
#include "TpDir.h"
#include "TpFile.h"


int TpAppmUnload::Unload(TpAppID uuid)
{
    if (is_valid_uuid(uuid.value.c_str()) != TP_TRUE)
    {
        fprintf(stderr, "uuid error\n");
        return -1;
    }

    // 调用接口杀死进程

    // 执行remove.sh
    TpString path;
    path = TpString(APP_INSTALL_PATH) + "/" + uuid.value;

    if (find_directory(path.c_str(), APP_REMOVE_SH) > 0)
    {
        system_command("./" APP_REMOVE_SH);
    }

    // 删除安装文件
    TpString command = TpString(APP_INSTALL_PATH) + "/" + uuid.value;
    if (!TpDir::removeRecursively(command))
    {
        fprintf(stderr, "delete app error\n");
    }

    // 删除conf里面的uuid.json文件
    command = TpString(APP_JSON_PATH) + "/" + uuid.value + ".json";
    if (!TpFile::remove(command))
    {
        fprintf(stderr, "delete app conf json (%s) error\n",command.c_str());
        // return -1;
    }

    // 修改install_conf
    /*	if(del_appuuid_install_safe(uuid,APP_INSTALL_CONF_PATH)<0)
        {
            fprintf(stderr,"delete install json error\n");
            //return -1;
        }
    */
    // 删除用户
    if (TpAppmPurview::RemovePurview(uuid) < 0)
    {
        fprintf(stderr, "delete user error\n");
    }
    return 0;
}
