/*
 * @file
 *
 * User header file
 */

#ifndef PLATFORM_USER_H
#define PLATFORM_USER_H

#include "platform_app.h"

/*
** global data
*/
PLATFORM_Data_t PLATFORM_Data;
int32            status;
uint32 i;

void PLATFORM_User_Main(void);
void PLATFORM_User_Init(void);


void PLATFORM_User_Receive_Sensor_data(void);


void PLATFORM_User_StartPressure_sensorApp(void);
void PLATFORM_User_StopPressure_sensorApp(void);


void PLATFORM_User_StartTemperature_sensorApp(void);
void PLATFORM_User_StopTemperature_sensorApp(void);

#endif /* PLATFORM_USER_H */
