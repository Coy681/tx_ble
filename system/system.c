
#include"system.h"
#include"system/ble/controller/controller.h"

void system_init()
{
	tx_malloc_init();//must init first
	ble_controller_init();
}

ARCH_INIT(system_init);

