#ifndef AUDIORECORDER_H_
#define AUDIORECORDER_H_

#include "hidapi.h"
#include <iostream>

class AudioRecorder
{
private:
	std::string mAudioPath;
	static hid_device *handle;
public:

	AudioRecorder(const std::string& audioPath);
	~AudioRecorder();
	bool startRecord();
	void stopRecord();
    bool startRecordOriginal();
	void stopRecordOriginal();
	bool setRecordAngle(int angle);

	static bool if_success_boot;
	static bool if_awake;
	static int business_proc_callback(business_msg_t businessMsg);
};

#endif /* AUDIORECORDER_H_ */
