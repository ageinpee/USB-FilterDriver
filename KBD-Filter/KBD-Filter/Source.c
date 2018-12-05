#include <ntddk.h>

typedef struct {
	PDEVICE_OBJECT LowerKbdDevice;
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

PDEVICE_OBJECT KbdDevice = NULL;

VOID DriverUnload(PDRIVER_OBJECT DriverObject)
{
	PDEVICE_OBJECT DeviceObject = DriverObject->DeviceObject;
	IoDetachDevice( ((PDEVICE_EXTENSION)DeviceObject->DeviceExtension)->LowerKbdDevice );
	IoDeleteDevice(KbdDevice);
	KdPrint(("unload Driver\r\n"));
}


NTSTATUS DispatchPass(PDEVICE_OBJECT DriverObject, PIRP Irp) {
	DriverObject = NULL;
	Irp = NULL;
	return STATUS_SUCCESS;
}


NTSTATUS DispatchRead(PDEVICE_OBJECT DriverObject, PIRP Irp) {
	DriverObject = NULL;
	Irp = NULL;
	return STATUS_SUCCESS;
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