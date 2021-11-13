﻿#include <yangwebrtc/YangRtpPacketWrap.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpRawPayload.h>
#include <yangrtp/YangRtpSTAPPayload.h>
#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/sys/YangLog.h>

YangRtpPacketWrap::YangRtpPacketWrap() {
    m_videoSsrc = 0;
    m_videoSeq = 0;
    m_audioSeq = 0;
    m_audioSsrc = 0;
    m_rtpBuffer = NULL;
    m_session = NULL;
    m_videoBuf = new char[kRtpPacketSize];
}

YangRtpPacketWrap::~YangRtpPacketWrap() {
    m_session = NULL;
    yang_deleteA(m_videoBuf);
}
void YangRtpPacketWrap::init(YangRtpBuffer *rtpBuffer,
                             YangRtcSessionI *psession) {
    m_rtpBuffer = rtpBuffer;
    m_session = psession;
}
int32_t YangRtpPacketWrap::on_audio(YangStreamCapture *audioFrame) {
    int err = 0;
    m_pushAudioPacket.reset();
    m_pushAudioPacket.m_header.set_payload_type(kAudioPayloadType);
    m_pushAudioPacket.m_header.set_ssrc(m_audioSsrc);
    m_pushAudioPacket.m_frame_type = YangFrameTypeAudio;
    m_pushAudioPacket.m_header.set_marker(true);

    m_pushAudioPacket.m_header.set_sequence(m_audioSeq++);
    m_pushAudioPacket.m_header.set_timestamp(audioFrame->getAudioTimestamp());
    m_pushAudioPacket.m_header.set_padding(0);
    m_pushAudioPacket.m_payload_type = YangRtspPacketPayloadTypeRaw;

    m_raw.m_payload = m_videoBuf;
    m_raw.m_size = audioFrame->getAudioLen();
    memcpy(m_raw.m_payload, audioFrame->getAudioData(), m_raw.m_size);
    if ((err = encode(&m_pushAudioPacket)) != Yang_Ok) {
        return yang_error_wrap(err, "encode packet");
    }

    return err;
}
int32_t YangRtpPacketWrap::on_h264_video(YangStreamCapture *videoFrame) {
    int32_t err = Yang_Ok;

    if (videoFrame->getVideoLen() <= kRtpMaxPayloadSize) {
        if ((err = package_single_nalu(videoFrame)) != Yang_Ok) {
            return yang_error_wrap(err, "package single nalu");
        }
    } else {
        if ((err = package_fu_a(videoFrame, kRtpMaxPayloadSize)) != Yang_Ok) {
            return yang_error_wrap(err, "package fu-a");
        }
    }

    return err;
}

int32_t YangRtpPacketWrap::package_single_nalu(YangStreamCapture *videoFrame) {
    int32_t err = Yang_Ok;

    m_pushVideoPacket.reset();
    m_pushVideoPacket.m_header.set_payload_type(kVideoPayloadType);
    m_pushVideoPacket.m_header.set_ssrc(m_videoSsrc);
    m_pushVideoPacket.m_frame_type = YangFrameTypeVideo;
    m_pushVideoPacket.m_header.set_sequence(m_videoSeq++);
    m_pushVideoPacket.m_header.set_timestamp(videoFrame->getVideoTimestamp());

    m_pushVideoPacket.m_payload_type = YangRtspPacketPayloadTypeRaw;
    m_raw.m_payload = m_videoBuf;
    m_raw.m_size = videoFrame->getVideoLen();
    memcpy(m_raw.m_payload, videoFrame->getVideoData(), m_pushVideoPacket.m_nb);
    if ((err = encode(&m_pushVideoPacket)) != Yang_Ok) {
        return yang_error_wrap(err, "encode packet");
    }
    return err;
}
int32_t YangRtpPacketWrap::package_single_nalu(char *p, int32_t plen,
                                               int64_t timestamp) {

    int32_t err = Yang_Ok;
    m_pushVideoPacket.reset();
    m_pushVideoPacket.m_header.set_payload_type(kVideoPayloadType);
    m_pushVideoPacket.m_header.set_ssrc(m_videoSsrc);
    m_pushVideoPacket.m_frame_type = YangFrameTypeVideo;
    m_pushVideoPacket.m_header.set_sequence(m_videoSeq++);
    m_pushVideoPacket.m_header.set_timestamp(timestamp);


    m_pushVideoPacket.m_payload_type = YangRtspPacketPayloadTypeRaw;
    m_raw.m_payload = m_videoBuf;
    m_raw.m_size = plen;
    memcpy(m_raw.m_payload, p, plen);
    if ((err = encode(&m_pushVideoPacket)) != Yang_Ok) {
        return yang_error_wrap(err, "encode packet");
    }

    return err;
}
int32_t YangRtpPacketWrap::package_fu_a(YangStreamCapture *videoFrame,
                                        int32_t fu_payload_size) {
    int32_t err = Yang_Ok;
    int32_t plen = videoFrame->getVideoLen();
    uint8_t *pdata = videoFrame->getVideoData();
    char *p = (char*) pdata + 1;
    int32_t nb_left = plen - 1;
    uint8_t header = pdata[0];
    uint8_t nal_type = header & kNalTypeMask;

    int32_t num_of_packet = 1 + (plen - 1) / fu_payload_size;
    for (int32_t i = 0; i < num_of_packet; ++i) {
        int32_t packet_size = yang_min(nb_left, fu_payload_size);
        m_pushVideoPacket.reset();
        m_pushVideoPacket.m_header.set_payload_type(kVideoPayloadType);
        m_pushVideoPacket.m_header.set_ssrc(m_videoSsrc);
        m_pushVideoPacket.m_frame_type = YangFrameTypeVideo;
        m_pushVideoPacket.m_header.set_sequence(m_videoSeq++);
        m_pushVideoPacket.m_header.set_timestamp(videoFrame->getVideoTimestamp());
        if (i == num_of_packet - 1)
            m_pushVideoPacket.m_header.set_marker(true);

        m_pushVideoPacket.m_payload_type = YangRtspPacketPayloadTypeFUA2;

        m_fua2.m_nri = (YangAvcNaluType) header;
        m_fua2.m_nalu_type = (YangAvcNaluType) nal_type;
        m_fua2.m_start = bool(i == 0);
        m_fua2.m_end = bool(i == num_of_packet - 1);

        m_fua2.m_payload = m_videoBuf;
        m_fua2.m_size = packet_size;
        memcpy(m_fua2.m_payload, p, packet_size);

        p += packet_size;
        nb_left -= packet_size;

        if ((err = encode(&m_pushVideoPacket)) != Yang_Ok) {
            return yang_error_wrap(err, "encode packet");
        }

    }

    return err;
}
int32_t YangRtpPacketWrap::encode(YangRtpPacket *pkt) {
    int err = 0;
    m_buf.init(m_rtpBuffer->getBuffer(), kRtpPacketSize);

    if ((err = pkt->m_header.encode(&m_buf)) != Yang_Ok) {
        return yang_error_wrap(err, "rtp header(%d) encode packet fail",
                               pkt->m_payload_type);
    }
    if (pkt->m_payload_type == YangRtspPacketPayloadTypeRaw) {
        err = m_raw.encode(&m_buf);
    } else if (pkt->m_payload_type == YangRtspPacketPayloadTypeFUA2) {
        err = m_fua2.encode(&m_buf);
    } else if (pkt->m_payload_type == YangRtspPacketPayloadTypeSTAP) {
        err = m_stap.encode(&m_buf);
    }

    if (err != Yang_Ok) {
        return yang_error_wrap(err, "rtp payload(%d) encode packet fail",
                               pkt->m_payload_type);
    }
    if (pkt->m_header.get_padding() > 0) {
        uint8_t padding = pkt->m_header.get_padding();
        if (!m_buf.require(padding)) {
            return yang_error_wrap(ERROR_RTC_RTP_MUXER,
                                   "padding requires %d bytes", padding);
        }
        memset(m_buf.head(), padding, padding);
        m_buf.skip(padding);
    }

    if (m_session)
        return m_session->send_avpacket(pkt, &m_buf);
    return err;
}
int32_t YangRtpPacketWrap::package_stap_a(YangStreamCapture *videoFrame) {
    int err = Yang_Ok;
    uint8_t *buf = videoFrame->getVideoData();
    int32_t spsLen = *(buf + 12) + 1;
    uint8_t *sps = buf + 13;
    int32_t ppsLen = *(sps + spsLen + 1) + 1;
    uint8_t *pps = buf + 13 + spsLen + 2;
    m_pushVideoPacket.reset();
    m_pushVideoPacket.m_header.set_payload_type(kVideoPayloadType);
    m_pushVideoPacket.m_header.set_ssrc(m_videoSsrc);
    m_pushVideoPacket.m_frame_type = YangFrameTypeVideo;
    m_pushVideoPacket.m_nalu_type = (YangAvcNaluType) kStapA;
    m_pushVideoPacket.m_header.set_marker(false);
    m_pushVideoPacket.m_header.set_sequence(m_videoSeq++);
    m_pushVideoPacket.m_header.set_timestamp(0);


    m_pushVideoPacket.m_payload_type = YangRtspPacketPayloadTypeSTAP;
    m_stap.reset();
    uint8_t header = sps[0];

    m_stap.m_nri = (YangAvcNaluType) header;


    YangSample *sps_sample = new YangSample();
    sps_sample->m_bytes = (char*) sps;
    sps_sample->m_size = spsLen;
    m_stap.m_nalus.push_back(sps_sample);


    YangSample *pps_sample = new YangSample();
    pps_sample->m_bytes = (char*) pps;
    pps_sample->m_size = ppsLen;
    m_stap.m_nalus.push_back(pps_sample);


    if ((err = encode(&m_pushVideoPacket)) != Yang_Ok) {
        return yang_error_wrap(err, "encode packet");
    }
    return err;

}

