/**
 * @file
 *
 * Main header file for the ttc application
 */

#ifndef TTC_H
#define TTC_H

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

#include "ttc_app_perfids.h"
#include "ttc_app_msgids.h"
#include "ttc_app_msg.h"
#include "ttc_app_events.h"


/***********************************************************************/
#define TTC_PIPE_DEPTH 32 /* Depth of the Command Pipe for Application */

#define TTC_APP_NAME "TTC_APP" /* App Name */



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
    TTC_HkTlm_t HkTlm;

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
    CFE_MSG_Message_t MsgPtr_Get_Req;
    CFE_SB_MsgId_t MsgId_Get_Req;
    CFE_MSG_Size_t Size_Get_Req;


    CFE_MSG_Message_t MsgPtr_Process_TC;
    CFE_SB_MsgId_t MsgId_Process_TC;
    CFE_MSG_Size_t Size_Process_TC;
    
    // ASN structs
    asn1SccMyInteger IN_currentmode;


asn1SccMyInteger IN_getres;


asn1SccMyInteger IN_last;
    asn1SccMyInteger OUT_getreq;


asn1SccMyInteger OUT_processtc;


    CFE_EVS_BinFilter_t EventFilters[TTC_EVENT_COUNTS];

} TTC_APP_Data_t;

int32            status;
uint32 i;

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (TTC_Main), these
**       functions are not called from any other source module.
*/
void  TTC_APP_Main(void);
int32 TTC_APP_Init(void);
void  TTC_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr);
void  TTC_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr);
int32 TTC_ReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg);
int32 TTC_ResetCounters(const TTC_ResetCountersCmd_t *Msg);
int32 TTC_Noop(const TTC_NoopCmd_t *Msg);
int32  TTC_FindAppId(void);


bool TTC_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength);

int32 TTC_Send_Get_Req_Init(void);
int32 TTC_User_Send_Get_Req(void);


int32 TTC_Send_Process_TC_Init(void);
int32 TTC_User_Send_Process_TC(void);
int32 TTC_Receive_Current_Mode_Init(void);
int32 TTC_User_Receive_Current_Mode(void);


int32 TTC_Receive_Get_Res_Init(void);
int32 TTC_User_Receive_Get_Res(void);


int32 TTC_Receive_Last_Init(void);
int32 TTC_User_Receive_Last(void);





#endif /* TTC_H */
