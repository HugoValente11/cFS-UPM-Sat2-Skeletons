#include "acs_algorithm_app_user.h"

void ACS_ALGORITHM_User_Main(void)
{
    // TODO Call here send/receive messages in order needed


    // status = OS_TaskDelay(period_ms);
}

void ACS_ALGORITHM_User_Init(void)
{
    // User initialization here

}

void ACS_ALGORITHM_User_Send_Step_Out() {
    // TODO Build structs here
    // ACS_ALGORITHM_Data.OUT_pStepOut;

    // Send message
    ACS_ALGORITHM_Send_Step_Out();

}
void ACS_ALGORITHM_User_Receive_Step_In() {
    // Receive message
    ACS_ALGORITHM_Receive_Step_In();

    // CFE_ES_WriteToSysLog("[ACS_ALGORITHM] Message received: %s", ACS_ALGORITHM_Data.IN_pStepIn);

}


