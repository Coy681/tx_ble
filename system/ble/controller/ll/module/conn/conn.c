#include"conn.h"





int ble_ll_enter_connection_state(ble_ll_event_e event)
{
    ll_sm_t* ll = ll_get_current_state_machine();
    if(BLE_LL_STATE_ADVERTISING == ll->state)
    {
    	ll->conn->role = CONN_ROLE_PERIPHERAL;
    }
    else if(BLE_LL_STATE_INITIATING == ll->state)
    {
    	ll->conn->role = CONN_ROLE_CENTRAL;
    }
    else 
    {
        return 0;
    }
//    ll->conn->maxTxOctets = BLE_CONN_TX_MAX_OCTETS;
//    ll->conn->maxTxTime   = ll_get_air_packet_time(ll->phy.mode,BLE_CONN_TX_MAX_OCTETS,1);
//    ll->conn->maxRxOctets = BLE_CONN_RX_MAX_OCTETS;
//    ll->conn->maxRxTime   = ll_get_air_packet_time(ll->phy.mode,BLE_CONN_RX_MAX_OCTETS,1);

	LOG_TRACE(LL_LOG_TRACE,"enter connection state",0,0)
    return 1;
}
