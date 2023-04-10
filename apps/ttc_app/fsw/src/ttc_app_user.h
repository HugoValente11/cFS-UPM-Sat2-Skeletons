/*
 * @file
 *
 * User header file
 */

#ifndef TTC_USER_H
#define TTC_USER_H

#include "ttc_app.h"

/*
** global data
*/
TTC_Data_t TTC_Data;
int32            status;
uint32 i;

void TTC_User_Main(void);
void TTC_User_Init(void);

void TTC_User_Send_Get_In(void);


void TTC_User_Send_Process_TC(void);
void TTC_User_Receive_Current_Mode(void);


void TTC_User_Receive_Get_Out(void);


void TTC_User_Receive_Last(void);




#endif /* TTC_USER_H */
