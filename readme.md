# PPlive

直播系统:

分成 控制器, 直播服务器

* pplive 就是控制器 同步网络中 所有的 直播信息 和 智能选取

## 机制

### 链接

### 断开

断开: 

客户端 断开某个资源, DisConnect

服务器 通知这个客户端的子节点 Ridirect

给客户端 SafeDis

### ping

ping 不通做

## 命令

开启 rtmp 服务器

### 开启rtmp 

```
./MediaServer
```

### 开启推流

* macos

```
ffmpeg -f avfoundation -framerate 30 -video_size 640x480 -i "0"  -vcodec libx264 -f flv rtmp://172.16.192.132:1935/test1
```

### 开启转发

```
ffmpeg -re -i src_url -c:a aac -strict -2 -ar 44100 -ab 48k -c:v libx264 -f flv dst_url
```

### 开启播放

```
ffplay url
```





