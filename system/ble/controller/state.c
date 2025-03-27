
#include"state.h"




void ble_ll_init_state_machine(ble_ll_state_machine_t* sm )
{
    sm->state = BLE_LL_STATE_STANDBY;
    sm->handler = 0;
}