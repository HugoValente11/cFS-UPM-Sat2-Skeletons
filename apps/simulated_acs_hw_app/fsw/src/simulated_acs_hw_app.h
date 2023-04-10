/**
 * @file
 *
 * Main header file for the simulated_acs_hw application
 */

#ifndef SIMULATED_ACS_HW_H
#define SIMULATED_ACS_HW_H

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

#include "simulated_acs_hw_app_perfids.h"
#include "simulated_acs_hw_app_msgids.h"
#include "simulated_acs_hw_app_msg.h"
#include "simulated_acs_hw_app_events.h"


/***********************************************************************/
#define SIMULATED_ACS_HW_PIPE_DEPTH 32 /* Depth of the Command Pipe for Application */

#define SIMULATED_ACS_HW_APP_NAME "SIMULATED_ACS_HW_APP" /* App Name */



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
    SIMULATED_ACS_HW_HkTlm_t HkTlm;

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
    CFE_MSG_Message_t MsgPtr_MGM_Data;
    CFE_SB_MsgId_t MsgId_MGM_Data;
    CFE_MSG_Size_t Size_MGM_Data;
    
    // ASN structs
    asn1SccMyInteger IN_pMGMData;
    asn1SccMyInteger OUT_pMGMData;


    CFE_EVS_BinFilter_t EventFilters[SIMULATED_ACS_HW_EVENT_COUNTS];

} SIMULATED_ACS_HW_Data_t;

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (SIMULATED_ACS_HW_Main), these
**       functions are not called from any other source module.
*/
void  SIMULATED_ACS_HW_APP_Main(void);
int32 SIMULATED_ACS_HW_Init(void);
void  SIMULATED_ACS_HW_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr);
void  SIMULATED_ACS_HW_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr);
int32 SIMULATED_ACS_HW_ReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg);
int32 SIMULATED_ACS_HW_ResetCounters(const SIMULATED_ACS_HW_ResetCountersCmd_t *Msg);
int32 SIMULATED_ACS_HW_Noop(const SIMULATED_ACS_HW_NoopCmd_t *Msg);
void  SIMULATED_ACS_HW_FindAppId(void);


bool SIMULATED_ACS_HW_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength);

void SIMULATED_ACS_HW_Send_MGM_Data(void);
void SIMULATED_ACS_HW_Receive_MGT_Data(void);




#endif /* SIMULATED_ACS_HW_H */
