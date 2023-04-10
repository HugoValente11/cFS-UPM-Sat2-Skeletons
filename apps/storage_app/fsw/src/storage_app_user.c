#include "storage_app_user.h"

void STORAGE_User_Main(void)
{
    // TODO Call here send/receive messages in order needed


    // status = OS_TaskDelay(period_ms);
}

void STORAGE_User_Init(void)
{
    // User initialization here

    // Call datastore initialization with 100 bytes
    STORAGE_Datastore_User_Init();

}

int32 STORAGE_Datastore_User_Init()
{
    // Check if there's data in Datastore,
    // if there is recover the data and store it in STORAGE_Data.CDSData
    int32 status = STORAGE_Datastore_Init();
    if (status == CFE_ES_CDS_ALREADY_EXISTS)
    {
        // TODO User implementation to handle data stored
    }

    return status;
}

int32 STORAGE_Datastore_User_Store(const void *dataToStore)
{
    return STORAGE_Datastore_Store(dataToStore);
}

void STORAGE_Datastore_User_Load(uint8 load[STORAGE_DATASTORE_SIZE])
{
    // Read data from CDS
    CFE_ES_RestoreFromCDS(load, STORAGE_Data.CDSHandlePtr);

}

void STORAGE_User_Send_Get_Out() {
    // TODO Build structs here
    // STORAGE_Data.OUT_pGetOut;

    // Send message
    STORAGE_Send_Get_Out();

}



void STORAGE_User_Send_Last() {
    // TODO Build structs here
    // STORAGE_Data.OUT_pLast;

    // Send message
    STORAGE_Send_Last();

}
void STORAGE_User_Receive_Get_In() {
    // Receive message
    STORAGE_Receive_Get_In();

    // CFE_ES_WriteToSysLog("[STORAGE] Message received: %s", STORAGE_Data.IN_pGetIn);

}



void STORAGE_User_Receive_Sensor_data() {
    // Receive message
    STORAGE_Receive_Sensor_data();

    // CFE_ES_WriteToSysLog("[STORAGE] Message received: %s", STORAGE_Data.IN_pSensorData);

}


