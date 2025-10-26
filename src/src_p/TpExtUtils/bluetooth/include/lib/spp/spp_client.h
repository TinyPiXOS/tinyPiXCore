#ifndef __SPP_AGENT_H
#define __SPP_AGENT_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>

typedef struct BluetSppClient_ BluetSppClient;
typedef struct BluetSppClientPrivate_ BluetSppClientPrivate;

struct BluetSppClient_{
	BluetSppClientPrivate *priv;
};





#ifdef	__cplusplus
}
#endif

#endif
