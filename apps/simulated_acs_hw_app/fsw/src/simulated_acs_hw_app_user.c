#include "simulated_acs_hw_app_user.h"

void SIMULATED_ACS_HW_User_Main(void)
{
    // TODO Call here send/receive messages in order needed


    // status = OS_TaskDelay(period_ms);
}

void SIMULATED_ACS_HW_User_Init(void)
{
    // User initialization here

}

void SIMULATED_ACS_HW_User_Send_MGM_Data() {
    // TODO Build structs here
    // SIMULATED_ACS_HW_Data.OUT_pMGMData;

    // Send message
    SIMULATED_ACS_HW_Send_MGM_Data();

}
void SIMULATED_ACS_HW_User_Receive_MGT_Data() {
    // Receive message
    SIMULATED_ACS_HW_Receive_MGT_Data();

    // CFE_ES_WriteToSysLog("[SIMULATED_ACS_HW] Message received: %s", SIMULATED_ACS_HW_Data.IN_pMGMData);

}


