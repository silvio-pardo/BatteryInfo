#include "BatteryService.h"

int main()
{
    std::cout << "Start..\n";
    BatteryService service = BatteryService();
    service.getBatteryList();
    Sleep(10000);
}
