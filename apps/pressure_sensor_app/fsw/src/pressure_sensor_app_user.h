/*
 * @file
 *
 * User header file
 */

#ifndef PRESSURE_SENSOR_USER_H
#define PRESSURE_SENSOR_USER_H

#include "pressure_sensor_app.h"

/*
** global data
*/
PRESSURE_SENSOR_Data_t PRESSURE_SENSOR_Data;
int32            status;
uint32 i;

void PRESSURE_SENSOR_User_Main(void);
void PRESSURE_SENSOR_User_Init(void);

void PRESSURE_SENSOR_User_Send_Sensor_data(void);





#endif /* PRESSURE_SENSOR_USER_H */
