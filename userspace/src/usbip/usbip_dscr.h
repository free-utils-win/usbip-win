#ifndef _USBIP_DSCR_H_
#define _USBIP_DSCR_H_

#include <WinSock2.h>
#include <usbspec.h>

extern int
fetch_device_descriptor(SOCKET sockfd, unsigned devid, PUSB_DEVICE_DESCRIPTOR dscr);
extern int
fetch_conf_descriptor(SOCKET sockfd, unsigned devid, PUSB_CONFIGURATION_DESCRIPTOR dscr, unsigned short *plen);

#endif /* _USBIP_DSCR_H_ */
