#pragma once

#include <ntddk.h>

#ifdef DBG

#define DBGE(part, fmt, ...)	DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_MASK | (part), DRVPREFIX ":(EE) " fmt, ## __VA_ARGS__)
#define DBGW(part, fmt, ...)	DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_MASK | ((part) << 1), DRVPREFIX ":(WW) " fmt, ## __VA_ARGS__)
#define DBGI(part, fmt, ...)	DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_MASK | ((part) << 2), DRVPREFIX ": " fmt, ## __VA_ARGS__)

#else

#define DBGE(part, fmt, ...)
#define DBGW(part, fmt, ...)
#define DBGI(part, fmt, ...)

#endif	

#define ERROR(fmt, ...)	DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, DRVPREFIX ":(EE) " fmt, ## __VA_ARGS__)
#define INFO(fmt, ...)	DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_INFO_LEVEL, DRVPREFIX ": " fmt, ## __VA_ARGS__)
