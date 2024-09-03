/**
 * @file
 *
 * Main header file for the acs_algorithm application
 */

#ifndef ACS_ALGORITHM_H
#define ACS_ALGORITHM_H

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

#include "acs_algorithm_app_perfids.h"
#include "acs_algorithm_app_msgids.h"
#include "acs_algorithm_app_msg.h"
#include "acs_algorithm_app_events.h"


/***********************************************************************/
#define ACS_ALGORITHM_PIPE_DEPTH 32 /* Depth of the Command Pipe for Application */

#define ACS_ALGORITHM_APP_NAME "ACS_ALGORITHM_APP" /* App Name */



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
    ACS_ALGORITHM_HkTlm_t HkTlm;

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
    CFE_MSG_Message_t MsgPtr_Step_Out;
    CFE_SB_MsgId_t MsgId_Step_Out;
    CFE_MSG_Size_t Size_Step_Out;
    
    // ASN structs
    asn1SccMyInteger IN_stepin;
    asn1SccMyInteger OUT_stepout;


    CFE_EVS_BinFilter_t EventFilters[ACS_ALGORITHM_EVENT_COUNTS];

} ACS_ALGORITHM_APP_Data_t;

int32            status;
uint32 i;

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (ACS_ALGORITHM_Main), these
**       functions are not called from any other source module.
*/
void  ACS_ALGORITHM_APP_Main(void);
int32 ACS_ALGORITHM_APP_Init(void);
void  ACS_ALGORITHM_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr);
void  ACS_ALGORITHM_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr);
int32 ACS_ALGORITHM_ReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg);
int32 ACS_ALGORITHM_ResetCounters(const ACS_ALGORITHM_ResetCountersCmd_t *Msg);
int32 ACS_ALGORITHM_Noop(const ACS_ALGORITHM_NoopCmd_t *Msg);
int32  ACS_ALGORITHM_FindAppId(void);


bool ACS_ALGORITHM_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength);

int32 ACS_ALGORITHM_Send_Step_Out_Init(void);
int32 ACS_ALGORITHM_User_Send_Step_Out(void);
int32 ACS_ALGORITHM_Receive_Step_In_Init(void);
int32 ACS_ALGORITHM_User_Receive_Step_In(void);





#endif /* ACS_ALGORITHM_H */
