# WechatDump
微信聊天记录提取

C++ 版：https://github.com/10cks/WechatDump

C# 版：https://github.com/AdminTest0/SharpWxDump

Python 版：https://github.com/xaoyaoo/PyWxDump

Rust 版：https://github.com/0xlane/wechat-dump-rs

编译：
```
x86_64-w64-mingw32-g++ main.cpp -o WechatDump.exe -lversion -w -s -g0 -static-libgcc -static-libstdc++
```

解密：
```
decrypt.exe -k 密钥 -d MSG1.db -o xxx.db
```
微信数据库加密方式： PBKDF2 加密算法以及 AES 加密算法，使用 PBKDF2 算法从输入的密码生成加密密钥。PBKDF2 算法是一种密码基准函数，它可以从输入的密码和盐值生成一个加密密钥。
读取要解密的文件，并检查 HMAC 是否正确。HMAC 是一种基于密钥的哈希函数，用于验证数据的完整性和真实性。
如果 HMAC 验证通过，那么就使用 AES 算法解密数据。AES 是一种对称加密算法，它使用相同的密钥进行加密和解密。

PBKDF2 生成密钥 A -> HMAC进行密码校验 -> 使用密钥 A 进行AES解密（CBC模式，初始化向量 IV 是 first[-48:-32]）

数据库存储路径：
```
C:\Users\[root]\Documents\WeChat Files\[wxid_yye8m0zgtooh22]\Msg
```

数据库结构：
```
.
├── FTSMSG0.db 搜索记录
├── FTSMSG1.db
├── FTSMSG2.db
├── MediaMSG0.db 语音消息
├── MediaMSG1.db
├── MediaMSG2.db
├── MicroMsg.db 通讯录
├── MSG0.db 聊天记录
├── MSG1.db
└── MSG2.db
```

数据库语法：
```
SELECT * FROM "Contact"
SELECT * FROM "MSG" WHERE StrContent  like'%账号%'
```

> PS：每台设备、每个微信账号对应一个key，切换设备或者微信账号，key都会变化；
> 同一台设备，同一个微信，只要安装新版微信时候，没有删除旧版的数据，那么key就不会改变。

decrypt 重构（go 版）
```
go mod init github.com/golang/crypto // 初始化 mod
go mod tidy // 进行包检测
set GOCACHE=D:\安全工具\微信取证 // 设置编译缓存路径，如果设置过可以不用设置了
go build decrypt.go // 编译
```

微信注册表：`\HKEY_CURRENT_USER\SOFTWARE\Tencent\WeChat`

输出：
```
D:\安全工具\微信取证\tools>WechatDump.exe
Program running...
[+] WeChat Running
[+] PID:28144
[+] WeChat can OpenProcess
[+] WeChatWin.dll Base: 0x7ff908c60000
[+] WeChat version: 3.9.7.29
[+] Memory at offset 63486984:
[+] Memory at offset 63488320: ret2myheart
[+] Memory at offset 63486792: 19845069465
[+] Memory at offset 0: MZ
[+] Memory at offset 63488256: 3f3efe5aa6e742afa0d60bd6ac8c2d5a960d2eddf55242dca7ccc248b79030bb
[+] DB Path at address 0x1fa0ff69000: C:\Users\root\Documents\WeChat Files\wxid_yye8m0zgtooh22\Msg\Multi
```

![image](https://github.com/10cks/WechatDump/assets/47177550/4daa0c29-05d0-4c01-82b3-f04ea7a89235)

