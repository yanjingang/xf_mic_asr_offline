#ifndef TESTLISTENER_H_
#define TESTLISTENER_H_
#include "aiui/AIUI.h"
#include "FileUtil.h"

class TestListener : public IAIUIListener
{
private:
	FileUtil::DataFileHelper* mTtsFileHelper;

public:
	void onEvent(const IAIUIEvent& event) const;

	TestListener();

	~TestListener();
};

#endif //TESTLISTENER_H_