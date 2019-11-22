/*
 * =====================================================================================
 *
 *       Filename:  packdef_unittest.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/07/2014 04:33:36 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include "gtest/gtest.h"
#include "PackDef.h"
#include "ParsePdu.h"
#include "TypeDef.h"

class PackPduTest : public testing::Test 
{
protected:
	struct PACK_RELAYSVR_AUTH_REQ m_nAuthReqPacket;
    struct PACK_RELAYSVR_AUTH_RESP m_nAuthRespPacket;
    struct PACK_RELAYSVR_HEARTBEAT_PACKET m_nHeartPacket;
    struct PACK_RELAYSVR_RTP_RTCP_PACKET m_nRtpRtcpPacket;
    struct PACK_RELAYSVR_RELAY_CONF_REQ m_nConfReqPacket;
    struct PACK_RELAYSVR_RELAY_IPMAP_RESP m_nIpmapResp;
    struct PACK_REPORT_OUTBAND_INFO m_outband_info;
    struct PACK_SPEAKER_RTCP m_rtcp;

    
    char m_nPacketBuf[1500];
	uint16_t m_nPacketLen;
    ParsePdu m_nParsePdu;
    
    void Reset()
    {
        memset(m_nPacketBuf, 0, sizeof(m_nPacketBuf));
        m_nPacketLen = 0;
    }
};

TEST_F(PackPduTest, AuthReq)
{
   /* Reset();
    m_nAuthReqPacket.ip_ = ntohl(inet_addr("127.0.0.1"));
    m_nAuthReqPacket.port_ = 80;
    m_nAuthReqPacket.datalen_ = 0;
    
    EXPECT_TRUE(m_nParsePdu.packPduBuf(m_nAuthReqPacket, m_nPacketBuf, m_nPacketLen));
    EXPECT_TRUE(m_nParsePdu.isOwnProtocol(m_nPacketBuf, m_nPacketLen));
    
    uint16_t nType = 0; 
    char *pPduData = NULL;
    uint16_t nPduLen = 0;
    EXPECT_TRUE(m_nParsePdu.parsePduHead(m_nPacketBuf, m_nPacketLen, nType, pPduData, nPduLen));
    EXPECT_EQ(nType, PACK_CMD_ID_RELAYSVR_AUTH_REQ);
    EXPECT_TRUE(m_nAuthReqPacket.unserial(pPduData, nPduLen));*/
}

TEST_F(PackPduTest, AuthResp)
{
    Reset();
    m_nAuthRespPacket.success_ = 1;
    m_nAuthRespPacket.reserved_ = 0;
    
    EXPECT_TRUE(m_nParsePdu.packPduBuf(m_nAuthRespPacket, m_nPacketBuf, m_nPacketLen));
    EXPECT_TRUE(m_nParsePdu.isOwnProtocol(m_nPacketBuf, m_nPacketLen));
    
    uint16_t nType = 0; 
    char *pPduData = NULL;
    uint16_t nPduLen = 0;
    EXPECT_TRUE(m_nParsePdu.parsePduHead(m_nPacketBuf, m_nPacketLen, nType, pPduData, nPduLen));
    EXPECT_EQ(nType, PACK_CMD_ID_RELAYSVR_AUTH_RESP);
    EXPECT_TRUE(m_nAuthRespPacket.unserial(pPduData, nPduLen));
}

TEST_F(PackPduTest, HeartPacket)
{
    Reset();
    m_nHeartPacket.heartbeat_tag_ = 1;
    m_nHeartPacket.reserved_ = 0;
    
    EXPECT_TRUE(m_nParsePdu.packPduBuf(m_nHeartPacket, m_nPacketBuf, m_nPacketLen));
    EXPECT_TRUE(m_nParsePdu.isOwnProtocol(m_nPacketBuf, m_nPacketLen));
    
    uint16_t nType = 0; 
    char *pPduData = NULL;
    uint16_t nPduLen = 0;
    EXPECT_TRUE(m_nParsePdu.parsePduHead(m_nPacketBuf, m_nPacketLen, nType, pPduData, nPduLen));
    EXPECT_EQ(nType, PACK_CMD_ID_HEARTBEAT_PACKET);
    EXPECT_TRUE(m_nHeartPacket.unserial(pPduData, nPduLen));
}

TEST_F(PackPduTest, RtpRtcp)
{
    Reset();
    char nRtp[100] = "hello rtp rtcp";
    int nRtpLen = strlen(nRtp);
    m_nRtpRtcpPacket.data_ = nRtp;
    m_nRtpRtcpPacket.data_len_ = nRtpLen;
    
    EXPECT_TRUE(m_nParsePdu.packPduBuf(m_nRtpRtcpPacket, m_nPacketBuf, m_nPacketLen));
    EXPECT_TRUE(m_nParsePdu.isOwnProtocol(m_nPacketBuf, m_nPacketLen));
    
    uint16_t nType = 0; 
    char *pPduData = NULL;
    uint16_t nPduLen = 0;
    EXPECT_TRUE(m_nParsePdu.parsePduHead(m_nPacketBuf, m_nPacketLen, nType, pPduData, nPduLen));
    EXPECT_EQ(nType, PACK_CMD_ID_RTP_RTCP_PACKET);
    EXPECT_TRUE(m_nRtpRtcpPacket.unserial(pPduData, nPduLen));
}

TEST_F(PackPduTest, ConfReq)
{
    Reset();
    m_nConfReqPacket.req_id_ = 10;
    
    EXPECT_TRUE(m_nParsePdu.packPduBuf(m_nConfReqPacket, m_nPacketBuf, m_nPacketLen));
    EXPECT_TRUE(m_nParsePdu.isOwnProtocol(m_nPacketBuf, m_nPacketLen));
    
    uint16_t nType = 0; 
    char *pPduData = NULL;
    uint16_t nPduLen = 0;
    EXPECT_TRUE(m_nParsePdu.parsePduHead(m_nPacketBuf, m_nPacketLen, nType, pPduData, nPduLen));
    EXPECT_EQ(nType, PACK_CMD_ID_RELAY_CONF_REQ);
    EXPECT_TRUE(m_nConfReqPacket.unserial(pPduData, nPduLen));
}

TEST_F(PackPduTest, IpmapResp)
{
    Reset();
    m_nIpmapResp.count_ = 1;
    m_nIpmapResp.wan_ip_[0] = 123;
    m_nIpmapResp.lan_ip_[0] = 321;
    
    EXPECT_TRUE(m_nParsePdu.packPduBuf(m_nIpmapResp, m_nPacketBuf, m_nPacketLen));
    EXPECT_TRUE(m_nParsePdu.isOwnProtocol(m_nPacketBuf, m_nPacketLen));
    
    uint16_t nType = 0; 
    char *pPduData = NULL;
    uint16_t nPduLen = 0;
    EXPECT_TRUE(m_nParsePdu.parsePduHead(m_nPacketBuf, m_nPacketLen, nType, pPduData, nPduLen));
    EXPECT_EQ(nType, PACK_CMD_ID_RELAY_IPMAP_RESP);
    EXPECT_TRUE(m_nIpmapResp.unserial(pPduData, nPduLen));
}

TEST_F(PackPduTest, OutbandInfo)
{
    {
        Reset();
        m_outband_info.user_id_ = 100;
        m_outband_info.download_fraction_loss_ = 12;
        m_outband_info.download_fraction_pack_count_ = 100;
        m_outband_info.rtt_ms_ = 1000;
        m_outband_info.has_network_info_ = 5;
        m_outband_info.network_type_ = 0;
        m_outband_info.network_opt_ = 1;
        m_outband_info.platform_ = 2;

        EXPECT_TRUE(m_nParsePdu.packPduBuf(m_outband_info, m_nPacketBuf, m_nPacketLen));
        EXPECT_TRUE(m_nParsePdu.isOwnProtocol(m_nPacketBuf, m_nPacketLen));
        memset(&m_outband_info, 0, sizeof(m_outband_info));

        uint16_t nType = 0;
        char *pPduData = NULL;
        uint16_t nPduLen = 0;
        EXPECT_TRUE(m_nParsePdu.parsePduHead(m_nPacketBuf, m_nPacketLen, nType, pPduData, nPduLen));
        EXPECT_EQ(nType, PACK_CMD_ID_REPORT_OUTBAND_INFO);
        EXPECT_TRUE(m_outband_info.unserial(pPduData, nPduLen));
        EXPECT_EQ(m_outband_info.user_id_, 100);
        EXPECT_EQ(m_outband_info.download_fraction_loss_, 12);
        EXPECT_EQ(m_outband_info.download_fraction_pack_count_, 100);
        EXPECT_EQ(m_outband_info.has_network_info_, 5);
        EXPECT_EQ(m_outband_info.network_type_, 0);
        EXPECT_EQ(m_outband_info.network_opt_, 1);
        EXPECT_EQ(m_outband_info.platform_, 2);
    }

     {
         Reset();
         m_outband_info.user_id_ = 99;
         m_outband_info.download_fraction_loss_ = -1;
         m_outband_info.download_fraction_pack_count_ = -1;
         m_outband_info.rtt_ms_ = 100;

         EXPECT_TRUE(m_nParsePdu.packPduBuf(m_outband_info, m_nPacketBuf, m_nPacketLen));
         EXPECT_TRUE(m_nParsePdu.isOwnProtocol(m_nPacketBuf, m_nPacketLen));
         memset(&m_outband_info, 0, sizeof(m_outband_info));

         uint16_t nType = 0;
         char *pPduData = NULL;
         uint16_t nPduLen = 0;
         EXPECT_TRUE(m_nParsePdu.parsePduHead(m_nPacketBuf, m_nPacketLen, nType, pPduData, nPduLen));
         EXPECT_EQ(nType, PACK_CMD_ID_REPORT_OUTBAND_INFO);
         EXPECT_TRUE(m_outband_info.unserial(pPduData, nPduLen));
         EXPECT_EQ(m_outband_info.user_id_, 99);
         EXPECT_EQ(m_outband_info.download_fraction_loss_, -1);
         EXPECT_EQ(m_outband_info.download_fraction_pack_count_, -1);
         EXPECT_EQ(m_outband_info.rtt_ms_, 100);
     }


      {
          Reset();
          m_outband_info.user_id_ = 99;
          m_outband_info.download_fraction_loss_ = 11;
          m_outband_info.download_fraction_pack_count_ = 12;
          m_outband_info.rtt_ms_ = -1;

          EXPECT_TRUE(m_nParsePdu.packPduBuf(m_outband_info, m_nPacketBuf, m_nPacketLen));
          EXPECT_TRUE(m_nParsePdu.isOwnProtocol(m_nPacketBuf, m_nPacketLen));
          memset(&m_outband_info, 0, sizeof(m_outband_info));

          uint16_t nType = 0;
          char *pPduData = NULL;
          uint16_t nPduLen = 0;
          EXPECT_TRUE(m_nParsePdu.parsePduHead(m_nPacketBuf, m_nPacketLen, nType, pPduData, nPduLen));
          EXPECT_EQ(nType, PACK_CMD_ID_REPORT_OUTBAND_INFO);
          EXPECT_TRUE(m_outband_info.unserial(pPduData, nPduLen));
          EXPECT_EQ(m_outband_info.user_id_, 99);
          EXPECT_EQ(m_outband_info.download_fraction_loss_, 11);
          EXPECT_EQ(m_outband_info.download_fraction_pack_count_, 12);
          EXPECT_EQ(m_outband_info.rtt_ms_, -1);
      }
   
    {
        Reset();
        m_outband_info.user_id_ = 99;
        m_outband_info.download_fraction_loss_ = -1;
        m_outband_info.download_fraction_pack_count_ = -1;
        m_outband_info.rtt_ms_ = -1;

        EXPECT_TRUE(m_nParsePdu.packPduBuf(m_outband_info, m_nPacketBuf, m_nPacketLen));
        EXPECT_TRUE(m_nParsePdu.isOwnProtocol(m_nPacketBuf, m_nPacketLen));
        memset(&m_outband_info, 0, sizeof(m_outband_info));

        uint16_t nType = 0;
        char *pPduData = NULL;
        uint16_t nPduLen = 0;
        EXPECT_TRUE(m_nParsePdu.parsePduHead(m_nPacketBuf, m_nPacketLen, nType, pPduData, nPduLen));
        EXPECT_EQ(nType, PACK_CMD_ID_REPORT_OUTBAND_INFO);
        EXPECT_TRUE(m_outband_info.unserial(pPduData, nPduLen));
        EXPECT_EQ(m_outband_info.user_id_, 99);
        EXPECT_EQ(m_outband_info.download_fraction_loss_, -1);
        EXPECT_EQ(m_outband_info.download_fraction_pack_count_, -1);
        EXPECT_EQ(m_outband_info.rtt_ms_, -1);
    }
}

TEST_F(PackPduTest, RtcpPack)
{
    {
        char rtcp_data[30] = { 0 };
        uint32_t rtcp_len = sizeof(rtcp_data);
        memset(rtcp_data, 1, rtcp_len);

        Reset();
        m_rtcp.channel_id_ = 0;
        m_rtcp.codec_id_ = 1;
        m_rtcp.LL_ = 2;
        m_rtcp.media_type_ = 3;
        m_rtcp.src_id_ = 4;
        m_rtcp.pack_len_ = rtcp_len;
        memcpy(m_rtcp.pack_, rtcp_data, rtcp_len);

        EXPECT_TRUE(m_nParsePdu.packPduBuf(m_rtcp, m_nPacketBuf, m_nPacketLen));
        EXPECT_TRUE(m_nParsePdu.isOwnProtocol(m_nPacketBuf, m_nPacketLen));
        memset(&m_rtcp, 0, sizeof(m_rtcp));

        uint16_t nType = 0;
        char *pPduData = NULL;
        uint16_t nPduLen = 0;
        EXPECT_TRUE(m_nParsePdu.parsePduHead(m_nPacketBuf, m_nPacketLen, nType, pPduData, nPduLen));
        EXPECT_EQ(nType, PACK_CMD_ID_RTCP_FROM_SPEAKER);
        EXPECT_TRUE(m_rtcp.unserial(pPduData, nPduLen));
        EXPECT_EQ(m_rtcp.channel_id_, 0);
        EXPECT_EQ(m_rtcp.codec_id_, 1);
        EXPECT_EQ(m_rtcp.LL_, 2);
        EXPECT_EQ(m_rtcp.media_type_, 3);
        EXPECT_EQ(m_rtcp.src_id_, 4);
        EXPECT_EQ(m_rtcp.pack_len_, rtcp_len);
        EXPECT_EQ(m_rtcp.pack_[rtcp_len - 1], rtcp_data[rtcp_len-1]);
    }
}
