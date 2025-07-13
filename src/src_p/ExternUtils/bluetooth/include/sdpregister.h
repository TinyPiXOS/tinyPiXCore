#ifndef __BLT_SDP_REGISTER_H_
#define __BLT_SDP_REGISTER_H_

#include<bluetooth/sdp.h>
#include<bluetooth/sdp_lib.h>


//struct SdpRegisterHandle{
//
//};


sdp_session_t *bluet_sdp_connect();
void bluet_sdp_close(sdp_session_t *session);

/* register a service on some rfcomm channel */
sdp_session_t *bluet_register_sdp_service(sdp_session_t *session_,const char *service_name, uint16_t svc_uuid_val, const char *prov, const char *desc,const char *proto_chain, uint8_t rfcomm_channel) ;
/* deregister a service */
void bluet_unregister_sdp_service(sdp_session_t *session,sdp_record_t *record);




#endif