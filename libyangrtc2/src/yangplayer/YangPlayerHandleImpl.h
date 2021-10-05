
#ifndef YANGMEETING_INCLUDE_YANGPLAYERHANDLE_H_
#define YANGMEETING_INCLUDE_YANGPLAYERHANDLE_H_
#include <yangplayer/YangPlayerHandle.h>
#include <yangplayer/YangPlayerBase.h>
#include <yangplayer/YangPlayReceive.h>
#include <yangstream/YangSynBuffer.h>
#include <yangutil/yangavinfotype.h>
#include "YangRtcReceive.h"

struct YangPlayerUrlType{
	int32_t netType;
	int32_t port;
	string server;
	string app;
	string stream;
};
class YangPlayerHandleImpl :public YangPlayerHandle{
public:
	YangPlayerHandleImpl(YangContext* pcontext);
	virtual ~YangPlayerHandleImpl();
	YangVideoBuffer* getVideoBuffer();
	void play(string url,int32_t localport);
	//void playRtc(char* serverIp,int32_t port,char* app,char* stream);
	int32_t playRtc(int32_t puid,std::string localIp,int32_t localPort, std::string server, int32_t pport,std::string app,std::string stream);
	void stopPlay();
protected:
	int32_t parse(string url);
	YangPlayerUrlType m_url;
	void initList();
	YangPlayReceive *m_recv;
	YangPlayerBase *m_play;
	YangRtcReceive *m_rtcRecv;
	//void stopPlay();
private:
	YangContext* m_context;
	YangVideoDecoderBuffer* m_outVideoBuffer;
	YangAudioEncoderBuffer* m_outAudioBuffer;
	//YangSynBuffer* m_syn;
};

#endif /* YANGMEETING_INCLUDE_YANGPLAYERHANDLE_H_ */
