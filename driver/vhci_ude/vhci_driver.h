#pragma once

#ifndef _VHCI_DRIVER_H
#define _VHCI_DRIVER_H

#include <ntddk.h>
#include <wdf.h>
#include <usb.h>
#include <usbdlib.h>
#include <wdfusb.h>
#include <initguid.h>

#include <UdeCx.h> //<ude/1.1/UdeCx.h>

#include "vhci_dev.h"
#include "vhci_dbg.h"
#include "vhci_trace.h"

EXTERN_C_START

#define INITABLE __declspec(code_seg("INIT"))
#define PAGEABLE __declspec(code_seg("PAGE"))

#define VHCI_POOLTAG	'ichv'

EXTERN_C_END

#endif //_VHCI_DRIVER_H
