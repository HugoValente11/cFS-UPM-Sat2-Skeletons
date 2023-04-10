/*
 * @file
 *
 * User header file
 */

#ifndef MEASURER_AND_ACTUATOR_USER_H
#define MEASURER_AND_ACTUATOR_USER_H

#include "measurer_and_actuator_app.h"

/*
** global data
*/
MEASURER_AND_ACTUATOR_Data_t MEASURER_AND_ACTUATOR_Data;
int32            status;
uint32 i;

void MEASURER_AND_ACTUATOR_User_Main(void);
void MEASURER_AND_ACTUATOR_User_Init(void);

void MEASURER_AND_ACTUATOR_User_Send_MGT_Data(void);


void MEASURER_AND_ACTUATOR_User_Send_Step_In(void);
void MEASURER_AND_ACTUATOR_User_Receive_MGM_Data(void);


void MEASURER_AND_ACTUATOR_User_Receive_Step_Out(void);




#endif /* MEASURER_AND_ACTUATOR_USER_H */
