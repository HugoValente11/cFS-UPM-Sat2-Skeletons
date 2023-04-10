/*
 * @file
 *
 * User header file
 */

#ifndef TEMPERATURE_SENSOR_USER_H
#define TEMPERATURE_SENSOR_USER_H

#include "temperature_sensor_app.h"

/*
** global data
*/
TEMPERATURE_SENSOR_Data_t TEMPERATURE_SENSOR_Data;
int32            status;
uint32 i;

void TEMPERATURE_SENSOR_User_Main(void);
void TEMPERATURE_SENSOR_User_Init(void);

void TEMPERATURE_SENSOR_User_Send_Sensor_data(void);





#endif /* TEMPERATURE_SENSOR_USER_H */
