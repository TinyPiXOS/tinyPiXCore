/* 
 * generated by definetoc /usr/local/include/openobex/obex_const.h ...
 */

#include <openobex/obex.h>
#include "obexnames.h"

#ifdef NULL
#else
#define NULL (void *)0
#endif

define obex_modenames[] = {
	{"OBEX_MODE_CLIENT", OBEX_MODE_CLIENT},
	{"OBEX_MODE_SERVER", OBEX_MODE_SERVER},
	{NULL, 0}
};

define obex_evnames[] = {
	{"OBEX_EV_PROGRESS", OBEX_EV_PROGRESS},
	{"OBEX_EV_REQHINT", OBEX_EV_REQHINT},
	{"OBEX_EV_REQ", OBEX_EV_REQ},
	{"OBEX_EV_REQDONE", OBEX_EV_REQDONE},
	{"OBEX_EV_LINKERR", OBEX_EV_LINKERR},
	{"OBEX_EV_PARSEERR", OBEX_EV_PARSEERR},
	{"OBEX_EV_ACCEPTHINT", OBEX_EV_ACCEPTHINT},
	{"OBEX_EV_ABORT", OBEX_EV_ABORT},
	{"OBEX_EV_STREAMEMPTY", OBEX_EV_STREAMEMPTY},
	{"OBEX_EV_STREAMAVAIL", OBEX_EV_STREAMAVAIL},
	{"OBEX_EV_UNEXPECTED", OBEX_EV_UNEXPECTED},
	{"OBEX_EV_REQCHECK", OBEX_EV_REQCHECK},
	{NULL, 0}
};

define obex_hdrnames[] = {
	{"OBEX_HDR_ID_COUNT", OBEX_HDR_ID_COUNT},
	{"OBEX_HDR_ID_NAME", OBEX_HDR_ID_NAME},
	{"OBEX_HDR_ID_TYPE", OBEX_HDR_ID_TYPE},
	{"OBEX_HDR_ID_LENGTH", OBEX_HDR_ID_LENGTH},
	{"OBEX_HDR_ID_TIME", OBEX_HDR_ID_TIME},
	{"OBEX_HDR_ID_DESCRIPTION", OBEX_HDR_ID_DESCRIPTION},
	{"OBEX_HDR_ID_TARGET", OBEX_HDR_ID_TARGET},
	{"OBEX_HDR_ID_HTTP", OBEX_HDR_ID_HTTP},
	{"OBEX_HDR_ID_BODY", OBEX_HDR_ID_BODY},
	{"OBEX_HDR_ID_BODY_END", OBEX_HDR_ID_BODY_END},
	{"OBEX_HDR_ID_WHO", OBEX_HDR_ID_WHO},
	{"OBEX_HDR_ID_CONNECTION", OBEX_HDR_ID_CONNECTION},
	{"OBEX_HDR_ID_APPARAM", OBEX_HDR_ID_APPARAM},
	{"OBEX_HDR_ID_AUTHCHAL", OBEX_HDR_ID_AUTHCHAL},
	{"OBEX_HDR_ID_AUTHRESP", OBEX_HDR_ID_AUTHRESP},
	{"OBEX_HDR_ID_CREATOR", OBEX_HDR_ID_CREATOR},
	{"OBEX_HDR_ID_WANUUID", OBEX_HDR_ID_WANUUID},
	{"OBEX_HDR_ID_OBJECTCLASS", OBEX_HDR_ID_OBJECTCLASS},
	{"OBEX_HDR_ID_SESSIONPARAM", OBEX_HDR_ID_SESSIONPARAM},
	{"OBEX_HDR_ID_SESSIONSEQ", OBEX_HDR_ID_SESSIONSEQ},
	{"OBEX_HDR_ACTION", OBEX_HDR_ACTION},
	{"OBEX_HDR_DESTNAME", OBEX_HDR_DESTNAME},
	{"OBEX_HDR_PERMISSIONS", OBEX_HDR_PERMISSIONS},
	{NULL, 0}
};

define obex_cmdnames[] = {
	{"OBEX_CMD_CONNECT", OBEX_CMD_CONNECT},
	{"OBEX_CMD_DISCONNECT", OBEX_CMD_DISCONNECT},
	{"OBEX_CMD_PUT", OBEX_CMD_PUT},
	{"OBEX_CMD_GET", OBEX_CMD_GET},
	{"OBEX_CMD_SETPATH", OBEX_CMD_SETPATH},
	{"OBEX_CMD_SESSION", OBEX_CMD_SESSION},
	{"OBEX_CMD_ABORT", OBEX_CMD_ABORT},
	{"OBEX_CMD_ACTION", OBEX_CMD_ACTION},
	{"OBEX_FINAL", OBEX_FINAL},
	{NULL, 0}
};

define obex_rspnames[] = {
	{"OBEX_RSP_CONTINUE", OBEX_RSP_CONTINUE},
	{"OBEX_RSP_SWITCH_PRO", OBEX_RSP_SWITCH_PRO},
	{"OBEX_RSP_SUCCESS", OBEX_RSP_SUCCESS},
	{"OBEX_RSP_CREATED", OBEX_RSP_CREATED},
	{"OBEX_RSP_ACCEPTED", OBEX_RSP_ACCEPTED},
	{"OBEX_RSP_NON_AUTHORITATIVE", OBEX_RSP_NON_AUTHORITATIVE},
	{"OBEX_RSP_NO_CONTENT", OBEX_RSP_NO_CONTENT},
	{"OBEX_RSP_RESET_CONTENT", OBEX_RSP_RESET_CONTENT},
	{"OBEX_RSP_PARTIAL_CONTENT", OBEX_RSP_PARTIAL_CONTENT},
	{"OBEX_RSP_MULTIPLE_CHOICES", OBEX_RSP_MULTIPLE_CHOICES},
	{"OBEX_RSP_MOVED_PERMANENTLY", OBEX_RSP_MOVED_PERMANENTLY},
	{"OBEX_RSP_MOVED_TEMPORARILY", OBEX_RSP_MOVED_TEMPORARILY},
	{"OBEX_RSP_SEE_OTHER", OBEX_RSP_SEE_OTHER},
	{"OBEX_RSP_NOT_MODIFIED", OBEX_RSP_NOT_MODIFIED},
	{"OBEX_RSP_USE_PROXY", OBEX_RSP_USE_PROXY},
	{"OBEX_RSP_BAD_REQUEST", OBEX_RSP_BAD_REQUEST},
	{"OBEX_RSP_UNAUTHORIZED", OBEX_RSP_UNAUTHORIZED},
	{"OBEX_RSP_PAYMENT_REQUIRED", OBEX_RSP_PAYMENT_REQUIRED},
	{"OBEX_RSP_FORBIDDEN", OBEX_RSP_FORBIDDEN},
	{"OBEX_RSP_NOT_FOUND", OBEX_RSP_NOT_FOUND},
	{"OBEX_RSP_METHOD_NOT_ALLOWED", OBEX_RSP_METHOD_NOT_ALLOWED},
	{"OBEX_RSP_NOT_ACCEPTABLE", OBEX_RSP_NOT_ACCEPTABLE},
	{"OBEX_RSP_PROXY_AUTH_REQUIRED", OBEX_RSP_PROXY_AUTH_REQUIRED},
	{"OBEX_RSP_REQUEST_TIME_OUT", OBEX_RSP_REQUEST_TIME_OUT},
	{"OBEX_RSP_CONFLICT", OBEX_RSP_CONFLICT},
	{"OBEX_RSP_GONE", OBEX_RSP_GONE},
	{"OBEX_RSP_LENGTH_REQUIRED", OBEX_RSP_LENGTH_REQUIRED},
	{"OBEX_RSP_PRECONDITION_FAILED", OBEX_RSP_PRECONDITION_FAILED},
	{"OBEX_RSP_REQ_ENTITY_TOO_LARGE", OBEX_RSP_REQ_ENTITY_TOO_LARGE},
	{"OBEX_RSP_REQ_URL_TOO_LARGE", OBEX_RSP_REQ_URL_TOO_LARGE},
	{"OBEX_RSP_UNSUPPORTED_MEDIA_TYPE", OBEX_RSP_UNSUPPORTED_MEDIA_TYPE},
	{"OBEX_RSP_INTERNAL_SERVER_ERROR", OBEX_RSP_INTERNAL_SERVER_ERROR},
	{"OBEX_RSP_NOT_IMPLEMENTED", OBEX_RSP_NOT_IMPLEMENTED},
	{"OBEX_RSP_BAD_GATEWAY", OBEX_RSP_BAD_GATEWAY},
	{"OBEX_RSP_SERVICE_UNAVAILABLE", OBEX_RSP_SERVICE_UNAVAILABLE},
	{"OBEX_RSP_GATEWAY_TIMEOUT", OBEX_RSP_GATEWAY_TIMEOUT},
	{"OBEX_RSP_VERSION_NOT_SUPPORTED", OBEX_RSP_VERSION_NOT_SUPPORTED},
	{"OBEX_RSP_DATABASE_FULL", OBEX_RSP_DATABASE_FULL},
	{"OBEX_RSP_DATABASE_LOCKED", OBEX_RSP_DATABASE_LOCKED},
	{NULL, 0}
};

char *obex_name (int value, define array[]) {
	int i;

	for(i=0; array[i].name!=NULL; i++)
		if(array[i].value==value)
			return array[i].name;
}

