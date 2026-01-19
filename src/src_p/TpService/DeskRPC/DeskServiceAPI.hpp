#ifndef __DESK_SERVICE_API_H
#define __DESK_SERVICE_API_H

#include "c_DeskService_server.h"

/*! 桌面服务输入RPC函数 */
bool DR_DesktSetService(const binary_t * params)
{
    return true;
}

/*! 桌面服务输出RPC函数 */
binary_t * DR_DesktGetService(const binary_t * params)
{

}

#endif