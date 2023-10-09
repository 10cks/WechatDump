# WechatDump
微信聊天记录提取（C++版）

C# 版：https://github.com/AdminTest0/SharpWxDump

Python 版：https://github.com/xaoyaoo/PyWxDump

编译：
```
x86_64-w64-mingw32-g++ main.cpp -o hello.exe -lversion -w -s -g0 -static-libgcc -static-libstdc++
```

解密：
```
python ./decrypt.py -k 密钥 -d ./MicroMsg.db
```
数据库存储路径：
```
C:\Users\[root]\Documents\WeChat Files\[wxid_yye8m0zgtooh22]\Msg
```

数据库结构：
```
.
├── FTSMSG0.db
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
