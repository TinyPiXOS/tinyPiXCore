# RPC相关配置

## 结构概述

### ServiceRPC

应用管理服务；
服务端为 TpService 可执行程序；客户端为桌面/应用；
用于获取当前运行应用信息、启动应用等

### DeskRPC

桌面RPC服务；
服务端为桌面程序，通过在TpApp判断当前应用是否为桌面决定启动服务端；客户端为应用；
用于应用于桌面间通信

## .erpc文件编译

使用 bin/erpcgen 将.erpc文件构建为客户端、服务端代码；命令如下：

```bash
# erpcgen -gc -o 文件生成路径 .erpc文件
erpcgen -gc -o ./c/shim/ hello_world.erpc
```