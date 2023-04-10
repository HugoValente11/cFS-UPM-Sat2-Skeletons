/*
 * @file
 *
 * User header file
 */

#ifndef SIMULATED_ACS_HW_USER_H
#define SIMULATED_ACS_HW_USER_H

#include "simulated_acs_hw_app.h"

/*
** global data
*/
SIMULATED_ACS_HW_Data_t SIMULATED_ACS_HW_Data;
int32            status;
uint32 i;

void SIMULATED_ACS_HW_User_Main(void);
void SIMULATED_ACS_HW_User_Init(void);

void SIMULATED_ACS_HW_User_Send_MGM_Data(void);
void SIMULATED_ACS_HW_User_Receive_MGT_Data(void);




#endif /* SIMULATED_ACS_HW_USER_H */
