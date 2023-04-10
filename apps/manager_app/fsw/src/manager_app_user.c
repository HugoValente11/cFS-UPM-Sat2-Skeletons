#include "manager_app_user.h"

void MANAGER_User_Main(void)
{
    // TODO Call here send/receive messages in order needed


    // status = OS_TaskDelay(period_ms);
}

void MANAGER_User_Init(void)
{
    // User initialization here

}

void MANAGER_User_Send_Current_Mode() {
    // TODO Build structs here
    // MANAGER_Data.OUT_pMode;

    // Send message
    MANAGER_Send_Current_Mode();

}
void MANAGER_User_Receive_Process_TC() {
    // Receive message
    MANAGER_Receive_Process_TC();

    // CFE_ES_WriteToSysLog("[MANAGER] Message received: %s", MANAGER_Data.IN_pTC);

}
void MANAGER_User_SOMEERROR_Event(void) {
    CFE_EVS_SendEvent(MANAGER_SOMEERROR_EID, CFE_EVS_EventType_ERROR, "[ERROR] Error in somewhere\n");
}


void MANAGER_User_MODE_Event(void) {
    CFE_EVS_SendEvent(MANAGER_MODE_EID, CFE_EVS_EventType_INFORMATION, "[INFORMATION] Current mode\n");
}

