#include <AudioRecorder.h>

using namespace std;
bool AudioRecorder::if_success_boot;
bool AudioRecorder::if_awake;
hid_device *AudioRecorder::handle;
AudioRecorder::AudioRecorder(const string& audioPath)
{
	//cout << "AudioRecoder creat begin!" << endl;
        if_success_boot = false;
	if_awake        = false;
	mAudioPath = audioPath;
	handle = hid_open();

	if (!handle)
	{
		printf(">>>>>无法打开麦克风设备，设备被占用\n");
		//return -1;
	}
	printf(">>>>>成功打开麦克风设备\n");

	protocol_proc_init(send_to_usb_device, recv_from_usb_device, business_proc_callback, err_proc);
	get_system_status();

	sleep(1);
	if (!if_success_boot)
	{
		printf(">>>>>开机中，请稍等！\n");
	}
	while (!if_success_boot)
	{
		if (is_reboot)
		{
			break;
		}
	}
	printf(">>>>>开机成功！\n");
}

AudioRecorder::~AudioRecorder()
{
    cout << ">>>>>销毁AIUI代理!\n" << endl;
    finish_to_record_denoised_sound();
    hid_close();
    cout << ">>>>>停止所有录音\n" << endl;
}

bool AudioRecorder::startRecord(){
    start_to_record_denoised_sound();
    cout << ">>>>>开始录降噪音频\n" << endl;
    return true;
}

void AudioRecorder::stopRecord(){
    finish_to_record_denoised_sound();
    cout << ">>>>>停止录降噪音频\n" << endl;
}

bool AudioRecorder::startRecordOriginal(){

    start_to_record_original_sound();
    cout << ">>>>>开始录音原始音频\n" << endl;
    return true;
}
void AudioRecorder::stopRecordOriginal(){
    finish_to_record_original_sound();
    cout << ">>>>>停止录原始音频\n" << endl;
}

bool AudioRecorder::setRecordAngle(int angle){
    if(!if_success_boot)
    {
        return false;
    }
    //set_mic_angle(angle);
    return true;
}


