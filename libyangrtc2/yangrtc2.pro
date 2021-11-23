CONFIG -= qt

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += __STDC_FORMAT_MACROS
HOME_BASE=../
INCLUDEPATH += $$HOME_BASE/include
INCLUDEPATH += $$HOME_BASE/thirdparty/include
INCLUDEPATH += $$HOME_BASE/thirdparty/user_include
INCLUDEPATH += $$HOME_BASE/thirdparty/user_include/ffmpeg
INCLUDEPATH += $$HOME_BASE/libyangrtc2/src
unix{
  CONFIG(debug, debug|release) {
        DESTDIR += $$HOME_BASE/bin/lib_debug
    }else{

        DESTDIR += $$HOME_BASE/bin/lib_release
    }
}
win32{
    DEFINES += _AMD64_
    INCLUDEPATH += $$HOME_BASE\thirdparty\include\win
  CONFIG(debug, debug|release) {
        DESTDIR += $$HOME_BASE\bin\lib_win_debug
    }else{
        DESTDIR += $$HOME_BASE\bin\lib_win_release
    }

    msvc{
        QMAKE_CFLAGS += /utf-8
        QMAKE_CXXFLAGS += /utf-8
       # QMAKE_CXXFLAGS += /source-charset:utf-8 /execution-charset:utf-8
        DEFINES +=HAVE_STRUCT_TIMESPEC
        DEFINES +=WIN32_LEAN_AND_MEAN
        INCLUDEPATH += $$HOME_BASE\thirdparty\include\win\include
    }

}
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/yangaudiodev/YangAudioCapture.cpp \
    src/yangaudiodev/YangAudioCaptureData.cpp \
    src/yangaudiodev/YangAudioCaptureHandle.cpp \
    src/yangaudiodev/YangAudioPlay.cpp \
    src/yangaudiodev/YangAudioRenderData.cpp \
    src/yangaudiodev/linux/YangAlsaDeviceHandle.cpp \
    src/yangaudiodev/linux/YangAlsaHandle.cpp \
    src/yangaudiodev/linux/YangAudioCaptureImpl.cpp \
    src/yangaudiodev/linux/YangAudioPlayAlsa.cpp \
    src/yangaudiodev/win/YangRecAudioCaptureHandle.cpp \
    src/yangaudiodev/win/YangWinAudioApi.cpp \
    src/yangaudiodev/win/YangWinAudioApiAec.cpp \
    src/yangaudiodev/win/YangWinAudioApiCapture.cpp \
    src/yangaudiodev/win/YangWinAudioApiDevice.cpp \
    src/yangaudiodev/win/YangWinAudioApiRender.cpp \
    src/yangaudiodev/win/YangWinAudioCapture.cpp \
    src/yangaudiodev/win/YangWinAudioCaptureHandle.cpp \
    src/yangaudiodev/win/YangWinRecordAudioCapture.cpp \
    src/yangavutil/YangAecBase.cpp \
    src/yangavutil/YangAecSpeex.cpp \
    src/yangavutil/YangAudioFactory.cpp \
    src/yangavutil/YangAudioMix.cpp \
    src/yangavutil/YangAudioUtil.cpp \
    src/yangavutil/YangConvert.cpp \
    src/yangavutil/YangImageConvert.cpp \
    src/yangavutil/YangMakeWave.cpp \
    src/yangavutil/YangNalu.cpp \
    src/yangavutil/YangPicUtilFfmpeg.cpp \
    src/yangavutil/YangPreProcess.cpp \
    src/yangavutil/YangPreProcessSpeex.cpp \
    src/yangavutil/YangResample.cpp \
    src/yangavutil/YangSwResample.cpp \
    src/yangavutil/YangYuvConvert.cpp \
    src/yangavutil/YangYuvUtil.cpp \
    src/yangcapture/YangAudioDeviceQuery.cpp \
    src/yangcapture/YangCaptureFactory.cpp \
    src/yangcapture/YangDXGIManager.cpp \
    src/yangcapture/YangScreenCaptureImpl.cpp \
    src/yangcapture/YangScreenShare.cpp \
    src/yangcapture/YangVideoCapture.cpp \
    src/yangcapture/YangVideoCaptureHandle.cpp \
    src/yangcapture/YangVideoCaptureImpl.cpp \
    src/yangcapture/YangVideoDeviceQuery.cpp \
    src/yangcapture/win/YangVideoSrc.cpp \
    src/yangcapture/win/YangWinVideoCapture.cpp \
    src/yangcapture/win/YangWinVideoCaptureHandle.cpp \
    src/yangdecoder/YangAudioDecoder.cpp \
    src/yangdecoder/YangAudioDecoderAac.cpp \
    src/yangdecoder/YangAudioDecoderHandle.cpp \
    src/yangdecoder/YangAudioDecoderHandles.cpp \
    src/yangdecoder/YangAudioDecoderOpus.cpp \
    src/yangdecoder/YangAudioDecoderSpeex.cpp \
    src/yangdecoder/YangDecoderFactory.cpp \
    src/yangdecoder/YangH2645VideoDecoderFfmpeg.cpp \
    src/yangdecoder/YangH264DecoderSoft.cpp \
    src/yangdecoder/YangH264Header.cpp \
    src/yangdecoder/YangHeaderParseFfmpeg.cpp \
    src/yangdecoder/YangVideoDecoderHandle.cpp \
    src/yangdecoder/YangVideoDecoderHandles.cpp \
    src/yangdecoder/YangVideoDecoderIntel.cpp \
    src/yangencoder/YangAudioEncoder.cpp \
    src/yangencoder/YangAudioEncoderAac.cpp \
    src/yangencoder/YangAudioEncoderHandle.cpp \
    src/yangencoder/YangAudioEncoderHandleCb.cpp \
    src/yangencoder/YangAudioEncoderMeta.cpp \
    src/yangencoder/YangAudioEncoderMp3.cpp \
    src/yangencoder/YangAudioEncoderOpus.cpp \
    src/yangencoder/YangAudioEncoderSpeex.cpp \
    src/yangencoder/YangEncoderFactory.cpp \
    src/yangencoder/YangFfmpegEncoderMeta.cpp \
    src/yangencoder/YangH264EncHeader.cpp \
    src/yangencoder/YangH264EncoderIntel.cpp \
    src/yangencoder/YangH264EncoderMeta.cpp \
    src/yangencoder/YangH264EncoderSoft.cpp \
    src/yangencoder/YangH265EncoderMeta.cpp \
    src/yangencoder/YangH265EncoderSoft.cpp \
    src/yangencoder/YangVideoEncoder.cpp \
    src/yangencoder/YangVideoEncoderFfmpeg.cpp \
    src/yangencoder/YangVideoEncoderHandle.cpp \
    src/yangencoder/YangVideoEncoderMeta.cpp \
    src/yangplayer/YangPlayFactory.cpp \
    src/yangplayer/YangPlayReceive.cpp \
    src/yangplayer/YangPlayerBase.cpp \
    src/yangplayer/YangPlayerDecoder.cpp \
    src/yangplayer/YangPlayerHandleImpl.cpp \
    src/yangplayer/YangPlayerPlay.cpp \
    src/yangplayer/YangRtcReceive.cpp \
    src/yangpush/YangPushCapture.cpp \
    src/yangpush/YangPushEncoder.cpp \
    src/yangpush/YangPushFactory.cpp \
    src/yangpush/YangPushHandleImpl.cpp \
    src/yangpush/YangPushMessageHandle.cpp \
    src/yangpush/YangPushPublish.cpp \
    src/yangpush/YangRtcPublish.cpp \
    src/yangpush/YangSendVideoImpl.cpp \
    src/yangrecord/YangFlvWrite.cpp \
    src/yangrecord/YangMp4File.cpp \
    src/yangrecord/YangMp4FileApp.cpp \
    src/yangrecord/YangRecEncoder.cpp \
    src/yangrecord/YangRecord.cpp \
    src/yangrecord/YangRecordApp.cpp \
    src/yangrecord/YangRecordCapture.cpp \
    src/yangrecord/YangRecordMp4.cpp \
    src/yangrtmp/YangInvokeBuffer.cpp \
    src/yangrtmp/YangRtmpBase.cpp \
    src/yangrtp/YangAudioPayload.cpp \
    src/yangrtp/YangCodecPayload.cpp \
    src/yangrtp/YangMediaPayloadType.cpp \
    src/yangrtp/YangMixQueue.cpp \
    src/yangrtp/YangPublishNackBuffer.cpp \
    src/yangrtp/YangRedPayload.cpp \
    src/yangrtp/YangRtcpApp.cpp \
    src/yangrtp/YangRtcpCommon.cpp \
    src/yangrtp/YangRtcpCompound.cpp \
    src/yangrtp/YangRtcpNack.cpp \
    src/yangrtp/YangRtcpPli.cpp \
    src/yangrtp/YangRtcpPsfbCommon.cpp \
    src/yangrtp/YangRtcpRR.cpp \
    src/yangrtp/YangRtcpRpsi.cpp \
    src/yangrtp/YangRtcpSR.cpp \
    src/yangrtp/YangRtcpSli.cpp \
    src/yangrtp/YangRtcpTWCC.cpp \
    src/yangrtp/YangRtcpXr.cpp \
    src/yangrtp/YangRtp.cpp \
    src/yangrtp/YangRtpBuffer.cpp \
    src/yangrtp/YangRtpFUAPayload.cpp \
    src/yangrtp/YangRtpFUAPayload2.cpp \
    src/yangrtp/YangRtpHeader.cpp \
    src/yangrtp/YangRtpNackForReceiver.cpp \
    src/yangrtp/YangRtpPacket.cpp \
    src/yangrtp/YangRtpPlayNackBuffer.cpp \
    src/yangrtp/YangRtpRawNALUs.cpp \
    src/yangrtp/YangRtpRawPayload.cpp \
    src/yangrtp/YangRtpRingBuffer.cpp \
    src/yangrtp/YangRtpSTAPPayload.cpp \
    src/yangrtp/YangRtxPayloadDes.cpp \
    src/yangrtp/YangVideoPayload.cpp \
    src/yangsrt/YangSrtBase.cpp \
    src/yangsrt/YangTsBuffer.cpp \
    src/yangsrt/YangTsMuxer.cpp \
    src/yangsrt/YangTsPacket.cpp \
    src/yangsrt/YangTsdemux.cpp \
    src/yangsrt/common.cpp \
    src/yangsrt/crc.cpp \
    src/yangsrt/srt_data.cpp \
    src/yangstream/YangAudioStreamCapture.cpp \
    src/yangstream/YangStreamCapture.cpp \
    src/yangstream/YangStreamFactory.cpp \
    src/yangstream/YangStreamHandle.cpp \
    src/yangstream/YangStreamManager.cpp \
    src/yangstream/YangStreamPlay.cpp \
    src/yangstream/YangStreamRtc.cpp \
    src/yangstream/YangStreamRtmp.cpp \
    src/yangstream/YangStreamSrt.cpp \
    src/yangstream/YangSynBuffer.cpp \
    src/yangstream/YangVideoStreamCapture.cpp \
    src/yangutil/YangAmf.cpp \
    src/yangutil/YangAvinfo.cpp \
    src/yangutil/YangAvtype.cpp \
    src/yangutil/YangEndian.cpp \
    src/yangutil/YangFile.cpp \
    src/yangutil/YangHttp.cpp \
    src/yangutil/YangIniImpl.cpp \
    src/yangutil/YangJson.cpp \
    src/yangutil/YangLoadLib.cpp \
    src/yangutil/YangLog.cpp \
    src/yangutil/YangSRtp.cpp \
    src/yangutil/YangSocket.cpp \
    src/yangutil/YangSsl.cpp \
    src/yangutil/YangSsrc.cpp \
    src/yangutil/YangString.cpp \
    src/yangutil/YangSysMessageHandle.cpp \
    src/yangutil/YangThread.cpp \
    src/yangutil/YangTime.cpp \
    src/yangutil/YangTimer.cpp \
    src/yangutil/YangUrl.cpp \
    src/yangutil/YangWebsocket.cpp \
    src/yangutil/YangWebsocketImpl.cpp \
    src/yangutil/YangWindowsMouse.cpp \
    src/yangutil/buffer/YangAudioBuffer.cpp \
    src/yangutil/buffer/YangAudioEncoderBuffer.cpp \
    src/yangutil/buffer/YangAudioPlayBuffer.cpp \
    src/yangutil/buffer/YangBuffer.cpp \
    src/yangutil/buffer/YangMediaBuffer.cpp \
    src/yangutil/buffer/YangVideoBuffer.cpp \
    src/yangutil/buffer/YangVideoDecoderBuffer.cpp \
    src/yangutil/buffer/YangVideoEncoderBuffer.cpp \
    src/yangwebrtc/YangMediaDesc.cpp \
    src/yangwebrtc/YangRecvTrack.cpp \
    src/yangwebrtc/YangRtcAudioRecvTrack.cpp \
    src/yangwebrtc/YangRtcContext.cpp \
    src/yangwebrtc/YangRtcDtls.cpp \
    src/yangwebrtc/YangRtcPlayStream.cpp \
    src/yangwebrtc/YangRtcPublishStream.cpp \
    src/yangwebrtc/YangRtcSdp.cpp \
    src/yangwebrtc/YangRtcSession.cpp \
    src/yangwebrtc/YangRtcStun.cpp \
    src/yangwebrtc/YangRtcVideoRecvTrack.cpp \
    src/yangwebrtc/YangRtpPacketWrap.cpp \
    src/yangwebrtc/YangSSRCInfo.cpp \
    src/yangwebrtc/YangSrsRtcHandleImpl.cpp \
    src/yangwebrtc/YangSrsSdp.cpp \
    src/yangwebrtc/YangUdpHandle.cpp


HEADERS += \
    src/yangaudiodev/YangAudioCaptureHandle.h \
    src/yangaudiodev/YangCaptureCallback.h \
    src/yangaudiodev/linux/YangAlsaDeviceHandle.h \
    src/yangaudiodev/linux/YangAlsaHandle.h \
    src/yangaudiodev/linux/YangAudioCaptureImpl.h \
    src/yangaudiodev/linux/YangAudioPlayAlsa.h \
    src/yangaudiodev/win/YangAudioApiCapture.h \
    src/yangaudiodev/win/YangRecAudioCaptureHandle.h \
    src/yangaudiodev/win/YangWinAudioApi.h \
    src/yangaudiodev/win/YangWinAudioApiAec.h \
    src/yangaudiodev/win/YangWinAudioApiCapture.h \
    src/yangaudiodev/win/YangWinAudioApiDevice.h \
    src/yangaudiodev/win/YangWinAudioApiRender.h \
    src/yangaudiodev/win/YangWinAudioCapture.h \
    src/yangaudiodev/win/YangWinAudioCaptureHandle.h \
    src/yangaudiodev/win/YangWinAudioDevice.h \
    src/yangaudiodev/win/YangWinRecordAudioCapture.h \
    src/yangavutil/YangAecSpeex.h \
    src/yangavutil/YangImageConvert.h \
    src/yangavutil/YangPreProcessSpeex.h \
    src/yangcapture/YangDXGIManager.h \
    src/yangcapture/YangScreenCaptureImpl.h \
    src/yangcapture/YangScreenShare.h \
    src/yangcapture/YangVideoCaptureHandle.h \
    src/yangcapture/YangVideoCaptureImpl.h \
    src/yangcapture/YangVideoDeviceQuery.h \
    src/yangcapture/win/YangVideoSrc.h \
    src/yangcapture/win/YangWinVideoCapture.h \
    src/yangcapture/win/YangWinVideoCaptureHandle.h \
    src/yangdecoder/YangAudioDecoderAac.h \
    src/yangdecoder/YangAudioDecoderOpus.h \
    src/yangdecoder/YangAudioDecoderSpeex.h \
    src/yangdecoder/YangH2645VideoDecoderFfmpeg.h \
    src/yangdecoder/YangH264Dec.h \
    src/yangdecoder/YangH264DecoderSoft.h \
    src/yangdecoder/YangH264DecoderSoftFactory.h \
    src/yangdecoder/YangH264Header.h \
    src/yangdecoder/YangH264Header1.h \
    src/yangdecoder/YangHeaderParseFfmpeg.h \
    src/yangdecoder/YangVideoDecoderIntel.h \
    src/yangencoder/YangAudioEncoderAac.h \
    src/yangencoder/YangAudioEncoderMp3.h \
    src/yangencoder/YangAudioEncoderOpus.h \
    src/yangencoder/YangAudioEncoderSpeex.h \
    src/yangencoder/YangFfmpegEncoderMeta.h \
    src/yangencoder/YangH264EncHeader.h \
    src/yangencoder/YangH264EncoderIntel.h \
    src/yangencoder/YangH264EncoderIntel1.h \
    src/yangencoder/YangH264EncoderMeta.h \
    src/yangencoder/YangH264EncoderSoft.h \
    src/yangencoder/YangH265EncoderMeta.h \
    src/yangencoder/YangH265EncoderSoft.h \
    src/yangencoder/YangVideoEncoderFfmpeg.h \
    src/yangencoder/lame.h \
    src/yangplayer/YangPlayerHandleImpl.h \
    src/yangplayer/YangRtcReceive.h \
    src/yangpush/YangPushEncoder.h \
    src/yangpush/YangPushHandleImpl.h \
    src/yangpush/YangPushMessageHandle.h \
    src/yangpush/YangSendVideoImpl.h \
    src/yangrtmp/YangRtmpBase1.h \
    src/yangrtp/YangAudioPayload.h \
    src/yangrtp/YangCodecPayload.h \
    src/yangrtp/YangMediaPayloadType.h \
    src/yangrtp/YangMixQueue.h \
    src/yangrtp/YangPublishNackBuffer.h \
    src/yangrtp/YangRedPayload.h \
    src/yangrtp/YangRtcp.h \
    src/yangrtp/YangRtcpApp.h \
    src/yangrtp/YangRtcpCommon.h \
    src/yangrtp/YangRtcpCompound.h \
    src/yangrtp/YangRtcpNack.h \
    src/yangrtp/YangRtcpPli.h \
    src/yangrtp/YangRtcpPsfbCommon.h \
    src/yangrtp/YangRtcpRR.h \
    src/yangrtp/YangRtcpRpsi.h \
    src/yangrtp/YangRtcpSR.h \
    src/yangrtp/YangRtcpSli.h \
    src/yangrtp/YangRtcpTWCC.h \
    src/yangrtp/YangRtcpXr.h \
    src/yangrtp/YangRtpBuffer.h \
    src/yangrtp/YangRtpFUAPayload.h \
    src/yangrtp/YangRtpFUAPayload2.h \
    src/yangrtp/YangRtpHeader.h \
    src/yangrtp/YangRtpNackForReceiver.h \
    src/yangrtp/YangRtpPacket.h \
    src/yangrtp/YangRtpPlayNackBuffer.h \
    src/yangrtp/YangRtpRawNALUs.h \
    src/yangrtp/YangRtpRawPayload.h \
    src/yangrtp/YangRtpRingBuffer.h \
    src/yangrtp/YangRtpSTAPPayload.h \
    src/yangrtp/YangRtxPayloadDes.h \
    src/yangrtp/YangVideoPayload.h \
    src/yangstream/YangStreamPlay.h \
    src/yangstream/YangStreamRtc.h \
    src/yangstream/YangStreamRtmp.h \
    src/yangstream/YangStreamSrt.h \
    src/yangutil/YangWebsocketImpl.h \
    src/yangwebrtc/YangLocalSdp.h \
    src/yangwebrtc/YangMediaDesc.h \
    src/yangwebrtc/YangRecvTrack.h \
    src/yangwebrtc/YangRtcAudioRecvTrack.h \
    src/yangwebrtc/YangRtcContext.h \
    src/yangwebrtc/YangRtcDtls.h \
    src/yangwebrtc/YangRtcPlayStream.h \
    src/yangwebrtc/YangRtcPublishStream.h \
    src/yangwebrtc/YangRtcSdp.h \
    src/yangwebrtc/YangRtcSession.h \
    src/yangwebrtc/YangRtcSessionI.h \
    src/yangwebrtc/YangRtcStun.h \
    src/yangwebrtc/YangRtcVideoRecvTrack.h \
    src/yangwebrtc/YangRtpPacketWrap.h \
    src/yangwebrtc/YangSSRCInfo.h \
    src/yangwebrtc/YangSrsRtcHandleImpl.h \
    src/yangwebrtc/YangUdpHandle.h

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
