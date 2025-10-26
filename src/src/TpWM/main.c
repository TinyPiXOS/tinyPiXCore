#include "pixwm.h"

int main(int argc, char *argv[])
{
	tpBool ret = pixwm_init(argc, argv);

	if(ret == TP_FALSE){
		return TP_INVALIDATE_VALUE;
	}

	PiXWMConfigs *configs = pixwm_configs();

	if(configs->inited == TP_FALSE){
		return TP_FALSE;
	}

	return pixwm_start(configs);
}
