#include "TpInputMethod.h"

int32_t main(int32_t argc, char *argv[])
{
	TpInputMethod *inputMethod = new TpInputMethod();

	inputMethod->inputKeySequence("zhangsan");

	TpVector<TpString> curPageList = inputMethod->currentPageCandidates();
	for (int i = 0; i < curPageList.size(); ++i)
	{
		std::cout << i << ": " << curPageList.at(i) << std::endl;
	}

	while (inputMethod->hasNext())
	{
		inputMethod->nextPage();

		TpVector<TpString> curPageList = inputMethod->currentPageCandidates();
		for (int i = 0; i < curPageList.size(); ++i)
		{
			std::cout << i << ": " << curPageList.at(i) << std::endl;
		}
	}


	// delete inputMethod;
	// inputMethod = nullptr;

	return 0;
}
