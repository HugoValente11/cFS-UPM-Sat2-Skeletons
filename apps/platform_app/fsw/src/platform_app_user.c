#include "platform_app_user.h"

void PLATFORM_User_Main(void)
{
    // TODO Call here send/receive messages in order needed


    // status = OS_TaskDelay(period_ms);
}

void PLATFORM_User_Init(void)
{
    // User initialization here

}


void PLATFORM_User_Receive_Sensor_data() {
    // Receive message
    PLATFORM_Receive_Sensor_data();

    // CFE_ES_WriteToSysLog("[PLATFORM] Message received: %s", PLATFORM_Data.IN_pSensorData);

}

void PLATFORM_User_StartPressure_sensorApp(void)
{
    PLATFORM_StartPressure_sensorApp();
}

void PLATFORM_User_StopPressure_sensorApp(void)
{
    PLATFORM_StopPressure_sensorApp();
}



void PLATFORM_User_StartTemperature_sensorApp(void)
{
    PLATFORM_StartTemperature_sensorApp();
}

void PLATFORM_User_StopTemperature_sensorApp(void)
{
    PLATFORM_StopTemperature_sensorApp();
}
