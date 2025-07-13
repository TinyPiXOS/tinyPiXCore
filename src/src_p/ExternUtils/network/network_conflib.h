#ifndef _NETWORK_CONFLIB_H_
#define _NETWORK_CONFLIB_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>             //地址转换
#include <unistd.h>
#include <error.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/wireless.h>
#include <errno.h>
#include <sys/time.h>		/* struct timeval */
#include <math.h>
#include <fcntl.h>
#include <ctype.h>
#include <netdb.h>		/* gethostbyname, getnetbyname */
#include <net/ethernet.h>	/* struct ether_addr */

#define SIOCSIWMODUL	0x8B2E		/* set Modulations settings */
#define SIOCGIWMODUL	0x8B2F		/* get Modulations settings */

#define IW_NUM_OPER_MODE	7
#define IW_NUM_OPER_MODE_EXT	8

typedef struct iw_statistics iwstats;
typedef struct iw_param iwparam;

typedef struct stream_descr
{
  char *	end;		/* End of the stream */
  char *	current;	/* Current event in stream of events */
  char *	value;		/* Current value in event */
} stream_descr;

typedef struct wireless_config
{
	char		name[IFNAMSIZ + 1];	/* Wireless/protocol name */
	int		has_nwid;
	iwparam		nwid;			/* Network ID */
	int		has_freq;
	double		freq;			/* Frequency/channel */
	int		freq_flags;
	int		has_key;
	unsigned char	key[IW_ENCODING_TOKEN_MAX];	/* Encoding key used */
	int		key_size;		/* Number of bytes */
	int		key_flags;		/* Various flags */
	int		has_essid;
	int		essid_on;
	char		essid[IW_ESSID_MAX_SIZE + 1];	/* ESSID (extended network) */
	int		has_mode;
	int		mode;			/* Operation mode */
} wireless_config;

typedef struct wireless_scan
{
  /* Linked list */
  struct wireless_scan *next;

  /* Cell identifiaction */
  int		has_ap_addr;
  struct sockaddr ap_addr;		/* Access point address */

  /* Other information */
  struct wireless_config	b;	/* Basic information */
  iwstats	stats;			/* Signal strength */
  int		has_stats;
  iwparam	maxbitrate;		/* Max bit rate in bps */
  int		has_maxbitrate;
} wireless_scan;

typedef struct wireless_scan_head
{
  wireless_scan *result;		/* Result of the scan */
  int	retry;		/* Retry level */
} wireless_scan_head;


void iw_init_event_stream(struct stream_descr *stream,char *data,int len);
int iw_extract_event_stream(struct stream_descr *stream,struct iw_event *iwe,int we_version);
double iw_freq2float(const struct iw_freq *	in);








#endif
