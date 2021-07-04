#ifndef DOWNLOADER_H_
#define DOWNLOADER_H_
#include <glog/logging.h>
#include <curl/curl.h>
#include<string>
namespace download {
class CurlDNSShareObj {
	public:
		static CurlDNSShareObj& getInstance() {
			static CurlDNSShareObj obj;
			return obj;
		}

		CurlDNSShareObj():share_handle(curl_share_init()) {
			if (share_handle) {
				LOG(INFO) << "初始化 curl shared handle : ";
				curl_share_setopt(share_handle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
			}
		}
		~CurlDNSShareObj() {
			LOG(INFO) << "清理 curl shared handle : ";
			curl_share_cleanup(share_handle);
			share_handle = nullptr;
		}
	private:
		CURLSH* share_handle = nullptr;

};

class Downloader{
	public:
		Downloader();
		~Downloader();

		void writeData(char* data, int length);
		const char* getAudioData();
		unsigned int getAudioSize();
		std::string getErrorInfo();
		bool downLoadFile(std::string url);
	private:
		long getDownloadFileLenth(const char* url);
	private:
		char* m_downloadBuffer = nullptr;
		unsigned int m_downloadLen = 0;
		long m_code = 0L;
		std::string m_msg = "";
};


} //namespace
#endif //DOWNLOADWE_H_

