#include <string.h>
#include "PackDef.h"

PACK_RELAYSVR_BIND_FORWARD_MAP_REQ::PACK_RELAYSVR_BIND_FORWARD_MAP_REQ()
{
    memset(this, 0, sizeof(PACK_RELAYSVR_BIND_FORWARD_MAP_REQ));
}

bool PACK_RELAYSVR_BIND_FORWARD_MAP_REQ::unserial ( char *in_buf, uint16_t in_buf_len )
{
    memcpy ( &req_ip_, in_buf, sizeof ( req_ip_ ) );
    in_buf += sizeof ( req_ip_ );
    memcpy ( &req_port_, in_buf, sizeof ( req_port_ ) );
    in_buf += sizeof ( req_port_ );
    
    
    memcpy ( &bind_ip_, in_buf, sizeof ( bind_ip_ ) );
    in_buf += sizeof ( bind_ip_ );
    memcpy ( &bind_port_, in_buf, sizeof ( bind_port_ ) );
    in_buf += sizeof ( bind_port_ );
    
    memcpy ( &datalen_, in_buf, sizeof ( datalen_ ) );
    if ( 1000 < datalen_ ) {
        return false;
    }
    if ( 0 == datalen_ ) {
        return true;
    }

    in_buf += sizeof ( datalen_ );
    memcpy ( data_, in_buf, datalen_ );
    return true;
}
bool PACK_RELAYSVR_BIND_FORWARD_MAP_REQ::serial ( char *out_buf, uint16_t out_buf_len, uint16_t &data_len, uint16_t &cmd_id )
{
    cmd_id = PACK_CMD_ID_RELAY_BIND_FORWARD_MAP_REQ;
    if (1000 < datalen_) {
        return false;
    }
    data_len = 0;
    data_len += sizeof(req_ip_);
    data_len += sizeof(req_port_);
    data_len += sizeof(bind_ip_);
    data_len += sizeof(bind_port_);
    data_len += sizeof(datalen_);
    data_len += datalen_;
    if (out_buf_len < data_len) {
        return false;
    }

    memcpy ( out_buf, &req_ip_, sizeof ( req_ip_ ) );
    out_buf += sizeof ( req_ip_ );

    memcpy ( out_buf, &req_port_, sizeof ( req_port_ ) );
    out_buf += sizeof ( req_port_ );
    
    memcpy ( out_buf, &bind_ip_, sizeof ( bind_ip_ ) );
    out_buf += sizeof ( bind_ip_ );

    memcpy ( out_buf, &bind_port_, sizeof ( bind_port_ ) );
    out_buf += sizeof ( bind_port_ );

    memcpy ( out_buf, &datalen_, sizeof ( datalen_ ) );
    if ( 0 == datalen_ ) {
        return true;
    }

    out_buf += sizeof ( datalen_ );
    memcpy ( out_buf, data_, datalen_ );
    return true;
}


PACK_RELAYSVR_BIND_FORWARD_MAP_RESP::PACK_RELAYSVR_BIND_FORWARD_MAP_RESP()
{
    memset ( this, 0, sizeof ( PACK_RELAYSVR_BIND_FORWARD_MAP_RESP ) );
}

bool PACK_RELAYSVR_BIND_FORWARD_MAP_RESP::serial ( char *out_buf, uint16_t out_buf_len, uint16_t &data_len, uint16_t &cmd_id )
{
    //输出cmd_id
    cmd_id = PACK_CMD_ID_RELAY_BIND_FORWARD_MAP_RESP;
    if (1000 < datalen_) {
        return false;
    }

    data_len = 0;
    data_len += sizeof(succ_);
    data_len += sizeof(public_ip_);
    data_len += sizeof(public_port_);
    
    data_len += sizeof(peer_ip_);
    data_len += sizeof(peer_port_);
    data_len += sizeof(datalen_);
    data_len += datalen_;
    if (out_buf_len < data_len)
    {
        return false;
    }

    memcpy( out_buf, &succ_, sizeof(succ_));
    out_buf += sizeof(succ_);

    memcpy ( out_buf, &public_ip_, sizeof ( public_ip_ ) );
    out_buf += sizeof ( public_ip_ );

    memcpy ( out_buf, &public_port_, sizeof ( public_port_ ) );
    out_buf += sizeof ( public_port_ );

    memcpy ( out_buf, &peer_ip_, sizeof ( peer_ip_ ) );
    out_buf += sizeof ( public_ip_ );

    memcpy ( out_buf, &peer_port_, sizeof ( peer_port_ ) );
    out_buf += sizeof ( peer_port_ );
    
    memcpy ( out_buf, &datalen_, sizeof ( datalen_ ) );
    if ( 0 == datalen_ ) {
        return true;
    }

    out_buf += sizeof ( datalen_ );
    memcpy ( out_buf, data_, datalen_ );
    return true;
}

bool PACK_RELAYSVR_BIND_FORWARD_MAP_RESP::unserial ( char *in_buf, uint16_t in_buf_len )
{
    memcpy(&succ_, in_buf, sizeof(succ_));
    in_buf += sizeof ( succ_ );
    
    memcpy ( &public_ip_, in_buf, sizeof ( public_ip_ ) );
    in_buf += sizeof ( public_ip_ );
    
    memcpy ( &public_port_, in_buf, sizeof ( public_port_ ) );
    in_buf += sizeof ( public_port_ );
    
    memcpy ( &peer_ip_, in_buf, sizeof ( peer_ip_ ) );
    in_buf += sizeof ( peer_ip_ );
    
    memcpy ( &peer_port_, in_buf, sizeof ( peer_port_ ) );
    in_buf += sizeof ( peer_port_ );
    
    memcpy ( &datalen_, in_buf, sizeof ( datalen_ ) );
    if ( 1000 < datalen_ ) {
        return false;
    }
    if ( 0 == datalen_ ) {
        return true;
    }

    in_buf += sizeof ( datalen_ );
    memcpy ( data_, in_buf, datalen_ );
    return true;
}






PACK_RELAYSVR_AUTH_REQ::PACK_RELAYSVR_AUTH_REQ()
{
    memset ( this, 0, sizeof ( PACK_RELAYSVR_AUTH_REQ ) );
}

bool PACK_RELAYSVR_AUTH_REQ::serial ( char *out_buf, uint16_t out_buf_len, uint16_t &data_len, uint16_t &cmd_id )
{
    //输出cmd_id
    cmd_id = PACK_CMD_ID_RELAYSVR_AUTH_REQ;
    if (1000 < datalen_) {
        return false;
    }

    data_len = 0;
    data_len += sizeof(ip_);
    data_len += sizeof(port_);
    data_len += sizeof(datalen_);
    data_len += datalen_;
    if (out_buf_len < data_len)
    {
        return false;
    }

    memcpy ( out_buf, &ip_, sizeof ( ip_ ) );
    out_buf += sizeof ( ip_ );

    memcpy ( out_buf, &port_, sizeof ( port_ ) );
    out_buf += sizeof ( port_ );

    memcpy ( out_buf, &datalen_, sizeof ( datalen_ ) );
    if ( 0 == datalen_ ) {
        return true;
    }

    out_buf += sizeof ( datalen_ );
    memcpy ( out_buf, data_, datalen_ );
    return true;

}

bool PACK_RELAYSVR_AUTH_REQ::unserial ( char *in_buf, uint16_t in_buf_len )
{
    memcpy ( &ip_, in_buf, sizeof ( ip_ ) );
    in_buf += sizeof ( ip_ );
    memcpy ( &port_, in_buf, sizeof ( port_ ) );
    in_buf += sizeof ( port_ );
    memcpy ( &datalen_, in_buf, sizeof ( datalen_ ) );
    if ( 1000 < datalen_ ) {
        return false;
    }
    if ( 0 == datalen_ ) {
        return true;
    }

    in_buf += sizeof ( datalen_ );
    memcpy ( data_, in_buf, datalen_ );
    return true;
}

PACK_RELAYSVR_AUTH_RESP::PACK_RELAYSVR_AUTH_RESP()
{
    memset ( this, 0, sizeof ( PACK_RELAYSVR_AUTH_RESP ) );
}

bool PACK_RELAYSVR_AUTH_RESP::serial ( char *out_buf, uint16_t out_buf_len, uint16_t &data_len, uint16_t &cmd_id )
{
    cmd_id = PACK_CMD_ID_RELAYSVR_AUTH_RESP;
    data_len = sizeof ( success_ ) + sizeof ( reserved_ );
    if ( out_buf_len < data_len ) {
        return false;
    }
    memcpy ( out_buf, &success_, sizeof ( success_ ) );
    out_buf += sizeof ( success_ );
    memcpy ( out_buf, &reserved_, sizeof ( reserved_ ) );
    return true;
}

bool PACK_RELAYSVR_AUTH_RESP::unserial ( char *in_buf, uint16_t in_buf_len )
{
    if ( in_buf_len < ( sizeof ( success_ ) + sizeof ( reserved_ ) ) ) {
        return false;
    }
    memcpy ( &success_, in_buf, sizeof ( success_ ) );
    in_buf += sizeof ( success_ );
    memcpy ( &reserved_, in_buf, sizeof ( reserved_ ) );
    return true;
}

PACK_RELAYSVR_HEARTBEAT_PACKET::PACK_RELAYSVR_HEARTBEAT_PACKET()
{
    memset ( this, 0, sizeof ( PACK_RELAYSVR_HEARTBEAT_PACKET ) );
}

bool PACK_RELAYSVR_HEARTBEAT_PACKET::serial ( char *out_buf, uint16_t out_buf_len, uint16_t &data_len, uint16_t &cmd_id )
{
    cmd_id = PACK_CMD_ID_HEARTBEAT_PACKET;
    data_len = sizeof ( heartbeat_tag_ ) + sizeof ( reserved_ );
    if ( out_buf_len < data_len ) {
        return false;
    }
    memcpy ( out_buf, &heartbeat_tag_, sizeof ( heartbeat_tag_ ) );
    out_buf += sizeof ( heartbeat_tag_ );
    memcpy ( out_buf, &reserved_, sizeof ( reserved_ ) );
    return true;
}

bool PACK_RELAYSVR_HEARTBEAT_PACKET::unserial ( char *in_buf, uint16_t in_buf_len )
{
    if ( in_buf_len < ( sizeof ( heartbeat_tag_ ) + sizeof ( reserved_ ) ) ) {
        return false;
    }
    memcpy ( &heartbeat_tag_, in_buf, sizeof ( heartbeat_tag_ ) );
    in_buf += sizeof ( heartbeat_tag_ );
    memcpy ( &reserved_, in_buf, sizeof ( reserved_ ) );
    return true;
}

PACK_RELAYSVR_RTP_RTCP_PACKET::PACK_RELAYSVR_RTP_RTCP_PACKET()
{
    memset ( this, 0, sizeof ( PACK_RELAYSVR_RTP_RTCP_PACKET ) );
}

bool PACK_RELAYSVR_RTP_RTCP_PACKET::serial ( char *out_buf, uint16_t out_buf_len, uint16_t &data_len, uint16_t &cmd_id )
{
    cmd_id = PACK_CMD_ID_RTP_RTCP_PACKET;
    data_len = data_len_;
    if ( out_buf_len < data_len ) {
        return false;
    }
    memcpy ( out_buf, data_, data_len );
    return true;
}

bool PACK_RELAYSVR_RTP_RTCP_PACKET::unserial ( char *in_buf, uint16_t in_buf_len )
{
    data_len_ = in_buf_len;
    data_ = in_buf;
    //if( in_buf_len > sizeof(data_) ){
    //    return false;
    //}
    //memcpy(data, in_buf, in_buf_len);
    return true;
}

PACK_RELAYSVR_RELAY_CONF_REQ::PACK_RELAYSVR_RELAY_CONF_REQ()
{
    memset ( this, 0, sizeof ( PACK_RELAYSVR_RELAY_CONF_REQ ) );
}

bool PACK_RELAYSVR_RELAY_CONF_REQ::serial ( char* out_buf, uint16_t out_buf_len, uint16_t& data_len, uint16_t& cmd_id )
{
    cmd_id = PACK_CMD_ID_RELAY_CONF_REQ;
    data_len = sizeof(req_id_);
    if (out_buf_len < data_len)
    {
        return false;
    }

    memcpy ( out_buf, &req_id_, data_len );
    return true;
}

bool PACK_RELAYSVR_RELAY_CONF_REQ::unserial ( char* in_buf, uint16_t in_buf_len )
{
    if (in_buf_len < sizeof(req_id_))
    {
        return false;
    }
    memcpy(&req_id_, in_buf, sizeof(req_id_));
    return true;
}

PACK_RELAYSVR_RELAY_IPMAP_RESP::PACK_RELAYSVR_RELAY_IPMAP_RESP()
{
    memset ( this, 0, sizeof ( PACK_RELAYSVR_RELAY_IPMAP_RESP ) );
}

bool PACK_RELAYSVR_RELAY_IPMAP_RESP::serial ( char* out_buf, uint16_t out_buf_len, uint16_t& data_len, uint16_t& cmd_id )
{
    cmd_id = PACK_CMD_ID_RELAY_IPMAP_RESP;
    if (count_ > 150)
    {
        return false;
    }
    data_len = sizeof(count_) + count_*sizeof(uint32_t)*2;
    if (out_buf_len < data_len)
    {
        return false;
    }

    memcpy(out_buf, &count_, sizeof(count_));

    if (0 == count_)
    {
        return true;
    }

    uint32_t array_bytes = count_*sizeof(uint32_t);
    memcpy(out_buf + sizeof(count_), wan_ip_, array_bytes);
    memcpy(out_buf + sizeof(count_) + array_bytes, lan_ip_, array_bytes);
    return true;
}

bool PACK_RELAYSVR_RELAY_IPMAP_RESP::unserial ( char* in_buf, uint16_t in_buf_len )
{
    if (in_buf_len < sizeof(count_))
    {
        return false;
    }
    memcpy(&count_, in_buf, sizeof(count_));
    if (count_ > 150)
    {
        return false;
    }
    if (0 == count_)
    {
        return true;
    }

    uint32_t array_bytes = count_*sizeof(uint32_t);
    memcpy(wan_ip_, in_buf+sizeof(count_), array_bytes);
    memcpy(lan_ip_, in_buf+sizeof(count_) + array_bytes, array_bytes);
    return true;
}

PACK_REPORT_BW_ESTIMATION::PACK_REPORT_BW_ESTIMATION()
{
  memset(this, 0, sizeof(PACK_REPORT_BW_ESTIMATION));
}

bool PACK_REPORT_BW_ESTIMATION::serial(char* out_buf, uint16_t out_buf_len, uint16_t& data_len, uint16_t& cmd_id)
{
  char *tmp = out_buf;
  cmd_id = PACK_CMD_ID_REPORT_BW_ESTIMATION;
  data_len = sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint8_t) + channel_cnt_*sizeof(uint8_t) + channel_cnt_*sizeof(uint16_t);
  if (out_buf_len < data_len || channel_cnt_ == 0) {
    return false;
  }

  memcpy(tmp, &rtt_ms_, sizeof(uint16_t));
  tmp += sizeof(uint16_t);
  memcpy(tmp, &user_id_, sizeof(uint32_t));
  tmp += sizeof(uint32_t);
  *tmp = channel_cnt_;
  if (channel_cnt_ == 0) {
    return true;
  }
  tmp++;
  memcpy(tmp, channel_ids_, channel_cnt_*sizeof(uint8_t));
  tmp += channel_cnt_*sizeof(uint8_t);
  memcpy(tmp, bitrates_kbps_, channel_cnt_*sizeof(uint16_t));

  return true;
}

bool PACK_REPORT_BW_ESTIMATION::unserial(char* in_buf, uint16_t in_buf_len)
{
  char *tmp = in_buf;
  if (in_buf_len < sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint8_t)) {
    return false;
  }

  memcpy(&rtt_ms_, tmp, sizeof(uint16_t));
  tmp += sizeof(uint16_t);
  memcpy(&user_id_, tmp, sizeof (uint32_t));
  tmp += sizeof(uint32_t);
  channel_cnt_ = *(uint8_t*)tmp;
  tmp++;
  memcpy(channel_ids_, tmp, channel_cnt_*sizeof(uint8_t));
  tmp += channel_cnt_*sizeof(uint8_t);
  memcpy(bitrates_kbps_, tmp, channel_cnt_*sizeof(uint16_t));

  return true;
}

PACK_REPORT_RECV_NETSTAT::PACK_REPORT_RECV_NETSTAT()
{
    memset(this, 0, sizeof(PACK_REPORT_RECV_NETSTAT));
}

bool PACK_REPORT_RECV_NETSTAT::serial(char* out_buf, uint16_t out_buf_len, uint16_t& data_len, uint16_t& cmd_id)
{
    char *tmp = out_buf;
    cmd_id = PACK_CMD_ID_REPORT_RECV_NETSTAT;
    data_len = sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint8_t) 
        + channel_cnt_*sizeof(uint8_t) + channel_cnt_*sizeof(uint16_t)+ channel_cnt_*sizeof(uint32_t);
    if (out_buf_len < data_len || channel_cnt_ == 0) {
        return false;
    }

    memcpy(tmp, &rtt_ms_, sizeof(uint16_t));
    tmp += sizeof(uint16_t);
    memcpy(tmp, &user_id_, sizeof(uint32_t));
    tmp += sizeof(uint32_t);
    *tmp = channel_cnt_;
    if (channel_cnt_ == 0) {
        return true;
    }
    tmp++;
    memcpy(tmp, channel_ids_, channel_cnt_*sizeof(uint8_t));
    tmp += channel_cnt_*sizeof(uint8_t);
    memcpy(tmp, bitrates_kbps_, channel_cnt_*sizeof(uint16_t));
    tmp += channel_cnt_*sizeof(uint16_t);
    memcpy(tmp, queue_delays_, channel_cnt_*sizeof(uint32_t));
    return true;
}

bool PACK_REPORT_RECV_NETSTAT::unserial(char* in_buf, uint16_t in_buf_len)
{
    char *tmp = in_buf;
    if (in_buf_len < sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint8_t)) {
        return false;
    }

    memcpy(&rtt_ms_, tmp, sizeof(uint16_t));
    tmp += sizeof(uint16_t);
    memcpy(&user_id_, tmp, sizeof(uint32_t));
    tmp += sizeof(uint32_t);
    channel_cnt_ = *(uint8_t*)tmp;
    tmp++;
    memcpy(channel_ids_, tmp, channel_cnt_*sizeof(uint8_t));
    tmp += channel_cnt_*sizeof(uint8_t);
    memcpy(bitrates_kbps_, tmp, channel_cnt_*sizeof(uint16_t));
    tmp += channel_cnt_*sizeof(uint16_t);
    memcpy(queue_delays_, tmp, channel_cnt_*sizeof(uint32_t));
    return true;
}

PACK_REPORT_OUTBAND_INFO::PACK_REPORT_OUTBAND_INFO()
{
    memset(this, 0, sizeof(PACK_REPORT_OUTBAND_INFO));
}

bool PACK_REPORT_OUTBAND_INFO::serial(char* out_buf, uint16_t out_buf_len, uint16_t& data_len, uint16_t& cmd_id)
{
    cmd_id = PACK_CMD_ID_REPORT_OUTBAND_INFO;
    data_len = sizeof(user_id_) + sizeof(rtt_ms_) + sizeof(download_fraction_loss_) + sizeof(download_fraction_pack_count_) + 
        sizeof(has_network_info_) + sizeof(network_type_) + sizeof(network_opt_) + sizeof(platform_);
    if (out_buf_len < data_len) {
        return false;
    }
    memcpy(out_buf, &user_id_, sizeof(user_id_));
    out_buf += sizeof(user_id_);
    memcpy(out_buf, &rtt_ms_, sizeof(rtt_ms_));
    out_buf += sizeof(rtt_ms_);
    memcpy(out_buf, &download_fraction_loss_, sizeof(download_fraction_loss_));
    out_buf += sizeof(download_fraction_loss_);
    memcpy(out_buf, &download_fraction_pack_count_, sizeof(download_fraction_pack_count_));
    out_buf += sizeof(download_fraction_pack_count_);

    memcpy(out_buf, &has_network_info_, sizeof(has_network_info_));
    out_buf += sizeof(has_network_info_);
    memcpy(out_buf, &network_type_, sizeof(network_type_));
    out_buf += sizeof(network_type_);
    memcpy(out_buf, &network_opt_, sizeof(network_opt_));
    out_buf += sizeof(network_opt_);
    memcpy(out_buf, &platform_, sizeof(platform_));

    return true;
}

bool PACK_REPORT_OUTBAND_INFO::unserial(char* in_buf, uint16_t in_buf_len)
{
    if (in_buf_len < (sizeof(user_id_) + sizeof(rtt_ms_) + sizeof(download_fraction_loss_) 
        + sizeof(download_fraction_pack_count_) + sizeof(network_type_) + sizeof(network_opt_) 
        + sizeof(platform_))) 
    {
        return false;
    }
    memcpy(&user_id_, in_buf, sizeof(user_id_));
    in_buf += sizeof(user_id_);
    memcpy(&rtt_ms_, in_buf, sizeof(rtt_ms_));
    in_buf += sizeof(rtt_ms_);
    memcpy(&download_fraction_loss_, in_buf, sizeof(download_fraction_loss_));
    in_buf += sizeof(download_fraction_loss_);
    memcpy(&download_fraction_pack_count_, in_buf, sizeof(download_fraction_pack_count_));
    in_buf += sizeof(download_fraction_pack_count_);
    
    memcpy(&has_network_info_, in_buf, sizeof(has_network_info_));
    in_buf += sizeof(has_network_info_);
    memcpy(&network_type_, in_buf, sizeof(network_type_));
    in_buf += sizeof(network_type_);
    memcpy(&network_opt_, in_buf, sizeof(network_opt_));
    in_buf += sizeof(network_opt_);
    memcpy(&platform_, in_buf, sizeof(platform_));

    return true;
}

PACK_OPEN_CHANNEL_REQ::PACK_OPEN_CHANNEL_REQ()
{
    memset(this, 0, sizeof(PACK_OPEN_CHANNEL_REQ));
}

bool PACK_OPEN_CHANNEL_REQ::serial(char *out_buf, uint16_t out_buf_len, uint16_t &data_len, uint16_t &cmd_id)
{
    //输出cmd_id
    cmd_id = PACK_CMD_ID_OPEN_CHANNEL_REQ;
    if (1000 < datalen_) {
        return false;
    }

    data_len = 0;
    data_len += sizeof(user_id_);
    data_len += sizeof(echo_data_);
    data_len += sizeof(datalen_);
    data_len += datalen_;
    if (out_buf_len < data_len)
    {
        return false;
    }

    memcpy(out_buf, &user_id_, sizeof(user_id_));
    out_buf += sizeof(user_id_);

    memcpy(out_buf, &echo_data_, sizeof(echo_data_));
    out_buf += sizeof(echo_data_);

    memcpy(out_buf, &datalen_, sizeof(datalen_));
    if (0 == datalen_) {
        return true;
    }

    out_buf += sizeof(datalen_);
    memcpy(out_buf, data_, datalen_);
    return true;

}

bool PACK_OPEN_CHANNEL_REQ::unserial(char *in_buf, uint16_t in_buf_len)
{
    memcpy(&user_id_, in_buf, sizeof(user_id_));
    in_buf += sizeof(user_id_);
    memcpy(&echo_data_, in_buf, sizeof(echo_data_));
    in_buf += sizeof(echo_data_);
    memcpy(&datalen_, in_buf, sizeof(datalen_));
    if (1000 < datalen_) {
        return false;
    }
    if (0 == datalen_) {
        return true;
    }

    in_buf += sizeof(datalen_);
    memcpy(data_, in_buf, datalen_);
    return true;
}

PACK_OPEN_CHANNEL_RESP::PACK_OPEN_CHANNEL_RESP()
{
    memset(this, 0, sizeof(PACK_OPEN_CHANNEL_RESP));
}

bool PACK_OPEN_CHANNEL_RESP::serial(char *out_buf, uint16_t out_buf_len, uint16_t &data_len, uint16_t &cmd_id)
{
    cmd_id = PACK_CMD_ID_OPEN_CHANNEL_RESP;
    data_len = sizeof(success_) + sizeof(echo_data_);
    if (out_buf_len < data_len) {
        return false;
    }
    memcpy(out_buf, &success_, sizeof(success_));
    out_buf += sizeof(success_);
    memcpy(out_buf, &echo_data_, sizeof(echo_data_));
    return true;
}

bool PACK_OPEN_CHANNEL_RESP::unserial(char *in_buf, uint16_t in_buf_len)
{
    if (in_buf_len < (sizeof(success_) + sizeof(echo_data_))) {
        return false;
    }
    memcpy(&success_, in_buf, sizeof(success_));
    in_buf += sizeof(success_);
    memcpy(&echo_data_, in_buf, sizeof(echo_data_));
    return true;
}

PACK_STUN_REQ::PACK_STUN_REQ()
{
	memset(this, 0, sizeof(PACK_STUN_REQ));
}
bool PACK_STUN_REQ::serial(char* out_buf, uint16_t out_buf_len, uint16_t& data_len, uint16_t& cmd_id)
{
    char *tmp = out_buf;
    cmd_id = PACK_CMD_ID_STUN_MESSAGE;
    data_len = sizeof(uint32_t) + sizeof(uint64_t) + 32 * sizeof(char);
    if (out_buf_len < data_len)
    {
        return false;
    }

    memcpy(tmp, &id, sizeof(uint32_t));
    tmp += sizeof(uint32_t);
    memcpy(tmp, &timestamp, sizeof(uint64_t));
    tmp += sizeof(uint64_t);
    memcpy(tmp, token, 32 * sizeof(char));

    return true;
}

bool PACK_STUN_REQ::unserial(char* in_buf, uint16_t in_buf_len)
{
    char *tmp = in_buf;
    if (in_buf_len < sizeof(uint32_t) + sizeof(uint64_t) + 32 * sizeof(char))
    {
        return false;
    }

    memcpy(&id, tmp, sizeof(uint32_t));
    tmp += sizeof(uint32_t);
    memcpy(&timestamp, tmp, sizeof(uint64_t));
    tmp += sizeof(uint64_t);
    memcpy(token, tmp, 32 * sizeof(char));

    return true;
}

bool PACK_STUN_RESP::serial(char* out_buf, uint16_t out_buf_len, uint16_t& data_len, uint16_t& cmd_id)
{
    char *tmp = out_buf;
    cmd_id = PACK_CMD_ID_STUN_MESSAGE;
    data_len = sizeof(uint32_t) + sizeof(uint64_t) + sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t) + 32 * sizeof(char);
    if (out_buf_len < data_len)
    {
        return false;
    }
    
    memcpy(tmp, &id, sizeof (uint32_t));
    tmp += sizeof(uint32_t);
    memcpy(tmp, &timestamp, sizeof (uint64_t));
    tmp += sizeof(uint64_t);
    memcpy(tmp, &mapped_ip, sizeof(uint32_t));
    tmp += sizeof(uint32_t);
    memcpy(tmp, &mapped_port, sizeof(uint16_t));
    tmp += sizeof(uint16_t);
    *tmp = error;
    tmp++;
    memcpy(tmp, token, 32 * sizeof(char));

    return true;
}

bool PACK_STUN_RESP::unserial(char* in_buf, uint16_t in_buf_len)
{
    char *tmp = in_buf;
    if (in_buf_len < sizeof(uint32_t) + sizeof(uint64_t) + sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t) + 32 * sizeof(char))
    {
        return false;
    }

    memcpy(&id, tmp, sizeof(uint32_t));
    tmp += sizeof(uint32_t);
    memcpy(&timestamp, tmp, sizeof(uint64_t));
    tmp += sizeof(uint64_t);
    memcpy(&mapped_ip, tmp, sizeof(uint32_t));
    tmp += sizeof(uint32_t);
    memcpy(&mapped_port, tmp, sizeof(uint16_t));
    tmp += sizeof(uint16_t);
    error = *(uint8_t*)tmp;
    tmp++;
    memcpy(token, tmp, 32 * sizeof(char));

    return true;
}


PACK_LOCAL_NODE_LST::PACK_LOCAL_NODE_LST()
{
    memset(this, 0, sizeof(PACK_LOCAL_NODE_LST));
}

bool PACK_LOCAL_NODE_LST::serial(char* out_buf, uint16_t out_buf_len, uint16_t& data_len, uint16_t& cmd_id)
{
    uint16_t len = sizeof(uint16_t) + sizeof(uint32_t)* user_cnt_;
    if (len > out_buf_len)
    {
        return false;
    }

    cmd_id = PACK_CMD_ID_LOCAL_NODE_LST;
    data_len = len;

    char *tmp = out_buf;
    memcpy(tmp, &user_cnt_, sizeof(uint16_t));
    tmp += sizeof(uint16_t);
    memcpy(tmp, users_lst_, sizeof(uint32_t)* user_cnt_);
    return true;
}

bool PACK_LOCAL_NODE_LST::unserial(char* in_buf, uint16_t in_buf_len)
{
    char *tmp = in_buf;
    memcpy(&user_cnt_, tmp, sizeof(uint16_t));
    if (in_buf_len < sizeof(uint32_t)* user_cnt_ + sizeof(uint16_t))
    {
        return false;
    }
    tmp += sizeof(uint16_t);
    memcpy(users_lst_, tmp, sizeof(uint32_t)* user_cnt_);
    return true;
}

PACK_SPEAKER_RTCP::PACK_SPEAKER_RTCP()
{
    memset(this, 0, sizeof(PACK_SPEAKER_RTCP));
}

bool PACK_SPEAKER_RTCP::serial(char *out_buf, uint16_t out_buf_len, uint16_t &data_len, uint16_t &cmd_id)
{
    //输出cmd_id
    cmd_id = PACK_CMD_ID_RTCP_FROM_SPEAKER;
    if (1400 < pack_len_) {
        return false;
    }

    data_len = 0;
    data_len += sizeof(media_type_);
    data_len += sizeof(channel_id_);
    data_len += sizeof(src_id_);
    data_len += sizeof(uint8_t);
    data_len += sizeof(pack_len_);
    data_len += pack_len_;
    if (out_buf_len < data_len)
    {
        return false;
    }

    memcpy(out_buf, &media_type_, sizeof(media_type_));
    out_buf += sizeof(media_type_);

    memcpy(out_buf, &channel_id_, sizeof(channel_id_));
    out_buf += sizeof(channel_id_);

    memcpy(out_buf, &src_id_, sizeof(src_id_));
    out_buf += sizeof(src_id_);

    uint8_t one_byte = codec_id_ << 2 | LL_;
    memcpy(out_buf, &one_byte, sizeof(one_byte));
    out_buf += sizeof(uint8_t);

    memcpy(out_buf, &pack_len_, sizeof(pack_len_));
    if (0 == pack_len_) {
        return true;
    }

    out_buf += sizeof(pack_len_);
    memcpy(out_buf, pack_, pack_len_);
    return true;

}

bool PACK_SPEAKER_RTCP::unserial(char *in_buf, uint16_t in_buf_len)
{
    memcpy(&media_type_, in_buf, sizeof(media_type_));
    in_buf += sizeof(media_type_);

    memcpy(&channel_id_, in_buf, sizeof(channel_id_));
    in_buf += sizeof(channel_id_);

    memcpy(&src_id_, in_buf, sizeof(src_id_));
    in_buf += sizeof(src_id_);

    uint8_t one_byte;
    memcpy(&one_byte, in_buf, sizeof(one_byte));
    codec_id_ = one_byte >> 2;
    LL_ = one_byte & 0x03;
    in_buf += sizeof(uint8_t);

    memcpy(&pack_len_, in_buf, sizeof(pack_len_));
    if (1400 < pack_len_) {
        return false;
    }
    if (0 == pack_len_) {
        return true;
    }

    in_buf += sizeof(pack_len_);
    memcpy(pack_, in_buf, pack_len_);
    return true;
}
