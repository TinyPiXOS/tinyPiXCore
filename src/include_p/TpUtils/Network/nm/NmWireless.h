#ifndef _NM_WIRELESS_H_
#define _NM_WIRELESS_H_

#ifdef __cplusplus
extern "C" {
#endif




int nm_wireless_connect_ssid(const char *name,const char *passwd, uint32_t timeout);
int nm_wireless_disconnect(const char *name);


#ifdef __cplusplus
}
#endif

#endif