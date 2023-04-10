#include "measurer_and_actuator_app_user.h"

void MEASURER_AND_ACTUATOR_User_Main(void)
{
    // TODO Call here send/receive messages in order needed


    // status = OS_TaskDelay(period_ms);
}

void MEASURER_AND_ACTUATOR_User_Init(void)
{
    // User initialization here

}

void MEASURER_AND_ACTUATOR_User_Send_MGT_Data() {
    // TODO Build structs here
    // MEASURER_AND_ACTUATOR_Data.OUT_pMGMData;

    // Send message
    MEASURER_AND_ACTUATOR_Send_MGT_Data();

}



void MEASURER_AND_ACTUATOR_User_Send_Step_In() {
    // TODO Build structs here
    // MEASURER_AND_ACTUATOR_Data.OUT_pStepIn;

    // Send message
    MEASURER_AND_ACTUATOR_Send_Step_In();

}
void MEASURER_AND_ACTUATOR_User_Receive_MGM_Data() {
    // Receive message
    MEASURER_AND_ACTUATOR_Receive_MGM_Data();

    // CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Message received: %s", MEASURER_AND_ACTUATOR_Data.IN_pMGMData);

}



void MEASURER_AND_ACTUATOR_User_Receive_Step_Out() {
    // Receive message
    MEASURER_AND_ACTUATOR_Receive_Step_Out();

    // CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Message received: %s", MEASURER_AND_ACTUATOR_Data.IN_pStepOut);

}


