#define INITGUID
#include <windows.h>
#include <batclass.h>
#pragma comment (lib, "Setupapi.lib")
#include <setupapi.h>
#define GBS_HASBATTERY 0x1
#define GBS_ONBATTERY  0x2
#include <iostream>

int main()
{
    std::cout << "Start..\n";
    HDEVINFO hdev = SetupDiGetClassDevs(&GUID_DEVICE_BATTERY, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    //cycle for 100 device of battery type
    //https://docs.microsoft.com/en-us/windows/win32/power/enumerating-battery-devices

    for (int idev = 0; idev < 100; idev++)
    {
        SP_DEVICE_INTERFACE_DATA did = { 0 };
        did.cbSize = sizeof(did);

        if (SetupDiEnumDeviceInterfaces(hdev, 0, &GUID_DEVICE_BATTERY, idev, &did))
        {
            DWORD cbRequired = 0;
            SetupDiGetDeviceInterfaceDetail(hdev, &did, 0, 0, &cbRequired, 0);

            if (ERROR_INSUFFICIENT_BUFFER == GetLastError())
            {
                PSP_DEVICE_INTERFACE_DETAIL_DATA pdidd = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LPTR, cbRequired);

                if (pdidd)
                {
                    pdidd->cbSize = sizeof(*pdidd);

                    if (SetupDiGetDeviceInterfaceDetail(hdev, &did, pdidd, cbRequired, &cbRequired, 0))
                    {
                        HANDLE hBattery = CreateFile(pdidd->DevicePath, GENERIC_READ | GENERIC_WRITE,
                            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

                        if (INVALID_HANDLE_VALUE != hBattery)
                        {
                            BATTERY_QUERY_INFORMATION bqi = { 0 };

                            DWORD dwWait = 0;
                            DWORD dwOut;
                            //retrieve the battery tag for perform other call
                            if (DeviceIoControl(hBattery,
                                IOCTL_BATTERY_QUERY_TAG,
                                &dwWait,
                                sizeof(dwWait),
                                &bqi.BatteryTag,
                                sizeof(bqi.BatteryTag),
                                &dwOut,
                                NULL)
                                && bqi.BatteryTag) {
                                    //battery information for retrieve the full charge capacity and other infos
                                    BATTERY_INFORMATION szBuf{};
                                    bqi.InformationLevel = BatteryInformation;
                                    if (DeviceIoControl(hBattery,
                                        IOCTL_BATTERY_QUERY_INFORMATION,
                                        &bqi,
                                        sizeof(bqi),
                                        &szBuf,
                                        sizeof(szBuf),
                                        &dwOut,
                                        NULL))
                                    {
                                        // check is not an ups battery
                                        if (szBuf.Capabilities & BATTERY_SYSTEM_BATTERY) {
                                            std::printf("battery tag: \n%ld\n", bqi.BatteryTag);
                                            std::printf("designed capacity: \n%ld\n", szBuf.DesignedCapacity);
                                            
                                            BATTERY_WAIT_STATUS bws = { 0 };
                                            bws.BatteryTag = bqi.BatteryTag;

                                            BATTERY_STATUS bs{};
                                            // get the current battery capacity
                                            if (DeviceIoControl(hBattery,
                                                IOCTL_BATTERY_QUERY_STATUS,
                                                &bws,
                                                sizeof(bws),
                                                &bs,
                                                sizeof(bs),
                                                &dwOut,
                                                NULL))
                                            {
                                                std::printf("current capacity: \n%lu\n", bs.Capacity);
                                            }
                                        }
                                    }
                            }
                            CloseHandle(hBattery);
                        }
                    }
                    LocalFree(pdidd);
                }
            }
        }
        else  if (ERROR_NO_MORE_ITEMS == GetLastError())
        {
            break;
        }
    }
}
