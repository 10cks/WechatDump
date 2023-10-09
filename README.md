# WechatDump
微信聊天记录提取

编译：
x86_64-w64-mingw32-g++ main.cpp -o hello.exe -lversion -w -s -g0 -static-libgcc -static-libstdc++

解密：
python ./decrypt.py -k 密钥 -d ./MicroMsg.db

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
