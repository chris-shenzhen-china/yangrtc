﻿//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGAPP_YANGDECODERAPP_H_
#define YANGAPP_YANGDECODERAPP_H_
#include <yangmeeting/yangmeetingtype.h>
#include <vector>
#include "yangutil/sys/YangIni.h"
#include "yangdecoder/YangAudioDecoderHandles.h"
#include "yangdecoder/YangVideoDecoderHandles.h"

using namespace std;

class YangMeetingDecoder{
public:
	YangMeetingDecoder(YangMeetingContext *pcontext);
	virtual ~YangMeetingDecoder();
	void initAudioDecoder();
	void initVideoDecoder();
	void setInVideoBuffer(YangVideoDecoderBuffer *pvel);
	void setInAudioBuffer(YangAudioEncoderBuffer *pael);
	vector<YangVideoBuffer*>* getOutVideoBuffer();
	vector<YangAudioPlayBuffer*>*  getOutAudioBuffer();
	void startAudioDecoder();
	void startVideoDecoder();
	void stopAll();
	YangAudioDecoderHandles *m_audioDec;
	YangVideoDecoderHandles *m_videoDec;
private:
	YangMeetingContext *m_context;
	vector<YangVideoBuffer*>* m_out_videoBuffer;
	vector<YangAudioPlayBuffer*>* m_out_audioBuffer;
};


#endif /* YANGAPP_YANGDECODERAPP_H_ */
