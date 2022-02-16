#include <TestListener.h>

TestListener::TestListener()
{
	mTtsFileHelper = new FileUtil::DataFileHelper("");
}

TestListener::~TestListener()
{
	if (mTtsFileHelper != NULL)
	{
		delete mTtsFileHelper;
		mTtsFileHelper = NULL;
	}
}