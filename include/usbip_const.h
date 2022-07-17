#pragma once

#ifndef _USBIP_CONST_H
#define _USBIP_CONST_H

/* FIXME: how to sync with drivers/usbip_common.h ? */
enum usbip_device_status {
	/* dev status unknown. */
	DEV_ST_UNKNOWN = 0x00,

	/* sdev is available. */
	SDEV_ST_AVAILABLE = 0x01,
	/* sdev is now used. */
	SDEV_ST_USED,
	/* sdev is unusable because of a fatal error. */
	SDEV_ST_ERROR,

	/* vdev does not connect a remote device. */
	VDEV_ST_NULL,
	/* vdev is used, but the USB address is not assigned yet */
	VDEV_ST_NOTASSIGNED,
	VDEV_ST_USED,
	VDEV_ST_ERROR
};

#endif //_USBIP_CONST_H
