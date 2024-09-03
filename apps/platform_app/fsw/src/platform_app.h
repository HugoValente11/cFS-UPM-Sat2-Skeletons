/**
 * @file
 *
 * Main header file for the platform application
 */

#ifndef PLATFORM_H
#define PLATFORM_H

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

#include "platform_app_perfids.h"
#include "platform_app_msgids.h"
#include "platform_app_msg.h"
#include "platform_app_events.h"


/***********************************************************************/
#define PLATFORM_PIPE_DEPTH 32 /* Depth of the Command Pipe for Application */

#define PLATFORM_APP_NAME "PLATFORM_APP" /* App Name */



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
    PLATFORM_HkTlm_t HkTlm;

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

    CFE_SB_Buffer_t * Buffer;
    
    
    // ASN structs
    asn1SccMyInteger IN_sensor;
    


    CFE_EVS_BinFilter_t EventFilters[PLATFORM_EVENT_COUNTS];

} PLATFORM_APP_Data_t;

int32            status;
uint32 i;

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (PLATFORM_Main), these
**       functions are not called from any other source module.
*/
void  PLATFORM_APP_Main(void);
int32 PLATFORM_APP_Init(void);
void  PLATFORM_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr);
void  PLATFORM_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr);
int32 PLATFORM_ReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg);
int32 PLATFORM_ResetCounters(const PLATFORM_ResetCountersCmd_t *Msg);
int32 PLATFORM_Noop(const PLATFORM_NoopCmd_t *Msg);
int32  PLATFORM_FindAppId(void);


bool PLATFORM_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength);


int32 PLATFORM_Receive_Sensor_data_Init(void);
int32 PLATFORM_User_Receive_Sensor_data(void);



int32 PLATFORM_Find_Pressure_sensorAppId(void);
int32 PLATFORM_Start_Pressure_sensorApp(void);
int32 PLATFORM_Stop_Pressure_sensorApp(void);


int32 PLATFORM_Find_Temperature_sensorAppId(void);
int32 PLATFORM_Start_Temperature_sensorApp(void);
int32 PLATFORM_Stop_Temperature_sensorApp(void);

#endif /* PLATFORM_H */
