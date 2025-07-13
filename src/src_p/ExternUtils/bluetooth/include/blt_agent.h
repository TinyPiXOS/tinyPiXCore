#ifndef _BLT_AGENT_H_
#define _BLT_AGENT_H_

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct BluetAgent_ BluetAgent;
typedef struct BluetAgentPrivate_ BluetAgentPrivate;

struct BluetAgent_{
	BluetAgentPrivate *priv;
};

BluetAgent *bluet_agent_creat();
int bluet_agent_delete(BluetAgent *self);
int bluet_agent();

#ifdef	__cplusplus
}
#endif

#endif