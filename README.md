
## 本fork旨在yangrtc基础上开发一些demo应用：
### 本地录制项目主要功能
+ 摄像和录屏功能切换
+ 设置MIC音量
+ 本地录制内容可预览
+ 简洁的UI，字体图标
+ 基于metartc-5.0-b4版本修改和增量开发

### UI截图
#### 简洁的悬浮主界面
![image](https://user-images.githubusercontent.com/42959931/192721633-dd6377c6-8fdb-4764-85a6-587a13cfc417.png)

+ 绿色按钮： 启动本地录制
+ 摄像头按钮：本地预览，可根据本地录制的类型切换图标为camera和screen的图标
+ 更多功能：系统设置，软件介绍，退出
![image](https://user-images.githubusercontent.com/42959931/192721728-64c2915f-6ee4-4ba9-942a-4b913c37df14.png)

#### 系统设置-摄像与录屏模式切换,camera分辨率选择
![image](https://user-images.githubusercontent.com/42959931/192721915-2b6539b8-eec6-4165-ab37-a715a833b767.png)

#### 系统设置-音频输入 MIC音量设置
![image](https://user-images.githubusercontent.com/42959931/192722129-27ad0b4b-3cc2-4f4a-82bb-6b8d4ef4b5aa.png)

#### 关于软件
![image](https://user-images.githubusercontent.com/42959931/192722229-c8c1fd35-5afa-4e96-9a09-7b3c68a004a0.png)

#### 图像预览
![image](https://user-images.githubusercontent.com/42959931/192722369-68d4692f-c7a3-42e0-b5f5-eb0b878eb6ed.png)

#### 录屏过程中闪烁红白灯, 点击此按钮停止本地录制
![image](https://user-images.githubusercontent.com/42959931/192722825-807e662c-48ef-48ae-b056-54909a457a7f.png)

#### 本地文件保存为mp4： ex： record-2022-09-28-15-57-28.mp4

---------------------------------- 以下为 Yangrtc 的内容 -----------------------------------------------------

## Yangrtc Overview
 
Yang Real-Time Communication，专业级的行业视音频应用的SDK。   
yangrtc是一个自主研发的支持Webrtc/Srt/Rtmp的rtc架构，包含多种视音频编解码和处理等。  

支持视频会议、高清录播直播、直播互动等多种视音频应用。  
 可用于远程教育、远程医疗、指挥调度、安防监控、影视录播、协同办公、直播互动等多种行业应用。  
webrtc支持为自主研发，非谷歌lib,兼容webrtc协议 ,可与谷歌Lib和浏览器互通  
支持Linux/Windows操作系统  
yangwebrtc已经转移到https://github.com/metartc/yangwebrtc  



### 目录功能  
#### libyangrtcmeeting2
视频会议类库  
#### yangmeeting2 
视频会议  
#### YangMeetingServer 
视频会议服务端程序  
#### librecord2
高清录播直播类库  
#### yangrecord2
高清录播直播系统   
#### yangvrscreen 
虚拟和桌面的录制和推流(webrtc/rtmp)  


### yangrtc功能

 1、视频编码 8bit:x264、x265、vaapi、nvenc等，二期增加AV1和多种硬件编码。  
 2、视频编码 10bit:x265、vaapi、nvenc等。  
 3、视频解码：ffmpeg和yangh264decoder。  
 4、VR:基于抠图实现虚拟视频的互动和录制、直播等。  
 5、8bit和10bit网络播放器：yangplayer  
 6、音频：Opus、Aac、Speex、Mp3等音频编解码。  
 7、音频：AEC、AGC、ANS及声音合成等处理。  
 8、传输：webrtc、rtmp、srt，webrtc为自己实现，没使用谷歌lib库。  
 9、直播：rtmp、srt、webrtc、HLS、HTTP-FLV。  
 10、8bit录制：h264、h265的mp4和flv。  
 11、10bit录制：h265的mp4  
 12、实现了屏幕共享与控制。  
 13、实现了声音和图像多种处理。  
 14、专业摄像头的云台控制与多镜头导播切换。  
 15、64位编程，不支持32位。   
  
  
### yangrtc3.0规划：  
1、传输加密改为国密加密，修改srtp支持国密。    
2、实现10位、16位全链路打通，从采集、编码、传输、解码、播放这些环节全部支持10位。  

### 视频会议编译

https://github.com/yangrtc/yangwebrtc/wiki/YangMeeting-Getting-Started
