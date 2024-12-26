#include <vector>

#include "CH9326DLL.h"

#define SERVICE_NAME TEXT("USBSwitchButtonService")
#define VID 0x337B
#define PID 0x4001

SERVICE_STATUS serviceStatus = {0};
SERVICE_STATUS_HANDLE statusHandle = NULL;
HANDLE serviceStopEvent = INVALID_HANDLE_VALUE;
HANDLE switchMonitorEvent = INVALID_HANDLE_VALUE;

bool testDoNotSwitch = false;

void switchUSB (HANDLE device) {
    CH9326WriteIOData(device, 0b0001);
    Sleep(50);
    CH9326WriteIOData(device, 0b0000);
}

void waitForDeviceConnection (int vid, int pid, HANDLE &device) {
    while (true) {
        HANDLE tempHandle = CH9326OpenDevice(vid, pid);
        if (tempHandle == INVALID_HANDLE_VALUE) {
            Sleep(100);
        } else {
            device = tempHandle;
            CH9326SetIODir(device, 0b0001);
            return;
        }
    }
}

void ReportStopWithError (DWORD serviceSpecificExitCode) {
    serviceStatus.dwControlsAccepted = 0;
    serviceStatus.dwCurrentState = SERVICE_STOPPED;
    serviceStatus.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
    serviceStatus.dwServiceSpecificExitCode = serviceSpecificExitCode;
    serviceStatus.dwCheckPoint = 1;
}

void WINAPI ServiceCtrlHandler (DWORD ctrlCode) {
    switch (ctrlCode) {
        case SERVICE_CONTROL_STOP:
            if (serviceStatus.dwCurrentState != SERVICE_RUNNING)
                break;

            serviceStatus.dwControlsAccepted = 0;
            serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
            serviceStatus.dwWin32ExitCode = 0;
            serviceStatus.dwCheckPoint = 4;

            SetServiceStatus(statusHandle, &serviceStatus);

            SetEvent(serviceStopEvent);
            break;

        default:
            break;
    }
}

DWORD WINAPI ServiceWorkerThread (LPVOID lpParam) {
    HANDLE device;

    waitForDeviceConnection(VID, PID, device);

    unsigned short ioValue = 0;
    bool priorButtonValue = false;

    while (WaitForSingleObject(serviceStopEvent, 1) != WAIT_OBJECT_0) {
        if (!CH9326ReadIOData(device, &ioValue)) {
            waitForDeviceConnection(VID, PID, device);
        }

        bool buttonValue = ioValue & 0b0010;
        if (buttonValue && !priorButtonValue) {
            if (!testDoNotSwitch) {
                switchUSB(device);
            }

            SetEvent(switchMonitorEvent);

            // switchMonitor(physicalMonitors[0].hPhysicalMonitor);
        }
        priorButtonValue = buttonValue;

        Sleep(50);
    }

    return ERROR_SUCCESS;
}

void WINAPI ServiceMain (DWORD argc, LPSTR *argv) {
    statusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);

    if (statusHandle == NULL) {
        return;
    }

    ZeroMemory(&serviceStatus, sizeof(serviceStatus));
    serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    serviceStatus.dwControlsAccepted = 0;
    serviceStatus.dwCurrentState = SERVICE_START_PENDING;
    serviceStatus.dwWin32ExitCode = 0;
    serviceStatus.dwServiceSpecificExitCode = 0;
    serviceStatus.dwCheckPoint = 0;
    SetServiceStatus(statusHandle, &serviceStatus);

    if (CH9326DllInt() != true) {
        ReportStopWithError(-1);
        return;
    }

    SECURITY_DESCRIPTOR sd;
    InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);

    SECURITY_ATTRIBUTES sa = {0};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = FALSE;
    sa.lpSecurityDescriptor = &sd;

    switchMonitorEvent = CreateEventA(&sa, FALSE, FALSE, TEXT("Global\\USBSWITCHSWITCHMONITOR"));

    serviceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (serviceStopEvent == NULL) {
        serviceStatus.dwControlsAccepted = 0;
        serviceStatus.dwCurrentState = SERVICE_STOPPED;
        serviceStatus.dwWin32ExitCode = GetLastError();
        serviceStatus.dwCheckPoint = 1;
        SetServiceStatus(statusHandle, &serviceStatus);
        return;
    }

    serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    serviceStatus.dwCurrentState = SERVICE_RUNNING;
    serviceStatus.dwWin32ExitCode = 0;
    serviceStatus.dwCheckPoint = 0;
    SetServiceStatus(statusHandle, &serviceStatus);

    HANDLE workerThread = CreateThread(NULL, 0, ServiceWorkerThread, NULL, 0, NULL);
    WaitForSingleObject(workerThread, INFINITE);
}

int main () {
    SERVICE_TABLE_ENTRY ServiceTable[] = {
            {SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION) ServiceMain},
            NULL, NULL
    };

    if (!StartServiceCtrlDispatcher(ServiceTable)) {
        return GetLastError();
    }

    return 0;
}