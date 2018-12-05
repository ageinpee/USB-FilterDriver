#include <ntddk.h>

typedef struct {
	PDEVICE_OBJECT LowerKbdDevice;
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;


typedef struct _KEYBOARD_INPUT_DATA {
	USHORT UnitId;
	USHORT MakeCode;
	USHORT Flags;
	USHORT Reserved;
	ULONG  ExtraInformation;
} KEYBOARD_INPUT_DATA, *PKEYBOARD_INPUT_DATA;


PDEVICE_OBJECT KbdDevice = NULL;
ULONG pendingkey = 0;


//---------------------------------------------------------------------------------
//-------------------------------- Functions --------------------------------------

VOID DriverUnload(PDRIVER_OBJECT DriverObject)
{
	LARGE_INTEGER interval = { 0 };

	PDEVICE_OBJECT DeviceObject = DriverObject->DeviceObject;
	interval.QuadPart = -10 * 1000 * 1000;
	IoDetachDevice( ((PDEVICE_EXTENSION)DeviceObject->DeviceExtension)->LowerKbdDevice );
	
	while (pendingkey) {
		KeDelayExecutionThread(KernelMode, FALSE, &interval);
	}

	IoDeleteDevice(KbdDevice);
	KdPrint(("unload Driver\r\n"));
}


NTSTATUS DispatchPass(PDEVICE_OBJECT DeviceObject, PIRP Irp) 
{
	IoCopyCurrentIrpStackLocationToNext(Irp);
	return IoCallDriver(((PDEVICE_EXTENSION)DeviceObject->DeviceExtension)->LowerKbdDevice, Irp); //pass key to lower driver
}


NTSTATUS ReadComplete(PDEVICE_OBJECT DeviceObject, PIRP Irp, PVOID Context) 
{
	DeviceObject = NULL;
	Context = NULL;

	CHAR* keyflag[4] = { "KeyDown","KeyUp","E0","E1" };
	PKEYBOARD_INPUT_DATA Keys = (PKEYBOARD_INPUT_DATA)Irp->AssociatedIrp.SystemBuffer;
	int structnum = Irp->IoStatus.Information / sizeof(KEYBOARD_INPUT_DATA);
	int i;

	if (Irp->IoStatus.Status == STATUS_SUCCESS) {
		for (i=0; i < structnum; i++) {
			KdPrint(("the scan code is %x (%s)\n", Keys->MakeCode, keyflag[Keys->Flags]));
		}
	}

	if (Irp->PendingReturned) {
		IoMarkIrpPending(Irp);
	}

	pendingkey--;
	return Irp->IoStatus.Status;
}


NTSTATUS DispatchRead(PDEVICE_OBJECT DeviceObject, PIRP Irp) 
{	
	IoCopyCurrentIrpStackLocationToNext(Irp);

	IoSetCompletionRoutine(Irp, ReadComplete, NULL, TRUE, TRUE, TRUE); //executes ReadComplete when Irp is returned from device

	pendingkey++;

	return IoCallDriver(((PDEVICE_EXTENSION)DeviceObject->DeviceExtension)->LowerKbdDevice, Irp); //pass key to lower driver
}


NTSTATUS AttachDevice(PDRIVER_OBJECT DriverObject)
{
	NTSTATUS status;
	UNICODE_STRING TargetDevice = RTL_CONSTANT_STRING(L"\\Device\\KeyboardClass0");

	status = IoCreateDevice(DriverObject, sizeof(DEVICE_EXTENSION), NULL, FILE_DEVICE_KEYBOARD, 0, FALSE, &KbdDevice);
	if (!NT_SUCCESS(status)) {
		return status;
	}

	KbdDevice->Flags |= DO_BUFFERED_IO;
	KbdDevice->Flags &= DO_DEVICE_INITIALIZING;

	RtlZeroMemory(KbdDevice->DeviceExtension, sizeof(DEVICE_EXTENSION));
	
	IoAttachDevice(KbdDevice, &TargetDevice, &((PDEVICE_EXTENSION)KbdDevice->DeviceExtension)->LowerKbdDevice);
	if (!NT_SUCCESS(status)) {
		IoDeleteDevice(KbdDevice);
		return status;
	}
	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{	
	RegistryPath = NULL;
	NTSTATUS status;
	int i;
	DriverObject->DriverUnload = DriverUnload;

	for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++) {
		DriverObject->MajorFunction[i] = DispatchPass;
	}

	DriverObject->MajorFunction[IRP_MJ_READ] = DispatchRead;
	
	KdPrint(("dirver is loaded\r\n"));
	status = AttachDevice(DriverObject);
	if (!NT_SUCCESS(status)) {
		KdPrint(("attaching is failing\r\n"));
	}
	else {
		KdPrint(("attaching succeeds\r\n"));
	}
	return status;
}