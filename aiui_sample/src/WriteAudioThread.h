
#ifndef WRITEAUDIOTHREAD_H_
#define WRITEAUDIOTHREAD_H_

#include "aiui/AIUI.h"
#include <iostream>
#include "FileUtil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//写test.pcm测试音频线程
class WriteAudioThread
{
private:
	IAIUIAgent* mAgent;
	string mAudioPath;
	bool mRepeat;
	bool mRun;
	FileUtil::DataFileHelper* mFileHelper;
  pthread_t thread_id;
  bool thread_created;

private:
	bool threadLoop();
	static void* thread_proc(void * paramptr);
public:
	WriteAudioThread(IAIUIAgent* agent, const string& audioPath, bool repeat);
	~WriteAudioThread();
	void stopRun();
	bool run();
};

#endif /* WRITEAUDIOTHREAD_H_ */