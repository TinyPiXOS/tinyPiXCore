#ifndef __SPP_AGENT_H
#define __SPP_AGENT_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>

typedef struct BluetSppAgent_ BluetSppAgent;
typedef struct BluetSppAgentPrivate_ BluetSppAgentPrivate;

struct BluetSppAgent_{
	struct BluetSppAgentPrivate *priv;
};





#ifdef	__cplusplus
}
#endif

#endif
