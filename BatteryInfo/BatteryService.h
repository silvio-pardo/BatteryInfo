#pragma once
#define INITGUID
#include <windows.h>
#include <batclass.h>
#pragma comment (lib, "Setupapi.lib")
#include <setupapi.h>
#include <iostream>
#include <array>
#include <vector>

class BatteryService
{
public:
	BatteryService(){}
	
	struct BatteryInfo {
		bool errors;
		std::string device_name;
		ULONG designed_capacity;
		ULONG full_charged_capacity;
		ULONG cycle_count;
		ULONG voltage;
		ULONG capacity;
		ULONG power_state;
		LONG rate;
	};

	std::vector<BatteryInfo> getBatteryList();
protected:
	BatteryInfo getBatteryInfo(const HDEVINFO& hd_ev, SP_DEVICE_INTERFACE_DATA& sp_device);
};

