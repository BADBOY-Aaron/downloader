#include "audio_manager.h"
#include <cstring>
#include <sstream>
namespace am {
const int MAX_FILE_BUFFER_LEN = 268435456; // 256M
const int MAX_RETRY_COUNT = 10;

AudioManager::AudioManager():m_msg("") {
	m_downloadBuffer = new char[MAX_FILE_BUFFER_LEN];
    memset(m_downloadBuffer, 0, MAX_FILE_BUFFER_LEN);
}

AudioManager::~AudioManager() {
	if(m_downloadBuffer) {
		delete[] m_downloadBuffer;
        m_downloadBuffer = nullptr;
	}
}

/// Callback must be declared static, otherwise it won't link...
static size_t WriteCallback(char* ptr, size_t size, size_t nmemb, void *userData)
{
	AudioManager* audioManager = static_cast<AudioManager*>(userData);
	audioManager->writeData(ptr, size*nmemb);
	return size*nmemb;
};

bool AudioManager::downLoadFile(std::string url) {
    if(url.empty()) {
		m_code = 404;
		m_msg = "url is empty";
        return false;
    }
	long file_size = getDownloadFileLenth(url.c_str());
    memset(m_downloadBuffer, 0, MAX_FILE_BUFFER_LEN);
    m_downloadLen = 0;

    bool ret = true;
    int count = 0;
    do
    {
		CURL * curl_handle = nullptr;
		
		/* init the curl session */ 
		curl_handle = curl_easy_init();

		/*设置DNS共享，减少非必要dns查询*/
		//curl_easy_setopt(curl_handle, CURLOPT_SHARE, &CurlDNSShareObj::getInstance());
		//curl_easy_setopt(curl_handle, CURLOPT_DNS_CACHE_TIMEOUT, 60 * 10);

		/* set URL to get here */ 
		curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());

		/* Switch on full protocol/debug output while testing */ 
		curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 0L);

		/* disable progress meter, set to 0L to enable and disable debug output */ 
		curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 20*60);
		/*设置最大接收速度，限制网速 600KB/S*/
		curl_easy_setopt(curl_handle, CURLOPT_MAX_RECV_SPEED_LARGE, (curl_off_t)(600 * 1024));

		/* send all data to this function  */ 
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, static_cast<void *>(this));

		LOG(INFO) << "start download file : " << url << " file size : " << file_size;
	    
		//执行请求
		CURLcode res = CURLE_OK;
		res = curl_easy_perform(curl_handle);
	    
		double namelookuptime = 0.0f;
		CURLcode nameLookUpRet = CURLE_OK;
	    nameLookUpRet = curl_easy_getinfo(curl_handle, CURLINFO_NAMELOOKUP_TIME , &namelookuptime);
	    if (CURLE_OK != nameLookUpRet)
		{
			LOG(INFO) << "get DNS time failed, return code " << nameLookUpRet << "info = " << curl_easy_strerror(nameLookUpRet);
		}

		double totalTime = 0.0f;
		if((CURLE_OK == curl_easy_getinfo(curl_handle,CURLINFO_TOTAL_TIME, &totalTime)))
		{
			LOG(INFO) << "url : " << url << " file size : "<< file_size 
                << " download size : " << m_downloadLen
				<< " ,total take time = "<< (static_cast<long>(totalTime) % 1000) << " ms"
				<< " ,DNS take time = "<< (static_cast<long>(namelookuptime) % 1000) << " ms";
        } else {
			LOG(INFO) << "get total time failed, url : " << url << " file size : "<< file_size 
                << " download size : " << m_downloadLen
				<< " ,total take time = "<< (static_cast<long>(totalTime) % 1000) << " ms"
				<< " ,DNS take time = "<< (static_cast<long>(namelookuptime) % 1000) << " ms";
		
		}
		
		if(res != CURLE_OK){
			std::ostringstream buffer;
			buffer << "down load failed, return code " << res << "info = " << curl_easy_strerror(res);
			m_msg = buffer.str();
			ret = false;
		} else {
			long code = 0L;
			CURLcode getResponseCodeRet = CURLE_OK;
			getResponseCodeRet = curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE , &code);

			if (CURLE_OK != getResponseCodeRet) {
				LOG(INFO) << "curl_easy_getinfo failed ret code= " <<  getResponseCodeRet 
					<< ", info = " <<  curl_easy_strerror(getResponseCodeRet);
			}

			m_code = code;
			
			switch(m_code) {
				case 200:
				case 206:
				case 304:
					{
						ret = true;
						break;
					}
				default:
					{
						std::ostringstream buffer;
						buffer << "down load failed, code = " << m_code << " info = " << std::string(m_downloadBuffer);
						m_msg = buffer.str();
						ret = false;
						break;
					}
			}
		}

		count++;
		if (ret == false) {
			LOG(ERROR) << "down load file failed code = "<< m_code <<" count = " << count;
		} else {
			LOG(ERROR) << "down load file success code = "<< m_code <<" count = " << count;
			m_msg = "success";
		}
        curl_easy_cleanup(curl_handle);
	}while((m_code == 200 || m_code == 206 || m_code == 304) && (ret == false && count < 10));
	
	return ret;
}

void AudioManager::writeData(char* data, int length) {
	if (data == nullptr || length <= 0) {
		return;
	}

	do {
		if(m_downloadLen >= MAX_FILE_BUFFER_LEN) {
			LOG(ERROR) << "file too large : more than 512M!!!";
			break;
		}

		if (m_downloadLen + length > MAX_FILE_BUFFER_LEN) {
			length = MAX_FILE_BUFFER_LEN - m_downloadLen;
		}

		memcpy(m_downloadBuffer + m_downloadLen, data, length);
		m_downloadLen += length;
	} while(0);
	return;
}

const char* AudioManager::getAudioData() {
	return m_downloadBuffer;
}

unsigned int AudioManager::getAudioSize() {
	return m_downloadLen;
}

std::string AudioManager::getErrorInfo() {
	return m_msg;
}

static size_t HeadCallback(char* ptr, size_t size, size_t nmemb, void *userData)
{
	return size*nmemb;
};

long AudioManager::getDownloadFileLenth(const char *url){
	double downloadFileLenth = 0.0f;
	CURL *handle = curl_easy_init();
	curl_easy_setopt(handle, CURLOPT_URL, url);
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, HeadCallback);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, nullptr);
	curl_easy_setopt(handle, CURLOPT_HEADER, 1);    //只需要header头
	curl_easy_setopt(handle, CURLOPT_NOBODY, 1);    //不需要body
	curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(handle, CURLOPT_TIMEOUT, 2*60);
	curl_easy_setopt(handle, CURLOPT_VERBOSE, 0L);
	if (curl_easy_perform(handle) == CURLE_OK) {
		curl_easy_getinfo(handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &downloadFileLenth);
	} else {
		downloadFileLenth = -1;
	}
	curl_easy_cleanup(handle);
	return static_cast<long>(downloadFileLenth);
}

}// namespce kafka
