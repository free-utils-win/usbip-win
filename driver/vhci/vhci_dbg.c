#include "vhci.h"

#include "dbgcode.h"

#include <usbdi.h>

#ifdef DBG

static namecode_t	namecodes_vhci_ioctl[] = {
	K_V(IOCTL_INTERNAL_USB_CYCLE_PORT)
	K_V(IOCTL_INTERNAL_USB_ENABLE_PORT)
	K_V(IOCTL_INTERNAL_USB_GET_BUS_INFO)
	K_V(IOCTL_INTERNAL_USB_GET_BUSGUID_INFO)
	K_V(IOCTL_INTERNAL_USB_GET_CONTROLLER_NAME) 
	K_V(IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE)
	K_V(IOCTL_INTERNAL_USB_GET_HUB_COUNT)
	K_V(IOCTL_INTERNAL_USB_GET_HUB_NAME)
	K_V(IOCTL_INTERNAL_USB_GET_PARENT_HUB_INFO)
	K_V(IOCTL_INTERNAL_USB_GET_PORT_STATUS)
	K_V(IOCTL_INTERNAL_USB_RESET_PORT)
	K_V(IOCTL_INTERNAL_USB_GET_ROOTHUB_PDO)
	K_V(IOCTL_INTERNAL_USB_SUBMIT_IDLE_NOTIFICATION)
	K_V(IOCTL_INTERNAL_USB_SUBMIT_URB)
	K_V(IOCTL_INTERNAL_USB_GET_TOPOLOGY_ADDRESS)
	K_V(IOCTL_USB_DIAG_IGNORE_HUBS_ON)
	K_V(IOCTL_USB_DIAG_IGNORE_HUBS_OFF)
	K_V(IOCTL_USB_DIAGNOSTIC_MODE_OFF)
	K_V(IOCTL_USB_DIAGNOSTIC_MODE_ON)
	K_V(IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION)
	K_V(IOCTL_USB_GET_HUB_CAPABILITIES)
	K_V(IOCTL_USB_GET_ROOT_HUB_NAME)
	K_V(IOCTL_GET_HCD_DRIVERKEY_NAME)
	K_V(IOCTL_USB_GET_NODE_INFORMATION)
	K_V(IOCTL_USB_GET_NODE_CONNECTION_INFORMATION)
	K_V(IOCTL_USB_GET_NODE_CONNECTION_ATTRIBUTES)
	K_V(IOCTL_USB_GET_NODE_CONNECTION_NAME)
	K_V(IOCTL_USB_GET_NODE_CONNECTION_DRIVERKEY_NAME)
	K_V(IOCTL_USB_HCD_DISABLE_PORT)
	K_V(IOCTL_USB_HCD_ENABLE_PORT)
	K_V(IOCTL_USB_HCD_GET_STATS_1)
	K_V(IOCTL_USB_HCD_GET_STATS_2)
	{0,0}
};

struct namecode	namecodes_urb_func[] = {
	K_V(URB_FUNCTION_SELECT_CONFIGURATION)
	K_V(URB_FUNCTION_SELECT_INTERFACE)
	K_V(URB_FUNCTION_ABORT_PIPE)
	K_V(URB_FUNCTION_TAKE_FRAME_LENGTH_CONTROL)
	K_V(URB_FUNCTION_RELEASE_FRAME_LENGTH_CONTROL)
	K_V(URB_FUNCTION_GET_FRAME_LENGTH)
	K_V(URB_FUNCTION_SET_FRAME_LENGTH)
	K_V(URB_FUNCTION_GET_CURRENT_FRAME_NUMBER)
	K_V(URB_FUNCTION_CONTROL_TRANSFER)
	K_V(URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER)
	K_V(URB_FUNCTION_ISOCH_TRANSFER)
	K_V(URB_FUNCTION_RESET_PIPE)
	K_V(URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE)
	K_V(URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT)
	K_V(URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE)
	K_V(URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE)
	K_V(URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT)
	K_V(URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE)
	K_V(URB_FUNCTION_SET_FEATURE_TO_DEVICE)
	K_V(URB_FUNCTION_SET_FEATURE_TO_INTERFACE)
	K_V(URB_FUNCTION_SET_FEATURE_TO_ENDPOINT)
	K_V(URB_FUNCTION_SET_FEATURE_TO_OTHER)
	K_V(URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE)
	K_V(URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE)
	K_V(URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT)
	K_V(URB_FUNCTION_CLEAR_FEATURE_TO_OTHER)
	K_V(URB_FUNCTION_GET_STATUS_FROM_DEVICE)
	K_V(URB_FUNCTION_GET_STATUS_FROM_INTERFACE)
	K_V(URB_FUNCTION_GET_STATUS_FROM_ENDPOINT)
	K_V(URB_FUNCTION_GET_STATUS_FROM_OTHER)
	K_V(URB_FUNCTION_RESERVED0)
	K_V(URB_FUNCTION_VENDOR_DEVICE)
	K_V(URB_FUNCTION_VENDOR_INTERFACE)
	K_V(URB_FUNCTION_VENDOR_ENDPOINT)
	K_V(URB_FUNCTION_VENDOR_OTHER)
	K_V(URB_FUNCTION_CLASS_DEVICE)
	K_V(URB_FUNCTION_CLASS_INTERFACE)
	K_V(URB_FUNCTION_CLASS_ENDPOINT)
	K_V(URB_FUNCTION_CLASS_OTHER)
	K_V(URB_FUNCTION_RESERVED)
	K_V(URB_FUNCTION_GET_CONFIGURATION)
	K_V(URB_FUNCTION_GET_INTERFACE)
	K_V(URB_FUNCTION_LAST)
	{0,0}
};

const char *
dbg_vhci_ioctl_code(unsigned int ioctl_code)
{
	return dbg_namecode(namecodes_vhci_ioctl, "ioctl", ioctl_code);
}

const char *
dbg_urbfunc(unsigned int urbfunc)
{
	return dbg_namecode(namecodes_urb_func, "urb function", urbfunc);
}

#endif
