﻿//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "YangMeetingHandleImpl.h"
#include <yangmeeting/YangMeetingNet.h>
#include <yangmeeting/yangmeetingtype.h>
#include <yangavutil/audio/YangRtcAec.h>
#include "yangutil/yang_unistd.h"
#include "pthread.h"
#include <iostream>

#include <vector>

YangMeetingHandle::YangMeetingHandle(){
	m_state=1;
	m_context=NULL;
}
YangMeetingHandle::~YangMeetingHandle(){
	m_context=NULL;
}
YangMeetingHandleImpl::YangMeetingHandleImpl(YangMeetingContext *p_ini):YangMeetingHandle() {
	m_context = p_ini;
	m_state=1;
	m_playMain = NULL;
	m_pubMain = NULL;
	m_netMain = NULL;

	m_hasAec = 0;
	m_aec=NULL;
	m_message=NULL;

	m_playvideo=NULL;
	m_aec=NULL;//(YangRtcAec*)calloc(sizeof(YangRtcAec),1);
	m_pre=(YangPreProcess*)calloc(sizeof(YangPreProcess),1);

	yang_create_preProcess(m_pre);
}

YangMeetingHandleImpl::~YangMeetingHandleImpl() {
	m_context = NULL;

	stopAll();
	if(m_state==Yang_Ok) disconnectServer();


	yang_delete(m_playMain);
	yang_delete(m_pubMain);
	yang_delete(m_netMain);
	yang_destroy_rtcaec(m_aec);
	yang_free(m_aec);
	yang_destroy_preProcess(m_pre);
	yang_free(m_pre);
}
void YangMeetingHandleImpl::setMeetingMessage(YangMeetingMessage* pms){
	m_message=pms;
}
void YangMeetingHandleImpl::setPlayvideoHandle(YangVideoPlayHandle* playvideo){
	m_playvideo=playvideo;
}
void YangMeetingHandleImpl::delMessage(){
	m_message=NULL;
}
void YangMeetingHandleImpl::stopAll(){
	if(m_pubMain) m_pubMain->setInAudioBuffer(NULL);
		if(m_pubMain) m_pubMain->stopAll();
		if(m_playMain) m_playMain->stopAll();
		if(m_netMain) m_netMain->stopAll();
}

void YangMeetingHandleImpl::change(int32_t st){
	if(m_pubMain) m_pubMain->change(st);
}
int32_t YangMeetingHandleImpl::sendHeartbeat(){
    if(m_netMain) return m_netMain->sendHeartbeat();
    return Yang_Ok;
}

void YangMeetingHandleImpl::sendKeyframe(){
    if(m_pubMain) m_pubMain->sendMsgToEncoder(Yang_Req_Sendkeyframe);
}
void YangMeetingHandleImpl::notifyMediaSource(int32_t puid){
	if(m_playMain) {
		map<int,YangAudioParam*>::iterator ita=m_context->remoteAudio.find(puid);
		map<int,YangVideoParam*>::iterator itv=m_context->remoteVideo.find(puid);
		YangAudioParam* audio=NULL;
		YangVideoParam* video=NULL;
		if(ita!=m_context->remoteAudio.end()) audio=ita->second;
		if(itv!=m_context->remoteVideo.end()) video=itv->second;
		if(audio||video)	m_playMain->notifyMediaSource(puid,audio,video);
	}
}

void YangMeetingHandleImpl::initAec(YangMeetingContext *p_ini) {
	if (m_aec != NULL) {


		//if(p_ini->avinfo.audio.usingMono){
            m_aec->init(m_aec->session,16000,1,320,p_ini->avinfo.audio.echoPath);
		//}else{
         //   m_aec->init(m_aec->context,p_ini->avinfo.audio.sample,p_ini->avinfo.audio.channel,1024,p_ini->avinfo.audio.echoPath);
		//}
	}
}

int32_t YangMeetingHandleImpl::getConectState(){
	return m_state;
}

int32_t YangMeetingHandleImpl::startAudioCapture() {
	if (m_pubMain == NULL)	m_pubMain = new YangMeetingPublish(m_context);

	m_pubMain->startAudioCapture(m_pre);	//startAudioCapture();

	if (m_context->avinfo.audio.enableAec&&!m_aec) 	{
		m_aec=(YangRtcAec*)calloc(sizeof(YangRtcAec),1);
		yang_create_rtcaec(m_aec);
		initAec(m_context);
		m_pubMain->setAec(m_aec);
	}
	return Yang_Ok;
}
int32_t YangMeetingHandleImpl::startVideoCapture() {

	if (m_pubMain == NULL)
		m_pubMain = new YangMeetingPublish(m_context);

	m_pubMain->startVideoCapture();
	return Yang_Ok;
}

int32_t YangMeetingHandleImpl::init(){

	startAudioCapture();
	startVideoCapture();

	if (!m_netMain) 		{
		m_netMain = new YangMeetingNet(m_context);
	}
	m_netMain->initNet();
	if(!m_playMain) m_playMain=new YangMeetingPlay(m_context);
	m_playMain->init(m_netMain);
	m_pubMain->setInAudioBuffer(m_playMain->m_ydb->getOutAudioBuffer());

	return Yang_Ok;
}





int32_t YangMeetingHandleImpl::reconnectServer(){
	return m_netMain->reconnectServer();
}

int32_t YangMeetingHandleImpl::connectServer() {
	return m_netMain->connectServer(0, 0);

}
int32_t YangMeetingHandleImpl::connectPushServer(int32_t puid){
	return m_netMain->connectPushServer(puid);
}
int32_t YangMeetingHandleImpl::connectPlayServer(int32_t puid){
	return m_netMain->connectPlayServer(puid);
}
void YangMeetingHandleImpl::loginServer(int32_t loginState){
	if(loginState==Yang_Ok){
		m_state=Yang_Ok;
	}else{
		m_state=1;
	}
	m_netMain->netstateHandle(loginState);
}
void YangMeetingHandleImpl::login(int32_t puid){
	if(m_netMain) m_netMain->login(puid);
}

void YangMeetingHandleImpl::logout(int32_t puid){
	if(m_playMain) {
		m_playMain->unPlayAudio(puid);
		m_playMain->unPlayVideo(puid);
	}
	if(m_netMain) m_netMain->logout(puid);
}

int32_t YangMeetingHandleImpl::disconnectServer(){
	m_state=Yang_LeaveRoom;
	unPushAudio();
	unPushVideo();
	if(m_playMain) m_playMain->removeAllStream();
	if(m_netMain)  m_netMain->disconnectServer();
	return m_state;
}


int32_t YangMeetingHandleImpl::pushAudio() {
	start_audioEncoder();
	if(m_netMain->pubAudio())	return ERROR_RTMP_PubFailure;
	m_context->user.micFlag=2;

	return Yang_Ok;

}

int32_t YangMeetingHandleImpl::pushVideo() {
	start_videoEncoder();
	if(m_netMain->pubVideo()) 	return ERROR_RTMP_PubFailure;
	m_context->user.camFlag=2;

	return Yang_Ok;

}
int32_t YangMeetingHandleImpl::unPushAudio() {
	stopAudioCaptureState();
	if (m_netMain)	return	m_netMain->unPubAudio();
	return 1;

}
int32_t YangMeetingHandleImpl::unPushVideo() {

	stopVideoCaptureState();
	if (m_netMain)	return	m_netMain->unPubVideo();
	return 1;
}

void YangMeetingHandleImpl::start_audioEncoder() {
	m_pubMain->initAudioEncoding(m_netMain);
	m_pubMain->startAudioEncoding();
}


void YangMeetingHandleImpl::start_videoEncoder() {
	m_pubMain->initVideoEncoding(m_netMain);
	m_pubMain->startVideoEncoding();

}
void YangMeetingHandleImpl::startAudioDecoder() {
	if (m_playMain==NULL)		m_playMain = new YangMeetingPlay(m_context);
		m_playMain->startAudioDecoder(m_netMain);

}
void YangMeetingHandleImpl::startVideoDecoder() {
	if (m_playMain == NULL)
		m_playMain = new YangMeetingPlay(m_context);
	m_playMain->startVideoDecoder(m_netMain);
}


YangVideoBuffer* YangMeetingHandleImpl::getPreviewVideo() {
	if (m_pubMain)	return m_pubMain->getPreVideoBuffer();
	return NULL;
}
YangVideoBuffer* YangMeetingHandleImpl::getVideoBuffer(int32_t uid) {
	vector<YangVideoBuffer*> *des = m_playMain->m_ydb->getOutVideoBuffer();
	for (uint32_t  i = 0; i < des->size(); i++) {
		if (des->at(i)->m_uid == uid)
			return des->at(i);
	}
	return NULL;
}

vector<YangVideoBuffer*>* YangMeetingHandleImpl::getDecoderVideoBuffers(){
	if (m_playMain == NULL)		m_playMain = new YangMeetingPlay(m_context);
	if(m_playMain->m_ydb) return m_playMain->m_ydb->getOutVideoBuffer();
	return NULL;
}

vector<YangUser*>* YangMeetingHandleImpl::getUserList() {
	if (m_netMain)
		return m_netMain->getUserList();
	return NULL;
}

int32_t YangMeetingHandleImpl::playAudio(int32_t puid) {
	startAudioDecoder();
	if(m_netMain&&m_netMain->playAudio(puid)) 		return ERROR_RTMP_PlayFailure;
	return Yang_Ok;
}
int32_t YangMeetingHandleImpl::playVideo(int32_t puid) {
	startVideoDecoder();
	if (m_netMain&&	m_netMain->playVideo(puid)) return ERROR_RTMP_PlayFailure;
	return Yang_Ok;
}
int32_t YangMeetingHandleImpl::unPlayAudio(int32_t puid) {
	if(m_playMain) m_playMain->unPlayAudio(puid);
	if (m_netMain&&m_netMain->unPlayAudio(puid))	return ERROR_RTMP_PlayFailure;
	return Yang_Ok;
}
int32_t YangMeetingHandleImpl::unPlayVideo(int32_t puid) {
	if(m_playvideo) m_playvideo->removePlayvideo(puid);
	if(m_playMain) m_playMain->unPlayVideo(puid);
	if (m_netMain&&m_netMain->unPlayVideo(puid))		return ERROR_RTMP_PlayFailure;

	return Yang_Ok;
}

#if Yang_Enable_Vr
void YangMeetingHandleImpl::addVr(){
	if (m_pubMain) m_pubMain->addVr();
}
void YangMeetingHandleImpl::delVr(){
	if (m_pubMain) m_pubMain->delVr();
}
#endif
void YangMeetingHandleImpl::startAudioCaptureState() {

	if (m_pubMain)
		m_pubMain->startAudioCaptureState();
}
void YangMeetingHandleImpl::startVideoCaptureState() {
	if (m_pubMain)		m_pubMain->startVideoCaptureState();
}
void YangMeetingHandleImpl::stopAudioCaptureState() {
	if (m_pubMain)
		m_pubMain->stopAudioCaptureState();
}
void YangMeetingHandleImpl::stopVideoCaptureState() {
	if (m_pubMain)
		m_pubMain->stopVideoCaptureState();
}
