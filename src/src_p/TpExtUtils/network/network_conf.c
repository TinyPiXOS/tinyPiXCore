/*///------------------------------------------------------------------------------------------------------------------------//
		网卡配置程序
说 明 : 网卡信息的设置和获取
日 期 : 2024.8.1
作 者 : Chinagn

/*///-----------------------------------------------------------------------------------------------------------------------//
#ifdef __cplusplus
extern "C" {
#endif
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <glib.h>       //dgbus
#include <gio/gio.h>    //gdbus
#include <fcntl.h>
#include <sys/select.h>
#include <signal.h>
#include <string.h>
#include "network_conf.h"
#include "network_conflib.h"

struct NETWORK_SET userset[MAX_NETWORK_THREAD];

static void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags != -1) {
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }
}

int system_cmd_timeout(const char *cmd, uint32_t timeout) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        // 子进程
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[1]);

        execl("/bin/sh", "sh", "-c", cmd, (char *)NULL);
        _exit(127);  // exec失败 fallback
    }

    // 父进程
    close(pipefd[1]);
    set_nonblocking(pipefd[0]);

    fd_set read_fds;
    struct timeval tv;
    int elapsed_ms = 0;
    const int interval_ms = 200;

    char buf[512];
    int status;

    while (elapsed_ms < timeout) {
        FD_ZERO(&read_fds);
        FD_SET(pipefd[0], &read_fds);

        tv.tv_sec = 0;
        tv.tv_usec = interval_ms * 1000;

        int sel = select(pipefd[0] + 1, &read_fds, NULL, NULL, &tv);
        if (sel > 0 && FD_ISSET(pipefd[0], &read_fds)) {
            int len = read(pipefd[0], buf, sizeof(buf) - 1);
            if (len > 0) {
                buf[len] = '\0';
                fputs(buf, stdout);
                fflush(stdout);

                if (strstr(buf, "error") || strstr(buf, "错误")) {
                    fprintf(stderr, "\n检测到错误输出，强制终止命令。\n");
                    kill(pid, SIGKILL);
                    waitpid(pid, NULL, 0);
                    close(pipefd[0]);
                    return -1;
                }
            }
        }

        // 检查子进程是否退出
        pid_t result = waitpid(pid, &status, WNOHANG);
        if (result == pid) {
            close(pipefd[0]);
            return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
        }

        usleep(interval_ms * 1000);
        elapsed_ms += interval_ms;
    }

    fprintf(stderr, "\n命令超时，强制终止。\n");
    kill(pid, SIGKILL);
    waitpid(pid, NULL, 0);
    close(pipefd[0]);
    return -1;
}



int my_ipv4_memcpy(char *d,char*s)
{
	int i=0;
	for(;i<15;i++)
	{
		if((s[i]<'0'||s[i]>'9')&&s[i]!='.')
			break;
		d[i]=s[i];
	}
	d[i]='\0';
}

int my_ascii_to_hex(char ascii)
{
	if(ascii>='0'&&ascii<='9')
		return (ascii-'0');
	else if(ascii>='A'&&ascii<='F')
		return (ascii-'A'+10);
	else if(ascii>='a'&&ascii<='f')
		return (ascii-'a'+10);
}

unsigned char my_hex_to_ascii(unsigned char hex)
{
	if(hex>0x09)
		return (hex+0x37);
	else
		return (hex+0x30);
}

struct NetworkDevice *Network_List_CreatHead() //初始化链表头
{
    //为头节点申请空间
	struct NetworkDevice *head=NULL;
	head = (struct NetworkDevice *) malloc(sizeof (struct NetworkDevice));
	if (head == NULL)
		printf("head malloc error!\n");

    //为头节点的指针域赋值
	head->next = NULL;
	 return head;
}

struct NetworkWireless *NetworkWireless_List_CreatHead() //初始化链表头
{
    //为头节点申请空间
	struct NetworkWireless *head=NULL;
	head = (struct NetworkWireless *) malloc(sizeof (struct NetworkWireless));
	if (head == NULL)
		printf("head malloc error!\n");
    //为头节点的指针域赋值
	head->next = NULL;
	 return head;
}
/*
struct wireless_scan_head *NetworkWireless_List2_CreatHead()//初始化链表头
{
	    //为头节点申请空间
	struct wireless_scan_head *head=NULL;
	head = (struct wireless_scan_head *) malloc(sizeof (struct wireless_scan_head));
	if (head == NULL)
		printf("head malloc error!\n");
    //为头节点的指针域赋值
	head->next = NULL;
	 return head;
}*/


//链表增加节点，网卡程序内部使用
static int Network_List_AddNode(struct NetworkDevice *head,char gateway[20],char name[20],int type,int metric)
{
	//为新节点申请空间
	struct NetworkDevice *Node = NULL;
	Node = (struct NetworkDevice *)malloc(sizeof(struct NetworkDevice));
	
	//为新节点赋值
	if(gateway!=NULL)
		strcpy(Node->gateway,gateway);
	if(name!=NULL)
		strcpy(Node->name,name);
	Node->type=type;
	Node->metric = metric;
	Node->next = NULL;
	
	//寻找最后一个节点，并尾插
	struct NetworkDevice *p = NULL;
	for(p=head;p->next!=NULL;p=p->next)
		; 
	//从循环出来时，p->next=NULL，也就是说，p指向最后一个节点！
	p->next = Node;
	
	return 0;
}

//删除链表
int Network_List_Free(struct NetworkDevice *head)
{
	struct NetworkDevice *node=head->next;
	while(head!=NULL)
	{
		node=head;
		head=head->next;
		free(node);
		node=NULL;
	}
}
	
//链表增加节点，无线网程序内部使用
static int NetworkWireless_List_Add(struct NetworkWireless *head,char ssid[30],uint8_t use,uint8_t intensity,uint8_t WPA1,uint8_t WPA2)
{
	//为新节点申请空间
	struct NetworkWireless *Node = NULL;
	Node = (struct NetworkWireless *)malloc(sizeof(struct NetworkWireless));
	
	strcpy(Node->SSID,ssid);
	Node->inuse=use;
	Node->signal=intensity;
	Node->WPA1=WPA1;
	Node->WPA2=WPA2;
	Node->next = NULL;
	
	//寻找最后一个节点，并尾插
	struct NetworkWireless *p = NULL;
	for(p=head;p->next!=NULL;p=p->next)
		; 
	p->next = Node;
	
	return 0;
}


static inline struct wireless_scan *iw_process_scanning_token(struct iw_event *event,struct wireless_scan *wscan)
{
  struct wireless_scan *	oldwscan;

  /* Now, let's decode the event */
  switch(event->cmd)
    {
    case SIOCGIWAP:
      /* New cell description. Allocate new cell descriptor, zero it. */
      oldwscan = wscan;
      wscan = (struct wireless_scan *) malloc(sizeof(struct wireless_scan));
      if(wscan == NULL)
	return(wscan);
      /* Link at the end of the list */
      if(oldwscan != NULL)
	oldwscan->next = wscan;

      /* Reset it */
      bzero(wscan, sizeof(struct wireless_scan));

      /* Save cell identifier */
      wscan->has_ap_addr = 1;
      memcpy(&(wscan->ap_addr), &(event->u.ap_addr), sizeof (struct sockaddr));
      break;
    case SIOCGIWNWID:
      wscan->b.has_nwid = 1;
      memcpy(&(wscan->b.nwid), &(event->u.nwid), sizeof(iwparam));
      break;
    case SIOCGIWFREQ:
      wscan->b.has_freq = 1;
      wscan->b.freq = iw_freq2float(&(event->u.freq));
      wscan->b.freq_flags = event->u.freq.flags;
      break;
    case SIOCGIWMODE:
      wscan->b.mode = event->u.mode;
      if((wscan->b.mode < IW_NUM_OPER_MODE) && (wscan->b.mode >= 0))
	wscan->b.has_mode = 1;
      break;
    case SIOCGIWESSID:
      wscan->b.has_essid = 1;
      wscan->b.essid_on = event->u.data.flags;
      memset(wscan->b.essid, '\0', IW_ESSID_MAX_SIZE+1);
      if((event->u.essid.pointer) && (event->u.essid.length))
	memcpy(wscan->b.essid, event->u.essid.pointer, event->u.essid.length);
      break;
    case SIOCGIWENCODE:
      wscan->b.has_key = 1;
      wscan->b.key_size = event->u.data.length;
      wscan->b.key_flags = event->u.data.flags;
      if(event->u.data.pointer)
	memcpy(wscan->b.key, event->u.essid.pointer, event->u.data.length);
      else
	wscan->b.key_flags |= IW_ENCODE_NOKEY;
      break;
    case IWEVQUAL:
      /* We don't get complete stats, only qual */
      wscan->has_stats = 1;
      memcpy(&wscan->stats.qual, &event->u.qual, sizeof(struct iw_quality));
      break;
    case SIOCGIWRATE:
      /* Scan may return a list of bitrates. As we have space for only
       * a single bitrate, we only keep the largest one. */
      if((!wscan->has_maxbitrate) ||
	 (event->u.bitrate.value > wscan->maxbitrate.value))
	{
	  wscan->has_maxbitrate = 1;
	  memcpy(&(wscan->maxbitrate), &(event->u.bitrate), sizeof(iwparam));
	}
    case IWEVCUSTOM:
      /* How can we deal with those sanely ? Jean II */
    default:
      break;
   }	/* switch(event->cmd) */

  return(wscan);
}

int socket_open_ipv4()
{
	int sock;
	sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) 
	{
		perror("socket");
		return -1;
    }	
	return sock;
}

int socket_open_ipv6()
{
	int sock;
	sock = socket(AF_INET6, SOCK_DGRAM, 0);
    if (sock < 0) 
	{
		perror("socket");
		return -1;
    }	
	return sock;
}


//获取网卡IP()
int network_get_addr(int fd,char *name, struct sockaddr_in *addr)
{
	struct ifreq if_req;
	strcpy(if_req.ifr_name,name);
	char *ip;
	if(ioctl(fd,SIOCGIFADDR,&if_req)!=-1)
	{
		memcpy(addr,&(if_req.ifr_addr),sizeof(struct sockaddr_in));   //两个结构体大小一样，内容不一样，使用内存拷贝
		ip=inet_ntoa(addr->sin_addr);
		printf("ip%s\n",ip);
	}
	else
	{
		perror("get addr error\n");
		return -1;
	}
	return 0;
}

//设置网卡IP
int network_set_addr(int fd,char *name,struct sockaddr_in *addr)
{
	struct ifreq if_req;
	strcpy(if_req.ifr_name,name);
	if_req.ifr_addr=*(struct sockaddr*)(addr);
	if(ioctl(fd,SIOCSIFADDR,&if_req)<0)
	{
		return -1;
	}
	return 0;

}

//获取mask 子网掩码
int network_get_netmask(int fd,char * name,struct sockaddr_in *addr)
{
	struct ifreq if_req;
	strcpy(if_req.ifr_name,name);
	if(ioctl(fd,SIOCGIFNETMASK,&if_req)<0)
	{
		return -1;
	}
	memcpy(addr,&(if_req.ifr_netmask),sizeof(struct sockaddr_in));
	return 0;
}

//设置mask 子网掩码
int network_set_netmask(int fd,char *name,struct sockaddr_in *addr)
{
	struct ifreq if_req;
	strcpy(if_req.ifr_name,name);
	if_req.ifr_netmask=*(struct sockaddr*)(addr);
	if(ioctl(fd,SIOCSIFNETMASK,&if_req)<0)
	{
		return -1;
	}
	return 0;
}

//获取广播地址
int network_get_broadaddr(int fd,char *name,struct sockaddr_in *addr)
{
	struct ifreq if_req;
	strcpy(if_req.ifr_name,name);
	if(ioctl(fd,SIOCGIFBRDADDR,&if_req)<0)
	{
		return -1;
	}
	memcpy(addr,&(if_req.ifr_broadaddr),sizeof(struct sockaddr_in));
	return 0;
}

//设置广播地址
int network_set_broadaddr(int fd,char *name,struct sockaddr_in *addr)
{
	struct ifreq if_req;
	strcpy(if_req.ifr_name,name);
	if_req.ifr_broadaddr=*(struct sockaddr*)(addr);
	if(ioctl(fd,SIOCSIFBRDADDR,&if_req)<0)
	{
		return -1;
	}
	return 0;
}

//获取MAC地址
int network_get_macaddr(int fd,char *name,struct sockaddr_in *addr)
{
	struct ifreq if_req;
	strcpy(if_req.ifr_name,name);
	if(ioctl(fd,SIOCGIFHWADDR,&if_req)<0)
	{
		return -1;
	}
	memcpy(addr,&(if_req.ifr_hwaddr),sizeof(struct sockaddr_in));
	return 0;
}

//设置MAC地址
int network_set_macaddr(int fd,char *name ,struct sockaddr_in *addr)
{
	struct ifreq if_req;
	strcpy(if_req.ifr_name,name);
	if_req.ifr_hwaddr=*(struct sockaddr*)(addr);
	if(ioctl(fd,SIOCSIFHWADDR,&if_req)<0)
	{
		return -1;
	}
	return 0;
}


//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------分割线 以下是无限网卡专用的配置------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------

//获取信道(频率)
int network_get_freq(int fd,char *name,struct iw_freq *freq)
{
	struct iwreq iw_req;
	strcpy(iw_req.ifr_name,name);    //网卡名字
	if(ioctl(fd,SIOCGIWFREQ,&iw_req)<0)
	{
		return -1;
	}
	memcpy(freq,&(iw_req.u.freq),sizeof(struct iw_freq));
	return 0;
}

//设置信道(频率)
int network_set_freq(int fd,char *name,struct iw_freq *freq)
{
	struct iwreq iw_req;
	strcpy(iw_req.ifr_name,name);
//	if_req.ifr_hwaddr=*(struct sockaddr*)(addr);
	iw_req.u.freq=*freq;
	if(ioctl(fd,SIOCSIWFREQ,&iw_req)<0)
	{
		return -1;
	}
	return 0;
}

//获取网络id
int network_get_nwid(int fd,char *name,struct iw_param *param)
{
	struct iwreq iw_req;
	strcpy(iw_req.ifr_name,name);
	if(ioctl(fd,SIOCGIWNWID,&iw_req)<0)
	{
		return -1;
	}
	memcpy(param,&(iw_req.u.nwid),sizeof(struct iw_param));
	return 0;
}

//设置网络id
int network_set_nwid(int fd,char *name,struct iw_param *param)
{
	struct iwreq iw_req;
	strcpy(iw_req.ifr_name,name);
	iw_req.u.nwid=*(param);
	if(ioctl(fd,SIOCSIWNWID,&iw_req)<0)
	{
		return -1;
	}
	return 0;
}

//获取灵敏度
int network_get_sens(int fd,char *name,struct iw_param *param)
{
	struct iwreq iw_req;
	strcpy(iw_req.ifr_name,name);
	if(ioctl(fd,SIOCGIWSENS,&iw_req)<0)
	{
		return -1;
	}
	memcpy(param,&(iw_req.u.sens),sizeof(struct iw_param));	
	return 0;
}

//设置灵敏度
int network_set_sens(int fd,char *name,struct iw_param *param)
{
	struct iwreq iw_req;
	strcpy(iw_req.ifr_name,name);
	iw_req.u.sens=*(param);
	if(ioctl(fd,SIOCSIWSENS,&iw_req)<0)
	{
		return -1;
	}
	return 0;
}

/*********************************************************
功能：打开网卡设备进行设置操作
参数：网络类型
返回：网络接口描述符
*********************************************************/
int Network_SockConf_Open()
{
	int socketfd;
/*	static const int families[] = {AF_INET, AF_IPX, AF_AX25, AF_APPLETALK};
	for(int i=0;i<sizeof(families)/sizeof(int);i++)
	{
		if((socketfd=socket(families[i],SOCK_DGRAM, 0)) >=0)
		{
		      return socketfd;
	      	}
      	}*/
	if((socketfd=socket(AF_INET,SOCK_DGRAM, 0)) >=0)
	{
	      return socketfd;
      	}
	printf("open sockfd error\n");
	return -1;
}


int Network_Sock_Close(int socketfd)
{
	close(socketfd);
	return 0;
}

/*********************************************************
功能：获取主机上所有网卡设备
参数：网络类型
返回：网络接口描述符
*********************************************************/
int NetworkConf_Get_Device(struct NetworkDevice *card_head)
{
//	struct NetworkDevice *Node = NULL;
//	struct NetworkDevice *p = NULL;
	//struct NetworkDevice *card_head=Network_List_CreatHead(); //初始化链表头
	char buff[2048];
	struct ifconf if_conf;
	int int_num=0;
	int i=0;
	int fd=Network_SockConf_Open();
	if(fd<0)
		return -1;
	if_conf.ifc_buf=buff;
	if_conf.ifc_len=sizeof(buff);
	if(ioctl(fd,SIOCGIFCONF,&if_conf)!=-1)
	{
		int_num=if_conf.ifc_len/sizeof(struct ifreq);
		printf("num dev:%d\n",int_num);
		struct ifreq *ifr;
		while(int_num--)
		{
			ifr=if_conf.ifc_req+int_num;
			printf("devname:%s\n",ifr->ifr_name);
			
			//Node = (struct NetworkDevice *)malloc(sizeof(struct NetworkDevice));
			//strcpy(Node->name,ifr->ifr_name);
			//Node->next = NULL;
			
			Network_List_AddNode(card_head,NULL,ifr->ifr_name,0,0);
			//for(p=card_head;p->next!=NULL;p=p->next)
			//{
			//	; 
			//}
			//p->next = Node;
		}
	}
	else
	{
		close(fd);
		return -1;
	}
	close(fd);
	return int_num;
}

//获取网卡ip
int NetworkConf_Get_Addr(int Netfd ,char *conf, char *name)
{
	int err;
	struct ifreq if_req;
	struct sockaddr_in addr_in;
	strcpy(if_req.ifr_name,name);

	if((err=ioctl(Netfd,SIOCGIFADDR,&if_req))<0)
	{
		perror("get addr error");
		return -1;
	}
	//memcpy(&addr_in,&(if_req.ifr_addr),sizeof(struct sockaddr_in));   //两个结构体大小一样，内容不一样，使用内存拷贝
	addr_in=*((struct sockaddr_in*)(&if_req.ifr_addr));	//或者先取地址尽享强制类型转换再取地址的值
//	char *ip=inet_ntoa(addr_in.sin_addr);
	char ip[20];
	if(inet_ntop(AF_INET, &addr_in.sin_addr, ip, sizeof(ip)) == NULL) {
		perror("inet_ntop");
		return -1;
    	}
	//memcpy(conf,ip,strlen(ip));
	my_ipv4_memcpy(conf,ip);
	//conf=inet_ntoa(addr_in.sin_addr);
	//printf("addr:%s,len=%d\n",conf,(int)strlen(ip));
	return 0;
}

//设置网卡IP
int NetworkConf_Set_Addr(int Netfd ,char *conf, char *name)
{
	struct in_addr inp;  //二进制网络地址
	inet_aton(conf,&inp);
	struct sockaddr_in addr_in;
	addr_in.sin_family= AF_INET;
	addr_in.sin_port=0;
	addr_in.sin_addr=inp;
	struct ifreq if_req;
	strcpy(if_req.ifr_name,name);
	memcpy(&if_req.ifr_addr,&addr_in,sizeof(struct sockaddr_in));
	if(ioctl(Netfd,SIOCSIFADDR,&if_req)<0)
	{
		return -1;
	}
	return 0;
}

int NetworkConf_Get_Addr6(char *conf, char *name)
{
	int fd=socket_open_ipv6();
	int err;
	struct ifreq if_req;
	struct sockaddr_in6 addr_in;
	strcpy(if_req.ifr_name,name);

	if((err=ioctl(fd,SIOCGIFADDR,&if_req))<0)
	{
		perror("get addr error");
		return -1;
	}
	//memcpy(&addr_in,&(if_req.ifr_addr),sizeof(struct sockaddr_in));   //两个结构体大小一样，内容不一样，使用内存拷贝
	addr_in=*((struct sockaddr_in6*)(&if_req.ifr_addr));	//或者先取地址尽享强制类型转换再取地址的值
//	char *ip=inet_ntoa(addr_in.sin_addr);
	char ip[40];
	if(inet_ntop(AF_INET6, &addr_in.sin6_addr, ip, sizeof(ip)) == NULL) {
		perror("inet_ntop");
		return -1;
    }
	memcpy(conf,ip,strlen(ip));
//	my_ipv4_memcpy(conf,ip);
	//conf=inet_ntoa(addr_in.sin_addr);
	//printf("addr:%s,len=%d\n",conf,(int)strlen(ip));
	return 0;
}

int NetworkConf_Set_Addr6(char *conf, char *name)
{
	int fd=socket_open_ipv6();
	struct sockaddr_in6 addr_in;
	addr_in.sin6_family= AF_INET6;
	addr_in.sin6_port=0;
	inet_pton(AF_INET6, conf, &addr_in.sin6_addr);
	struct ifreq if_req;
	strcpy(if_req.ifr_name,name);
	memcpy(&if_req.ifr_addr,&addr_in,sizeof(struct sockaddr_in));
	if(ioctl(fd,SIOCSIFADDR,&if_req)<0)
	{
		return -1;
	}
	return 0;
}

//获取网卡跃点数
int NetworkConf_Get_Metric(int Netfd ,int *met,char *name)
{
	struct ifreq if_req;
	strcpy(if_req.ifr_name,name);
	if(ioctl(Netfd,SIOCGIFMETRIC,&if_req)<0)
	{
		perror("");
		return -1;
	}
	*met=if_req.ifr_metric;
	printf("met=%d",*met);
	return 0;
}

//设置网卡跃点数
int NetworkConf_Set_Metric(int Netfd ,int met,char *name)
{
	struct ifreq if_req;
	if_req.ifr_metric=met;
	strcpy(if_req.ifr_name,name);
	if(ioctl(Netfd,SIOCSIFMETRIC,&if_req)<0)
	{
		perror("set metric error");
		return -1;
	}
	return 0;
}

//获取点到点ip
int NetworkConf_Get_DstAddr(int Netfd ,char *conf, char *name)
{
	struct ifreq if_req;
	struct sockaddr_in addr_in;
	strcpy(if_req.ifr_name,name);
	char *ip;
	if(ioctl(Netfd,SIOCGIFDSTADDR,&if_req)<0)
	{
		perror("get addr error\n");
		return -1;
	}
	//memcpy(&addr_in,&(if_req.ifr_addr),sizeof(struct sockaddr_in));   //两个结构体大小一样，内容不一样，使用内存拷贝
	addr_in=*(struct sockaddr_in*)(&if_req.ifr_addr);	//或者先取地址尽享强制类型转换再取地址的值
	ip=inet_ntoa(addr_in.sin_addr);
	memcpy(conf,ip,strlen(ip));
	//conf=inet_ntoa(addr_in.sin_addr);
	//printf("%s\n",conf);
	return 0;
}

//设置点到点IP
int NetworkConf_Set_DstAddr(int Netfd ,char *conf, char *name)
{
	struct in_addr inp;  //二进制网络地址
	inet_aton(conf,&inp);
	struct sockaddr_in addr_in;
	addr_in.sin_family= AF_INET;
	addr_in.sin_port=0;
	addr_in.sin_addr=inp;
	struct ifreq if_req;
	strcpy(if_req.ifr_name,name);
	memcpy(&if_req.ifr_addr,&addr_in,sizeof(struct sockaddr_in));
	if(ioctl(Netfd,SIOCSIFADDR,&if_req)<0)
	{
		return -1;
	}
	return 0;
}
//获取mask 子网掩码
int NetworkConf_Get_Netmask(int Netfd,char *conf, char *name)
{
	struct ifreq if_req;
	struct sockaddr_in addr_in;
	char *ip;
	strcpy(if_req.ifr_name,name);
	if(ioctl(Netfd,SIOCGIFNETMASK,&if_req)<0)
	{
		return -1;
	}
	//memcpy(&addr_in,&(if_req.ifr_netmask),sizeof(struct sockaddr_in));
	addr_in=*(struct sockaddr_in*)(&if_req.ifr_netmask);
	ip=inet_ntoa(addr_in.sin_addr);
	memcpy(conf,ip,strlen(ip));
//	conf=inet_ntoa(addr_in.sin_addr);
	//printf("%s\n",conf);
	return 0;
}

//设置mask 子网掩码
int NetworkConf_Set_Netmask(int Netfd,char *conf, char *name)
{
	struct in_addr inp;  //二进制网络地址
	inet_aton(conf,&inp);
	struct sockaddr_in addr_in;
	addr_in.sin_family= AF_INET;
	addr_in.sin_port=0;
	addr_in.sin_addr=inp;
	struct ifreq if_req;
	strcpy(if_req.ifr_name,name);
	memcpy(&if_req.ifr_addr,&addr_in,sizeof(struct sockaddr_in));
	if(ioctl(Netfd,SIOCSIFADDR,&if_req)<0)
	{
		return -1;
	}
	return 0;
}

//获取广播地址
int NetworkConf_Get_BroadAddr(int Netfd,char *conf, char *name)
{
	struct ifreq if_req;
	struct sockaddr_in addr_in;
	strcpy(if_req.ifr_name,name);
	char *ip;
	if(ioctl(Netfd,SIOCGIFBRDADDR,&if_req)<0)
	{
		return -1;
	}
	memcpy(&addr_in,&(if_req.ifr_broadaddr),sizeof(struct sockaddr_in));
	addr_in=*(struct sockaddr_in*)(&if_req.ifr_broadaddr);
	ip=inet_ntoa(addr_in.sin_addr);
	memcpy(conf,ip,strlen(ip));
//	conf=inet_ntoa(addr_in.sin_addr);
	//printf("%s\n",conf);
	return 0;
}

//设置广播地址
int NetworkConf_Set_BroadAddr(int Netfd,char *conf,char *name)
{
	struct in_addr inp;  //二进制网络地址
	inet_aton(conf,&inp);
	struct sockaddr_in addr_in;
	addr_in.sin_family= AF_INET;
	addr_in.sin_port=0;
	addr_in.sin_addr=inp;
	struct ifreq if_req;
	strcpy(if_req.ifr_name,name);
	memcpy(&if_req.ifr_addr,&addr_in,sizeof(struct sockaddr_in));
	if(ioctl(Netfd,SIOCSIFADDR,&if_req)<0)
	{
		perror("");
		return -1;
	}
	return 0;
}

//获取MAC地址
int NetworkConf_Get_MacAddr(int Netfd,char *conf, char *name)
{
	struct ifreq if_req;
	strcpy(if_req.ifr_name,name);
	if(ioctl(Netfd,SIOCGIFHWADDR,&if_req)<0)
	{
		return -1;
	}
	unsigned char *mac=malloc(18);
	mac[0]=my_hex_to_ascii(((unsigned char)if_req.ifr_hwaddr.sa_data[0])>>4);
	mac[1]=my_hex_to_ascii(((unsigned char)if_req.ifr_hwaddr.sa_data[0])&0x0f);
	mac[2]=0x3A;
	mac[3]=my_hex_to_ascii(((unsigned char)if_req.ifr_hwaddr.sa_data[1])>>4);
	mac[4]=my_hex_to_ascii(((unsigned char)if_req.ifr_hwaddr.sa_data[1])&0x0f);
	mac[5]=0x3A;
	mac[6]=my_hex_to_ascii(((unsigned char)if_req.ifr_hwaddr.sa_data[2])>>4);
	mac[7]=my_hex_to_ascii(((unsigned char)if_req.ifr_hwaddr.sa_data[2])&0x0f);
	mac[8]=0x3A;
	mac[9]=my_hex_to_ascii(((unsigned char)if_req.ifr_hwaddr.sa_data[3])>>4);
	mac[10]=my_hex_to_ascii(((unsigned char)if_req.ifr_hwaddr.sa_data[3])&0x0f);
	mac[11]=0x3A;
	mac[12]=my_hex_to_ascii(((unsigned char)if_req.ifr_hwaddr.sa_data[4])>>4);
	mac[13]=my_hex_to_ascii(((unsigned char)if_req.ifr_hwaddr.sa_data[4])&0x0f);
	mac[14]=0x3A;
	mac[15]=my_hex_to_ascii(((unsigned char)if_req.ifr_hwaddr.sa_data[5])>>4);
	mac[16]=my_hex_to_ascii(((unsigned char)if_req.ifr_hwaddr.sa_data[5])&0x0f);
	mac[17]='\0';
//	printf("MAC:%s\n",mac);
	memcpy(conf,mac,18);
	free(mac);
	//printf("%s\n",conf);
	return 0;
}

//设置MAC地址格式00:00:00:00:00:00
int NetworkConf_Set_MacAddr(int Netfd,char *conf, char *name)
{
	struct ifreq if_req;
	char *mac=malloc(17);
	memcpy(mac,conf,17);
	strcpy(if_req.ifr_name,name);
	if_req.ifr_hwaddr.sa_data[0]=(my_ascii_to_hex(mac[0])<<4)|(my_ascii_to_hex(mac[1])&0x0F);
	if_req.ifr_hwaddr.sa_data[1]=(my_ascii_to_hex(mac[3])<<4)|(my_ascii_to_hex(mac[4])&0x0F);
	if_req.ifr_hwaddr.sa_data[2]=(my_ascii_to_hex(mac[6])<<4)|(my_ascii_to_hex(mac[7])&0x0F);
	if_req.ifr_hwaddr.sa_data[3]=(my_ascii_to_hex(mac[9])<<4)|(my_ascii_to_hex(mac[10])&0x0F);
	if_req.ifr_hwaddr.sa_data[4]=(my_ascii_to_hex(mac[12])<<4)|(my_ascii_to_hex(mac[13])&0x0F);
	if_req.ifr_hwaddr.sa_data[5]=(my_ascii_to_hex(mac[15])<<4)|(my_ascii_to_hex(mac[16])&0x0F);
	if_req.ifr_hwaddr.sa_family= ARPHRD_ETHER; //设置时使用sa_family=ARPHRD_ETHER
	if(ioctl(Netfd,SIOCSIFHWADDR,&if_req)<0)
	{
		printf("set macaddr error");
		perror("");
		return -1;
	}
	
	return 0;
}

//获取接口标志
int NetworkConf_Get_Flags(int Netfd,short *flag, char *name)
{
	struct ifreq if_req;
	strcpy(if_req.ifr_name,name);
	if(ioctl(Netfd,SIOCGIFFLAGS,&if_req)<0)
	{
		printf("get mtu error\n");
		return -1;
	}
	*flag=if_req.ifr_flags;
	printf("flags=%d\n",*flag);
	return 0;
}

//设置接口标志
int NetworkConf_Set_Flags(int Netfd,short flag, char *name)
{
	struct ifreq if_req;
	printf("%s,%d\n",name,flag);
	strcpy(if_req.ifr_name,name);
	if_req.ifr_flags=flag;
	if(ioctl(Netfd,SIOCSIFFLAGS,&if_req)<0)
	{
		printf("set flags error\n");
		return -1;
	}
}
//获取接口MTU
int NetworkConf_Get_Mtu(int Netfd,int *mtu, char *name)
{
	struct ifreq if_req;
	strcpy(if_req.ifr_name,name);
	if(ioctl(Netfd,SIOCGIFMTU,&if_req)<0)
	{
		printf("get flags error\n");
		return -1;
	}
	*mtu=if_req.ifr_mtu;
	printf("mtu=%d\n",*mtu);
	return 0;
}
//---------------------------以下是无线网卡的配置----------------------------------------
//获取信道(频率)
int NetworkConf_Get_Freq(int Netfd,struct iw_freq *freq, char *name)
{
	struct iwreq iw_req;
	strcpy(iw_req.ifr_name,name);    //网卡名字
	if(ioctl(Netfd,SIOCGIWFREQ,&iw_req)<0)
	{
		return -1;
	}
	memcpy(freq,&(iw_req.u.freq),sizeof(struct iw_freq));
	return 0;
}

//设置信道(频率)
int NetworkConf_Set_Freq(int Netfd,struct iw_freq freq, char *name)
{
	struct iwreq iw_req;
	strcpy(iw_req.ifr_name,name);
	iw_req.u.freq=freq;
	if(ioctl(Netfd,SIOCSIWFREQ,&iw_req)<0)
	{
		return -1;
	}
	return 0;
}

//获取网络id
int NetworkConf_Get_Nwid(int Netfd,struct iw_param *param, char *name)
{
	struct iwreq iw_req;
	strcpy(iw_req.ifr_name,name);
	if(ioctl(Netfd,SIOCGIWNWID,&iw_req)<0)
	{
		return -1;
	}
	memcpy(param,&(iw_req.u.nwid),sizeof(struct iw_param));
	return 0;
}

//设置网络id
int NetworkConf_Set_Nwid(int Netfd,struct iw_param param, char *name)
{
	struct iwreq iw_req;
	strcpy(iw_req.ifr_name,name);
	iw_req.u.nwid=param;
	if(ioctl(Netfd,SIOCSIWNWID,&iw_req)<0)
	{
		return -1;
	}
	return 0;
}

//获取灵敏度
int NetworkConf_Get_Sens(int Netfd,struct iw_param *param, char *name)
{
	struct iwreq iw_req;
	strcpy(iw_req.ifr_name,name);
	if(ioctl(Netfd,SIOCGIWSENS,&iw_req)<0)
	{
		return -1;
	}
	memcpy(param,&(iw_req.u.sens),sizeof(struct iw_param));
	return 0;
}

//设置灵敏度
int NetworkConf_Set_Sens(int Netfd,struct iw_param param, char *name)
{
	struct iwreq iw_req;
	strcpy(iw_req.ifr_name,name);
	iw_req.u.sens=param;
	if(ioctl(Netfd,SIOCSIWSENS,&iw_req)<0)
	{
		return -1;
	}
	return 0;
}

//获取默认传输速率
int NetworkConf_Get_Rate(int Netfd,struct iw_param *param, char *name)
{
	struct iwreq iw_req;
	strcpy(iw_req.ifr_name,name);
	if(ioctl(Netfd,SIOCGIWRATE,&iw_req)<0)
	{
		return -1;
	}
	memcpy(param,&(iw_req.u.bitrate),sizeof(struct iw_param));
	return 0;
}

//设置默认传输速率
int NetworkConf_Set_Rate(int Netfd, struct iw_param param, char *name)
{
	struct iwreq iw_req;
	strcpy(iw_req.ifr_name,name);
	iw_req.u.bitrate=param;
	if(ioctl(Netfd,SIOCSIWRATE,&iw_req)<0)
	{
		return -1;
	}
	return 0;
}

//获取周围网络信息,内部调用
int NetworkConf_Get_Scan_Wifi(int fd,wireless_scan_head *context, char *name)
{
	struct iw_scan_req scanopt;//AP信息结构体
	int we_version=20;//临时为了程序不报错使用，后期删除
	unsigned char *buff=NULL;
	unsigned char *new_buf;
	int len=PI_SCAN_MAX_DATA;
	struct iwreq iw_req;
	//调整buff大小，有的可能保存不下
    realloc:
	new_buf=realloc(buff,len);
	if(new_buf == NULL)
	{
		if(buff)
			free(buff);
		errno = ENOMEM;
		return -1;
	}
	buff = new_buf;
	//read scan 
	printf("name=%s\n",name);
	strcpy(iw_req.ifr_name,name);
	iw_req.u.data.pointer = buff;
	iw_req.u.data.flags = 0;
	iw_req.u.data.length = len;
	if(ioctl(fd,SIOCGIWSCAN,&iw_req)<0)
	{
		printf("判断继续读还是直接返回\n");
		if(errno == E2BIG)
		{
			printf("空间不够\n");
			if(iw_req.u.data.length > len)
				len=iw_req.u.data.length;
			else
				len*=2;
			goto realloc;
		}
		if(errno == EAGAIN)
		{
			printf("等一下再读\n");
	 		free(buff);
	 			 /* Wait for only 100ms from now on */
	  		return 100;	/* Wait 100 ms */
		}
		free(buff);
		perror(":");
		return(-1);
	}
	//read scan wifi
	if(iw_req.u.data.length)
	{
		printf("read scan wifi\n");
		struct iw_event		iwe;
		struct stream_descr	stream;
		struct wireless_scan *	wscan = NULL;
		int			ret;
		
      		// Init 
		iw_init_event_stream(&stream, (char *) buff, iw_req.u.data.length);
      		// This is dangerous, we may leak user data... //
		context->result = NULL;

      		// Look every token 
      		do
		{
			// Extract an event and print it 
			ret = iw_extract_event_stream(&stream, &iwe, we_version);
			if(ret > 0)
			{
				// Convert to wireless_scan struct 
				wscan = iw_process_scanning_token(&iwe, wscan);
				// Check problems 
				if(wscan == NULL)
				{
					free(buff);
					errno = ENOMEM;
					return(-1);
				}
				
				// Save head of list 
				if(context->result == NULL)
					context->result = wscan;
			}
		}while(ret > 0);
	}
	free(buff);
	return 0;
}

//扫描周围网络信息，内部调用
int NetworkConf_Set_Scan_Wifi(int fd,char *name)
{
	struct iwreq iw_req;//
	strcpy(iw_req.ifr_name,name);
	iw_req.u.data.pointer=NULL;
	iw_req.u.data.length=0;
	iw_req.u.data.flags=0;
	if((ioctl(fd,SIOCSIWSCAN,&iw_req)<0) && (errno != EPERM))
	{
		return -1;
	}
	return 0;
}


//获取无线网信息
int Network_Get_Wifi(struct NetworkWireless *head, char *name)
{
	int err=0;
	uint8_t used;
	struct iwreq iw_req;
	wireless_scan_head context;
	context.result = NULL;
  	context.retry = 0;
	int err_num;
	int Netfd= Network_SockConf_Open();
	
	if(NetworkConf_Set_Scan_Wifi(Netfd,name)<0)
	{
		printf("set scanning error\n");
		Network_Sock_Close(Netfd);
		return -1;
	}
	usleep(250000);
	while((err=NetworkConf_Get_Scan_Wifi(Netfd,&context,name))>0) //大于0就重新调用
	{
		err_num++;
		if(err_num>100)
			return -1;//超时
		usleep(err*1000);
		
	}
	if(err<0)
	{
		printf("get wifi error\n");
		Network_Sock_Close(Netfd);
		return -1;
	}
	//read context
	wireless_scan *node=context.result;
	while(node!=NULL)
	{
		if(node->stats.qual.qual!=0)
			used=1;
		else
			used=0;
		NetworkWireless_List_Add(head,node->b.essid,used,node->stats.qual.level,0,0);
		printf("ssid=%s,\thasstats=%d,\tqual=%d,\tlevel=%d\n",node->b.essid,node->has_stats,node->stats.qual.qual,node->stats.qual.level);
		node=node->next;
	}
	Network_Sock_Close(Netfd);
	return 0;
}

//暂时实现不了，调用_userset的接口
int Network_Connect()
{
	
}

//暂时实现不了，调用_userset的接口
int Network_Down_Connect()
{
	
}

void Network_Connect_Wireless(const char* dev,const char* ssid, const char* psk)
{
    GError* error = NULL;

    // 创建一个 GDBusConnection
    GDBusConnection* connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
    if (error) {
        printf("Error getting D-Bus connection: %s\n",error->message);
        g_error_free(error);
        return;
    }
//	printf();
    // 创建参数字典
    GVariantBuilder builder;
    g_variant_builder_init(&builder, G_VARIANT_TYPE("a{sv}"));
    
    // 添加 SSID 和 PSK
    g_variant_builder_add(&builder, "sv", "ssid", g_variant_new_string(ssid));
    g_variant_builder_add(&builder, "sv", "psk", g_variant_new_string(psk));

    GVariant* params = g_variant_builder_end(&builder);

	char obj_path[256];
	snprintf(obj_path,sizeof(obj_path),"/fi/w1/wpa_supplicant1/Interfaces/%s",dev);
    // 调用 AddNetwork 方法
    GVariant* result = g_dbus_connection_call_sync(
        connection,
        "fi.w1.wpa_supplicant1", // D-Bus 名称
        obj_path,
        "fi.w1.wpa_supplicant1.Interface", // 方法名
        "AddNetwork",
        params, // 参数
        G_VARIANT_TYPE("u"), // 期望的返回值类型 (uint32)
        G_DBUS_CALL_FLAGS_NONE, // 调用标志
        -1, // 超时（-1 表示无限制）
        NULL, // 取消标志
        &error // 错误信息
    );

    if (error) {
        printf( "Error adding network: %s\n",  error->message);
        g_error_free(error);
    } else {
        // 处理返回的网络 ID
        guint32 network_id;
        g_variant_get(result, "(u)", &network_id);
        printf("Network added successfully! Network ID: %d\n",network_id);
        g_variant_unref(result); // 释放返回值
    }

    // 清理
    g_variant_unref(params);
    g_object_unref(connection);
}

//设置dhcp
int Network_Set_DHCP(const char* dev,uint8_t status) {
	int ret=0;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, dev, IFNAMSIZ);

    // 启用 DHCP
    if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0) {
        perror("ioctl (SIOCGIFFLAGS)");
        close(sock);
        return -1;
    }

    // 设置DHCP接口
	if(status)
    	ifr.ifr_flags |= IFF_UP;
	else
		ifr.ifr_flags &= ~IFF_UP;
    if (ioctl(sock, SIOCSIFFLAGS, &ifr) < 0) {
        perror("ioctl (SIOCSIFFLAGS)");
        close(sock);
        return -1;
    }

    // 配置为 DHCP
    // 这里需要安装 dhclient 并调用它
	if(status)
	{
		char command[128];
		snprintf(command,sizeof(command),"dhclient %s",dev);
		ret = system(command);
	}

	close(sock);
	return ret;
}


//下面是DHCP部分，后期考虑优化
#include <sys/wait.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/stat.h>
// PID 文件路径模板
#define PIDFILE_FMT "/var/run/dhclient.%s.pid"
// Netlink 缓冲区大小
#define NL_BUF_SIZE 8192

#define LEASEFILE_FMT "/var/lib/dhcp/dhclient.%s.leases"

// —— 内部工具函数 —— //

// 把接口 up/down（不涉及 DHCP，只是物理启用/禁用）
static int iface_updown(const char *ifname, int up) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return -1;
    struct ifreq ifr = {};
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ-1);
    if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0) { close(sock); return -1; }
    if (up)  ifr.ifr_flags |=  IFF_UP;
    else     ifr.ifr_flags &= ~IFF_UP;
    if (ioctl(sock, SIOCSIFFLAGS, &ifr) < 0) { close(sock); return -1; }
    close(sock);
    return 0;
}

// 启动 dhclient，并将其 PID 写入 PID 文件
static int start_dhclient(const char *ifname) {
    pid_t pid = fork();
    if (pid < 0) return -1;
    if (pid == 0) {
        // 子进程：执行 dhclient
        char *argv[] = { "dhclient", (char*)ifname, NULL };
        execvp(argv[0], argv);
        _exit(127);
    }
    // 父进程：写 PID 文件
    char pidfile[128];
    snprintf(pidfile, sizeof(pidfile), PIDFILE_FMT, ifname);
    int fd = open(pidfile, O_WRONLY|O_CREAT|O_TRUNC, 0644);
    if (fd < 0) return -1;
    char buf[32];
    int n = snprintf(buf, sizeof(buf), "%d\n", pid);
    write(fd, buf, n);
    close(fd);
    return 0;
}

// 停止 dhclient：读 PID 文件并发送 SIGTERM
static int stop_dhclient(const char *ifname) {
    char pidfile[128];
    snprintf(pidfile, sizeof(pidfile), PIDFILE_FMT, ifname);
    FILE *f = fopen(pidfile, "r");
    if (!f) return -1;
    pid_t pid;
    if (fscanf(f, "%d", &pid) != 1) { fclose(f); return -1; }
    fclose(f);
    // 发送 SIGTERM（如果进程不存在也算成功）
    if (kill(pid, SIGTERM) < 0 && errno != ESRCH) return -1;
    waitpid(pid, NULL, 0);        // 等待子进程退出
	char leasefile[128];
	snprintf(leasefile, sizeof(leasefile), LEASEFILE_FMT, ifname);
	unlink(leasefile);
    unlink(pidfile);              // 删除 PID 文件
    return 0;
}

// 通过 Netlink 查询接口上的 IPv4 地址，
// 只要有一条地址的 IFA_F_PERMANENT == 0，就认为是 DHCP 动态地址
static int is_dhcp_enabled(const char *ifname) {
    int sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (sock < 0) return -1;

    // 准备并发送 RTM_GETADDR 请求
    struct {
        struct nlmsghdr nh;
        struct ifaddrmsg ifa;
    } req = {};
    req.nh.nlmsg_len   = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
    req.nh.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    req.nh.nlmsg_type  = RTM_GETADDR;
    req.ifa.ifa_family = AF_INET;
    req.ifa.ifa_index  = if_nametoindex(ifname);
    if (req.ifa.ifa_index == 0) {
        close(sock);
        return -1;
    }
    if (send(sock, &req, req.nh.nlmsg_len, 0) < 0) {
        close(sock);
        return -1;
    }

    // 读取并解析内核返回的数据
    char buf[NL_BUF_SIZE];
    int len;
    while ((len = recv(sock, buf, sizeof(buf), 0)) > 0) {
        struct nlmsghdr *nh;
        for (nh = (struct nlmsghdr *)buf; NLMSG_OK(nh, (unsigned int)len); nh = NLMSG_NEXT(nh, len)) {
            if (nh->nlmsg_type == NLMSG_DONE) {
                close(sock);
                return 0;  // 没发现动态地址
            }
            if (nh->nlmsg_type == NLMSG_ERROR) {
                close(sock);
                return -1;
            }
            if (nh->nlmsg_type != RTM_NEWADDR) continue;

            struct ifaddrmsg *ifa = NLMSG_DATA(nh);
            // 只检查我们关心的接口和 IPv4
            if (ifa->ifa_index != req.ifa.ifa_index || ifa->ifa_family != AF_INET)
                continue;

            // 遍历属性，查找 IFA_FLAGS
            int rtl = IFA_PAYLOAD(nh);
            struct rtattr *rta;
            for (rta = IFA_RTA(ifa); RTA_OK(rta, rtl); rta = RTA_NEXT(rta, rtl)) {
                if (rta->rta_type == IFA_FLAGS) {
                    unsigned int flags = *(unsigned int*)RTA_DATA(rta);
                    // 动态地址 IFA_F_PERMANENT == 0
                    if (!(flags & IFA_F_PERMANENT)) {
                        close(sock);
                        return 1;  // DHCP 已启用
                    }
                }
            }
        }
    }
    close(sock);
    return -1;
}

// —— 对外接口 —— //

// 返回 0 成功，-1 失败
int Network_Enable_DHCP(const char *ifname) {
//    if (iface_updown(ifname, 1) < 0) return -1;
    return start_dhclient(ifname);
}

// 返回 0 成功，-1 失败
int Network_Disable_DHCP(const char *ifname) {
//    if (iface_updown(ifname, 0) < 0) return -1;
    return stop_dhclient(ifname);
}

#define NMCLI_CONFIG_NAME			"tpNetworkManager"
//检查配置是否存在
static int config_exists(const char *conn_name) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "nmcli -g NAME connection show | grep -w \"%s\"", conn_name);
    return system(cmd) == 0 ? 1 : 0;  // 存在返回1，否则返回0
}
//检查配置是否已经激活
static int config_is_active(const char *conn_name)
{
	char cmd[256];
	char state[64] = {0};  // 存储状态输出
    snprintf(cmd, sizeof(cmd), "nmcli -g GENERAL.STATE connection show \"%s\"", conn_name);
   // 执行命令并获取输出
    FILE *fp = popen(cmd, "r");
    if (!fp) return -1;  // 命令执行失败
    
    if (fgets(state, sizeof(state), fp) == NULL) {
        pclose(fp);
        return 0;  // 无输出视为未激活
    }
    pclose(fp);
    
    // 精确检查激活状态（核心逻辑）[2,4,6](@ref)
    return (strstr(state, "activated") != NULL) ? 1 : 0;
}
static int exec_cmd_safe(const char *cmd) {
    int ret = system(cmd);
    // 忽略网络错误(255)和接口未激活(4)错误[5](@ref)
    return (ret == 0 || WEXITSTATUS(ret) == 4 || WEXITSTATUS(ret) == 255) ? 0 : -1;
}
//创建DHCP配置并应用
int Network_Create_DHCP_Config_Command(const char *ifname)
{
	char cmd[256];
	if(config_exists(NMCLI_CONFIG_NAME))
	{
		;
	}
	else{
		snprintf(cmd, sizeof(cmd), "nmcli con add type ethernet con-name \"%s\" ifname %s ipv4.method auto", NMCLI_CONFIG_NAME,ifname);
		if(system(cmd)<0)
			return -1;
	}
	return 0;
}

//命令行启动DHCP
int Network_Enable_DHCP_Command(const char *ifname)
{
	char cmd[256];
	int ret=0;
	if(Network_Create_DHCP_Config_Command(ifname)<0)
		return -1;
	if((ret=config_is_active(NMCLI_CONFIG_NAME))==1)
	{
		return 0;
	}
	else if(ret<0)
		return -1;

	snprintf(cmd, sizeof(cmd), "nmcli connection up \"%s\"", NMCLI_CONFIG_NAME);
    return system(cmd);
}

//命令和关闭DHCP
//ifname:网卡名
int Network_Disable_DHCP_Command(const char *ifname, 
                      const char *ip, 
					  int prefix ,
                      const char *gateway,uint8_t dns_flag)
{
	if(Network_Create_DHCP_Config_Command(ifname)<0)
		return -1;

    char cmd[512];

    // 2. 配置静态 IP（核心操作）[1,4,6](@ref)
	if(dns_flag==0)
		snprintf(cmd, sizeof(cmd), 
        	"nmcli con mod \"%s\" ipv4.method manual ipv4.addresses %s/%d ipv4.gateway %s ",
       		 NMCLI_CONFIG_NAME, ip, prefix, gateway );
	else
		snprintf(cmd, sizeof(cmd), 
        	"nmcli con mod \"%s\" ipv4.method manual ipv4.addresses %s/%d ipv4.gateway %s ipv4.ignore-auto-dns no",
        	NMCLI_CONFIG_NAME, ip, prefix, gateway );

    if (exec_cmd_safe(cmd) != 0) return -1;

    // 3. 应用配置[5](@ref)
    snprintf(cmd, sizeof(cmd), "nmcli con reload");
    system(cmd);
    
    // 4. 激活连接（后台执行避免阻塞）[5](@ref)
    snprintf(cmd, sizeof(cmd), "nmcli con up \"%s\" &>/dev/null &", NMCLI_CONFIG_NAME);
    return exec_cmd_safe(cmd);
}

//获取DNS(暂时也使用nmcli，后续更换为dbus)
int Network_Get_DNS_Command(const char *ifname, char *dns_servers[], int max_servers) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd),
             "nmcli -t -f IP4.DNS device show %s 2>/dev/null",
             ifname);

    FILE *fp = popen(cmd, "r");
    if (!fp) {
        perror("popen");
        return -1;
    }

    char buffer[128];
    int count = 0;
    while (fgets(buffer, sizeof(buffer), fp) && count < max_servers) {
        // 去掉行尾换行
        buffer[strcspn(buffer, "\n")] = '\0';

        // 找到冒号后面的部分
        char *p = strchr(buffer, ':');
        if (!p) continue;
        p++;

        // 如果这一行本身是一个逗号分隔的列表，就再拆一次
        char *token = strtok(p, ",");
        while (token && count < max_servers) {
            // 跳过空串
            if (*token) {
                dns_servers[count] = strdup(token);
                if (!dns_servers[count]) {
                    pclose(fp);
                    return count;
                }
                count++;
            }
            token = strtok(NULL, ",");
        }
    }

    pclose(fp);
	printf("获取到%d个DNS\n",count);
    return (count > 0) ? count : -1;
}

//返回0,dhcp成功
int Network_Is_DHCP_Command(const char *ifname)
{
	char cmd[256];
	snprintf(cmd, sizeof(cmd), "nmcli -t -f NAME,DEVICE connection show --active | grep %s | cut -d: -f1 | xargs -I {} nmcli -f ipv4.method connection show {} | grep auto", ifname);
	return exec_cmd_safe(cmd);
}

//设置动态DNS
int Network_Set_Auto_DNS_Command(const char *ifname)
{
	 char cmd[512];

    // 1. 确保连接已创建
    if (Network_Create_DHCP_Config_Command(ifname) < 0)
        return -1;

    // 2. 只修改 DNS 相关字段：清空静态 DNS，取消忽略自动 DNS
    snprintf(cmd, sizeof(cmd),
        "nmcli con mod \"%s\" "
        "ipv4.dns \"\" "
        "ipv4.ignore-auto-dns no",
        NMCLI_CONFIG_NAME);
    if (exec_cmd_safe(cmd) != 0)
        return -1;

    // 3. 重新加载配置
    snprintf(cmd, sizeof(cmd), "nmcli con reload");
    system(cmd);

    // 4. 激活连接（在后台执行避免阻塞）
    snprintf(cmd, sizeof(cmd),
        "nmcli con up \"%s\" &>/dev/null &",
        NMCLI_CONFIG_NAME);
    return exec_cmd_safe(cmd);
}


//设置DNS
//dns_servers：dns列表
//dns_count：dns列表数量
int Network_Set_DNS_Command(const char *ifname,
                            const char *dns_servers[], int dns_count)
{
    if (Network_Create_DHCP_Config_Command(ifname) < 0)
        return -1;

    // 拼接 DNS 列表为空格分隔字符串
    char dns_list[1024] = {0};
    size_t used = 0;
    for (int i = 0; i < dns_count; ++i) {
        const char *d = dns_servers[i];
        size_t len = strlen(d);
        if (used + len + 2 > sizeof(dns_list)) {
            fprintf(stderr, "DNS 字符串过长，超出缓冲区\n");
            return -1;
        }
        if (i > 0) {
            dns_list[used++] = ' ';
        }
        memcpy(dns_list + used, d, len);
        used += len;
        dns_list[used] = '\0';
    }

    // 构造 nmcli 命令：修改 DNS，并重启连接
    char cmd[1280];
	
    snprintf(cmd, sizeof(cmd),
        "nmcli con mod \"%s\" ipv4.dns \"%s\" && "
        "nmcli con down \"%s\" && "
        "nmcli con up \"%s\" >/dev/null 2>&1",
        NMCLI_CONFIG_NAME,
        dns_list,
        NMCLI_CONFIG_NAME,
        NMCLI_CONFIG_NAME
    );

    return system(cmd);
}

static char *trim_whitespace(char *str) {
    char *end;

    // 去除前导空白
    while (*str && isspace((unsigned char)*str)) {
        str++;
    }
    // 如果整行都是空白，直接返回空串
    if (*str == '\0') {
        return str;
    }
    // 去除末尾空白
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
    return str;
}

int Network_Is_Have_Static_DNS_Command(const char *ifname)
{
	char cmd[128];
	char line[128];
	int has_dns=0;
	snprintf(cmd, sizeof(cmd),
             "nmcli -t -g IP4.DNS device show %s 2>/dev/null",
             ifname);
    FILE *fp = popen(cmd, "r");
    if (fp) 
	{
        while (fgets(line, sizeof(line), fp)) {
			// 去掉换行  
			line[strcspn(line, "\n")] = '\0';
			// 去掉首尾空白后检查是否非空
			char *trimmed = trim_whitespace(line);
			if (*trimmed) {
				has_dns = 1;
				break;
			}
    	}
        pclose(fp);
    }
	return has_dns;
}

int Network_Is_Have_Server_DNS_Command(const char *ifname)
{
	char cmd[128];
	char line[128];
	int has_dns=0;
	    snprintf(cmd, sizeof(cmd),
             "nmcli -t -g IP4.DNS-SERVERS device show %s 2>/dev/null",
             ifname);
    FILE *fp = popen(cmd, "r");
    if (fp) {
         while (fgets(line, sizeof(line), fp)) {
			// 去掉换行  
			line[strcspn(line, "\n")] = '\0';
			// 去掉首尾空白后检查是否非空
			char *trimmed = trim_whitespace(line);
			if (*trimmed) {
				has_dns = 1;
				break;
			}
    	}
        pclose(fp);
    }
	return has_dns;
}




int Network_Set_Gateway_Command(const char *ifname, const char *gateway) {
	char cmd[256];

    // 1) 修改该接口对应连接的网关设置
    snprintf(cmd, sizeof(cmd),
             "nmcli device modify %s ipv4.gateway %s",
             ifname, gateway);
    if (system(cmd) != 0) {
        fprintf(stderr, "修改网关失败: %s\n", cmd);
        return -1;
    }

    // 2) 重启该网卡接口：disconnect/connect
    snprintf(cmd, sizeof(cmd),
             "nmcli device disconnect %s >/dev/null 2>&1 && "
             "nmcli device connect    %s >/dev/null 2>&1",
             ifname, ifname);
    if (system(cmd) != 0) {
        fprintf(stderr, "重启接口失败: %s\n", cmd);
        return -1;
    }

    return 0;
}


int Network_Get_Gateway_Command(const char *ifname, char *gateway, size_t len) {
	char cmd[256];
    // 从 device show 中取 IP4.GATEWAY
    snprintf(cmd, sizeof(cmd),
             "nmcli -g IP4.GATEWAY device show %s 2>/dev/null",
             ifname);

    FILE *fp = popen(cmd, "r");
    if (!fp) {
        perror("popen");
        return -1;
    }

    if (fgets(gateway, len, fp) == NULL) {
        pclose(fp);
        return -1;
    }
    pclose(fp);

    // 去掉行尾换行
    size_t l = strlen(gateway);
    if (l > 0 && gateway[l-1] == '\n') gateway[l-1] = '\0';
    return 0;
}


/*********************************************************
功能：网络
参数：网络类型
返回：网络接口描述符
*********************************************************/
int Network_Conf_Device(char *name,int type,void *conf)
{
	struct ifreq ifr;
	struct in_addr inp;  //二进制网络地址
	struct sockaddr_in addr_in;
	//char *p_conf=(char *)conf;
	int sock,num;
	sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) 
	{
		perror("socket");
		return -1;
    }	

	
	switch (type)
	{
		case NET_S_ADDR:					//网卡IP地址	192.168.0.1
			//inet_aton(*conf,&inp);
			//addr_in.sin_addr=inp;
			//network_set_addr(sockfd,&addr_in);
			printf("user addr=%s\n",(char *)conf);
			printf("name=%s\n",name);
			NetworkConf_Set_Addr(sock,(char *)conf, name);
			break;
		case NET_G_ADDR:
			//network_get_addr(sockfd,&addr_in);
			//*conf=inet_ntoa(addr_in.sin_addr);
			//printf("%s\n",*conf);
			NetworkConf_Get_Addr(sock,(char *)conf, name);
			break;
		case NET_S_ADDR6:
			NetworkConf_Set_Addr6((char *)conf, name);
			break;
		case NET_G_ADDR6:
			NetworkConf_Get_Addr6((char *)conf, name);
			break;
		case NET_S_DSTADDR:					//
			NetworkConf_Set_DstAddr(sock,(char *)conf, name);
			break;
		case NET_G_DSTADDR:
			NetworkConf_Get_DstAddr(sock,(char *)conf, name);
			break;
		case NET_S_NETMASK:					//子网掩码	255:255:255:0
			//inet_aton(*conf,&inp);
			//addr_in.sin_addr=inp;
			//network_set_netmask(sockfd,&addr_in);
			NetworkConf_Set_Netmask(sock,(char *)conf, name);
			break;
		case NET_G_NETMASK:
			//network_get_netmask(sockfd,&addr_in);
			//*conf=inet_ntoa(addr_in.sin_addr);
			NetworkConf_Get_Netmask(sock,(char *)conf, name);
			//printf("%s\n",*conf);
			break;
		case NET_S_BRDADDR:					//广播地址
			//inet_aton(*conf,&inp);
			//addr_in.sin_addr=inp;
			//network_set_broadaddr(sockfd,&addr_in);
			NetworkConf_Set_BroadAddr(sock,(char *)conf, name);
			break;
		case NET_G_BRDADDR:
			//network_get_broadaddr(sockfd,&addr_in);
			//*conf=inet_ntoa(addr_in.sin_addr);
			NetworkConf_Get_BroadAddr(sock,(char *)conf, name);
			//printf("%s\n",*conf);
			break;
		case NET_S_HWADDR:					//MAC地址
			NetworkConf_Set_MacAddr(sock,(char *)conf, name);
			break;
		case NET_G_HWADDR:
			NetworkConf_Get_MacAddr(sock,(char *)conf, name);
			break;
		case NET_S_METRIC:					//跃点数，表示网卡到目的网络中要经过多少个网段，也就是经过多少个路由，当有多个网卡，默认会从跃点数少的网卡走
			num=*((int*)conf);
			printf("conf=%d",num);
			NetworkConf_Set_Metric(sock,num,name);
			break;
		case NET_G_METRIC:
			NetworkConf_Get_Metric(sock,(int *)conf,name);
			break;
		case NET_S_FLAGS:
		{
			short flag=*((short*)conf);	
			printf("set flags%d\n",flag);
			NetworkConf_Set_Flags(sock,flag,name);
			break;
		}
		case NET_G_FLAGS:
		{
			short flag;
			int *p=(int *)conf;
			//printf("addr:%p,%p\n",flag,((short *)(conf)+2));
			NetworkConf_Get_Flags(sock,&flag,name);
			*p=(int)flag;
			break;
		}
		default:
			printf("该功能未编写\n");
			break;
	}
	close(sock);
	return 0;
}

int Network_Conf_Wireless(char *name,int type,void *conf)
{
	struct iwreq iwr;
	struct iw_freq freq;
	struct iw_param param;
	int sockfd;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    	if (sockfd < 0) 
	{
		perror("socket");
		return -1;
    	}	

	memset(&freq,0,sizeof(struct iw_freq));
	memset(&param,0,sizeof(struct iw_param));
	switch (type)
	{
		case NET_S_FREQ:          //0x8B04    /待编辑
			//inet_aton(*conf,&inp);
			//addr_in.sin_addr=inp;
			network_set_freq(sockfd,name,&freq);
			break;		  //0X8B05
		case NET_G_FREQ:
			network_get_freq(sockfd,name,&freq);
			//*conf=inet_ntoa(addr_in.sin_addr);
			printf("%d  %d  %d  %d\n",(int32_t)freq.m,(int16_t)freq.e,(uint8_t)freq.i,(uint8_t)freq.flags);
			break;
		case NET_S_NWID:          //0X8B02
			param.value=*((int*)conf);
			network_set_nwid(sockfd,name,&param);
			break;
		case NET_G_NWID:          //0X8B03    
			network_get_nwid(sockfd,name,&param);
			printf("nwid:%d\n",param.value);
			break; 
		case NET_S_COMMIT:        //0x8B00
			break;
		case NET_G_NAME:          //0X8B01
			break;
		case NET_S_MODE:          //0x8B06
			break;
		case NET_G_MODE:          //0x8B07
			break;
		case NET_S_SENS:          //0x8B08
			param.value=*((int*)conf);
			network_set_sens(sockfd,name,&param);
			break;
		case NET_G_SENS:          //0x8B09
			network_get_sens(sockfd,name,&param);
			printf("sens:%d\n",param.value);
			break;
		default:
			printf("该功能未编写\n");
			break;
	}
	close(sockfd);
	return 0;
}




#ifdef __cplusplus
}
#endif



