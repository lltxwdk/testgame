#ifndef __SR_MEDIA_COMMON_H__
#define __SR_MEDIA_COMMON_H__
#include <assert.h>
#include "TypeDef.h"

typedef enum tageSRVideoFrameType
{
    SRIFrame = 0,     // key frame
    SRPFrame,         // P frame
    SRBFrame,         // B frame
    SRSFrame          // S info
}SRVideoFrameType;

enum emVideoCodec
{
    SR_H264,
    SR_H264_SVC,
	VIDEOCODEC_TOTAL
};

struct SRVideoH264
{
    uint8_t qlevel_;
    uint8_t Fi_;
};

struct SRVideoH264Svc
{
    uint8_t qlevel_;
    uint8_t DID_;
    uint8_t TID_;
    bool I_;
    
    uint8_t TT_;
    bool N_;
    uint8_t PRI_;
    uint8_t QID_;
    bool U_;
    uint8_t FR_; // 原有的6位 reserve, 低3位改为帧率: 0:NA, 1:8, 2:16, 3:24, 4:30, 5:60, 6:NA, 7:NA
	uint8_t DT_;

    uint16_t sub_seq[4];
};

union SRVideoUnion
{
    SRVideoH264 video_h264;
    SRVideoH264Svc video_h264_svc;
};

struct SRMediaVideo
{
    uint8_t video_codec_;
    SRVideoUnion  video_union;
};

enum emSRAudioTransType
{
    SRATT_Broadcast, //YANCH: SR终端发送的音频流, 其TT字段为SRATT_Broadcast(0)
    SRATT_Unicast, //YANCH: MP混音后, 分别为各讲话者(参与混音)生成的混音流(不包含该讲话者的声音), TT字段设置为单播. (CC为1, 表示只包含一个CCID, 即该讲话者的通道id)
    SRATT_Multicast //YANCH: MP混音后, 所有讲话者声音的总混音流, 发给所有只听不讲(不参与混音)的成员, TT字段设置为组播. (CC代表参与混音的成员数, CCIDs包含各讲话成员的通道id, 该混音流应转发给除这些讲话成员外的其他成员)
};

//copy from MCUAudioMixer.h
enum SRAudioCodecType
{
    G711A,		// 8K sampling rate
    G711U,		// 8K
    G722_64,	// 16K
    G7221_24,	// 16K
    G7221_32,	//16K
    G7221C_48,	// 32K
    G729A,		// 8K
    ILBC20,		// 8K
    ILBC30,		// 8K
    ISAC_16,
    ISAC_32,
    OPUS_16,//16k
    OPUS_32,//32k
    OPUS_48,//48k
    G7231,//wyz,2.14.7.30
	RAW_DATA,
    AUDIOCODEC_TOTAL
};

enum SRAudioFrequency
{
	kNbInHz = 8000,
	kWbInHz = 16000,
	kSwbInHz = 32000,
	kFbInHz = 48000,   //~~~wyz,2014.6.20
	kLowestPossible = -1,
	//modify code for sampling rate by ldq
	//kDefaultFrequency = 32000  //~~~~~wyz,2014.6.19
	kDefaultFrequency = 48000
};
enum SRAudioChannelNum
{
	MonoChannel = 1,
	StereoChannel = 2
};

enum SRVideoCodecType
{
	Video_H261,
	Video_H263,
	Video_H263_1998,
	Video_H264,
	Video_H264_HP,
	Video_VP8,
	Video_H264_SVC,
	Video_MAX_VIDEOCODEC_TYPE,
	Video_Unknown  // 代表视频格式未设置, 或未知的视频格式.
};

struct SRMediaAudio
{
    uint8_t audio_codec_;
    uint8_t TT_;
    uint8_t CC_;
    uint8_t LL_;
    uint16_t* CCIDs_;
    uint8_t CCBytes(){ return CC_ * sizeof(uint16_t); }
};

enum emSRMT
{
    SRMT_AUDIO,
    SRMT_VIDEO,
    SRMT_DATA,
    SRMT_OTHER
};

union SRMediaUnion
{
    SRMediaAudio audio_info_;
    SRMediaVideo video_info_;
};

struct SRMediaInfo
{
    uint8_t MT_;
    bool F_;
    uint16_t channel_id_;
    uint16_t src_id_;

    SRMediaUnion media_;
    SRVideoH264Svc& GetSvcInfo()
    {
        //assert(SRMT_VIDEO == MT_||SRMT_DATA == MT_);
        assert(SR_H264_SVC == media_.video_info_.video_codec_);
        return media_.video_info_.video_union.video_h264_svc;
    }
    SRMediaAudio& GetAudioInfo()
    {
        assert(SRMT_AUDIO == MT_);
        return media_.audio_info_;
    }
};

#endif
