/*******************************************************************************
**
**      GSC-18128-1, "Core Flight Executive Version 6.7"
**
**      Copyright (c) 2006-2019 United States Government as represented by
**      the Administrator of the National Aeronautics and Space Administration.
**      All Rights Reserved.
**
**      Licensed under the Apache License, Version 2.0 (the "License");
**      you may not use this file except in compliance with the License.
**      You may obtain a copy of the License at
**
**        http://www.apache.org/licenses/LICENSE-2.0
**
**      Unless required by applicable law or agreed to in writing, software
**      distributed under the License is distributed on an "AS IS" BASIS,
**      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**      See the License for the specific language governing permissions and
**      limitations under the License.
**
** File: simulated_acs_hw.c
**
** Purpose:
**   This file contains the source code for the Sample App.
**
*******************************************************************************/

/*
** Include Files:
*/
#include "simulated_acs_hw_app_version.h"
#include "simulated_acs_hw_app.h"
#include "simulated_acs_hw_app_user.h"
#include "dataview_init.h"


/* The sample_lib module provides the SAMPLE_LIB_Function() prototype */
#include <string.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/* SIMULATED_ACS_HW_AppMain() -- Application entry point and main process loop         */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void SIMULATED_ACS_HW_APP_Main(void)
{

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(SIMULATED_ACS_HW_PERF_ID);

    /*
    ** Perform application specific initialization
    ** If the Initialization fails, set the RunStatus to
    ** CFE_ES_RunStatus_APP_ERROR and the App will not enter the RunLoop
    */
    status = SIMULATED_ACS_HW_Init();
    if (status != CFE_SUCCESS)
    {
        SIMULATED_ACS_HW_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }


    // ---------   Send messages  ------------
    // uint32 period =  1000; // ms
    // --------- Receive messages ------------
    // Initialize pipe MGT_Data
    // Subscribe to MGT_Data     0x016 msg id
    status = CFE_SB_Subscribe(0x016,
                              SIMULATED_ACS_HW_Data.CommandPipe);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[SIMULATED_ACS_HW] Error Subscribing to communication pipe, RC = 0x%08lX\n",
                             (unsigned long) status);
    }

    // Main app loop
    while (CFE_ES_RunLoop(&SIMULATED_ACS_HW_Data.RunStatus) == true)
    {
        // Call user loop
        SIMULATED_ACS_HW_User_Main();
    }



    // Delete pipe to free resources
    status = CFE_SB_DeletePipe (SIMULATED_ACS_HW_Data.CommandPipe);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[SIMULATED_ACS_HW] Error Deleting the pipe, RC = 0x%08lX\n",
                             (unsigned long)status);
    }


    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(SIMULATED_ACS_HW_PERF_ID);

    SIMULATED_ACS_HW_Data.RunStatus = CFE_ES_RunStatus_APP_EXIT;

    CFE_ES_ExitApp(SIMULATED_ACS_HW_Data.RunStatus);

} /* End of SIMULATED_ACS_HW_APP_Main() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* SIMULATED_ACS_HW_Init() --  initialization                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 SIMULATED_ACS_HW_Init(void)
{
    int32 status;

    SIMULATED_ACS_HW_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Initialize app command execution counters
    */
    SIMULATED_ACS_HW_Data.CmdCounter = 0;
    SIMULATED_ACS_HW_Data.ErrCounter = 0;

    /*
    ** Initialize app configuration data
    */
    SIMULATED_ACS_HW_Data.PipeDepth = SIMULATED_ACS_HW_PIPE_DEPTH;

    strncpy(SIMULATED_ACS_HW_Data.PipeName, "SIMULATED_ACS_HW_CMD_PIPE", sizeof(SIMULATED_ACS_HW_Data.PipeName));
    SIMULATED_ACS_HW_Data.PipeName[sizeof(SIMULATED_ACS_HW_Data.PipeName) - 1] = 0;

    /*
    ** Initialize event filter table...
    */
    SIMULATED_ACS_HW_Data.EventFilters[0].EventID = SIMULATED_ACS_HW_STARTUP_INF_EID;
    SIMULATED_ACS_HW_Data.EventFilters[0].Mask    = 0x0000;
    SIMULATED_ACS_HW_Data.EventFilters[1].EventID = SIMULATED_ACS_HW_COMMAND_ERR_EID;
    SIMULATED_ACS_HW_Data.EventFilters[1].Mask    = 0x0000;
    SIMULATED_ACS_HW_Data.EventFilters[2].EventID = SIMULATED_ACS_HW_COMMANDNOP_INF_EID;
    SIMULATED_ACS_HW_Data.EventFilters[2].Mask    = 0x0000;
    SIMULATED_ACS_HW_Data.EventFilters[3].EventID = SIMULATED_ACS_HW_COMMANDRST_INF_EID;
    SIMULATED_ACS_HW_Data.EventFilters[3].Mask    = 0x0000;
    SIMULATED_ACS_HW_Data.EventFilters[4].EventID = SIMULATED_ACS_HW_INVALID_MSGID_ERR_EID;
    SIMULATED_ACS_HW_Data.EventFilters[4].Mask    = 0x0000;
    SIMULATED_ACS_HW_Data.EventFilters[5].EventID = SIMULATED_ACS_HW_LEN_ERR_EID;
    SIMULATED_ACS_HW_Data.EventFilters[5].Mask    = 0x0000;
    SIMULATED_ACS_HW_Data.EventFilters[6].EventID = SIMULATED_ACS_HW_PIPE_ERR_EID;
    SIMULATED_ACS_HW_Data.EventFilters[6].Mask    = 0x0000;
    

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(SIMULATED_ACS_HW_Data.EventFilters, SIMULATED_ACS_HW_EVENT_COUNTS, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[SIMULATED_ACS_HW] Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Initialize housekeeping packet (clear user data area).
    */
    CFE_MSG_Init(&SIMULATED_ACS_HW_Data.HkTlm.TlmHeader.Msg, CFE_SB_ValueToMsgId(SIMULATED_ACS_HW_HK_TLM_MID),
                 sizeof(SIMULATED_ACS_HW_Data.HkTlm));

    /*
    ** Create Software Bus message pipe.
    */
    status = CFE_SB_CreatePipe(&SIMULATED_ACS_HW_Data.CommandPipe, SIMULATED_ACS_HW_Data.PipeDepth, SIMULATED_ACS_HW_Data.PipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[SIMULATED_ACS_HW] Error creating pipe, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Subscribe to Housekeeping request commands
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(SIMULATED_ACS_HW_SEND_HK_MID), SIMULATED_ACS_HW_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[SIMULATED_ACS_HW] Error Subscribing to HK request, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Subscribe to ground command packets
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(SIMULATED_ACS_HW_CMD_MID), SIMULATED_ACS_HW_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[SIMULATED_ACS_HW] Error Subscribing to Command, RC = 0x%08lX\n", (unsigned long) status);

        return (status);
    }


    CFE_EVS_SendEvent(SIMULATED_ACS_HW_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION, "SIMULATED_ACS_HW App Initialized.%s",
                      SIMULATED_ACS_HW_VERSION_STRING);

    /*
    ** Create Software Bus message pipe.
    */
    SIMULATED_ACS_HW_Data.PipeDepth = SIMULATED_ACS_HW_PIPE_DEPTH;

    strcpy(SIMULATED_ACS_HW_Data.PipeName, "SIMULATED_ACS_HW_COM_PIPE");
    CFE_ES_WriteToSysLog("[SIMULATED_ACS_HW] COM_PIPE Name = %s", SIMULATED_ACS_HW_Data.PipeName);

    status = CFE_SB_CreatePipe(&SIMULATED_ACS_HW_Data.CommandPipe, SIMULATED_ACS_HW_Data.PipeDepth, SIMULATED_ACS_HW_Data.PipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[SIMULATED_ACS_HW] Error creating communication pipe,210, SIMULATED_ACS_HW_app.c, RC = 0x%08lX\n",
                             (unsigned long)status);
    }
    CFE_ES_WriteToSysLog("[SIMULATED_ACS_HW] Pipe creation successful");

    // Get App Id
    SIMULATED_ACS_HW_FindAppId();

    // Call user init
    SIMULATED_ACS_HW_User_Init();

    return (CFE_SUCCESS);

} /* End of SIMULATED_ACS_HW_Init() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  SIMULATED_ACS_HW_ProcessCommandPacket                                    */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the SIMULATED_ACS_HW    */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void SIMULATED_ACS_HW_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);

    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        case SIMULATED_ACS_HW_CMD_MID:
            SIMULATED_ACS_HW_ProcessGroundCommand(SBBufPtr);
            break;

        case SIMULATED_ACS_HW_SEND_HK_MID:
            SIMULATED_ACS_HW_ReportHousekeeping((CFE_MSG_CommandHeader_t *)SBBufPtr);
            break;

        default:
            CFE_EVS_SendEvent(SIMULATED_ACS_HW_INVALID_MSGID_ERR_EID, CFE_EVS_EventType_ERROR,
                              "SIMULATED_ACS_HW: invalid command packet,MID = 0x%x", (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;
    }

    return;

} /* End SIMULATED_ACS_HW_ProcessCommandPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* SIMULATED_ACS_HW_ProcessGroundCommand() -- SIMULATED_ACS_HW ground commands                */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void SIMULATED_ACS_HW_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_MSG_FcnCode_t CommandCode = 0;

    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);

    /*
    ** Process "known" SIMULATED_ACS_HW app ground commands
    */
    switch (CommandCode)
    {
        case SIMULATED_ACS_HW_NOOP_CC:
            if (SIMULATED_ACS_HW_VerifyCmdLength(&SBBufPtr->Msg, sizeof(SIMULATED_ACS_HW_NoopCmd_t)))
            {
                SIMULATED_ACS_HW_Noop((SIMULATED_ACS_HW_NoopCmd_t *)SBBufPtr);
            }

            break;

        case SIMULATED_ACS_HW_RESET_COUNTERS_CC:
            if (SIMULATED_ACS_HW_VerifyCmdLength(&SBBufPtr->Msg, sizeof(SIMULATED_ACS_HW_ResetCountersCmd_t)))
            {
                SIMULATED_ACS_HW_ResetCounters((SIMULATED_ACS_HW_ResetCountersCmd_t *)SBBufPtr);
            }

            break;


        /* default case already found during FC vs length test */
        default:
            CFE_EVS_SendEvent(SIMULATED_ACS_HW_COMMAND_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Invalid ground command code: CC = %d", CommandCode);
            break;
    }

    return;

} /* End of SIMULATED_ACS_HW_ProcessGroundCommand() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  SIMULATED_ACS_HW_ReportHousekeeping                                          */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 SIMULATED_ACS_HW_ReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg)
{

    /*
    ** Get command execution counters...
    */
    SIMULATED_ACS_HW_Data.HkTlm.Payload.CommandErrorCounter = SIMULATED_ACS_HW_Data.ErrCounter;
    SIMULATED_ACS_HW_Data.HkTlm.Payload.CommandCounter      = SIMULATED_ACS_HW_Data.CmdCounter;

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg(&SIMULATED_ACS_HW_Data.HkTlm.TlmHeader.Msg);
    CFE_SB_TransmitMsg(&SIMULATED_ACS_HW_Data.HkTlm.TlmHeader.Msg, true);


    return CFE_SUCCESS;

} /* End of SIMULATED_ACS_HW_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* SIMULATED_ACS_HW_Noop -- SIMULATED_ACS_HW NOOP commands                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 SIMULATED_ACS_HW_Noop(const SIMULATED_ACS_HW_NoopCmd_t *Msg)
{

    SIMULATED_ACS_HW_Data.CmdCounter++;

    CFE_EVS_SendEvent(SIMULATED_ACS_HW_COMMANDNOP_INF_EID, CFE_EVS_EventType_INFORMATION, "SIMULATED_ACS_HW: NOOP command %s",
                      SIMULATED_ACS_HW_VERSION);

    return CFE_SUCCESS;

} /* End of SIMULATED_ACS_HW_Noop */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  SIMULATED_ACS_HW_ResetCounters                                               */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 SIMULATED_ACS_HW_ResetCounters(const SIMULATED_ACS_HW_ResetCountersCmd_t *Msg)
{

    SIMULATED_ACS_HW_Data.CmdCounter = 0;
    SIMULATED_ACS_HW_Data.ErrCounter = 0;

    CFE_EVS_SendEvent(SIMULATED_ACS_HW_COMMANDRST_INF_EID, CFE_EVS_EventType_INFORMATION, "SIMULATED_ACS_HW: RESET command");

    return CFE_SUCCESS;

} /* End of SIMULATED_ACS_HW_ResetCounters() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* SIMULATED_ACS_HW_VerifyCmdLength() -- Verify command packet length                   */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
bool SIMULATED_ACS_HW_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
{
    bool              result       = true;
    size_t            ActualLength = 0;
    CFE_SB_MsgId_t    MsgId        = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t FcnCode      = 0;

    CFE_MSG_GetSize(MsgPtr, &ActualLength);

    /*
    ** Verify the command packet length.
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_MSG_GetMsgId(MsgPtr, &MsgId);
        CFE_MSG_GetFcnCode(MsgPtr, &FcnCode);

        CFE_EVS_SendEvent(SIMULATED_ACS_HW_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid Msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)FcnCode, (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);

        result = false;

        SIMULATED_ACS_HW_Data.ErrCounter++;
    }

    return (result);

} /* End of SIMULATED_ACS_HW_VerifyCmdLength() */




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIMULATED_ACS_HW_Send_MGM_Data -- Send message         */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SIMULATED_ACS_HW_Send_MGM_Data() {
    // Initialize message MGM_Data
    SIMULATED_ACS_HW_Data.MsgId_MGM_Data = SIMULATED_ACS_HW_APP_COMMUNICATION_MID_15;
    SIMULATED_ACS_HW_Data.Size_MGM_Data = asn1SccMyInteger_REQUIRED_BITS_FOR_ENCODING;

    status = CFE_MSG_Init(&SIMULATED_ACS_HW_Data.MsgPtr_MGM_Data, SIMULATED_ACS_HW_Data.MsgId_MGM_Data, SIMULATED_ACS_HW_Data.Size_MGM_Data);

    if (status != CFE_SUCCESS) {
        SIMULATED_ACS_HW_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        CFE_ES_WriteToSysLog("[SIMULATED_ACS_HW] Error creating the message, RC = %lu\n", (unsigned long) status);
    }

    // Define buffer where stream will be written to
    byte perBuffer[asn1SccMyInteger_REQUIRED_BYTES_FOR_ENCODING];

    // Define bitstream
    BitStream bitstream;

    // Initialize bitstream
    BitStream_Init(&bitstream, perBuffer, asn1SccMyInteger_REQUIRED_BYTES_FOR_ENCODING);

    // Initialize error code
    int errorCode;

    // Encode
    if (!asn1SccMyInteger_Encode(&SIMULATED_ACS_HW_Data.OUT_pMGMData, &bitstream, &errorCode, TRUE))
    {
        CFE_ES_WriteToSysLog("[SIMULATED_ACS_HW] Encode failed. Error code %d.\n", errorCode);
    }


    status = CFE_PSP_MemCpy(CFE_SB_GetUserData(&SIMULATED_ACS_HW_Data.MsgPtr_MGM_Data), &perBuffer, sizeof(perBuffer));

    if (status != CFE_SUCCESS) {
        SIMULATED_ACS_HW_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        CFE_ES_WriteToSysLog("[SIMULATED_ACS_HW] Error writing the struct, RC = %lu\n", (unsigned long) status);
    }

    status = CFE_SB_TransmitMsg(&SIMULATED_ACS_HW_Data.MsgPtr_MGM_Data, false);

    if (status != CFE_SUCCESS) {
        SIMULATED_ACS_HW_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        CFE_ES_WriteToSysLog("[SIMULATED_ACS_HW] Error sending the message, RC = %lu\n", (unsigned long) status);
    } else {
        // CFE_ES_WriteToSysLog("[SIMULATED_ACS_HW] Message sent successfully");
    }

}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIMULATED_ACS_HW_Receive_MGT_Data -- Receive message   */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SIMULATED_ACS_HW_Receive_MGT_Data() {
    // CFE_ES_WriteToSysLog("[SIMULATED_ACS_HW] Try number %i of receiving message", i);
    status = CFE_SB_ReceiveBuffer(&SIMULATED_ACS_HW_Data.Buffer,
                           SIMULATED_ACS_HW_Data.CommandPipe,
                           CFE_SB_PEND_FOREVER);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[SIMULATED_ACS_HW] Error ReceiveBuffer, RC = 0x%08lX\n",
                             (unsigned long)status);
    } else
    {
        // Get buffer data
        byte *data = CFE_SB_GetUserData(&SIMULATED_ACS_HW_Data.Buffer->Msg);

        // Declare bit stream
        BitStream bitstream;

        // Initialize bitstream
        BitStream_AttachBuffer(&bitstream, data, asn1SccMyInteger_REQUIRED_BITS_FOR_ENCODING);

        // Declare errorCode
        int errorCode;

        // Decode data
        if (!asn1SccMyInteger_Decode(&SIMULATED_ACS_HW_Data.IN_pMGMData, &bitstream, &errorCode))
        {
            CFE_ES_WriteToSysLog("[SIMULATED_ACS_HW] Decode failed. Error code: %d", errorCode);
        }




    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIMULATED_ACS_HW_FindAppId -- Get App Id and set it in App data   */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SIMULATED_ACS_HW_FindAppId(void)
{
    /*
    ** Search the ES Application table for the name that matches this app
    */
    for (int i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++ )
    {

        if (!strcmp(CFE_ES_Global.AppTable[i].AppName, SIMULATED_ACS_HW_APP_NAME))
        {
            CFE_ES_GetAppIDByName(&SIMULATED_ACS_HW_Data.AppId, CFE_ES_Global.AppTable[i].AppName);
        }

    } /* end for */
}



