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
#include "audio_manager.h"
#include <thread>
using namespace am;
using namespace std;

int main() {
	curl_global_init(CURL_GLOBAL_ALL);
	auto mylamthread = [] {//这是一个lambda表达式
		auto d = make_shared<AudioManager>();
		d->downLoadFile("http://file.ljcdn.com/psd-sinan-file/loadVoice_0923ad673360dbf02e986400dabd7da6.wav");
		std::cout << "---------" << d->getAudioSize() << std::endl;
		std::cout << "---------" << d->getErrorInfo() << std::endl;
	};

	thread mythread1(mylamthread);
	mythread1.join();
	curl_global_cleanup();
	return 0;
}
