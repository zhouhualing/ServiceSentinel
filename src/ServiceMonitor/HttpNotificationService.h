#ifndef __Http_Notification_Service_H__
#define __Http_Notification_Service_H__

class CMutex;
class CHttpNotificationService
{
public:
    static CHttpNotificationService& Instance();
	void Initialize();
	void Publish(const std::string& url, const std::string& jsonData, const std::string method);
	~CHttpNotificationService();
private:
	CHttpNotificationService();

    typedef std::vector<std::string> Urls_t;

	void DefaultCurlSendHandler(const std::string method, const std::string& url, const std::string& jsonData);
};

#endif /* __Http_Notification_Service_H__ */

