#ifndef PACKDEF_H
#define PACKDEF_H
#include "TypeDef.h"
#include "SRProtoSerializer.h"

//client <---> relayserver
#define PACK_CMD_ID_RELAYSVR_BASE 100
#define PACK_CMD_ID_RELAYSVR_AUTH_REQ       PACK_CMD_ID_RELAYSVR_BASE + 1
#define PACK_CMD_ID_RELAYSVR_AUTH_RESP      PACK_CMD_ID_RELAYSVR_BASE + 2
#define PACK_CMD_ID_HEARTBEAT_PACKET        PACK_CMD_ID_RELAYSVR_BASE + 3
#define PACK_CMD_ID_RTP_RTCP_PACKET         PACK_CMD_ID_RELAYSVR_BASE + 4
#define PACK_CMD_ID_RELAY_CONF_REQ          PACK_CMD_ID_RELAYSVR_BASE + 5
#define PACK_CMD_ID_RELAY_IPMAP_RESP        PACK_CMD_ID_RELAYSVR_BASE + 6
#define PACK_CMD_ID_RTP_RTCP_PACKET2        PACK_CMD_ID_RELAYSVR_BASE + 7

//client <---> netmp
#define PACK_CMD_ID_RTP_MIXED_AUDIO         PACK_CMD_ID_RELAYSVR_BASE + 8 //unused
#define PACK_CMD_ID_DESTROY_CONFERENCE      PACK_CMD_ID_RELAYSVR_BASE + 9 //unused
#define PACK_CMD_ID_JOIN_CONFERENCE         PACK_CMD_ID_RELAYSVR_BASE + 10//unused
#define PACK_CMD_ID_LOCAL_NODE_LST          PACK_CMD_ID_RELAYSVR_BASE + 11
#define PACK_CMD_ID_LOCAL_NODE_LST_RESP     PACK_CMD_ID_RELAYSVR_BASE + 12
#define PACK_CMD_ID_REPORT_BW_ESTIMATION    PACK_CMD_ID_RELAYSVR_BASE + 13
#define PACK_CMD_ID_RTCP_FROM_SPEAKER       PACK_CMD_ID_RELAYSVR_BASE + 14
#define PACK_CMD_ID_OPEN_CHANNEL_REQ        PACK_CMD_ID_RELAYSVR_BASE + 15
#define PACK_CMD_ID_OPEN_CHANNEL_RESP       PACK_CMD_ID_RELAYSVR_BASE + 16
#define PACK_CMD_ID_REPORT_RECV_NETSTAT     PACK_CMD_ID_RELAYSVR_BASE + 17
#define PACK_CMD_ID_REPORT_OUTBAND_INFO     PACK_CMD_ID_RELAYSVR_BASE + 18

#define PACK_CMD_ID_RELAY_BIND_FORWARD_MAP_REQ   PACK_CMD_ID_RELAYSVR_BASE + 19
#define PACK_CMD_ID_RELAY_BIND_FORWARD_MAP_RESP  PACK_CMD_ID_RELAYSVR_BASE + 20 
#define PACK_CMD_ID_LOOPBACK_TEST                PACK_CMD_ID_RELAYSVR_BASE + 21

//for p2p
#define PACK_CMD_ID_RTP_AUDIO          PACK_CMD_ID_RELAYSVR_BASE + 100
#define PACK_CMD_ID_RTP_VIDEO_LOW      PACK_CMD_ID_RELAYSVR_BASE + 101
#define PACK_CMD_ID_RTP_VIDEO_MID      PACK_CMD_ID_RELAYSVR_BASE + 102
#define PACK_CMD_ID_RTP_VIDEO_HIGH     PACK_CMD_ID_RELAYSVR_BASE + 103
#define PACK_CMD_ID_RTP_DATA           PACK_CMD_ID_RELAYSVR_BASE + 104
#define PACK_CMD_ID_RTCP_AUDIO         PACK_CMD_ID_RELAYSVR_BASE + 105
#define PACK_CMD_ID_RTCP_VIDEO_LOW     PACK_CMD_ID_RELAYSVR_BASE + 106
#define PACK_CMD_ID_RTCP_VIDEO_MID     PACK_CMD_ID_RELAYSVR_BASE + 107
#define PACK_CMD_ID_RTCP_VIDEO_HIGH    PACK_CMD_ID_RELAYSVR_BASE + 108
#define PACK_CMD_ID_RTCP_DATA          PACK_CMD_ID_RELAYSVR_BASE + 109

//peer test cmd
#define PACK_CMD_ID_STUN_MESSAGE       PACK_CMD_ID_RELAYSVR_BASE + 110
#define PACK_CMD_ID_PEER_STUN_REQ      PACK_CMD_ID_RELAYSVR_BASE + 111
#define PACK_CMD_ID_PEER_STUN_RESP     PACK_CMD_ID_RELAYSVR_BASE + 112

//response
namespace RESPONSE {
    enum RESPONSE_AUTH {FAILED_AUTH = 0, SUCC_AUTH, NEED_REAUTH, INVALID_ARG};
    enum HEARTBEAT_TAG {HEARTBEAT_REQ, HEARTBEAT_RESP};
    enum BIND_FORWARD_PAIR {FAILED_BIND = 0, SUCC_BIND, NEED_REBIND};
}

struct PACK_RELAYSVR_AUTH_REQ {
	PACK_RELAYSVR_AUTH_REQ();
    uint32_t ip_;
    uint16_t port_;
    uint16_t datalen_;
    char data_[1000];

    bool serial ( char* out_buf, uint16_t out_buf_len, uint16_t& data_len, uint16_t& cmd_id );
    bool unserial ( char* in_buf, uint16_t in_buf_len );
};


struct PACK_RELAYSVR_BIND_FORWARD_MAP_REQ
{
    PACK_RELAYSVR_BIND_FORWARD_MAP_REQ();
    uint32_t req_ip_;
    uint16_t req_port_;
    
    uint32_t bind_ip_;
    uint16_t bind_port_;
    
    uint16_t  datalen_;
    char data_[1000];

    bool serial ( char* out_buf, uint16_t out_buf_len, uint16_t& data_len, uint16_t& cmd_id );
    bool unserial ( char* in_buf, uint16_t in_buf_len );
};

struct PACK_RELAYSVR_BIND_FORWARD_MAP_RESP
{
    PACK_RELAYSVR_BIND_FORWARD_MAP_RESP();
   
    uint8_t  succ_; 
    uint32_t public_ip_;
    uint16_t public_port_;
   
    uint32_t peer_ip_;
    uint16_t peer_port_;

    uint16_t  datalen_;
    char data_[1000];

    bool serial ( char* out_buf, uint16_t out_buf_len, uint16_t& data_len, uint16_t& cmd_id );
    bool unserial ( char* in_buf, uint16_t in_buf_len );
};


struct PACK_RELAYSVR_AUTH_RESP {
    PACK_RELAYSVR_AUTH_RESP();
    uint8_t success_;
    uint64_t reserved_;

    bool serial ( char* out_buf, uint16_t out_buf_len, uint16_t& data_len, uint16_t& cmd_id );
    bool unserial ( char* in_buf, uint16_t in_buf_len );
};

struct PACK_RELAYSVR_HEARTBEAT_PACKET {
	PACK_RELAYSVR_HEARTBEAT_PACKET();
    uint8_t heartbeat_tag_;
    uint64_t reserved_;

    bool serial ( char* out_buf, uint16_t out_buf_len, uint16_t& data_len, uint16_t& cmd_id );
    bool unserial ( char* in_buf, uint16_t in_buf_len );
};

struct PACK_RELAYSVR_RTP_RTCP_PACKET {
    PACK_RELAYSVR_RTP_RTCP_PACKET();
    PACK_RELAYSVR_RTP_RTCP_PACKET ( uint16_t len, char *data ) :data_len_ ( len ), data_ ( data ) {}
    uint16_t data_len_;
    char *data_;

    bool serial ( char* out_buf, uint16_t out_buf_len, uint16_t& data_len, uint16_t& cmd_id );
    bool unserial ( char* in_buf, uint16_t in_buf_len );
};

struct PACK_RELAYSVR_RELAY_CONF_REQ {
    PACK_RELAYSVR_RELAY_CONF_REQ();
    uint16_t req_id_;

    bool serial ( char* out_buf, uint16_t out_buf_len, uint16_t& data_len, uint16_t& cmd_id );
    bool unserial ( char* in_buf, uint16_t in_buf_len );
};
struct PACK_RELAYSVR_RELAY_IPMAP_RESP {
    PACK_RELAYSVR_RELAY_IPMAP_RESP();
    uint16_t count_;
    uint32_t wan_ip_[150];
    uint32_t lan_ip_[150];

    bool serial ( char* out_buf, uint16_t out_buf_len, uint16_t& data_len, uint16_t& cmd_id );
    bool unserial ( char* in_buf, uint16_t in_buf_len );
};

struct PACK_REPORT_BW_ESTIMATION {
    PACK_REPORT_BW_ESTIMATION();
    uint16_t rtt_ms_;
    uint32_t user_id_;
    uint8_t channel_cnt_;
    uint8_t channel_ids_[255];
    uint16_t bitrates_kbps_[255];
    bool serial(char* out_buf, uint16_t out_buf_len, uint16_t& data_len, uint16_t& cmd_id);
    bool unserial(char* in_buf, uint16_t in_buf_len);
};

struct PACK_REPORT_RECV_NETSTAT {
    PACK_REPORT_RECV_NETSTAT();
    uint16_t rtt_ms_;
    uint32_t user_id_;
    uint8_t channel_cnt_;
    uint8_t channel_ids_[255];
    uint16_t bitrates_kbps_[255];
    uint32_t queue_delays_[255];
    bool serial(char* out_buf, uint16_t out_buf_len, uint16_t& data_len, uint16_t& cmd_id);
    bool unserial(char* in_buf, uint16_t in_buf_len);
};

struct PACK_REPORT_OUTBAND_INFO {
	PACK_REPORT_OUTBAND_INFO();
    uint32_t user_id_;
    int32_t rtt_ms_;
    int32_t download_fraction_loss_;
    int32_t download_fraction_pack_count_;

    //has_network_info_ > 0，后面的值才有意义
    uint8_t has_network_info_;
    uint16_t network_type_;
    uint16_t network_opt_;
    uint16_t platform_;

    bool serial(char* out_buf, uint16_t out_buf_len, uint16_t& data_len, uint16_t& cmd_id);
    bool unserial(char* in_buf, uint16_t in_buf_len);
};

struct PACK_OPEN_CHANNEL_REQ {
	PACK_OPEN_CHANNEL_REQ();
    uint32_t user_id_;
	uint32_t echo_data_;
    uint16_t datalen_;
    char data_[1000];

    bool serial(char* out_buf, uint16_t out_buf_len, uint16_t& data_len, uint16_t& cmd_id);
    bool unserial(char* in_buf, uint16_t in_buf_len);
};

struct PACK_OPEN_CHANNEL_RESP {
	PACK_OPEN_CHANNEL_RESP();
    uint8_t success_;
    uint32_t echo_data_;

    bool serial(char* out_buf, uint16_t out_buf_len, uint16_t& data_len, uint16_t& cmd_id);
    bool unserial(char* in_buf, uint16_t in_buf_len);
};

struct PACK_STUN_REQ
{
	PACK_STUN_REQ();
    uint32_t id;
    uint64_t timestamp;
    char token[32];

    bool serial(char* out_buf, uint16_t out_buf_len, uint16_t& data_len, uint16_t& cmd_id);
    bool unserial(char* in_buf, uint16_t in_buf_len);
};

struct PACK_STUN_RESP
{
    PACK_STUN_RESP() { }
    uint32_t id;
    uint64_t timestamp;
    uint32_t mapped_ip;
    uint16_t mapped_port;
    uint8_t error;
    char token[32];

    bool serial(char* out_buf, uint16_t out_buf_len, uint16_t& data_len, uint16_t& cmd_id);
    bool unserial(char* in_buf, uint16_t in_buf_len);
};

struct PACK_LOCAL_NODE_LST
{
    PACK_LOCAL_NODE_LST();
    uint16_t user_cnt_;
    uint32_t users_lst_[300];

    bool serial(char* out_buf, uint16_t out_buf_len, uint16_t& data_len, uint16_t& cmd_id);
    bool unserial(char* in_buf, uint16_t in_buf_len);
};

struct PACK_SPEAKER_RTCP
{
	PACK_SPEAKER_RTCP();
    uint8_t media_type_;
    uint16_t channel_id_;   //用户channel id
    uint16_t src_id_;       //用户流的 id
    uint8_t codec_id_ : 6;  //编码器类型
    uint8_t LL_ : 2;        //丢包等级
    uint16_t pack_len_;
    char pack_[1400];
    
    bool serial(char* out_buf, uint16_t out_buf_len, uint16_t& data_len, uint16_t& cmd_id);
    bool unserial(char* in_buf, uint16_t in_buf_len);
};
#endif // PACKDEF_H


