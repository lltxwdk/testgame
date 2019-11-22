
#include "SRMediaHeadV2.h"

SRPackErr SRMedia::Parse(uint8_t media_fmt[4], uint8_t* pack_buf, uint16_t buf_len)
{
    media_info_.MT_ = (media_fmt[0] & 0x06) >> 1;
    if (SRMT_OTHER == media_info_.MT_)
    {
        return SR_OK;//¼æÈÝÐÔ¿¼ÂÇ;
    }
    SRPackErr res = SR_OK;
    if (SRMT_AUDIO == media_info_.MT_)
    {
        res = ParseAudio(media_fmt[1], pack_buf,
            buf_len, rtp_pack_, rtp_len_);
    }
    else
    {
        res = ParseVideo(media_fmt[1], pack_buf,
            buf_len, rtp_pack_, rtp_len_);
    }
    if (SR_OK != res)
    {
        return res;
    }
    media_info_.F_ = (media_fmt[0] & 0x01) != 0;

    uint16_t result = (*(uint16_t*)(media_fmt + 2));

    media_info_.channel_id_ = (result >> 6);
    media_info_.src_id_ = result & 0X3F;
    return SR_OK;
}

SRPackErr SRMedia::Pack(uint8_t media_fmt[4], uint8_t* pack_buf, uint16_t buf_len, uint16_t &out_pack_len)
{
    assert(media_info_.src_id_ <= 0X3F);
    if (media_info_.MT_ > 3)
    {
        assert(false);
        return SR_INVALIDED;
    }
    if (SRMT_OTHER == media_info_.MT_)
    {
        assert(false);
        return SR_INVALIDED;
    }

    SRPackErr res = SR_OK;
    if (SRMT_AUDIO == media_info_.MT_)
    {
        res = PackAudio(media_fmt + 1, pack_buf, buf_len, out_pack_len, rtp_pack_, rtp_len_);
    }
    else
    {
        res = PackVideo(media_fmt + 1, pack_buf, buf_len, out_pack_len, rtp_pack_, rtp_len_);
    }
    if (SR_OK != res)
    {
        return res;
    }
    media_fmt[0] &= 0xf8;
    media_fmt[0] |= (media_info_.MT_ << 1);
    if (media_info_.F_)
    {
        media_fmt[0] |= 0x01;
    }

    uint16_t result = (media_info_.channel_id_ << 6) | (media_info_.src_id_ & 0X3F);
    *(uint16_t*)(media_fmt + 2) = result;
    return SR_OK;
}

SRPackErr SRMedia::ParseAudio(uint8_t media_fmt, uint8_t* pack_buf, uint16_t buf_len,
    uint8_t*& rtp_pack, uint16_t& rtp_len)
{
    SRMediaAudio &audio_info = media_info_.media_.audio_info_;

    uint16_t result = *(uint16_t*)pack_buf;
    rtp_len = result & 0X07FF;
    audio_info.CC_ = result >> 13;
    audio_info.LL_ = (result >> 11) & 0X03;
    if (buf_len < rtp_len + audio_info.CCBytes() + 2)
    {
        return SR_INSUFFICIENT;
    }

    audio_info.TT_ = media_fmt & 0x03;
    audio_info.audio_codec_ = media_fmt >> 2;

    audio_info.CCIDs_ = (uint16_t*)(pack_buf + 2);
    rtp_pack = pack_buf + 2 + audio_info.CCBytes();
    return SR_OK;
}

SRPackErr SRMedia::PackAudio(uint8_t* media_fmt, uint8_t* pack_buf, uint16_t buf_len, uint16_t &out_pack_len,
    uint8_t* rtp_pack, uint16_t rtp_len)
{
    SRMediaAudio &audio_info = media_info_.media_.audio_info_;

    assert(audio_info.CC_ <= 7); 
    assert(audio_info.TT_ <= 3);
    assert(audio_info.LL_ <= 3);
    if (buf_len < rtp_len + audio_info.CCBytes() + 2)
    {
        return SR_INSUFFICIENT;
    }

    *media_fmt = (audio_info.audio_codec_ << 2) + audio_info.TT_;

    uint16_t result = rtp_len | (audio_info.CC_ << 13) | (audio_info.LL_ << 11);
    *(uint16_t*)pack_buf = result;
    if (audio_info.CCBytes() > 0)
    {
        memcpy(pack_buf + 2, audio_info.CCIDs_, audio_info.CCBytes());
    }

    memcpy(pack_buf + 2 + audio_info.CCBytes(), rtp_pack, rtp_len);

    out_pack_len += (rtp_len + audio_info.CCBytes() + 2);
    return SR_OK;
}

SRPackErr SRMedia::ParseVideo(uint8_t media_fmt, uint8_t* pack_buf, uint16_t buf_len,
    uint8_t*& rtp_pack, uint16_t& rtp_len)
{
    SRMediaVideo &video_info = media_info_.media_.video_info_;
    video_info.video_codec_ = (media_fmt >> 4) & 0x0f;

    uint16_t media_info = *(uint16_t*)pack_buf;
    rtp_len = media_info & 0X07FF;
    if (buf_len < rtp_len + 2)
    {
        return SR_INSUFFICIENT;
    }

    if (SR_H264 == video_info.video_codec_)
    {
        SRVideoH264 &video_h264 = video_info.video_union.video_h264;
        video_h264.qlevel_ = media_fmt & 0x0f;
        video_h264.Fi_ = (media_info >> 11);

        rtp_pack = pack_buf + 2;
    }
    else if (SR_H264_SVC == video_info.video_codec_)
    {
        SRVideoH264Svc &video_h264_svc = video_info.video_union.video_h264_svc;
        video_h264_svc.qlevel_ = media_fmt & 0x0f;

        uint8_t one_byte = (media_info >> 11);
        video_h264_svc.DID_ = (one_byte & 0x18) >> 3;
        video_h264_svc.TID_ = (one_byte & 0x6) >> 1;
        video_h264_svc.I_ = (one_byte & 0x01) > 0 ? true : false;

        uint8_t sub_one_byte = *(uint8_t*)(pack_buf + 2);
        video_h264_svc.TT_ = (sub_one_byte & 0xC0) >> 6;
        video_h264_svc.N_ = ((sub_one_byte & 0x20) >> 5) > 0 ? true : false;
        video_h264_svc.PRI_ = (sub_one_byte & 0x18) >> 3;
        video_h264_svc.QID_ = (sub_one_byte & 0x06) >> 1;   
        video_h264_svc.U_ = (sub_one_byte & 0x01) > 0 ? true : false;
        video_h264_svc.FR_ = (*(uint8_t*)(pack_buf + 3) >> 2) & 0x07;
		video_h264_svc.DT_ = *(uint8_t*)(pack_buf + 3) & 0x03;

        uint8_t* sub_seq_start_addr = pack_buf + 4;
        assert(video_h264_svc.TT_ <= 3);
        int total_sub = video_h264_svc.TT_ - video_h264_svc.TID_+1;
        assert(total_sub >= 1);
        assert(total_sub <= 4);
        for (int i = 0; i < total_sub; i++)
        {
            video_h264_svc.sub_seq[i] = *(uint16_t*)(sub_seq_start_addr + (2 * i));
        }

        rtp_pack = sub_seq_start_addr + 2 * total_sub;
    }
    else
    {
        assert(false);
    }

    return SR_OK;
}

SRPackErr SRMedia::PackVideo(uint8_t* media_fmt, uint8_t* pack_buf, uint16_t buf_len, uint16_t &out_pack_len,
    uint8_t* rtp_pack, uint16_t rtp_len)
{
    assert(rtp_len <= 0x0fff);
    if (buf_len < rtp_len + 2)
    {
        return SR_INSUFFICIENT;
    }

    SRMediaVideo &video_info = media_info_.media_.video_info_;
    uint16_t media_info = 0;

    if (SR_H264 == video_info.video_codec_)
    {
        SRVideoH264 &video_h264 = video_info.video_union.video_h264;
        assert(video_info.video_codec_ < 16
            && video_h264.qlevel_ < 16
            && video_h264.Fi_ < 16);
        *media_fmt = (video_info.video_codec_ << 4) + video_h264.qlevel_;

        media_info = rtp_len | (video_h264.Fi_ << 11);

        *(uint16_t*)pack_buf = media_info;
        memcpy(pack_buf + 2, rtp_pack, rtp_len);
        out_pack_len += rtp_len + 2;
    }
    else if (SR_H264_SVC == video_info.video_codec_)
    {
        SRVideoH264Svc &video_h264_svc = video_info.video_union.video_h264_svc;
        assert(video_info.video_codec_ < 16
            && video_h264_svc.qlevel_ < 16
            && video_h264_svc.DID_ < 4
            && video_h264_svc.TID_ < 4
            && video_h264_svc.TT_ < 4
            && video_h264_svc.PRI_ < 4
            && video_h264_svc.QID_ < 4);
        *media_fmt = (video_info.video_codec_ << 4) + video_h264_svc.qlevel_;

        uint8_t one_byte = 0;
        one_byte = (video_h264_svc.DID_ << 3) | (video_h264_svc.TID_ << 1) | (video_h264_svc.I_ ? 1 : 0);
        media_info = rtp_len | (one_byte << 11);
        *(uint16_t*)pack_buf = media_info;

        uint8_t sub_one_byte = (video_h264_svc.TT_ << 6) | ((video_h264_svc.N_ ? 1 : 0) << 5) 
            | (video_h264_svc.PRI_ << 3) | (video_h264_svc.QID_ << 1) | (video_h264_svc.U_ ? 1 : 0);
        *(uint8_t*)(pack_buf + 2) = sub_one_byte;
        *(uint8_t*)(pack_buf + 3) = ((video_h264_svc.FR_ & 0x07) << 2) | (video_h264_svc.DT_ & 0x03);

        uint8_t* sub_seq_start_addr = pack_buf + 4;
        int total_sub = video_h264_svc.TT_ - video_h264_svc.TID_ + 1;
        assert(total_sub >= 1);
        assert(total_sub <= 4);
        for (int i = 0; i < total_sub; i++)
        {
            *(uint16_t*)(sub_seq_start_addr + 2 * i) = video_h264_svc.sub_seq[i];
        }

        memcpy(sub_seq_start_addr + total_sub * 2, rtp_pack, rtp_len);
        out_pack_len += rtp_len + 4 + total_sub * 2;
    }
    else
    {
        assert(false);
    }

    return SR_OK;
}

SRPacket::SRPacket()
{
	memset(this, 0, sizeof(SRPacket));
}

SRPackErr SRPacket::Parse(uint8_t* raw_pack, uint16_t raw_pack_len)
{
    SRPackErr res = base_head_.Parse(raw_pack, raw_pack_len);
    if (SR_OK != res)
    {
        return res;
    }
    if (base_head_.R_)
    {
        return packet_.media_pack_.Parse(raw_pack, base_head_.remain_buf_, base_head_.remain_len_);
    }
    else
    {
        return packet_.signal_pack_.Parse(raw_pack, base_head_.remain_buf_, base_head_.remain_len_);
    }
}

SRPackErr SRPacket::Pack(uint8_t* raw_pack, uint16_t raw_pack_len, uint16_t &out_pack_len)
{
	out_pack_len = 0;
    SRPackErr res = base_head_.Pack(raw_pack, raw_pack_len, out_pack_len);
    if (SR_OK != res)
    {
        return res;
    }
    if (base_head_.R_)
    {
        res = packet_.media_pack_.Pack(raw_pack, base_head_.remain_buf_, base_head_.remain_len_, out_pack_len);
    }
    else
    {
        res = packet_.signal_pack_.Pack(raw_pack, base_head_.remain_buf_, base_head_.remain_len_, out_pack_len);
    }
    return res;
}

