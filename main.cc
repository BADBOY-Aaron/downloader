#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <curl/curl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <vector>
#include "downloader.h"
#include <thread>
using namespace download;
using namespace std;

int main() {
	curl_global_init(CURL_GLOBAL_ALL);
	auto mylamthread = [] {//这是一个lambda表达式
		auto d = make_shared<Downloader>();
		d->downLoadFile("magnet:?xt=urn:btih:a660b6f7f6aeb4e9a2f5451cd240c73d514a3636&dn=[%E7%94%B5%E5%BD%B1%E5%A4%A9%E5%A0%82www.dytt89.com]%E8%A1%80%E6%88%98%E6%91%A9%E8%8B%8F%E5%B0%94BD%E4%B8%AD%E5%AD%97.mp4&tr=http://t.t789.me:2710/announce&tr=http://t.t789.co:2710/announce&tr=http://t.t789.vip:2710/announce");
		std::cout << "---------" << d->getAudioSize() << std::endl;
		std::cout << "---------" << d->getErrorInfo() << std::endl;
	};

	thread mythread1(mylamthread);
	mythread1.join();
	curl_global_cleanup();
	return 0;
}
