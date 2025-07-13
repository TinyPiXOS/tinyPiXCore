#ifndef _BLT_HARD_H_
#define _BLT_HARD_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <bluetooth/bluetooth.h>
#include "bluetooth_inc.h"
#include "blt_dbussignal.h"

#define BLUETOOTH_ADDR_MAX_LEN	18
#define BLUETOOTH_NAME_MAX_LEN	256

#define BLUETOOTH_UUID_LEN		20


#define BLUEZ_BUS_NAME "org.bluez"
#define ADAPTER_INTERFACE "org.bluez.Adapter1"
#define ADAPTER_PATH "/org/bluez/hci0"  // 根据你的实际情况设置适配器路径

typedef char BltUUID[BLUETOOTH_UUID_LEN];
typedef int BltID;
typedef int BltSock;

typedef struct BluetDeviceScan_ BluetDeviceScan;
typedef struct BluetDeviceScanPrivate_ BluetDeviceScanPrivate;

struct BluetDeviceScan_{
	BluetDeviceScanPrivate *priv;
};


struct BluetoothAdapter{
	int id;
	char address[BLUETOOTH_ADDR_MAX_LEN];		// 蓝牙 MAC 地址
    char name[BLUETOOTH_NAME_MAX_LEN];			// 适配器名称
};


typedef struct BluetoothRemote{
	char address[BLUETOOTH_ADDR_MAX_LEN]; 		// 蓝牙 MAC 地址
    char name[BLUETOOTH_NAME_MAX_LEN];			// 蓝牙的名字
	BltUUID uuid;	//uuid
	int8_t rssi;	//信号质量
	uint32_t class_type;	//设备类型
	uint8_t paired;
	uint8_t legacy_pairing;	//是否支持传统配对方式，0表示仅支持
	char *alias;
	char *icon;

	//以下为内部使用，禁止用户获取
	char *object_path;	//可以用于标记唯一的蓝牙设备
	
}BluetoothRemote;



typedef void (*BluetoothAdapterCallback)(const struct BluetoothAdapter* adapter, void* user_data);
typedef void (*BluetoothRemoteCallback)(const struct BluetoothRemote* remote, void* user_data);



struct BluetConf{
	int id;
	int sock;
};

void bluet_set_adapter_node_callback(struct LinkedList *list);
void bluet_callback_get_adapter(const struct BluetoothAdapter* adapter, void* user_data);
int bluet_get_adapters(BluetoothAdapterCallback callback, void* user_data);


int bluet_hci_device_inquiry(int dev_id,int sock,BluetoothRemoteCallback callback, void *user_data);
void bluet_callback_get_remote(const struct BluetoothRemote* remote, void* user_data);
void bluet_set_remote_node_callback(struct LinkedList *list);

BltID bluet_get_device_id(bdaddr_t *bdaddr);
BltSock bluet_open_device_hci(BltID dev_id);
int bluet_close_device(BltSock sock);


BluetDeviceScan *bluet_adapter_scan_creat(const char *local);
int bluet_adapter_scan_delete(BluetDeviceScan *scan);


int bluet_adapter_start_discovery(BluetDeviceScan *scan);
int bluet_adapter_stop_discovery(BluetDeviceScan *scan);

BluetDbusSignal *bluet_adapter_interfaces_added(BluetDeviceScan *scan,BluetoothRemoteCallback callback,void *userdata);
BluetDbusSignal *bluet_adapter_interfaces_removed(BluetDeviceScan *scan,BluetoothRemoteCallback callback,void *userdata);
BluetDbusSignal *bluet_adapter_properties_changed(BluetDeviceScan *scan,BluetoothRemoteCallback callback,void *userdata);
void bluet_adapter_sbus_signal_delete(BluetDbusSignal *sig_sub);
int scan_device_glib();


int bluet_adapter_get_connected_device_list(Adapter *adapter,BluetoothRemoteCallback callback, void *user_data);
int bluet_adapter_get_paired_device_list(Adapter *adapter,BluetoothRemoteCallback callback, void *user_data);
int bluet_adapter_get_trusted_device_list(Adapter *adapter,BluetoothRemoteCallback callback, void *user_data);


int bluet_adapter_set_discoverable(Adapter *adapter,uint8_t discoverable);
int bluet_adapter_set_discoverable_timeout(Adapter *adapter,uint32_t timeout);
int bluet_adapter_get_powered(Adapter *adapter);
int bluet_adapter_set_powered(Adapter *adapter,uint8_t state);

#ifdef	__cplusplus
}
#endif

#endif