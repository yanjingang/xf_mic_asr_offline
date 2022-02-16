#include <WriteAudioThread.h>

bool WriteAudioThread::threadLoop()
{
	char audio[1280];
	int len = mFileHelper->read(audio, 1280);

	if (len > 0)
	{
		Buffer* buffer = Buffer::alloc(len);//申请的内存会在sdk内部释放
		memcpy(buffer->data(), audio, len);

		IAIUIMessage * writeMsg = IAIUIMessage::create(AIUIConstant::CMD_WRITE,
			0, 0,  "data_type=audio,sample_rate=16000", buffer);	

		if (NULL != mAgent)
		{
			mAgent->sendMessage(writeMsg);
		}		
		writeMsg->destroy();
		usleep(40 * 1000);
	} else {
		if (mRepeat)
		{
			mFileHelper->rewindReadFile();
		} else {
			IAIUIMessage * stopWrite = IAIUIMessage::create(AIUIConstant::CMD_STOP_WRITE,
				0, 0, "data_type=audio,sample_rate=16000");

			if (NULL != mAgent)
			{
				mAgent->sendMessage(stopWrite);
			}
			stopWrite->destroy();

			mFileHelper->closeReadFile();
			mRun = false;
		}
	}

	return mRun;
}

void* WriteAudioThread::thread_proc(void * paramptr)
{
	WriteAudioThread * self = (WriteAudioThread *)paramptr;

	while (1) {
		if (! self->threadLoop())
			break;
	}
	return NULL;
}

WriteAudioThread::WriteAudioThread(IAIUIAgent* agent, const string& audioPath, bool repeat):
mAgent(agent), mAudioPath(audioPath), mRepeat(repeat), mRun(true), mFileHelper(NULL)
,thread_created(false)
{
	mFileHelper = new FileUtil::DataFileHelper("");
	mFileHelper->openReadFile(mAudioPath, false);
}

WriteAudioThread::~WriteAudioThread()
{
	if (NULL != mFileHelper)
	{
		delete mFileHelper;
		mFileHelper = NULL;
	}
}

void WriteAudioThread::stopRun()
{
    if (thread_created) {
        mRun = false;
        void * retarg;
        pthread_join(thread_id, &retarg);
        thread_created = false;
    }
}

bool WriteAudioThread::run()
{
    if (thread_created == false) {
        int rc = pthread_create(&thread_id, NULL, thread_proc, this);
        if (rc != 0) {
            exit(-1);
        }
        thread_created = true;
        return true;
    }

    return false;
}