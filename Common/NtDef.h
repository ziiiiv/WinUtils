#ifndef _NT_DEF_H_
#define _NT_DEF_H_


// User Mode Headers
#ifndef _KERNEL_MODE
#ifndef _WINDOWS_
#include <Windows.h>
#include <winternl.h>

#endif

#endif
#ifdef __cplusplus
extern "C" {
#endif





// User Mode Values
#ifndef _KERNEL_MODE
#define _KERNEL_MODE
typedef struct _FILE_PIPE_LOCAL_INFORMATION {
    ULONG NamedPipeType;
    ULONG NamedPipeConfiguration;
    ULONG MaximumInstances;
    ULONG CurrentInstances;
    ULONG InboundQuota;
    ULONG ReadDataAvailable;
    ULONG OutboundQuota;
    ULONG WriteQuotaAvailable;
    ULONG NamedPipeState;
    ULONG NamedPipeEnd;
} FILE_PIPE_LOCAL_INFORMATION, * PFILE_PIPE_LOCAL_INFORMATION;


#endif
#ifdef __cplusplus
}
#endif

#endif