
#ifndef LIST_CONFERENCE_H
#define LIST_CONFERENCE_H

#include <iostream>
#include "DBconnector.h"

using std::string;

#ifdef LINUX

#endif

class CConference
{
public:
	CConference();
	virtual ~CConference();
	//static CConference* GetInstance();

	bool GetExist() { return m_exist; }
	//
	unsigned long long GetConfID() { return m_conf_id; }
	unsigned int GetUserID() { return m_user_id; }
	std::string GetConfPasswd() { return m_conf_passwd; }
	std::string GetRecvPasswd() { return m_recv_passwd; }
	std::string GetConfSubject() { return m_conf_subject; }
	std::string GetCreateData() { return m_create_date; }
	int GetConfType() { return m_conf_type; }
	unsigned int GetConfChairman() { return m_conf_chairman; }
	int GetConfStartType() { return m_conf_starttype; }
	std::string GetConfStartTime() { return m_conf_starttime; }
	std::string GetConfEndTime() { return m_conf_endtime; }
	int GetConfMaxterm() { return m_conf_maxterm; }
	int GetAudioEnable() { return m_audio_enable; }
	int GetVideoEnable() { return m_video_enable; }
	int GetPermanentEnable() { return m_permanent_enable; }
	int GetIsDeleted() { return m_isdeleted; }
	int GetCtrlUserID() { return m_ctrl_user_id; }
	std::string GetCtrlPWD() { return m_ctrl_pwd; }
	int GetAnonymousEnable() { return m_anonymous_enable; }
	std::string GetConfName() { return m_conf_name; }
	int GetEncryptAlg() { return m_encrypt_alg; }
	int GetAutoREC() { return m_auto_rec; }
	std::string GetConfRealStartTime() { return m_conf_realstarttime; }
	std::string GetConfRealEndTime() { return m_conf_realendtime; }
	int GetConfStatus() { return m_conf_status; }
	int GetVideoSize() { return m_video_size; }

	//
	void SetConfID(unsigned long long conf_id) { m_conf_id = conf_id; }
	void SetUserID(unsigned int user_id) { m_user_id = user_id; }
	void SetConfPasswd(std::string conf_passwd) { m_conf_passwd = conf_passwd; }
	void SetRecvPasswd(std::string recv_passwd) { m_recv_passwd = recv_passwd; }
	void SetConfSubject(std::string conf_subject) { m_conf_subject = conf_subject; }
	void SetCreateData(std::string create_date) { m_create_date = create_date; }
	void SetConfType(int conf_type) { m_conf_type = conf_type; }
	void SetConfChairman(unsigned int conf_chairman) { m_conf_chairman = conf_chairman; }
	void SetConfStartType(int conf_starttype) { m_conf_starttype = conf_starttype; }
	void SetConfStartTime(std::string conf_starttime) { m_conf_starttime = conf_starttime; }
	void SetConfEndTime(std::string conf_endtime) { m_conf_endtime = conf_endtime; }
	void SetConfMaxterm(int conf_maxterm) { m_conf_maxterm = conf_maxterm; }
	void SetAudioEnable(int audio_enable) { m_audio_enable = audio_enable; }
	void SetVideoEnable(int video_enable) { m_video_enable = video_enable; }
	void SetPermanentEnable(int permanent_enable) { m_permanent_enable = permanent_enable; }
	void SetIsDeleted(int isdeleted) { m_isdeleted = isdeleted; }
	void SetCtrlUserID(int ctrl_user_id) { m_ctrl_user_id = ctrl_user_id; }
	void SetCtrlPWD(std::string ctrl_pwd) { m_ctrl_pwd = ctrl_pwd; }
	void SetAnonymousEnable(int anonymous_enable) { m_anonymous_enable = anonymous_enable; }
	void SetConfName(std::string conf_name) { m_conf_name = conf_name; }
	void SetEncryptAlg(int encrypt_alg) { m_encrypt_alg = encrypt_alg; }
	void SetConfRealStartTime(std::string conf_realstarttime) { m_conf_realstarttime = conf_realstarttime; }
	void SetConfRealEndTime(std::string conf_realendtime) { m_conf_realendtime = conf_realendtime; }
	void SetAutoREC(int auto_rec) { m_auto_rec = auto_rec; }
	void SetConfStatus(int conf_status) { m_conf_status = conf_status; }
	void SetVideoSize(int video_size) { m_video_size = video_size; }

	//
	bool InsertDB();
	bool DeleteDB();
	bool UpdateDB(int realtimetype);
	bool SelectDB();

protected:
	//CConference();

private:
	bool m_exist;
	unsigned long long m_conf_id;
	unsigned int m_user_id;
	std::string m_conf_passwd;
	std::string m_recv_passwd;
	std::string m_conf_subject;
	std::string m_create_date;
	int m_conf_type;
	unsigned int m_conf_chairman;
	int m_conf_starttype;
	std::string m_conf_starttime;
	std::string m_conf_endtime;
	int m_conf_maxterm;
	int m_audio_enable;
	int m_video_enable;
	int m_permanent_enable;
	int m_isdeleted;
	int m_ctrl_user_id;
	std::string m_ctrl_pwd;
	int m_anonymous_enable;
	std::string m_conf_name;
	int m_encrypt_alg;
	int m_device_id;


	std::string m_conf_realstarttime;
	std::string m_conf_realendtime;
	int m_auto_rec;
	int m_conf_status;
	int m_video_size;//期望MP编码流的大小(目前只有录制直播屏设置),0：不看、非零：分辨率大小(1:160x90、2:256x144、3:320x180、4:384x216、5:432x240、6:480x270、7:640x360、8:864x480、9:960x540、10:1280x720、11:1920x1080)

	CCriticalSection m_csConf;
};

#endif //LIST_CONFERENCE_H