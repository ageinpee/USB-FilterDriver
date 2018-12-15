<!---
    name: Keyboard Input WDF Filter Driver (Kbfiltr)
    platform: KMDF
    language: cpp
    category: HID WDF
    description: A WDF example of a keyboard input filter driver.
    samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620194
--->
Keyboard Input Filter for Modifier Keys
=======================================
This Keyboard Filter Driver is based on Microsofts Keyboard Input WDF Filter Driver sample, that is described after this paragraph. 
The Driver currently filters the Keys: 
- Left- and Right-Ctrl
- Left- and Right-Windows
- Tab
- Alt
- Alt-Gr

Instructions to install the keyboardfilter:
-------------------------------------------
Info: 
-----
Currently only works for Win10 x32 Systems. 

Set up Keyboardfilter for a specific keyboard:
----------------------------------------------
	1) Got to Devicemanager by search "Devcemanager"/"Gerätemanager"
		in the Tab Bar. 
	2) Go to "Keyboards"/"Tastaturen" and choose the keyboard you want
		to install the driver on. 
	3) Go to "Details"
	4) Change Property to "Hardware-ID" and copy one of them. Preferablly 
		one that starts with "*". If the driver is not working later
		pick another ID. 
	5) Go to "~\USB-FilterDriver\WDF-KbdFiltr(x32)\kbfiltr\sys\Debug" and open 
		"kbfiltr.inf". This file defines the instllation of the driver so don't
		mess things up too much. 
	6) Search for "[Standard.NTx86]" and change the value afte "kbfiltr, " to the 
		Hardware-ID you retrieved from your device in the Devicemanager
	7) Save and close

Install driver:
---------------
	1) Got to Devicemanager again by search "Devcemanager"/"Gerätemanager"
		in the Tab Bar. 
	2) Go to "Keyboards"/"Tastaturen" and choose the keyboard you want
		to install the driver on. 
	3) Go to "Driver"/"Treiber".
	4) Go to "update driver"/"Treiber aktualisieren"
	5) Choose the option to search on your device for drivers.
	6) Choose the option to choose from a list of available drivers on this device. 
	7) Click on "Datenträger"
	8) Click on "Durchsuchen" and search in "~\USB-FilterDriver\WDF-KbdFiltr(x32)\
		kbfiltr\sys\Debug" for the kbfiltr.inf file. 
	9) Click "OK"
	10) Ignore all warnings that the driver is not signed yet and proceed.
	11) The system should tell you now that the driver has been installed successfully.
	12) close the windows. 
	13) The system should tell you now, that you have to restart your computer. Do that.
	14) After the restart the driver is installed and functioning. 

Important Links and Sources for this project:
---------------------------------------------
- Microsofts WDF Documentation: https://docs.microsoft.com/en-us/windows-hardware/drivers/wdf/using-the-framework-to-develop-a-driver
- Wikipedia on Windows Driver Framework: https://en.wikipedia.org/wiki/Windows_Driver_Frameworks
- Wikipedia on Windows Driver Model: https://en.wikipedia.org/wiki/Windows_Driver_Model
- Microsofts Windows driver samples: https://github.com/Microsoft/Windows-driver-samples
- KEYBOARD_INPUT_DATA structure: https://docs.microsoft.com/de-de/windows/desktop/api/ntddkbd/ns-ntddkbd-_keyboard_input_data
- KbFilter_ServiceCallback routine: https://docs.microsoft.com/en-us/previous-versions/ff542297%28v%3dvs.85%29

Keyboard Input WDF Filter Driver (Kbfiltr)
==========================================

The Kbdfltr sample is an example of a keyboard input filter driver.

This sample is WDF version of the original WDM filter driver sample. The WDM version of this sample has been deprecated.

This is an upper device filter driver sample for PS/2 keyboard. This driver layers in between the KbdClass driver and i8042prt driver and hooks the callback routine that moves keyboard inputs from the port driver to class driver. In its current state, it only hooks into the keyboard packet report chain, the keyboard initialization function, and the keyboard ISR, but does not do any processing of the data that it sees. (The hooking of the initialization function and ISR is only available in the i8042prt stack.) With additions to this current filter-only code base, the filter could conceivably add, remove, or modify input as needed.

This sample also creates a raw PDO and registers an interface so that applications can talk to the filter driver directly without going through the PS/2 devicestack. The reason for providing this additional interface is because the keyboard device is an exclusive secure device and it's not possible to open the device from usermode and send custom ioctls through it.

This driver filters input for a particular keyboard on the system. If you want to filter keyboard inputs from all the keyboards plugged into the system, you can install this driver as a class filter below the KbdClass filter driver by adding the service name of this filter driver before the KbdClass filter in the registry at:
`HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Class\{4D36E96B-E325-11CE-BFC1-08002BE10318}\UpperFilters`

## Universal Windows Driver Compliant
This sample builds a Universal Windows Driver. It uses only APIs and DDIs that are included in OneCoreUAP.

Set the hardware ID in the inx file
-----------------------------------

This step is required for automatic deployment (described later) to work properly. In the kbfiltr.inx file (located with the driver source files), find the [DDK\_Ex.Mfg.NT\$ARCH\$] section. Change the hardware ID in the %DDK\_Ex% entry from the dummy value to the hardware ID of the PS/2 keyboard on the target computer. The following example shows the hardware ID change.

```
; For XP and above
[DDK_Ex.Mfg.NT$ARCH$]
;%DDK_Ex% = kbfiltr, *PNP0BAAD
%DDK_Ex% = kbfiltr, ACPI\VEN_PNP&DEV_0303
```

Build the sample using Visual Studio
------------------------------------

In Visual Studio, on the **Build** menu, choose **Build Solution**.

For more information about using Visual Studio to build a driver package, see [Building a Driver](http://msdn.microsoft.com/en-us/library/windows/hardware/ff554644).

The test application, *kbftest.exe* is also built as part of the solution under the 'exe' folder.

Locate the built driver package
-------------------------------

In File Explorer, navigate to the folder that contains your built driver package. The location of this folder varies depending on what you set for configuration and platform. For example, if your settings are **Debug** and **x64**, the package is your solution folder under \\Debug\\Package.

The package contains these files:

File | Description 
-----|------------
Kmdfsamples.cat | A signed catalog file, which serves as the signature for the entire package. 
kbfiltr.inf | An information (INF) file that contains information needed to install the driver. 
WdfCoinstaller010xx.dll | The coinstaller for version 1.xx of KMDF. 
kbfiltr.sys | The driver file. 

Using MSBuild
-------------

As an alternative to building the Kbfiltr Filter Driver sample in Visual Studio, you can build it in a Visual Studio Command Prompt window. In Visual Studio, on the **Tools** menu, choose **Visual Studio Command Prompt**. In the Visual Studio Command Prompt window, navigate to the folder that has the solution file, kbfiltr.sln. Use the [MSBuild](http://go.microsoft.com/fwlink/p/?linkID=262804) command to build the solution. Here are some examples:

**msbuild /p:configuration="Debug" /p:platform="x64" kbfiltr.sln**

**msbuild /p:configuration="Release" /p:platform="Win32" kbfiltr.sln**

For more information about using [MSBuild](http://go.microsoft.com/fwlink/p/?linkID=262804) to build a driver package, see [Building a Driver](http://msdn.microsoft.com/en-us/library/windows/hardware/ff554644).

Run the sample
--------------

The computer where you install the driver is called the *target computer* or the *test computer*. Typically this is a separate computer from where you develop and build the driver package. The computer where you develop and build the driver is called the *host computer*.

The process of moving the driver package to the target computer and installing the driver is called *deploying the driver*. You can deploy kbfiltr sample driver automatically or manually.

### Automatic deployment

Before you automatically deploy a driver, you must provision the target computer. For instructions, see [Configuring a Computer for Driver Deployment, Testing, and Debugging](http://msdn.microsoft.com/en-us/library/windows/hardware/).

1.  On the host computer, in Visual Studio, in Solution Explorer, right click **package** (lower case), and choose **Properties**. Navigate to **Configuration Properties \> Driver Install \> Deployment**.
2.  Check **Enable deployment**, and check **Remove previous driver versions before deployment**. For **Target Computer Name**, select the name of a target computer that you provisioned previously. Select **Install and Verify**, and choose **Default Driver Package Installation Task** in the list. Click **OK**.
3.  On the **Build** menu, choose **Deploy Package** or **Build Solution**.

### Manual deployment

Before you manually deploy a driver, you must turn on test signing and install a certificate on the target computer. You also need to copy the [DevCon](http://msdn.microsoft.com/en-us/library/windows/hardware/ff544707) tool to the target computer. For instructions, see [Preparing a Computer for Manual Driver Deployment](http://msdn.microsoft.com/en-us/library/windows/hardware/dn265571).

1.  Copy all of the files in your driver package to a folder on the target computer (for example, c:\\KbfiltrDriverPackage).
2.  On the target computer, open a Command Prompt window as Administrator. Navigate to your driver package folder, and enter the Devcon command with the correct hardware ID, such as:

    **Devcon install kbfiltr.inf ACPI\\VEN\_PNP&DEV\_0303**

    -or-

    Using Device Manager, update the driver for the PS/2 Keyboard by manually selecting kbfiltr.inf from the location where you copied the driver files.

View the installed driver in Device Manager
-------------------------------------------

On the target computer, in a Command Prompt window, enter **devmgmt** to open Device Manager. In Device Manager, on the **View** menu, choose **Devices by type**. In the device tree, locate **DDK Example Device that needs filtering** under the **Keyboards** node.

Testing
-------

To use the test application provided with the sample, it must be copied to the target computer manually. Save the kbftest.exe file from the folder where the build result is placed (for example, exe\\Debug). This file is copied somewhere on the target, possibly where the driver package files are located. The test application is the executed on the target computer in a Command Prompt using **kbftest** as the command.

**Tip** To avoid DLL dependencies for kbftext.exe, and the need to copy additional files, select the statically linked run-time library when building.

