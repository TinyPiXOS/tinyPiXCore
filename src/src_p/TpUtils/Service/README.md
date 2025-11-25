
### .erpc文件编译

使用 bin/erpcgen 将.erpc文件构建为客户端、服务端代码；命令如下：

```bash
# erpcgen -gc -o 文件生成路径 .erpc文件
erpcgen -gc -o ./c/shim/ hello_world.erpc
```