# EasyChat
C++版本的简易聊天软件
## 简要介绍
        运用TCP协议，允许多人在线聊天，注册。对方不在线时，服务器可缓存消息，并在对方登陆时发送给用户。用户名，
    密码等信息存储在指定文件夹中。
## 使用说明
        下载所有源代码和CMakeLists.txt文件到同一文件夹中，使用CMake，指定源代码目录和目标文件目录生成对应
    集成开发环境的构建档，后编译运行，或直接下载Debug文件夹下的所有文件，点击EasyChat.exe运行程序。
    程序使用cmd命令行执行，需要提供可执行文件名和Server或Client两个参数。
        文件夹已经包含三个用户，用户名和密码分别是{A,PWA},{B,PWB},{C,PWC}。
        具体操作详见程序运行时提示。
