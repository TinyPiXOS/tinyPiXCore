#include "tpInputMethod.h"

int32_t main(int32_t argc, char *argv[])
{
	tpInputMethod *inputMethod = new tpInputMethod();

	inputMethod->inputKeySequence("zhangsan");

	tpVector<tpString> curPageList = inputMethod->currentPageCandidates();
	for (int i = 0; i < curPageList.size(); ++i)
	{
		std::cout << i << ": " << curPageList.at(i) << std::endl;
	}

	while (inputMethod->hasNext())
	{
		inputMethod->nextPage();

		tpVector<tpString> curPageList = inputMethod->currentPageCandidates();
		for (int i = 0; i < curPageList.size(); ++i)
		{
			std::cout << i << ": " << curPageList.at(i) << std::endl;
		}
	}


	// delete inputMethod;
	// inputMethod = nullptr;

	return 0;
}
