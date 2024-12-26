//
// Created by maria on 09/06/2024.
//

#include <iostream>
#include <vector>

#include <windows.h>
#include <physicalmonitorenumerationapi.h>
#include <lowlevelmonitorconfigurationapi.h>

std::vector<HMONITOR> monitors;

DWORD physicalMonitorCount;
LPPHYSICAL_MONITOR physicalMonitors = NULL;

DWORD dwWaitResult;
HANDLE switchMonitorEvent = INVALID_HANDLE_VALUE;

void switchMonitor (HANDLE monitor) {
    DWORD currentValue = 0;
    GetVCPFeatureAndVCPFeatureReply(monitor, 0x60, nullptr, &currentValue, nullptr);

    // 15 is DisplayPort, 27 is USB C
    DWORD newValue;
    if ((currentValue & 0x00FF) == 15) {
        newValue = 27;
    } else {
        newValue = 15;
    }

    SetVCPFeature(monitor, 0x60, newValue);
}

WINBOOL CALLBACK MonitorEnumProc (HMONITOR hMonitor, HDC hdc, LPRECT lprcMonitor, LPARAM pData) {
    monitors.push_back(hMonitor);
    return true;
}

int main () {
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, NULL);

    if (!GetNumberOfPhysicalMonitorsFromHMONITOR(monitors[0], &physicalMonitorCount)) {
        std::cout << "Failed to get number of physical monitors\n";
        return 1;
    } else {
        physicalMonitors = (LPPHYSICAL_MONITOR) malloc(physicalMonitorCount* sizeof(PHYSICAL_MONITOR));

        if (physicalMonitors == NULL) {
            std::cout << "No physical monitors\n";
            return 2;
        } else {
            if (!GetPhysicalMonitorsFromHMONITOR(monitors[0], physicalMonitorCount, physicalMonitors)) {
                std::cout << "Failed to get physical monitors\n";
                return 3;
            }
        }
    }

    switchMonitorEvent = OpenEventA(0x00100000L, FALSE, TEXT("Global\\USBSWITCHSWITCHMONITOR"));

    while (true) {
        dwWaitResult = WaitForSingleObject(switchMonitorEvent, INFINITE);

        switch (dwWaitResult) {
            case WAIT_OBJECT_0:
                switchMonitor(physicalMonitors[0].hPhysicalMonitor);
                break;

            default:
                printf("Wait error (%d)\n", GetLastError());
                return 0;
        }
    }
}
