/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that app can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_KBDFilter,
    0xc1e7bea2,0xd62e,0x45e8,0x88,0x48,0xb7,0x44,0xb6,0xcd,0x85,0xe8);
// {c1e7bea2-d62e-45e8-8848-b744b6cd85e8}
