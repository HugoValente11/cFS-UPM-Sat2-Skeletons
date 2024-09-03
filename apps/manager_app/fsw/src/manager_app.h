/**
 * @file
 *
 * Main header file for the manager application
 */

#ifndef MANAGER_H
#define MANAGER_H

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

#include "manager_app_perfids.h"
#include "manager_app_msgids.h"
#include "manager_app_msg.h"
#include "manager_app_events.h"


/***********************************************************************/
#define MANAGER_PIPE_DEPTH 32 /* Depth of the Command Pipe for Application */

#define MANAGER_APP_NAME "MANAGER_APP" /* App Name */



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
    MANAGER_HkTlm_t HkTlm;

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
    CFE_MSG_Message_t MsgPtr_Current_Mode;
    CFE_SB_MsgId_t MsgId_Current_Mode;
    CFE_MSG_Size_t Size_Current_Mode;
    
    // ASN structs
    asn1SccMyInteger IN_processtc;
    asn1SccMyInteger OUT_currentmode;


    CFE_EVS_BinFilter_t EventFilters[MANAGER_EVENT_COUNTS];

} MANAGER_APP_Data_t;

int32            status;
uint32 i;

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (MANAGER_Main), these
**       functions are not called from any other source module.
*/
void  MANAGER_APP_Main(void);
int32 MANAGER_APP_Init(void);
void  MANAGER_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr);
void  MANAGER_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr);
int32 MANAGER_ReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg);
int32 MANAGER_ResetCounters(const MANAGER_ResetCountersCmd_t *Msg);
int32 MANAGER_Noop(const MANAGER_NoopCmd_t *Msg);
int32  MANAGER_FindAppId(void);


bool MANAGER_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength);

int32 MANAGER_Send_Current_Mode_Init(void);
int32 MANAGER_User_Send_Current_Mode(void);
int32 MANAGER_Receive_Process_TC_Init(void);
int32 MANAGER_User_Receive_Process_TC(void);

int32 MANAGER_User_TM_Error_Event(void);


int32 MANAGER_User_TM_Mode_Event(void);



#endif /* MANAGER_H */
