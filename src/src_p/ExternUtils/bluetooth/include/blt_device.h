#ifndef _BLT_DEVICE_H_
#define _BLT_DEVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <stdint.h>
#include <gio/gio.h>
#include <glib.h>
#include "bluetooth_inc.h"


typedef struct BluetDevice_ BluetDevice;
typedef struct BluetDevicePrivate_ BluetDevicePrivate;

struct BluetDevice_{
	BluetDevicePrivate *priv;
};

BluetDevice *bluet_device_creat(Adapter *adapter,const char *name);
int bluet_device_delete(BluetDevice *self);
Device *bluet_device_get_device(BluetDevice *device);

int bluet_device_pair_with_remote(BluetDevice *self, uint8_t trused);
int bluet_device_cancel_paie_with_remote(Device *device);
int bluet_cancel_paie_with_remote(BluetDevice *self);
int bluet_connect_remote_device(BluetDevice *device,const char *uuid);
int bluet_disconnect_remote_device(BluetDevice *device,const char *uuid);
int bluet_remove_remote(Adapter *adapter,const char *name);

const char *bluet_device_get_address(BluetDevice *self);
const char *bluet_device_get_name(BluetDevice *self);


int bluet_device_get_trusted(Device *device);
int bluet_adapter_get_trusted(Adapter *adapter,const char *name);
int bluet_device_set_trusted(Device *device,uint8_t value);
int bluet_adapter_set_trusted(Adapter *adapter,const char *name,uint8_t value);
int bluet_device_get_paired(Device *device);
int bluet_adapter_get_paired(Adapter *adapter,const char *name);

#ifdef __cplusplus
}
#endif

#endif
