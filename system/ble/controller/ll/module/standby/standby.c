#include"standby.h"
int ble_ll_enter_standby_state(ble_ll_event_e event)
{
    ll_ctrl_t* ll = ll_get_current_state_machine();
    if(BLE_LL_EVENT_STOP_ADVERTISING == event)
    {
        tx_free((_u8*)ll->adv);
    }
	LOG_TRACE(LL_LOG_TRACE,"enter standby state",0,0)
    return 1;
}