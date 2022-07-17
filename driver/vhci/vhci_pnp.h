#pragma once

#include "basetype.h"
#include "vhci_dev.h"

#define HWID_ROOT	L"USBIPWIN\\root"
#define HWID_VHCI	L"USBIPWIN\\vhci"

#define DEVID_VHCI	HWID_VHCI

/*
 * The first hardware ID in the list should be the device ID, and
 * the remaining IDs should be listed in order of decreasing suitability.
 */
#define HWIDS_VHCI	DEVID_VHCI L"\0"

#define INITIALIZE_PNP_STATE(_Data_)    \
        (_Data_)->common.DevicePnPState =  NotStarted;\
        (_Data_)->common.PreviousPnPState = NotStarted;

#define SET_NEW_PNP_STATE(vdev, _state_) \
        do { (vdev)->PreviousPnPState = (vdev)->DevicePnPState;\
        (vdev)->DevicePnPState = (_state_); } while (0)

#define RESTORE_PREVIOUS_PNP_STATE(vdev)   \
        do { (vdev)->DevicePnPState = (vdev)->PreviousPnPState; } while (0)

extern PAGEABLE NTSTATUS vhci_unplug_port(pvhci_dev_t vhci, CHAR port);
