﻿#include "public.h"
#include "minifilter.h"
#include <fltKernel.h>
#include "rDirectory.h"
#include "utiltools.h"

// extern count +1 kflt.c
PFLT_FILTER         g_FltServerPortEvnet = NULL;
static ULONG        g_fltregstatus = FALSE;

static  BOOLEAN		    g_fsflt_ips_monitorprocess = FALSE;
static  KSPIN_LOCK		g_fsflt_ips_monitorlock = 0;

#define PTDBG_TRACE_ROUTINES            0x00000001
#define PTDBG_TRACE_OPERATION_STATUS    0x00000002

ULONG gTraceFlags1 = 0;
#define PT_DBG_PRINT( _dbgLevel, _string )          \
    (FlagOn(gTraceFlags1,(_dbgLevel)) ?              \
        DbgPrint _string :                          \
        ((int)0))

NTSTATUS
FsFilter1Unload(
    _In_ FLT_FILTER_UNLOAD_FLAGS Flags
);

NTSTATUS
FsFilter1InstanceQueryTeardown(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
);

NTSTATUS
FsFilter1InstanceSetup(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_SETUP_FLAGS Flags,
    _In_ DEVICE_TYPE VolumeDeviceType,
    _In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType
);

VOID
FsFilter1InstanceTeardownStart(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
);

VOID
FsFilter1InstanceTeardownComplete(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
);

//
//  Assign text sections for each routine.
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, FsFilter1Unload)
#pragma alloc_text(PAGE, FsFilter1InstanceQueryTeardown)
#pragma alloc_text(PAGE, FsFilter1InstanceSetup)
#pragma alloc_text(PAGE, FsFilter1InstanceTeardownStart)
#pragma alloc_text(PAGE, FsFilter1InstanceTeardownComplete)
#endif

FLT_PREOP_CALLBACK_STATUS
FsFilter1PreOperation(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID* CompletionContext
);

FLT_POSTOP_CALLBACK_STATUS
FsFilter1PostOperation(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_opt_ PVOID CompletionContext,
    _In_ FLT_POST_OPERATION_FLAGS Flags
);

FLT_PREOP_CALLBACK_STATUS
FsFilterAntsDrvPreExe(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID* CompletionContext
);

FLT_POSTOP_CALLBACK_STATUS
FsFilterAntsDrPostFileHide(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_opt_ PVOID CompletionContext,
    _In_ FLT_POST_OPERATION_FLAGS Flags
);

//
//  operation registration
//

CONST FLT_OPERATION_REGISTRATION Callbacks[] = {
      // file create
      { IRP_MJ_CREATE,
        0,
        FsFilter1PreOperation,
        NULL/*FsFilter1PostOperation*/},

      //// hide file
      //{ IRP_MJ_DIRECTORY_CONTROL,
      //  0,
      //  FsFilterAntsDrPostFileHide,
      //  NULL },

      //// disable exe execute
      //{ IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION,
      //  0,
      //  FsFilterAntsDrvPreExe,
      //  NULL },

      //// delete rename
      //{ IRP_MJ_SET_INFORMATION,
      //  0,
      //  FsFilter1PreOperation,
      //  NULL },


#if 0 // TODO - List all of the requests to filter.
    { IRP_MJ_CREATE,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_CREATE_NAMED_PIPE,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_CLOSE,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_READ,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_WRITE,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_QUERY_INFORMATION,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_SET_INFORMATION,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_QUERY_EA,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_SET_EA,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_FLUSH_BUFFERS,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_QUERY_VOLUME_INFORMATION,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_SET_VOLUME_INFORMATION,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_DIRECTORY_CONTROL,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_FILE_SYSTEM_CONTROL,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_DEVICE_CONTROL,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_INTERNAL_DEVICE_CONTROL,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_SHUTDOWN,
      0,
      FsFilter1PreOperationNoPostOperation,
      NULL },                               //post operations not supported

    { IRP_MJ_LOCK_CONTROL,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_CLEANUP,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_CREATE_MAILSLOT,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_QUERY_SECURITY,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_SET_SECURITY,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_QUERY_QUOTA,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_SET_QUOTA,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_PNP,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_ACQUIRE_FOR_MOD_WRITE,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_RELEASE_FOR_MOD_WRITE,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_ACQUIRE_FOR_CC_FLUSH,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_RELEASE_FOR_CC_FLUSH,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_NETWORK_QUERY_OPEN,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_MDL_READ,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_MDL_READ_COMPLETE,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_PREPARE_MDL_WRITE,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_MDL_WRITE_COMPLETE,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_VOLUME_MOUNT,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

    { IRP_MJ_VOLUME_DISMOUNT,
      0,
      FsFilter1PreOperation,
      FsFilter1PostOperation },

#endif // TODO

    { IRP_MJ_OPERATION_END }
};

//
//  This defines what we want to filter with FltMgr
//

CONST FLT_REGISTRATION FilterRegistration = {

    sizeof(FLT_REGISTRATION),           //  Size
    FLT_REGISTRATION_VERSION,           //  Version
    0,                                  //  Flags

    NULL,                               //  Context
    Callbacks,                          //  Operation callbacks

    NULL,                               //  MiniFilterUnload

    FsFilter1InstanceSetup,                    //  InstanceSetup
    FsFilter1InstanceQueryTeardown,            //  InstanceQueryTeardown
    FsFilter1InstanceTeardownStart,            //  InstanceTeardownStart
    FsFilter1InstanceTeardownComplete,         //  InstanceTeardownComplete

    NULL,                               //  GenerateFileName
    NULL,                               //  GenerateDestinationFileName
    NULL                                //  NormalizeNameComponent

};

void FsFlt_SetDirectoryIpsMonitor(code)
{
    KLOCK_QUEUE_HANDLE lh;
    KeAcquireInStackQueuedSpinLock(&g_fsflt_ips_monitorlock, &lh);
    g_fsflt_ips_monitorprocess = code;
    KeReleaseInStackQueuedSpinLock(&lh);

    if (FALSE == code)
        utiltools_sleep(500);
}

NTSTATUS FsMini_Init(PDRIVER_OBJECT DriverObject)
{
    NTSTATUS nStatus = FltRegisterFilter(DriverObject, &FilterRegistration, &g_FltServerPortEvnet);
    if (NT_SUCCESS(nStatus))
    {
        g_fltregstatus = TRUE;
        // Init Rule
        rDirectory_IpsInit();
    }

    KeInitializeSpinLock(&g_fsflt_ips_monitorlock);

    return nStatus;
}

NTSTATUS FsMini_Clean()
{
    rDirectory_IpsClean();
    if ((TRUE == g_fltregstatus) && g_FltServerPortEvnet)
    {
        FltUnregisterFilter(g_FltServerPortEvnet);
        g_FltServerPortEvnet = NULL;
    }
    return STATUS_SUCCESS;
}

NTSTATUS FsMini_Free()
{
    return FsMini_Clean();
}

NTSTATUS Mini_StartFilter()
{
    //
    //  Start filtering i/o
    //
    if ((g_FltServerPortEvnet == NULL) || !g_fltregstatus)
        return STATUS_UNSUCCESSFUL;

    NTSTATUS status = FltStartFiltering(g_FltServerPortEvnet);
    if (!NT_SUCCESS(status)) {

        FltUnregisterFilter(g_FltServerPortEvnet);
    }

    return status;
}

NTSTATUS
FsFilter1Unload(
    _In_ FLT_FILTER_UNLOAD_FLAGS Flags
)
{
    UNREFERENCED_PARAMETER(Flags);

    PAGED_CODE();

    PT_DBG_PRINT(PTDBG_TRACE_ROUTINES,
        ("FsFilter1!FsFilter1Unload: Entered\n"));

    if ((TRUE == g_fltregstatus) && g_FltServerPortEvnet)
        FltUnregisterFilter(g_FltServerPortEvnet);

    return STATUS_SUCCESS;
}

NTSTATUS
FsFilter1InstanceSetup(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_SETUP_FLAGS Flags,
    _In_ DEVICE_TYPE VolumeDeviceType,
    _In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType
)
{
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Flags);
    UNREFERENCED_PARAMETER(VolumeDeviceType);
    UNREFERENCED_PARAMETER(VolumeFilesystemType);

    PAGED_CODE();

    PT_DBG_PRINT(PTDBG_TRACE_ROUTINES,
        ("FsFilter1!FsFilter1InstanceSetup: Entered\n"));

    return STATUS_SUCCESS;
}

NTSTATUS
FsFilter1InstanceQueryTeardown(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
)
{
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Flags);

    PAGED_CODE();

    PT_DBG_PRINT(PTDBG_TRACE_ROUTINES,
        ("FsFilter1!FsFilter1InstanceQueryTeardown: Entered\n"));

    return STATUS_SUCCESS;
}

VOID
FsFilter1InstanceTeardownStart(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
)
{
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Flags);

    PAGED_CODE();

    PT_DBG_PRINT(PTDBG_TRACE_ROUTINES,
        ("FsFilter1!FsFilter1InstanceTeardownStart: Entered\n"));
}

VOID
FsFilter1InstanceTeardownComplete(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
)
{
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Flags);

    PAGED_CODE();

    PT_DBG_PRINT(PTDBG_TRACE_ROUTINES,
        ("FsFilter1!FsFilter1InstanceTeardownComplete: Entered\n"));
}


FLT_PREOP_CALLBACK_STATUS
FsFilter1PreOperation(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID* CompletionContext
)
{
    UNREFERENCED_PARAMETER(Data);
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(CompletionContext);

    if (!g_fsflt_ips_monitorprocess)
        return FLT_PREOP_SUCCESS_WITH_CALLBACK;

    const KIRQL irql = KeGetCurrentIrql();
    if (irql <= APC_LEVEL)
    {
        DbgBreakPoint();
        // 1. find Rule Mods directoryPath 
        PFLT_FILE_NAME_INFORMATION nameInfo = NULL;
        NTSTATUS status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &nameInfo);
        if (!NT_SUCCESS(status))
            return FLT_PREOP_SUCCESS_WITH_CALLBACK;
        FltReleaseFileNameInformation(Data);
        
        // 2. query processid to processpath
        BOOLEAN QueryPathStatus = FALSE;
        WCHAR path[260 * 2] = { 0 };
        const ULONG pid = FltGetRequestorProcessId(Data);
        const ULONG processid = (int)PsGetCurrentProcessId();
        if (!QueryProcessNamePath((DWORD)processid, path, sizeof(path)))
            return FLT_PREOP_SUCCESS_WITH_CALLBACK;
            
        // 3. find processpath to ruleName
        int iRuleMods = 0;
        QueryPathStatus = rDirectory_IsIpsProcessNameInList(path, &iRuleMods);
        if (!QueryPathStatus || !iRuleMods)
            return FLT_PREOP_SUCCESS_WITH_CALLBACK;

        do {
            const unsigned char IRP_MJ_CODE = Data->Iopb->MajorFunction;
            if (IRP_MJ_CODE == IRP_MJ_CREATE)
            {
                BOOLEAN bhitOpear = FALSE;
                // create file
                if (((Data->Iopb->Parameters.Create.Options >> 24) & 0x000000ff) == FILE_CREATE ||
                    ((Data->Iopb->Parameters.Create.Options >> 24) & 0x000000ff) == FILE_OPEN_IF ||
                    ((Data->Iopb->Parameters.Create.Options >> 24) & 0x000000ff) == FILE_OVERWRITE_IF)
                {
                    bhitOpear = TRUE;
                }
                // move into folder
                //if (Data->Iopb->OperationFlags == '\x05')
                //  bhitOpear = TRUE;

                // block rule
                if (bhitOpear && (iRuleMods == 2))
                    break;
            }
            else if (IRP_MJ_CODE == IRP_MJ_SET_INFORMATION)
            {
                // delete file
                if (iRuleMods == 2 && Data->Iopb->Parameters.SetFileInformation.FileInformationClass == FileDispositionInformation)
                    break;

                // rename file
                if (iRuleMods == 2 && Data->Iopb->Parameters.SetFileInformation.FileInformationClass == FileRenameInformation)
                    break;
            }
            return FLT_PREOP_SUCCESS_WITH_CALLBACK;
        } while (FALSE);

        Data->IoStatus.Status = STATUS_ACCESS_DENIED;
        Data->IoStatus.Information = 0;
        return FLT_PREOP_COMPLETE;
    }

    return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}

FLT_POSTOP_CALLBACK_STATUS
FsFilter1PostOperation(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_opt_ PVOID CompletionContext,
    _In_ FLT_POST_OPERATION_FLAGS Flags
)
{
    UNREFERENCED_PARAMETER(Data);
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(CompletionContext);
    UNREFERENCED_PARAMETER(Flags);

    PT_DBG_PRINT(PTDBG_TRACE_ROUTINES,
        ("FsFilter1!FsFilter1PostOperation: Entered\n"));

    return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_POSTOP_CALLBACK_STATUS
FsFilterAntsDrPostFileHide(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_opt_ PVOID CompletionContext,
    _In_ FLT_POST_OPERATION_FLAGS Flags
)
{
    PWCHAR HideFileName = L"HideTest";

    DbgPrint("Entry function hide\n");
    UNREFERENCED_PARAMETER(Data);
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(CompletionContext);
    UNREFERENCED_PARAMETER(Flags);

    PVOID Bufferptr = NULL;

    if (FlagOn(Flags, FLTFL_POST_OPERATION_DRAINING))
    {
        return FLT_POSTOP_FINISHED_PROCESSING;
    }

    if (Data->Iopb->MinorFunction == IRP_MN_QUERY_DIRECTORY &&
        (Data->Iopb->Parameters.DirectoryControl.QueryDirectory.FileInformationClass == FileBothDirectoryInformation) &&
        Data->Iopb->Parameters.DirectoryControl.QueryDirectory.Length > 0 &&
        NT_SUCCESS(Data->IoStatus.Status))
    {
        if (Data->Iopb->Parameters.DirectoryControl.QueryDirectory.MdlAddress != NULL)
        {

            Bufferptr = MmGetSystemAddressForMdl(Data->Iopb->Parameters.DirectoryControl.QueryDirectory.MdlAddress,
                NormalPagePriority);
        }
        else
        {
            Bufferptr = Data->Iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer;
        }

        if (Bufferptr == NULL)
            return FLT_POSTOP_FINISHED_PROCESSING;

        PFILE_BOTH_DIR_INFORMATION Currentfileptr = (PFILE_BOTH_DIR_INFORMATION)Bufferptr;
        PFILE_BOTH_DIR_INFORMATION prefileptr = Currentfileptr;
        PFILE_BOTH_DIR_INFORMATION nextfileptr = 0;
        ULONG nextOffset = 0;
        if (Currentfileptr == NULL)
            return FLT_POSTOP_FINISHED_PROCESSING;

        int nModifyflag = 0;
        int removedAllEntries = 1;
        do {
            nextOffset = Currentfileptr->NextEntryOffset;

            nextfileptr = (PFILE_BOTH_DIR_INFORMATION)((PCHAR)(Currentfileptr)+nextOffset);

            if ((prefileptr == Currentfileptr) &&
                (_wcsnicmp(Currentfileptr->FileName, HideFileName, wcslen(HideFileName)) == 0) &&
                (Currentfileptr->FileNameLength == 2)
                )
            {
                RtlCopyMemory(Currentfileptr->FileName, L".", 2);
                Currentfileptr->FileNameLength = 0;
                FltSetCallbackDataDirty(Data);
                return FLT_POSTOP_FINISHED_PROCESSING;
            }

            if (_wcsnicmp(Currentfileptr->FileName, HideFileName, wcslen(HideFileName)) == 0 &&
                (Currentfileptr->FileNameLength == 2)
                )
            {
                if (nextOffset == 0)
                    prefileptr->NextEntryOffset = 0;
                else
                    prefileptr->NextEntryOffset = (ULONG)((PCHAR)Currentfileptr - (PCHAR)prefileptr + nextOffset);
                nModifyflag = 1;
            }
            else
            {
                removedAllEntries = 0;
                prefileptr = Currentfileptr;
            }
            Currentfileptr = nextfileptr;

        } while (nextOffset != 0);

        if (nModifyflag)
        {
            if (removedAllEntries)
                Data->IoStatus.Status = STATUS_NO_MORE_ENTRIES;
            else
                FltSetCallbackDataDirty(Data);
        }
    }
    return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FsFilterAntsDrvPreExe(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID* CompletionContext
)
{
    DbgPrint("[MiniFilter]: Read\n");
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(CompletionContext);
    PAGED_CODE();
    __try {
        if (Data->Iopb->Parameters.AcquireForSectionSynchronization.PageProtection == PAGE_EXECUTE)
        {
            return FLT_PREOP_SUCCESS_NO_CALLBACK;
        }
        /*
            DbPrint("access denied");
            Data->IoStatus.Status = STATUS_ACCESS_DENIED
            Data->Iostatus.information = 0;
            return FLT_PREOP_COMPLETE;
        */
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        DbgPrint("NPPreRead EXCEPTION_EXECUTE_HANDLER\n");
    }

    return FLT_PREOP_SUCCESS_NO_CALLBACK;
}