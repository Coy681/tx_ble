
#include"system.h"
#include"system/ble/controller/ll/ll.h"

void system_init()
{
	tx_malloc_init();//must init first
	ll_init_state_machine(1);
}

ARCH_INIT(system_init);

