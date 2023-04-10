#include "ttc_app_user.h"

void TTC_User_Main(void)
{
    // TODO Call here send/receive messages in order needed


    // status = OS_TaskDelay(period_ms);
}

void TTC_User_Init(void)
{
    // User initialization here

}

void TTC_User_Send_Get_In() {
    // TODO Build structs here
    // TTC_Data.OUT_pGetIn;

    // Send message
    TTC_Send_Get_In();

}



void TTC_User_Send_Process_TC() {
    // TODO Build structs here
    // TTC_Data.OUT_pTC;

    // Send message
    TTC_Send_Process_TC();

}
void TTC_User_Receive_Current_Mode() {
    // Receive message
    TTC_Receive_Current_Mode();

    // CFE_ES_WriteToSysLog("[TTC] Message received: %s", TTC_Data.IN_pMode);

}



void TTC_User_Receive_Get_Out() {
    // Receive message
    TTC_Receive_Get_Out();

    // CFE_ES_WriteToSysLog("[TTC] Message received: %s", TTC_Data.IN_pGetOut);

}



void TTC_User_Receive_Last() {
    // Receive message
    TTC_Receive_Last();

    // CFE_ES_WriteToSysLog("[TTC] Message received: %s", TTC_Data.IN_pLast);

}


