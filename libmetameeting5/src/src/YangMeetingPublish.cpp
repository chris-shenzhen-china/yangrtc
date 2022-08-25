﻿//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangmeeting/YangMeetingPublish.h>
#include <yangmeeting/YangVrCapture.h>
#include "YangMeetingCaptureImpl.h"

YangMeetingPublish::YangMeetingPublish(YangMeetingContext *pcontext) {
	m_context = pcontext;

	m_context->streams.setSendRequestCallback(this);
	m_encoder = NULL;
	m_capture = NULL;
	isStartAudioCapture = 0, isStartVideoCapture = 0;
	isStartAudioEncoder = 0, isStartVideoEncoder = 0;


}

YangMeetingPublish::~YangMeetingPublish() {
	stopAll();
	m_context = NULL;

	yang_delete(m_encoder);
	yang_delete(m_capture);
}

void YangMeetingPublish::sendRequest(int32_t puid,uint32_t ssrc,YangRequestType req){
    if(req<Yang_Req_Connected) {
        sendMsgToEncoder(req);
    }
    if(req==Yang_Req_Connected) {


    }
}

void YangMeetingPublish::stopAll(){
	if(m_capture) m_capture->stopAll();
	if(m_encoder) m_encoder->stopAll();
}
void YangMeetingPublish::sendMsgToEncoder(YangRequestType req){
    if(m_encoder) m_encoder->sendMsgToEncoder(req);
}

void YangMeetingPublish::initCapture(){
	if (m_capture == NULL){
			//if(m_context->avinfo.sys.hasVr){
			//	m_capture=new YangVrCapture(m_context);
			//}else{
				if(m_context->avinfo.sys.enableMultCamera)
					m_capture=new YangVrCapture(m_context);
				else
					m_capture = new YangMeetingCaptureImpl(m_context);
			//}
		}
}

void YangMeetingPublish::startAudioCapture(YangPreProcess *pp) {
	if (isStartAudioCapture == 1)	return;
	initCapture();
	m_capture->initAudio(pp);
	m_capture->startAudioCapture();
	isStartAudioCapture = 1;

}
void YangMeetingPublish::startVideoCapture() {
	if (isStartVideoCapture == 1)	return;
	initCapture();

	m_capture->initVideo();
	m_capture->startVideoCapture();
	isStartVideoCapture = 1;

}
void YangMeetingPublish::setNetBuffer(YangMeetingNet *prr){
	yang_reindex(m_encoder->getOutAudioBuffer());
	yang_reindex(m_encoder->getOutVideoBuffer());
	m_encoder->getOutVideoBuffer()->resetIndex();
	prr->setInAudioList(m_encoder->getOutAudioBuffer());
	prr->setInVideoList(m_encoder->getOutVideoBuffer());
	prr->setInVideoMetaData(m_encoder->getOutVideoMetaData());
}
void YangMeetingPublish::initAudioEncoding(YangMeetingNet *prr) {

	if (isStartAudioEncoder == 1)		return;
	if (m_encoder == NULL)
		m_encoder = new YangMeetingEncoder(m_context);
	m_encoder->initAudioEncoder();
	m_encoder->setInAudioBuffer(m_capture->getOutAudioBuffer());
	prr->setInAudioList(m_encoder->getOutAudioBuffer());
	isStartAudioEncoder = 1;
}
void YangMeetingPublish::setAec(YangRtcAec *paec){
	if(m_capture) m_capture->setAec(paec);
}
void YangMeetingPublish::change(int32_t st){
	if(m_capture) m_capture->change(st);
}
void YangMeetingPublish::setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuf){
	if(m_capture) m_capture->setInAudioBuffer(pbuf);
}
void YangMeetingPublish::initVideoEncoding(YangMeetingNet *prr) {
	if (isStartVideoEncoder == 1)	return;
	if (m_encoder == NULL)
		m_encoder = new YangMeetingEncoder(m_context);
	m_encoder->initVideoEncoder();
	m_encoder->setInVideoBuffer(m_capture->getOutVideoBuffer());
	prr->setInVideoList(m_encoder->getOutVideoBuffer());
	prr->setInVideoMetaData(m_encoder->getOutVideoMetaData());
	isStartVideoEncoder = 1;

}
void YangMeetingPublish::startAudioEncoding() {
	if (m_encoder)
		m_encoder->startAudioEncoder();
}
void YangMeetingPublish::startVideoEncoding() {
	if (m_encoder)
		m_encoder->startVideoEncoder();
}
void YangMeetingPublish::startAudioCaptureState() {
	if (m_capture)
		m_capture->startAudioCaptureState();
}
YangVideoBuffer* YangMeetingPublish::getPreVideoBuffer(){
	if (m_capture)  return	m_capture->getPreVideoBuffer();
	return NULL;
}

void YangMeetingPublish::startVideoCaptureState() {
	if (m_capture)
		m_capture->startVideoCaptureState();
}

void YangMeetingPublish::stopAudioCaptureState() {
	if (m_capture)
		m_capture->stopAudioCaptureState();
}
void YangMeetingPublish::stopVideoCaptureState() {
	if (m_capture)
		m_capture->stopVideoCaptureState();
}
#if Yang_Enable_Vr
void YangMeetingPublish::addVr(){
	if (m_capture) m_capture->addVr();
}
void YangMeetingPublish::delVr(){
	if (m_capture) m_capture->delVr();
}
#endif
