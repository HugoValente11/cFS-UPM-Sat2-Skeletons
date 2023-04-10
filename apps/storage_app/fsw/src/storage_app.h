/**
 * @file
 *
 * Main header file for the storage application
 */

#ifndef STORAGE_H
#define STORAGE_H

/*
** Required header files.
*/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include "cfe_es_module_all.h"
#include "dataview-uniq.h"

#include "storage_app_perfids.h"
#include "storage_app_msgids.h"
#include "storage_app_msg.h"
#include "storage_app_events.h"


/***********************************************************************/
#define STORAGE_PIPE_DEPTH 32 /* Depth of the Command Pipe for Application */

#define STORAGE_APP_NAME "STORAGE_APP" /* App Name */

#define STORAGE_DATASTORE_SIZE 100 /* Datastore size */


/************************************************************************
** Type Definitions
*************************************************************************/

/*
** Global Data
*/
typedef struct
{
    /*
    ** Command interface counters...
    */
    uint8 CmdCounter;
    uint8 ErrCounter;

    /*
    ** Housekeeping telemetry packet...
    */
    STORAGE_HkTlm_t HkTlm;

    /*
    ** Run Status variable used in the main processing loop
    */
    uint32 RunStatus;

    /*
    ** App Id variable assigned to the app at startup
    */
    uint32 AppId;

    /*
    ** Operational data (not reported in housekeeping)...
    */
    CFE_SB_PipeId_t CommandPipe;

    /*
    ** Initialization data (not reported in housekeeping)...
    */
    char   PipeName[CFE_MISSION_MAX_API_LEN];
    uint16 PipeDepth;

    //For the message (line 104):
    CFE_SB_Buffer_t * Buffer;
    CFE_MSG_Message_t MsgPtr_Get_Out;
    CFE_SB_MsgId_t MsgId_Get_Out;
    CFE_MSG_Size_t Size_Get_Out;


    CFE_MSG_Message_t MsgPtr_Last;
    CFE_SB_MsgId_t MsgId_Last;
    CFE_MSG_Size_t Size_Last;
    
    // ASN structs
    asn1SccMyInteger IN_pGetIn;


asn1SccMyInteger IN_pSensorData;
    asn1SccMyInteger OUT_pGetOut;


asn1SccMyInteger OUT_pLast;

    /*
    ** Data store properties
    */
    CFE_ES_CDSHandle_t CDSHandlePtr;
    uint8 CDSData[STORAGE_DATASTORE_SIZE];

    CFE_EVS_BinFilter_t EventFilters[STORAGE_EVENT_COUNTS];

} STORAGE_Data_t;

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (STORAGE_Main), these
**       functions are not called from any other source module.
*/
void  STORAGE_APP_Main(void);
int32 STORAGE_Init(void);
void  STORAGE_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr);
void  STORAGE_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr);
int32 STORAGE_ReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg);
int32 STORAGE_ResetCounters(const STORAGE_ResetCountersCmd_t *Msg);
int32 STORAGE_Noop(const STORAGE_NoopCmd_t *Msg);
void  STORAGE_FindAppId(void);

int32 STORAGE_Datastore_Init(void);
int32 STORAGE_Datastore_Store(const void *dataToStore);

bool STORAGE_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength);

void STORAGE_Send_Get_Out(void);


void STORAGE_Send_Last(void);
void STORAGE_Receive_Get_In(void);


void STORAGE_Receive_Sensor_data(void);




#endif /* STORAGE_H */
