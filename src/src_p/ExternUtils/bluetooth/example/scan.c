//扫描测试
#include <stdio.h>
#include <pthread.h>
#include "blt_hard.h"
#include "blt_gatt.h"
#include "blt_sdp.h"
#include "bluetooth_inc.h"

//扫描本机蓝牙适配器
int scan_adapter()
{
	struct LinkedList *adapters=creat_linked_list();
	bluet_set_adapter_node_callback(adapters);
	int num=bluet_get_adapters(bluet_callback_get_adapter,(void *)adapters);

	printf("get %d adp\n",num);
	struct Node *node=adapters->head;
	for(int i=0;i<num;i++)
	{
		node=node->next;
		if(!node)
			break;
		struct BluetoothAdapter *adp=(struct BluetoothAdapter *)node->data;
		printf("id:%d  name:%s  addr:%s\n",adp->id,adp->name,adp->address);
	}

	delete_linked_list(adapters);
}

//扫描蓝牙(00:E0:4C:23:99:87)
int hci_scan_device()
{
	struct LinkedList *remote=creat_linked_list();
	bluet_set_remote_node_callback(remote);
	BltID id=bluet_get_device_id(NULL);
	BltSock sock=bluet_open_device_hci(id);
	printf("devid=%d,sock=%d\n",id,sock);
	int num=bluet_hci_device_inquiry(id,sock,bluet_callback_get_remote, (void *)remote);
}

int main()
{
//	scan_adapter();
//	sdp_dbus_test("6C:D1:99:69:BF:F0") ;
//	int channel=get_obex_channel("6C:D1:99:69:BF:F0");
//	printf("obex channel=%d\n",channel);
//	gatt_main();
	//hci_scan_device();
	//scan_test();
//	scan_device_glib();
//	send_file_dbus("6C:D1:99:69:BF:F0");
//	send_file_dbus_("6C:D1:99:69:BF:F0","/home/jiyuchao/桌面/phone.wav");
//	send_file_obex("6C:D1:99:69:BF:F0");
	recv_file_dbus();
//	send_file_obex("6C:D1:99:69:BF:F0");

//	gatt_test2();
}
