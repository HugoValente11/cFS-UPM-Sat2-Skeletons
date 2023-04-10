/*
 * @file
 *
 * User header file
 */

#ifndef STORAGE_USER_H
#define STORAGE_USER_H

#include "storage_app.h"

/*
** global data
*/
STORAGE_Data_t STORAGE_Data;
int32            status;
uint32 i;

void STORAGE_User_Main(void);
void STORAGE_User_Init(void);

int32 STORAGE_Datastore_User_Init(void);
int32 STORAGE_Datastore_User_Store(const void *dataToStore);
void STORAGE_Datastore_User_Load(uint8 load[STORAGE_DATASTORE_SIZE]);

void STORAGE_User_Send_Get_Out(void);


void STORAGE_User_Send_Last(void);
void STORAGE_User_Receive_Get_In(void);


void STORAGE_User_Receive_Sensor_data(void);




#endif /* STORAGE_USER_H */
