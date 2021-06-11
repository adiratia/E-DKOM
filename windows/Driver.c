#include <ntddk.h>
#include <wdf.h>

#define PRCB_STRING L"KeQueryPrcbAddress"
#define DISPATCHER_READY_LIST_HEAD_OFFSET 0x7c80
#define KAPC_STATE_OFFSET 0x98
#define KPROCESS_OFFSET 0x20
#define WAITLISTENTRY_OFFSET 0xD8
#define IMAGEFILENAME_OFFSET 0x5a8
typedef BYTE* (*KeQueryPrcbAddress) (int);

DRIVER_INITIALIZE DriverEntry;
LIST_ENTRY* frozenProcess;
LIST_ENTRY* head;


 /**
 * @brief search and remove the process from the DispatcherReadyQueue
 */
static BOOLEAN ModifyDispatcher(LIST_ENTRY* dispatcherReadyQueue) {
	PETHREAD currentThread;
	PEPROCESS currentProcess;

	__try {

		head = dispatcherReadyQueue;
		dispatcherReadyQueue = dispatcherReadyQueue->Flink; // points to the first list element.
		while ((head != dispatcherReadyQueue) && dispatcherReadyQueue) {
			// get the current process stuct
			currentThread = (PETHREAD)((BYTE*)dispatcherReadyQueue - WAITLISTENTRY_OFFSET);
			// get the current thread stuct
			currentProcess = *(PEPROCESS*)((BYTE*)currentThread + (KAPC_STATE_OFFSET + KPROCESS_OFFSET));
			const char* currentProcessName = (const char*)((BYTE*)currentProcess + IMAGEFILENAME_OFFSET);
			if (strncmp("sleeper.exe", currentProcessName, 14) == 0) {
				// process is found => remove it from the linked list.
				frozenProcess = dispatcherReadyQueue;
				RemoveEntryList(dispatcherReadyQueue);
				return TRUE;
			}
			dispatcherReadyQueue = dispatcherReadyQueue->Flink;
		}
		
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		DbgPrint("ERROR\n");
	}
	return FALSE;
}

 /**
 * @brief get the PRCB struct
 */
static LIST_ENTRY* GetDispatcherList() {
	UNICODE_STRING name;
	KeQueryPrcbAddress func = NULL;
	RtlInitUnicodeString(&name, L"KeQueryPrcbAddress");
	func = (KeQueryPrcbAddress)MmGetSystemRoutineAddress(&name);
	return (LIST_ENTRY*) (func(0) + DISPATCHER_READY_LIST_HEAD_OFFSET);
}

 /**
 * @brief function called when unloading the kernal driver
 */
VOID Unload() {
	DbgPrint("Driver Unloaded\n");
	// return the frozen process into the linked list
	InsertTailList(head, frozenProcess);
}

/**
 * @brief search each dispatcher ready list for the process
 */
static VOID FindTaskInDispatcher(LIST_ENTRY* dispatcherReadyList) {
	BOOLEAN isFound = FALSE;
	while (!isFound) {
		// for each queue
		for (int i = 0; i < 31; i++) {
			isFound = ModifyDispatcher(&dispatcherReadyList[i]);
			if (isFound) break;
		}
	}
}

/**
 * @brief function called when loading the kernal driver
 */
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
	DbgPrint("Driver loaded!\n");
	
	DriverObject->DriverUnload = (PDRIVER_UNLOAD)Unload;	
	LIST_ENTRY* DispatcherReadyList = GetDispatcherList();

	FindTaskInDispatcher(DispatcherReadyList);

	// DbgPrint("%lu\n", KeQueryActiveProcessorCount(NULL));
	
	return STATUS_SUCCESS;
}
