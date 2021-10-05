#include <queue>
#include <yangutil/yang_unistd.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangwebrtc/YangRtcSession.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangwebrtc/YangRtcStun.h>
#include <yangutil/sys/YangLog.h>

bool yang_is_rtp_or_rtcp(const uint8_t *data, size_t len) {
	return (len >= 12 && (data[0] & 0xC0) == 0x80);
}
// For STUN packet, 0x00 is binding request, 0x01 is binding success response.
bool yang_is_stun(const uint8_t *data, size_t size) {
	return size > 0 && (data[0] == 0 || data[0] == 1);
}

// For RTCP, PT is [128, 223] (or without marker [0, 95]).
// Literally, RTCP starts from 64 not 0, so PT is [192, 223] (or without marker [64, 95]).
// @note For RTP, the PT is [96, 127], or [224, 255] with marker.
bool yang_is_rtcp(const uint8_t *data, size_t len) {
	return (len >= 12) && (data[0] & 0x80) && (data[1] >= 192 && data[1] <= 223);
}

// change_cipher_spec(20), alert(21), handshake(22), application_data(23)
// @see https://tools.ietf.org/html/rfc2246#section-6.2.1
bool yang_is_dtls(const uint8_t *data, size_t len) {
	return (len >= 13 && (data[0] > 19 && data[0] < 64));
}
YangRtcSession::YangRtcSession() {
	//m_cer = YangCertificate::createCertificate();


	m_dtls = NULL;
	m_srtp = NULL;
	m_context=NULL;
	isSendDtls = 0;
	twcc_id_ = 1;
	//nack_enabled_ = 0;
	m_play = NULL;
	m_publish = NULL;


	m_20ms = new YangTimer();
	m_20ms->setTaskId(20);
	m_20ms->setTimelen(20);
	m_20ms->setTask(this);

	m_1s = new YangTimer();
	m_1s->setTaskId(1);
	m_1s->setTask(this);
	m_1s->setTimelen(1000);

	m_100ms = new YangTimer();
	m_100ms->setTaskId(100);
	m_100ms->setTimelen(100);
	m_100ms->setTask(this);

	m_udp = NULL;
	m_startRecv = 0;

	m_stunBuffer = new char[1024];
	m_stunBufferLen = 0;

	m_isSendStun = 0;

//	cache_buffer_=NULL;

	m_rtpBuffer=NULL;

	//m_audioPublishBuf=new char[kRtcpPacketSize];
	m_videoPublishBuf=new char[kRtcpPacketSize];
}

YangRtcSession::~YangRtcSession() {
	yang_stop(m_20ms);
	yang_stop(m_1s);
	yang_stop(m_100ms);

	yang_delete(m_20ms);
	yang_delete(m_1s);
	yang_delete(m_100ms);

	yang_delete(m_publish);
	yang_delete(m_play);
	yang_delete(m_dtls);

	yang_deleteA(m_stunBuffer);
	yang_delete(m_rtpBuffer);
	yang_deleteA(m_videoPublishBuf);
	m_udp = NULL;
	m_srtp = NULL;
	m_context=NULL;


}

void YangRtcSession::setSsrc(uint32_t audioSsrc,uint32_t videoSsrc){

	m_packet.m_audioSsrc=audioSsrc;
	m_packet.m_videoSsrc=videoSsrc;

}

int32_t YangRtcSession::init(YangRtcContext* pconf,YangSendUdpData *pudp, YangReceiveCallback *cbk,	YangStreamOptType role) {
	m_udp = pudp;
	m_context=pconf;

	if (m_dtls == NULL) {
		m_dtls = new YangRtcDtls();
		m_dtls->init(pudp);

	}
	m_srtp = m_dtls->getSrtp();
	if(m_rtpBuffer==NULL) m_rtpBuffer=new YangRtpBuffer(m_context->streamConf->streamOptType);
	m_packet.init(m_rtpBuffer);
	if (role == Yang_Stream_Play) {
		if (m_play == NULL){
			m_play = new YangRtcPlayStream(this);
			m_play->initialize(m_context,m_context->source, cbk,m_rtpBuffer);

			//m_play->m_twcc_enabled
		}
	} else {
		if (m_publish == NULL){
			m_publish = new YangRtcPublishStream(this);
			m_publish->init(m_context->audioSsrc,m_context->videoSsrc);
		}
	}
	return Yang_Ok;
}
void YangRtcSession::startStunTimer() {
	if (m_1s)
		m_1s->start();
	m_isSendStun = 1;
}
void YangRtcSession::startTimers() {
	if (m_20ms&&!m_20ms->m_isStart)		m_20ms->start();
	if (m_context->context&&m_context->context->rtc.sendTwcc&&m_100ms&&!m_100ms->m_isStart)		m_100ms->start();
}
void YangRtcSession::setStunBuffer(char *p, int32_t plen) {
	memcpy(m_stunBuffer, p, plen);
	m_stunBufferLen = plen;
}
int32_t YangRtcSession::on_rtcp(char *data, int32_t nb_data) {
	int32_t err = Yang_Ok;

	int32_t nb_unprotected_buf = nb_data;
	if ((err = m_srtp->dec_rtcp(data, &nb_unprotected_buf)) != Yang_Ok) {
		return yang_error_wrap(err, "rtcp unprotect");
	}

	char *unprotected_buf = data;
	YangBuffer *buffer = new YangBuffer(unprotected_buf, nb_unprotected_buf);
	YangAutoFree(YangBuffer, buffer);

	YangRtcpCompound rtcp_compound;
	if (Yang_Ok != (err = rtcp_compound.decode(buffer))) {
		return yang_error_wrap(err, "decode rtcp plaintext=%u",
				nb_unprotected_buf);
	}

	YangRtcpCommon *rtcp = NULL;
	while (NULL != (rtcp = rtcp_compound.get_next_rtcp())) {
		err = dispatch_rtcp(rtcp);
		YangAutoFree(YangRtcpCommon, rtcp);

		if (Yang_Ok != err) {
			return yang_error_wrap(err,
					"cipher=%u, plaintext=%u,  rtcp=(%u,%u,%u,%u)", nb_data,
					nb_unprotected_buf, rtcp->size(), rtcp->get_rc(),
					rtcp->type(), rtcp->get_ssrc(), rtcp->size());
		}
	}

	return err;

}
void YangRtcSession::receive(char *data, int32_t size) {

	bool is_rtp_or_rtcp = yang_is_rtp_or_rtcp((uint8_t*) data, size);
	bool is_rtcp = yang_is_rtcp((uint8_t*) data, size);

	if (!is_rtp_or_rtcp && yang_is_stun((uint8_t*) data, size)) {

		YangStunPacket ping;
		int32_t err = 0;
		if ((err = ping.decode(data, size)) != Yang_Ok) {
			yang_error("decode stun packet failed");
			return;
		}
		if (!isSendDtls) {
			if (m_dtls->startHandShake()) {
				yang_error("dtls start handshake failed!");
			}
			isSendDtls = 1;
		}
	} else if (is_rtcp) {
		on_rtcp(data, size);
	} else if (is_rtp_or_rtcp) {
		//printf("v%d,",size);
		m_startRecv = 1;
		if (m_play) 	m_play->on_rtp(data, size);

	} else if (yang_is_dtls((uint8_t*) data, size)) {

		if (m_dtls) {
			if (m_dtls->decodeHandshake(data, size) == Yang_Ok) {
				//printf("\ndtls shakehand success!.............................");
				if(m_dtls->getDtlsState()==YangDtlsStateClientDone) {
					printf("\ndtls shakehand success!.startTimers......audiossrc=%u............vssrc=%u,uid==%d",
							m_context->audioSsrc,m_context->videoSsrc,m_context->streamConf->uid);
					startTimers();
					m_context->state=1;

					if(m_context&&m_context->context)
						m_context->context->streams.sendRequest(m_context->streamConf->uid,0,Yang_Req_Connected);
				}
			}
		}

	}
}


int32_t YangRtcSession::send_video_meta(YangStreamCapture* p)
{
    YangRtpPacket* pkt=new YangRtpPacket();
    YangAutoFree(YangRtpPacket,pkt);
    m_packet.package_stap_a(p,pkt);
    return send_avpacket(pkt);
}
int32_t YangRtcSession::publishVideo(YangStreamCapture* p) {

	if(p->getVideoFrametype()==YANG_Frametype_Spspps) return send_video_meta(p);
	vector<YangRtpPacket*> pkts;
	m_packet.on_h264_video(p,pkts);
	int32_t err=Yang_Ok;
    for (int32_t i = 0; i < (int)pkts.size(); i++) {
        YangRtpPacket* pkt = pkts[i];
        if ((err = send_avpacket(pkt)) != Yang_Ok) {
            err = yang_error_wrap(err, "send videp packet");
            break;
        }
    }

    for (int32_t i = 0; i < (int)pkts.size(); i++) {
        YangRtpPacket* pkt = pkts[i];
        yang_delete(pkt);
    }
    pkts.clear();
	return err;
}
int32_t YangRtcSession::publishAudio(YangStreamCapture *p) {
	YangRtpPacket *pkt=new YangRtpPacket();
	YangAutoFree(YangRtpPacket,pkt);
	m_packet.on_audio(p,pkt);
	return send_avpacket(pkt);
	return Yang_Ok;
}
void YangRtcSession::disconnect() {
	if(m_dtls) m_dtls->sendDtlsAlert();
}

YangSRtp* YangRtcSession::getSrtp() {
	return m_dtls->getSrtp();
}

void YangRtcSession::doTask(int32_t taskId) {
	if (m_isSendStun && taskId == 1) {
		if (m_udp && m_stunBuffer && m_stunBufferLen > 0)
			m_udp->sendData(m_stunBuffer, m_stunBufferLen);
	}
	if (!m_startRecv)
		return;
	if (m_play) {
		//int32_t err=0;
		if (taskId == 20) {
			m_play->check_send_nacks();
		}
		if (taskId == 1) {

			if (m_play->send_rtcp_rr())
				yang_error("RTCP Error:RR err ");
			if (m_play->send_rtcp_xr_rrtr())
				yang_error("RTCP Error:XR err ");
		}
		if (taskId == 100) {
			if (m_play->send_periodic_twcc())
				yang_error("RTCP Error:send twcc err ");
			//if(m_play->request_keyframe(this->m_videoSsrc)) yang_error("RTCP Error:request keyframe  err ");
		}
	}
}
int32_t YangRtcSession::send_rtcp(char *data, int32_t nb_data) {
	int32_t err = Yang_Ok;
	int32_t nb_buf = nb_data;
	if ((err = m_srtp->enc_rtcp(data, &nb_buf)) != Yang_Ok) {
		return yang_error_wrap(err, "protect rtcp");
	}

	if ((err = m_udp->sendData(data, nb_buf)) != Yang_Ok) {
		return yang_error_wrap(err, "send");
	}

	return err;
}
int32_t YangRtcSession::send_avpacket(YangRtpPacket *pkt) {
	int32_t err = Yang_Ok;

	YangBuffer buf(m_videoPublishBuf, kRtpPacketSize);
	if ((err = pkt->encode(&buf)) != Yang_Ok) {
		return yang_error_wrap(err, "encode packet");
	}
	int32_t nn_encrypt = buf.pos();
	if ((err = m_srtp->enc_rtp(m_videoPublishBuf, &nn_encrypt)) != Yang_Ok) {
		return yang_error_wrap(err, "srtp protect");
	}
	if(m_publish) m_publish->cache_nack(pkt);
	return m_udp->sendData(m_videoPublishBuf, nn_encrypt);
}
int32_t YangRtcSession::send_packet(YangRtpPacket *pkt) {
	int32_t err = Yang_Ok;
	char bufs[kRtpPacketSize];
	YangBuffer buf(bufs, kRtpPacketSize);
	if ((err = pkt->encode(&buf)) != Yang_Ok) {
		return yang_error_wrap(err, "encode packet");
	}

	int32_t nn_encrypt = buf.pos();
	if ((err = m_srtp->enc_rtp(bufs, &nn_encrypt)) != Yang_Ok) {
		return yang_error_wrap(err, "srtp protect");
	}

	return m_udp->sendData(bufs, nn_encrypt);
}
int32_t YangRtcSession::send_rtcp_fb_pli(uint32_t ssrc) {
	int32_t err = Yang_Ok;
	char buf[kRtpPacketSize];
	YangBuffer stream(buf, sizeof(buf));
	stream.write_1bytes(0x81);
	stream.write_1bytes(kPsFb);
	stream.write_2bytes(2);
	stream.write_4bytes(ssrc);
	stream.write_4bytes(ssrc);

	int32_t nb_protected_buf = stream.pos();
	if ((err = m_srtp->enc_rtcp(stream.data(), &nb_protected_buf))
			!= Yang_Ok) {
		return yang_error_wrap(err, "protect rtcp psfb pli");
	}

	return m_udp->sendData(stream.data(), nb_protected_buf);
}

int32_t YangRtcSession::check_send_nacks(YangRtpNackForReceiver *nack, uint32_t ssrc,
		uint32_t &sent_nacks, uint32_t &timeout_nacks) {

	YangRtcpNack rtcpNack(ssrc);
	rtcpNack.set_media_ssrc(ssrc);
	nack->get_nack_seqs(rtcpNack, timeout_nacks);
	if (rtcpNack.empty()) {
		return 0;
	}

	char buf[kRtcpPacketSize];
	YangBuffer stream(buf, sizeof(buf));


	rtcpNack.encode(&stream);
	int32_t nb_protected_buf = stream.pos();
	m_srtp->enc_rtcp(stream.data(), &nb_protected_buf);
	m_udp->sendData(stream.data(), nb_protected_buf);
	return Yang_Ok;
}
int32_t YangRtcSession::dispatch_rtcp(YangRtcpCommon *rtcp) {
	int32_t err = Yang_Ok;

	// For TWCC packet.
	if (YangRtcpType_rtpfb == rtcp->type() && 15 == rtcp->get_rc()) {
		return on_rtcp_feedback_twcc(rtcp->data(), rtcp->size());
	}

	// For REMB packet.
	if (YangRtcpType_psfb == rtcp->type()) {
		YangRtcpPsfbCommon *psfb = dynamic_cast<YangRtcpPsfbCommon*>(rtcp);
		if (15 == psfb->get_rc()) {
			return on_rtcp_feedback_remb(psfb);
		}
	}

	// Ignore special packet.
	if (YangRtcpType_rr == rtcp->type()) {
		YangRtcpRR *rr = dynamic_cast<YangRtcpRR*>(rtcp);
		if (rr->get_rb_ssrc() == 0) { //for native client
			return err;
		}
	}


	if (m_publish && Yang_Ok != (err = m_publish->on_rtcp(rtcp))) {
		return yang_error_wrap(err, "handle rtcp");
	}
	if (m_play && Yang_Ok != (err = m_play->on_rtcp(rtcp))) {
		return yang_error_wrap(err, "handle rtcp");
	}

	return err;
}

int32_t YangRtcSession::on_rtcp_feedback_twcc(char *data, int32_t nb_data) {
	return Yang_Ok;
}

int32_t YangRtcSession::on_rtcp_feedback_remb(YangRtcpPsfbCommon *rtcp) {
	//ignore REMB
	return Yang_Ok;
}
void YangRtcSession::do_request_keyframe(uint32_t ssrc){
	if(m_context&&m_context->context)
							m_context->context->streams.sendRequest(m_context->streamConf->uid,ssrc,Yang_Req_Sendkeyframe);

}
int32_t YangRtcSession::send_rtcp_rr(uint32_t ssrc, YangRtpRingBuffer *rtp_queue,
		const uint64_t &last_send_systime, const YangNtp &last_send_ntp) {
	int32_t err = Yang_Ok;
	// @see https://tools.ietf.org/html/rfc3550#section-6.4.2
	char buf[kRtpPacketSize];
	YangBuffer stream(buf, sizeof(buf));
	stream.write_1bytes(0x81);
	stream.write_1bytes(kRR);
	stream.write_2bytes(7);
	stream.write_4bytes(ssrc); // TODO: FIXME: Should be 1?

	uint8_t fraction_lost = 0;
	uint32_t cumulative_number_of_packets_lost = 0 & 0x7FFFFF;
	uint32_t extended_highest_sequence =
			rtp_queue->get_extended_highest_sequence();
	uint32_t interarrival_jitter = 0;

	uint32_t rr_lsr = 0;
	uint32_t rr_dlsr = 0;

	if (last_send_systime > 0) {
		rr_lsr = (last_send_ntp.ntp_second_ << 16)
				| (last_send_ntp.ntp_fractions_ >> 16);
		uint32_t dlsr = (yang_update_system_time() - last_send_systime) / 1000;
		rr_dlsr = ((dlsr / 1000) << 16) | ((dlsr % 1000) * 65536 / 1000);
	}

	stream.write_4bytes(ssrc);
	stream.write_1bytes(fraction_lost);
	stream.write_3bytes(cumulative_number_of_packets_lost);
	stream.write_4bytes(extended_highest_sequence);
	stream.write_4bytes(interarrival_jitter);
	stream.write_4bytes(rr_lsr);
	stream.write_4bytes(rr_dlsr);


	int32_t nb_protected_buf = stream.pos();
	if ((err = m_srtp->enc_rtcp(stream.data(), &nb_protected_buf))
			!= Yang_Ok) {
		return yang_error_wrap(err, "protect rtcp rr");
	}

	return m_udp->sendData(stream.data(), nb_protected_buf);
}

int32_t YangRtcSession::send_rtcp_xr_rrtr(uint32_t ssrc) {
	int32_t err = Yang_Ok;

	/*
	 @see: http://www.rfc-editor.org/rfc/rfc3611.html#section-2

	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |V=2|P|reserved |   PT=XR=207   |             length            |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                              SSRC                             |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 :                         report blocks                         :
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	 @see: http://www.rfc-editor.org/rfc/rfc3611.html#section-4.4

	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |     BT=4      |   reserved    |       block length = 2        |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |              NTP timestamp, most significant word             |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |             NTP timestamp, least significant word             |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 */
	int64_t now = yang_update_system_time();
	YangNtp cur_ntp = YangNtp::from_time_ms(now / 1000);

	char buf[kRtpPacketSize];
	YangBuffer stream(buf, sizeof(buf));
	stream.write_1bytes(0x80);
	stream.write_1bytes(kXR);
	stream.write_2bytes(4);
	stream.write_4bytes(ssrc);
	stream.write_1bytes(4);
	stream.write_1bytes(0);
	stream.write_2bytes(2);
	stream.write_4bytes(cur_ntp.ntp_second_);
	stream.write_4bytes(cur_ntp.ntp_fractions_);

	int32_t nb_protected_buf = stream.pos();
	if ((err = m_srtp->enc_rtcp(stream.data(), &nb_protected_buf))
			!= Yang_Ok) {
		return yang_error_wrap(err, "protect rtcp xr");
	}

	return m_udp->sendData(stream.data(), nb_protected_buf);
}

