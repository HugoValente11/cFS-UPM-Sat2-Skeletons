/**
 * @file
 *
 * Main header file for the pressure_sensor application
 */

#ifndef PRESSURE_SENSOR_H
#define PRESSURE_SENSOR_H

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

#include "pressure_sensor_app_perfids.h"
#include "pressure_sensor_app_msgids.h"
#include "pressure_sensor_app_msg.h"
#include "pressure_sensor_app_events.h"


/***********************************************************************/
#define PRESSURE_SENSOR_PIPE_DEPTH 32 /* Depth of the Command Pipe for Application */

#define PRESSURE_SENSOR_APP_NAME "PRESSURE_SENSOR_APP" /* App Name */



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
    PRESSURE_SENSOR_HkTlm_t HkTlm;

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
    CFE_MSG_Message_t MsgPtr_Sensor_data;
    CFE_SB_MsgId_t MsgId_Sensor_data;
    CFE_MSG_Size_t Size_Sensor_data;
    
    // ASN structs
    
    asn1SccMyInteger OUT_sensor;


    CFE_EVS_BinFilter_t EventFilters[PRESSURE_SENSOR_EVENT_COUNTS];

} PRESSURE_SENSOR_APP_Data_t;

int32            status;
uint32 i;

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (PRESSURE_SENSOR_Main), these
**       functions are not called from any other source module.
*/
void  PRESSURE_SENSOR_APP_Main(void);
int32 PRESSURE_SENSOR_APP_Init(void);
void  PRESSURE_SENSOR_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr);
void  PRESSURE_SENSOR_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr);
int32 PRESSURE_SENSOR_ReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg);
int32 PRESSURE_SENSOR_ResetCounters(const PRESSURE_SENSOR_ResetCountersCmd_t *Msg);
int32 PRESSURE_SENSOR_Noop(const PRESSURE_SENSOR_NoopCmd_t *Msg);
int32  PRESSURE_SENSOR_FindAppId(void);


bool PRESSURE_SENSOR_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength);

int32 PRESSURE_SENSOR_Send_Sensor_data_Init(void);
int32 PRESSURE_SENSOR_User_Send_Sensor_data(void);






#endif /* PRESSURE_SENSOR_H */
