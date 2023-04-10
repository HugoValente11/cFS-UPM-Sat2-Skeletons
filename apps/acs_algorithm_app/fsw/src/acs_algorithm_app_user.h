/*
 * @file
 *
 * User header file
 */

#ifndef ACS_ALGORITHM_USER_H
#define ACS_ALGORITHM_USER_H

#include "acs_algorithm_app.h"

/*
** global data
*/
ACS_ALGORITHM_Data_t ACS_ALGORITHM_Data;
int32            status;
uint32 i;

void ACS_ALGORITHM_User_Main(void);
void ACS_ALGORITHM_User_Init(void);

void ACS_ALGORITHM_User_Send_Step_Out(void);
void ACS_ALGORITHM_User_Receive_Step_In(void);




#endif /* ACS_ALGORITHM_USER_H */
