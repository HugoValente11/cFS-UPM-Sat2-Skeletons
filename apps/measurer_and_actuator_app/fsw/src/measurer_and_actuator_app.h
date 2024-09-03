/**
 * @file
 *
 * Main header file for the measurer_and_actuator application
 */

#ifndef MEASURER_AND_ACTUATOR_H
#define MEASURER_AND_ACTUATOR_H

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

#include "measurer_and_actuator_app_perfids.h"
#include "measurer_and_actuator_app_msgids.h"
#include "measurer_and_actuator_app_msg.h"
#include "measurer_and_actuator_app_events.h"


/***********************************************************************/
#define MEASURER_AND_ACTUATOR_PIPE_DEPTH 32 /* Depth of the Command Pipe for Application */

#define MEASURER_AND_ACTUATOR_APP_NAME "MEASURER_AND_ACTUATOR_APP" /* App Name */



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
    MEASURER_AND_ACTUATOR_HkTlm_t HkTlm;

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
    CFE_MSG_Message_t MsgPtr_MGT_Data;
    CFE_SB_MsgId_t MsgId_MGT_Data;
    CFE_MSG_Size_t Size_MGT_Data;


    CFE_MSG_Message_t MsgPtr_Step_In;
    CFE_SB_MsgId_t MsgId_Step_In;
    CFE_MSG_Size_t Size_Step_In;
    
    // ASN structs
    asn1SccMyInteger IN_mgmdata;


asn1SccMyInteger IN_stepout;
    asn1SccMyInteger OUT_mgtdata;


asn1SccMyInteger OUT_stepin;


    CFE_EVS_BinFilter_t EventFilters[MEASURER_AND_ACTUATOR_EVENT_COUNTS];

} MEASURER_AND_ACTUATOR_APP_Data_t;

int32            status;
uint32 i;

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (MEASURER_AND_ACTUATOR_Main), these
**       functions are not called from any other source module.
*/
void  MEASURER_AND_ACTUATOR_APP_Main(void);
int32 MEASURER_AND_ACTUATOR_APP_Init(void);
void  MEASURER_AND_ACTUATOR_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr);
void  MEASURER_AND_ACTUATOR_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr);
int32 MEASURER_AND_ACTUATOR_ReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg);
int32 MEASURER_AND_ACTUATOR_ResetCounters(const MEASURER_AND_ACTUATOR_ResetCountersCmd_t *Msg);
int32 MEASURER_AND_ACTUATOR_Noop(const MEASURER_AND_ACTUATOR_NoopCmd_t *Msg);
int32  MEASURER_AND_ACTUATOR_FindAppId(void);


bool MEASURER_AND_ACTUATOR_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength);

int32 MEASURER_AND_ACTUATOR_Send_MGT_Data_Init(void);
int32 MEASURER_AND_ACTUATOR_User_Send_MGT_Data(void);


int32 MEASURER_AND_ACTUATOR_Send_Step_In_Init(void);
int32 MEASURER_AND_ACTUATOR_User_Send_Step_In(void);
int32 MEASURER_AND_ACTUATOR_Receive_MGM_Data_Init(void);
int32 MEASURER_AND_ACTUATOR_User_Receive_MGM_Data(void);


int32 MEASURER_AND_ACTUATOR_Receive_Step_Out_Init(void);
int32 MEASURER_AND_ACTUATOR_User_Receive_Step_Out(void);





#endif /* MEASURER_AND_ACTUATOR_H */
