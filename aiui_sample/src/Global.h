#ifndef GLOBAL_AIUI_H_
#define GLOBAL_AIUI_H_

#include <iostream>
#include "aiui/AIUI.h"
#include "FileUtil.h"
#include <AudioPlayer.h>
IAIUIAgent* globalAgent;
AudioPlayer* globalAudioPlayer;
bool if_print_proc_log = false;
bool if_print_event_log = false;
bool if_save_record_file = false; //是否保存音频到本地
bool wait_for_awake_word = false;
bool no_tts = false;
// class Global
// {
// private:

// public:
// 	Global();
// 	~Global();
// 	// virtual bool startRecord();
// 	// virtual void stopRecord();

// 	static IAIUIAgent* sAIUIAgent;
	


// };

#endif /* GLOBAL_AIUI_H_ */
