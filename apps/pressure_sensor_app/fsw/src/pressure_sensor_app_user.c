#include "pressure_sensor_app_user.h"

void PRESSURE_SENSOR_User_Main(void)
{
    // TODO Call here send/receive messages in order needed


    // status = OS_TaskDelay(period_ms);
}

void PRESSURE_SENSOR_User_Init(void)
{
    // User initialization here

}

void PRESSURE_SENSOR_User_Send_Sensor_data() {
    // TODO Build structs here
    // PRESSURE_SENSOR_Data.OUT_pSensorData;

    // Send message
    PRESSURE_SENSOR_Send_Sensor_data();

}



