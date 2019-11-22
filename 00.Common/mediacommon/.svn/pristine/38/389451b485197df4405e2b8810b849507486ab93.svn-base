#include "gtest/gtest.h"
#include <stdarg.h>
#include "SRMediaHeadV2.h"

class SRMediaHeadV2 : public testing::Test
{
protected:
};

//信号的打包、解析测试
TEST_F(SRMediaHeadV2, Signal)
{
#if 1
    //打包
    uint32_t send_len = 0;
    SRPacket pack;

    uint8_t pack_bufer[1500] = { 0 };
    uint8_t data[100];
    memset(data, 0x08, sizeof(data));
    uint16_t out_len = 0;

    //base head
    pack.base_head_.E_ = false;
    pack.base_head_.R_ = false;
    pack.base_head_.X_ = false;

    pack.packet_.signal_pack_.PT_ = 115;
    //src, dst id
    pack.packet_.signal_pack_.signal_len_ = 100;
    pack.packet_.signal_pack_.signal_body_ = data;

    EXPECT_EQ(pack.Pack(pack_bufer, 1500, out_len), SR_OK);
    EXPECT_EQ(out_len, (6 + pack.packet_.signal_pack_.signal_len_));

    //解析
    SRPacket parse;
    uint8_t recv_data[1500] = { 0 };
    uint32_t recv_len = 0;
    EXPECT_EQ(parse.Parse(pack_bufer, out_len), SR_OK);


    EXPECT_EQ(parse.packet_.signal_pack_.PT_, 115);
    EXPECT_EQ(parse.base_head_.E_, false);
    EXPECT_EQ(parse.base_head_.R_, false);
    EXPECT_EQ(parse.base_head_.X_, false);
    EXPECT_EQ(parse.packet_.signal_pack_.signal_len_, 100);
    EXPECT_EQ(memcmp(parse.packet_.signal_pack_.signal_body_, data, parse.packet_.signal_pack_.signal_len_), 0);
#endif
}

//信号加密的打包、解析测试
TEST_F(SRMediaHeadV2, Signal_E)
{
#if 1
    //打包
    uint32_t send_len = 0;
    SRPacket pack;

    uint8_t pack_bufer[1500] = { 0 };
    uint8_t data[100];
    memset(data, 0x08, sizeof(data));
    uint16_t out_len = 0;

    //base head
    pack.base_head_.E_ = true;
    pack.base_head_.R_ = false;
    pack.base_head_.X_ = false;

    pack.packet_.signal_pack_.PT_ = 115;
    //src, dst id
    pack.packet_.signal_pack_.signal_len_ = 100;
    pack.packet_.signal_pack_.signal_body_ = data;

    EXPECT_EQ(pack.Pack(pack_bufer, 1500, out_len), SR_OK);
    EXPECT_EQ(out_len, (6 + pack.packet_.signal_pack_.signal_len_));

    //解析
    SRPacket parse;
    uint8_t recv_data[1500] = { 0 };
    uint32_t recv_len = 0;
    EXPECT_EQ(parse.Parse(pack_bufer, out_len), SR_OK);


    EXPECT_EQ(parse.packet_.signal_pack_.PT_, 115);
    EXPECT_EQ(parse.base_head_.E_, true);
    EXPECT_EQ(parse.base_head_.R_, false);
    EXPECT_EQ(parse.base_head_.X_, false);
    EXPECT_EQ(parse.packet_.signal_pack_.signal_len_, 100);
    EXPECT_EQ(memcmp(parse.packet_.signal_pack_.signal_body_, data, parse.packet_.signal_pack_.signal_len_), 0);
#endif
}

//视频H264的打包、解析测试
TEST_F(SRMediaHeadV2, VIDEO_1)
{
#if 1
    //打包
    uint32_t send_len = 0;
    SRPacket pack;

    uint8_t pack_bufer[1500] = { 0 };
    uint8_t data[100];
    memset(data, 0x08, sizeof(data));
    uint16_t out_len = 0;

    //base head
    pack.base_head_.E_ = true;
    pack.base_head_.R_ = true;
    pack.base_head_.X_ = false;

    SRMedia &media_pack = pack.packet_.media_pack_;
    SRMediaInfo &media_info = pack.packet_.media_pack_.media_info_;

    media_info.channel_id_ = 100;
    media_info.F_ = true;
    media_info.MT_ = SRMT_VIDEO;
    media_info.media_.video_info_.video_codec_ = SR_H264;
    media_info.media_.video_info_.video_union.video_h264.Fi_ = 1;
    media_info.media_.video_info_.video_union.video_h264.qlevel_ = 1;
    media_info.src_id_ = 10;
    media_pack.rtp_pack_ = data;
    media_pack.rtp_len_ = 100;

    EXPECT_EQ(pack.Pack(pack_bufer, 1500, out_len), SR_OK);
    EXPECT_EQ(out_len, (6 + media_pack.rtp_len_));

    //解析
    SRPacket parse;
    uint8_t recv_data[1500] = { 0 };
    uint32_t recv_len = 0;
    EXPECT_EQ(parse.Parse(pack_bufer, out_len), SR_OK);

    EXPECT_EQ(parse.base_head_.E_, true);
    EXPECT_EQ(parse.base_head_.R_, true);
    EXPECT_EQ(parse.base_head_.X_, false);

    EXPECT_EQ(parse.packet_.media_pack_.media_info_.channel_id_, 100);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.F_, true);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.MT_, SRMT_VIDEO);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.media_.video_info_.video_codec_, SR_H264);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.media_.video_info_.video_union.video_h264.Fi_, 1);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.media_.video_info_.video_union.video_h264.qlevel_, 1);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.src_id_, 10);
    EXPECT_EQ(parse.packet_.media_pack_.rtp_len_, 100);
    EXPECT_EQ(memcmp(parse.packet_.media_pack_.rtp_pack_, data, parse.packet_.media_pack_.rtp_len_), 0);
#endif
}

//视频H264 (带扩展字段) 的打包、解析测试
TEST_F(SRMediaHeadV2, VIDEO_X)
{
#if 1
    //打包
    uint32_t send_len = 0;
    SRPacket pack;

    uint8_t pack_bufer[1500] = { 0 };
    uint8_t data[100];
    memset(data, 0x08, sizeof(data));
    uint16_t out_len = 0;

    //base head
    pack.base_head_.E_ = true;
    pack.base_head_.R_ = true;
    pack.base_head_.X_ = true;
    pack.base_head_.extend_.has_data_ = true;
    pack.base_head_.extend_.extend_info_.x_data_.x_len_ = 2; //扩展数据长度(字节)
    pack.base_head_.extend_.extend_info_.x_data_.x_reserve_ = 0;
    uint16_t bitrate = 1000;
    pack.base_head_.extend_.extend_info_.x_data_.x_data_ = (uint8_t*)&bitrate; // 采用小端模式

    SRMedia &media_pack = pack.packet_.media_pack_;
    SRMediaInfo &media_info = pack.packet_.media_pack_.media_info_;

    media_info.channel_id_ = 100;
    media_info.F_ = true;
    media_info.MT_ = SRMT_VIDEO;
    media_info.media_.video_info_.video_codec_ = SR_H264;
    media_info.media_.video_info_.video_union.video_h264.Fi_ = 1;
    media_info.media_.video_info_.video_union.video_h264.qlevel_ = 1;
    media_info.src_id_ = 10;
    media_pack.rtp_pack_ = data;
    media_pack.rtp_len_ = 100;

    EXPECT_EQ(pack.Pack(pack_bufer, 1500, out_len), SR_OK);
    EXPECT_EQ(out_len, (6 + media_pack.rtp_len_ + 2/*xheader*/ + 2/*xlen*/));

    //解析
    SRPacket parse;
    uint8_t recv_data[1500] = { 0 };
    uint32_t recv_len = 0;
    EXPECT_EQ(parse.Parse(pack_bufer, out_len), SR_OK);

    EXPECT_EQ(parse.base_head_.E_, true);
    EXPECT_EQ(parse.base_head_.R_, true);
    EXPECT_EQ(parse.base_head_.X_, true);
    EXPECT_EQ(parse.base_head_.extend_.has_data_, true);
    EXPECT_EQ(parse.base_head_.extend_.extend_info_.x_data_.x_len_, 2);
    EXPECT_NE(parse.base_head_.extend_.extend_info_.x_data_.x_data_, (uint8_t*)NULL);
    uint8_t* x_data = parse.base_head_.extend_.extend_info_.x_data_.x_data_;
    uint16_t bitrate1 = (x_data[1] << 8) + x_data[0]; // 采用小端模式
    EXPECT_EQ(bitrate1, bitrate);

    EXPECT_EQ(parse.packet_.media_pack_.media_info_.channel_id_, 100);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.F_, true);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.MT_, SRMT_VIDEO);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.media_.video_info_.video_codec_, SR_H264);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.media_.video_info_.video_union.video_h264.Fi_, 1);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.media_.video_info_.video_union.video_h264.qlevel_, 1);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.src_id_, 10);
    EXPECT_EQ(parse.packet_.media_pack_.rtp_len_, 100);
    EXPECT_EQ(memcmp(parse.packet_.media_pack_.rtp_pack_, data, parse.packet_.media_pack_.rtp_len_), 0);
#endif
}

//视频H264 SVC 的打包、解析测试
TEST_F(SRMediaHeadV2, VIDEO_H264_SVC_1)
{
#if 1
    //打包
    uint32_t send_len = 0;
    SRPacket pack;

    uint8_t pack_bufer[1500] = { 0 };
    uint8_t data[92];
    memset(data, 0x08, sizeof(data));
    uint16_t out_len = 0;

    //base head
    pack.base_head_.E_ = false;
    pack.base_head_.R_ = true;
    pack.base_head_.X_ = false;

    SRMedia &media_pack = pack.packet_.media_pack_;
    SRMediaInfo &media_info = pack.packet_.media_pack_.media_info_;

    media_info.channel_id_ = 3;
    media_info.F_ = false;
    media_info.MT_ = 1;
    media_info.media_.video_info_.video_codec_ = 1;
    media_info.media_.video_info_.video_union.video_h264_svc.DID_   = 0;
    media_info.media_.video_info_.video_union.video_h264_svc.TID_   = 2;
    media_info.media_.video_info_.video_union.video_h264_svc.I_     = true;
    media_info.media_.video_info_.video_union.video_h264_svc.TT_    = 2;
    media_info.media_.video_info_.video_union.video_h264_svc.N_     = false;
    media_info.media_.video_info_.video_union.video_h264_svc.PRI_   = 0;
    media_info.media_.video_info_.video_union.video_h264_svc.QID_   = 0;
    media_info.media_.video_info_.video_union.video_h264_svc.U_     = false;
    media_info.media_.video_info_.video_union.video_h264_svc.FR_ = 5;
	media_info.media_.video_info_.video_union.video_h264_svc.DT_ = 3;
    media_info.media_.video_info_.video_union.video_h264_svc.sub_seq[0] = 1;
    media_info.media_.video_info_.video_union.video_h264_svc.sub_seq[1] = 2;
    media_info.media_.video_info_.video_union.video_h264_svc.sub_seq[2] = 3;
    media_info.src_id_ = 3;
    media_pack.rtp_pack_ = data;
    media_pack.rtp_len_ = sizeof(data);

    EXPECT_EQ(pack.Pack(pack_bufer, 1500, out_len), SR_OK);
    int sub_num = media_info.media_.video_info_.video_union.video_h264_svc.TT_ - media_info.media_.video_info_.video_union.video_h264_svc.TID_ + 1;
    EXPECT_EQ(out_len, (8 + media_pack.rtp_len_ + 2 * sub_num));

    //解析
    SRPacket parse;
    uint8_t recv_data[1500] = { 0 };
    uint32_t recv_len = 0;
    EXPECT_EQ(parse.Parse(pack_bufer, out_len), SR_OK);

    EXPECT_EQ(parse.base_head_.E_, false);
    EXPECT_EQ(parse.base_head_.R_, true);
    EXPECT_EQ(parse.base_head_.X_, false);


    const SRVideoH264Svc &video_h264_svc = parse.packet_.media_pack_.media_info_.media_.video_info_.video_union.video_h264_svc;

    EXPECT_EQ(parse.packet_.media_pack_.media_info_.channel_id_, 3);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.F_, false);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.MT_, 1);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.media_.video_info_.video_codec_, 1);
    EXPECT_EQ(video_h264_svc.DID_, 0);
    EXPECT_EQ(video_h264_svc.TID_, 2);
    EXPECT_TRUE(video_h264_svc.I_);
    EXPECT_EQ(video_h264_svc.TT_, 2);
    EXPECT_FALSE(video_h264_svc.N_);
    EXPECT_EQ(video_h264_svc.PRI_, 0);
    EXPECT_EQ(video_h264_svc.QID_, 0);
	EXPECT_FALSE(video_h264_svc.U_);
    EXPECT_EQ(video_h264_svc.FR_, 5);
	EXPECT_EQ(video_h264_svc.DT_, 3);
    EXPECT_EQ(video_h264_svc.sub_seq[0], 1);
    EXPECT_EQ(video_h264_svc.sub_seq[1], 0);
    EXPECT_EQ(video_h264_svc.sub_seq[2], 0);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.src_id_, 3);
    EXPECT_EQ(parse.packet_.media_pack_.rtp_len_, sizeof(data));
    EXPECT_EQ(memcmp(parse.packet_.media_pack_.rtp_pack_, data, parse.packet_.media_pack_.rtp_len_), 0);
#endif
}

//视频H264 SVC 的解包测试
TEST_F(SRMediaHeadV2, VIDEO_H264_SVC_2)
{
#if 1
    //uint32_t send_len = 0;
    //SRPacket pack;

    //char pack_bufer[1500] = { 0xd2, 0x10, 0x8a, 0x02, 0x5c, 0x08, 0xc0, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x90, 0x79, 0x3a, 0xd2, 0x1b, 0x10, 0x3c, 0xc5, 0x10, 0x55, 0x39, 0xd8, 0xbe, 0xde, 0x00, 0x01, 0x22, 0x00, 0x0a, 0x32, 0x67, 0x4d, 0x70, 0x0b, 0x8c, 0x8d, 0x71, 0x46, 0xbc, 0x90, 0x0f, 0x08, 0x84, 0x6e, 0x00, 0x00, 0x00, 0x01, 0x6f, 0x56, 0x00, 0x0c, 0xac, 0x19, 0x1a, 0xe1, 0x41, 0x97, 0x9d, 0x0a, 0x40, 0x00, 0x00, 0x00, 0x01, 0x6f, 0x56, 0x00, 0x16, 0x4b, 0x06, 0x46, 0xb8, 0x28, 0x0b, 0xde, 0x54, 0x29, 0x00, 0x00, 0x00, 0x01, 0x68, 0xee, 0x3c, 0x80, 0x00, 0x00, 0x00, 0x01, 0x68, 0x5b, 0x8f, 0x20, 0x00, 0x00, 0x00, 0x01, 0x68, 0x6a, 0xe3, 0xc8 };
    //uint16_t out_len = 105;

    ////解析
    //SRPacket parse;
    //uint8_t recv_data[1500] = { 0 };
    //uint32_t recv_len = 0;
    //EXPECT_EQ(parse.Parse((uint8_t*)pack_bufer, out_len), SR_OK);

    //EXPECT_EQ(parse.base_head_.E_, false);
    //EXPECT_EQ(parse.base_head_.R_, true);
    //EXPECT_EQ(parse.base_head_.X_, false);


    //const SRVideoH264Svc &video_h264_svc = parse.packet_.media_pack_.media_info_.media_.video_info_.video_union.video_h264_svc;

    //EXPECT_EQ(parse.packet_.media_pack_.media_info_.channel_id_, 100);
    //EXPECT_EQ(parse.packet_.media_pack_.media_info_.F_, true);
    //EXPECT_EQ(parse.packet_.media_pack_.media_info_.MT_, SRMT_VIDEO);
    //EXPECT_EQ(parse.packet_.media_pack_.media_info_.media_.video_info_.video_codec_, SR_H264_SVC);
    //EXPECT_EQ(video_h264_svc.DID_, 1);
    //EXPECT_EQ(video_h264_svc.TID_, 2);
    //EXPECT_TRUE(video_h264_svc.I_);
    //EXPECT_EQ(video_h264_svc.TT_, 3);
    //EXPECT_FALSE(video_h264_svc.N_);
    //EXPECT_EQ(video_h264_svc.PRI_, 2);
    //EXPECT_EQ(video_h264_svc.QID_, 1);
    //EXPECT_TRUE(video_h264_svc.U_);
    //EXPECT_EQ(video_h264_svc.sub_seq[0], 0);
    //EXPECT_EQ(video_h264_svc.sub_seq[1], 1);
    //EXPECT_EQ(video_h264_svc.sub_seq[2], 2);
    //EXPECT_EQ(parse.packet_.media_pack_.media_info_.src_id_, 10);
    //EXPECT_EQ(pack.packet_.media_pack_.rtp_len_, 100);
    //EXPECT_EQ(pack.packet_.media_pack_.rtp_pack_[0], 0x08);
    //EXPECT_EQ(pack.packet_.media_pack_.rtp_pack_[99], 0x08);
#endif
}

//共享的打包、解析测试
TEST_F(SRMediaHeadV2, VIDEO_2)
{
#if 1
    //打包
    uint32_t send_len = 0;
    SRPacket pack;

    uint8_t pack_bufer[1500] = { 0 };
    uint8_t data[100];
    memset(data, 0x08, sizeof(data));
    uint16_t out_len = 0;

    //base head
    pack.base_head_.E_ = true;
    pack.base_head_.R_ = true;
    pack.base_head_.X_ = false;

    pack.packet_.media_pack_.media_info_.channel_id_ = 1;
    pack.packet_.media_pack_.media_info_.F_ = true;
    pack.packet_.media_pack_.media_info_.MT_ = SRMT_DATA;
    pack.packet_.media_pack_.media_info_.media_.video_info_.video_union.video_h264.Fi_ = SRSFrame;
    pack.packet_.media_pack_.media_info_.media_.video_info_.video_union.video_h264.qlevel_ = 3;
    pack.packet_.media_pack_.media_info_.media_.video_info_.video_codec_ = SR_H264;
    pack.packet_.media_pack_.media_info_.src_id_ = 5;
    pack.packet_.media_pack_.rtp_pack_ = data;
    pack.packet_.media_pack_.rtp_len_ = 6;

    EXPECT_EQ(pack.Pack(pack_bufer, 1500, out_len), SR_OK);
    EXPECT_EQ(out_len, (6 + pack.packet_.media_pack_.rtp_len_));

    //解析
    SRPacket parse;
    uint8_t recv_data[1500] = { 0 };
    uint32_t recv_len = 0;
    EXPECT_EQ(parse.Parse(pack_bufer, out_len), SR_OK);

    EXPECT_EQ(parse.base_head_.E_, true);
    EXPECT_EQ(parse.base_head_.R_, true);
    EXPECT_EQ(parse.base_head_.X_, false);


    EXPECT_EQ(parse.packet_.media_pack_.media_info_.channel_id_, 1);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.F_, true);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.MT_, SRMT_DATA);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.media_.video_info_.video_union.video_h264.Fi_, SRSFrame);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.media_.video_info_.video_union.video_h264.qlevel_, 3);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.media_.video_info_.video_codec_, SR_H264);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.src_id_, 5);
    EXPECT_EQ(parse.packet_.media_pack_.rtp_len_, 6);
    EXPECT_EQ(memcmp(parse.packet_.media_pack_.rtp_pack_, data, parse.packet_.media_pack_.rtp_len_), 0);
#endif
}

//音频的打包、解析测试
TEST_F(SRMediaHeadV2, AUDIO_1)
{
#if 1
    //打包
    uint32_t send_len = 0;
    SRPacket pack;

    uint8_t pack_bufer[1500] = { 0 };
    uint8_t data[100];
    memset(data, 0x08, sizeof(data));
    uint16_t out_len = 0;

    //base head
    pack.base_head_.E_ = true;
    pack.base_head_.R_ = true;
    pack.base_head_.X_ = false;

    pack.packet_.media_pack_.media_info_.channel_id_ = 1;
    pack.packet_.media_pack_.media_info_.F_ = false;
    pack.packet_.media_pack_.media_info_.MT_ = SRMT_AUDIO;

    uint16_t ccid[3];
    ccid[0] = 0;
    ccid[1] = 1;
    ccid[2] = 2;
    pack.packet_.media_pack_.media_info_.media_.audio_info_.audio_codec_ = 2;
    pack.packet_.media_pack_.media_info_.media_.audio_info_.CC_ = 3;
    pack.packet_.media_pack_.media_info_.media_.audio_info_.CCIDs_ = ccid;
    pack.packet_.media_pack_.media_info_.media_.audio_info_.TT_ = 3;
    pack.packet_.media_pack_.media_info_.media_.audio_info_.LL_ = 1;
    pack.packet_.media_pack_.media_info_.src_id_ = 5;
    pack.packet_.media_pack_.rtp_pack_ = data;
    pack.packet_.media_pack_.rtp_len_ = 6;

    EXPECT_EQ(pack.Pack(pack_bufer, 1500, out_len), SR_OK);
    EXPECT_EQ(out_len, (6 + 2 * pack.packet_.media_pack_.media_info_.media_.audio_info_.CC_ + pack.packet_.media_pack_.rtp_len_));

    //解析
    SRPacket parse;
    uint8_t recv_data[1500] = { 0 };
    uint32_t recv_len = 0;
    EXPECT_EQ(parse.Parse(pack_bufer, out_len), SR_OK);

    EXPECT_EQ(parse.base_head_.E_, true);
    EXPECT_EQ(parse.base_head_.R_, true);
    EXPECT_EQ(parse.base_head_.X_, false);

    EXPECT_EQ(parse.packet_.media_pack_.media_info_.channel_id_, 1);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.F_, false);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.MT_, SRMT_AUDIO);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.media_.audio_info_.audio_codec_, 2);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.media_.audio_info_.TT_, 3);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.media_.audio_info_.LL_, 1);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.media_.audio_info_.CC_, 3);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.media_.audio_info_.CCIDs_[0], 0);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.media_.audio_info_.CCIDs_[1], 1);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.media_.audio_info_.CCIDs_[2], 2);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.src_id_, 5);
    EXPECT_EQ(parse.packet_.media_pack_.rtp_len_, 6);
    EXPECT_EQ(memcmp(parse.packet_.media_pack_.rtp_pack_, data, parse.packet_.media_pack_.rtp_len_), 0);
#endif
}

//音频的打包、解析测试
TEST_F(SRMediaHeadV2, AUDIO_2)
{
#if 1
    //打包
    uint32_t send_len = 0;
    SRPacket pack;

    uint8_t pack_bufer[1500] = { 0 };
    uint8_t data[100];
    memset(data, 0x08, sizeof(data));
    uint16_t out_len = 0;

    //base head
    pack.base_head_.E_ = true;
    pack.base_head_.R_ = true;
    pack.base_head_.X_ = false;

    pack.packet_.media_pack_.media_info_.channel_id_ = 5;
    pack.packet_.media_pack_.media_info_.F_ = true;
    pack.packet_.media_pack_.media_info_.MT_ = SRMT_AUDIO;

    uint16_t ccid[3];
    memset(ccid, -1, sizeof(ccid));
    pack.packet_.media_pack_.media_info_.media_.audio_info_.audio_codec_ = 3;
    pack.packet_.media_pack_.media_info_.media_.audio_info_.CC_ = sizeof(ccid);
    pack.packet_.media_pack_.media_info_.media_.audio_info_.CCIDs_ = ccid;
    pack.packet_.media_pack_.media_info_.media_.audio_info_.TT_ = 3;
    pack.packet_.media_pack_.media_info_.media_.audio_info_.LL_ = 2;
    pack.packet_.media_pack_.media_info_.src_id_ = 1;
    pack.packet_.media_pack_.rtp_pack_ = data;
    pack.packet_.media_pack_.rtp_len_ = 2;

    EXPECT_EQ(pack.Pack(pack_bufer, 1500, out_len), SR_OK);
    EXPECT_EQ(out_len, (6 + 2 * pack.packet_.media_pack_.media_info_.media_.audio_info_.CC_ + pack.packet_.media_pack_.rtp_len_));

    //解析
    SRPacket parse;
    uint8_t recv_data[1500] = { 0 };
    uint32_t recv_len = 0;
    EXPECT_EQ(parse.Parse(pack_bufer, out_len), SR_OK);

    EXPECT_EQ(parse.base_head_.E_, true);
    EXPECT_EQ(parse.base_head_.R_, true);
    EXPECT_EQ(parse.base_head_.X_, false);


    EXPECT_EQ(parse.packet_.media_pack_.media_info_.channel_id_, 5);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.F_, true);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.MT_, SRMT_AUDIO);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.media_.audio_info_.audio_codec_, 3);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.media_.audio_info_.TT_, 3);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.media_.audio_info_.LL_, 2);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.media_.audio_info_.CC_, sizeof(ccid));
    //EXPECT_EQ(strcmp((char*)ccid, (char*)parse.packet_.media_pack_.media_info_.audio_info_.CCIDs_), 0);
    EXPECT_EQ(parse.packet_.media_pack_.media_info_.src_id_, 1);
    EXPECT_EQ(parse.packet_.media_pack_.rtp_len_, 2);
    EXPECT_EQ(memcmp(parse.packet_.media_pack_.rtp_pack_, data, parse.packet_.media_pack_.rtp_len_), 0);
#endif
}

