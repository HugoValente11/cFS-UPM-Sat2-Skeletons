/*
 * @file
 *
 * User header file
 */

#ifndef MANAGER_USER_H
#define MANAGER_USER_H

#include "manager_app.h"

/*
** global data
*/
MANAGER_Data_t MANAGER_Data;
int32            status;
uint32 i;

void MANAGER_User_Main(void);
void MANAGER_User_Init(void);

void MANAGER_User_Send_Current_Mode(void);
void MANAGER_User_Receive_Process_TC(void);

void MANAGER_User_SOMEERROR_Event(void);


void MANAGER_User_MODE_Event(void);


#endif /* MANAGER_USER_H */
