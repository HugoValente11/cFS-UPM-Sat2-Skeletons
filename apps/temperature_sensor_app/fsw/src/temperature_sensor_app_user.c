#include "temperature_sensor_app_user.h"

void TEMPERATURE_SENSOR_User_Main(void)
{
    // TODO Call here send/receive messages in order needed


    // status = OS_TaskDelay(period_ms);
}

void TEMPERATURE_SENSOR_User_Init(void)
{
    // User initialization here

}

void TEMPERATURE_SENSOR_User_Send_Sensor_data() {
    // TODO Build structs here
    // TEMPERATURE_SENSOR_Data.OUT_pSensorData;

    // Send message
    TEMPERATURE_SENSOR_Send_Sensor_data();

}



