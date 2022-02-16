#ifndef AIUIAGENTTESTER_H_
#define AIUIAGENTTESTER_H_

#include <aiui/AIUI.h>
#include <FileUtil.h>
#include <WriteAudioThread.h>
#include <AudioRecorder.h>
#include <TestListener.h>
#include <AudioPlayer.h>
#include <msp_cmn.h>


class AIUITester
{
private:
	IAIUIAgent* agent;//aiui代理
	TestListener listener;
	AudioRecorder* audioRecorder;//与录音相关的
	AudioPlayer* audioPlayer;//与音频播放相关
public:
	AIUITester() ;
	~AIUITester();
private:
	void showIntroduction(bool detail);
	//创建AIUI 代理，通过AIUI代理与SDK发送消息通信
	void createAgent();
	//唤醒接口
	void wakeup();

	//开始AIUI，调用stop()之后需要调用此接口才可以与AIUI继续交互
	void start();
	//停止AIUI
	void stop();

	//usb麦克风设备创建
    void recorder_creat();
	//麦克风录音开始
	void recorder_start();
	//麦克风录音结束
	void recorder_stop();

	void stopWriteThread();

	void reset();

	void destory();

	void buildGrammar();

	void updateLocalLexicon();
	// void gTTS();

public:
	void readCmd();
	void test();
};

#endif /* AIUIAGENTTESTER_H_ */