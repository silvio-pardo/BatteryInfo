#include "BatteryService.h"

int main()
{
    std::cout << "Start..\n";
    BatteryService service = BatteryService();
    service.getSystemInfo();
    Sleep(10000);
}
