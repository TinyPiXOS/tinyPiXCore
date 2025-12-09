
#include "TpMediaDevice.h"
#include "TpSound.h"

TpString TpMediaDevice::getFormatName(const TpString& audio_name)
{
	TpString usedAudioDev;
	if(audio_name == TpString("default"))
		usedAudioDev=TpSound::getUsedDevice();
	else
		usedAudioDev=audio_name;
    size_t pos = usedAudioDev.find(' ');      			// 查找第一个空格位置
	if (pos == std::string::npos) // 无空格时返回整个字符串
        return usedAudioDev;
	else
   		return usedAudioDev.substr(0, pos);      // 截取开头到空格前的部分
}
