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
** File: measurer_and_actuator.c
**
** Purpose:
**   This file contains the source code for the Sample App.
**
*******************************************************************************/

/*
** Include Files:
*/
#include "measurer_and_actuator_app_version.h"
#include "measurer_and_actuator_app.h"
#include "measurer_and_actuator_app_user.h"
#include "dataview_init.h"


/* The sample_lib module provides the SAMPLE_LIB_Function() prototype */
#include <string.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/* MEASURER_AND_ACTUATOR_AppMain() -- Application entry point and main process loop         */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void MEASURER_AND_ACTUATOR_APP_Main(void)
{

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(MEASURER_AND_ACTUATOR_PERF_ID);

    /*
    ** Perform application specific initialization
    ** If the Initialization fails, set the RunStatus to
    ** CFE_ES_RunStatus_APP_ERROR and the App will not enter the RunLoop
    */
    status = MEASURER_AND_ACTUATOR_Init();
    if (status != CFE_SUCCESS)
    {
        MEASURER_AND_ACTUATOR_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }


    // ---------   Send messages  ------------
    // uint32 period =  1000; // ms



    // uint32 period =  1000; // ms
    // --------- Receive messages ------------
    // Initialize pipe MGM_Data
    // Subscribe to MGM_Data     0x015 msg id
    status = CFE_SB_Subscribe(0x015,
                              MEASURER_AND_ACTUATOR_Data.CommandPipe);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Error Subscribing to communication pipe, RC = 0x%08lX\n",
                             (unsigned long) status);
    }



    // Initialize pipe Step_Out
    // Subscribe to Step_Out     0x013 msg id
    status = CFE_SB_Subscribe(0x013,
                              MEASURER_AND_ACTUATOR_Data.CommandPipe);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Error Subscribing to communication pipe, RC = 0x%08lX\n",
                             (unsigned long) status);
    }

    // Main app loop
    while (CFE_ES_RunLoop(&MEASURER_AND_ACTUATOR_Data.RunStatus) == true)
    {
        // Call user loop
        MEASURER_AND_ACTUATOR_User_Main();
    }



    // Delete pipe to free resources
    status = CFE_SB_DeletePipe (MEASURER_AND_ACTUATOR_Data.CommandPipe);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Error Deleting the pipe, RC = 0x%08lX\n",
                             (unsigned long)status);
    }


    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(MEASURER_AND_ACTUATOR_PERF_ID);

    MEASURER_AND_ACTUATOR_Data.RunStatus = CFE_ES_RunStatus_APP_EXIT;

    CFE_ES_ExitApp(MEASURER_AND_ACTUATOR_Data.RunStatus);

} /* End of MEASURER_AND_ACTUATOR_APP_Main() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* MEASURER_AND_ACTUATOR_Init() --  initialization                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 MEASURER_AND_ACTUATOR_Init(void)
{
    int32 status;

    MEASURER_AND_ACTUATOR_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Initialize app command execution counters
    */
    MEASURER_AND_ACTUATOR_Data.CmdCounter = 0;
    MEASURER_AND_ACTUATOR_Data.ErrCounter = 0;

    /*
    ** Initialize app configuration data
    */
    MEASURER_AND_ACTUATOR_Data.PipeDepth = MEASURER_AND_ACTUATOR_PIPE_DEPTH;

    strncpy(MEASURER_AND_ACTUATOR_Data.PipeName, "MEASURER_AND_ACTUATOR_CMD_PIPE", sizeof(MEASURER_AND_ACTUATOR_Data.PipeName));
    MEASURER_AND_ACTUATOR_Data.PipeName[sizeof(MEASURER_AND_ACTUATOR_Data.PipeName) - 1] = 0;

    /*
    ** Initialize event filter table...
    */
    MEASURER_AND_ACTUATOR_Data.EventFilters[0].EventID = MEASURER_AND_ACTUATOR_STARTUP_INF_EID;
    MEASURER_AND_ACTUATOR_Data.EventFilters[0].Mask    = 0x0000;
    MEASURER_AND_ACTUATOR_Data.EventFilters[1].EventID = MEASURER_AND_ACTUATOR_COMMAND_ERR_EID;
    MEASURER_AND_ACTUATOR_Data.EventFilters[1].Mask    = 0x0000;
    MEASURER_AND_ACTUATOR_Data.EventFilters[2].EventID = MEASURER_AND_ACTUATOR_COMMANDNOP_INF_EID;
    MEASURER_AND_ACTUATOR_Data.EventFilters[2].Mask    = 0x0000;
    MEASURER_AND_ACTUATOR_Data.EventFilters[3].EventID = MEASURER_AND_ACTUATOR_COMMANDRST_INF_EID;
    MEASURER_AND_ACTUATOR_Data.EventFilters[3].Mask    = 0x0000;
    MEASURER_AND_ACTUATOR_Data.EventFilters[4].EventID = MEASURER_AND_ACTUATOR_INVALID_MSGID_ERR_EID;
    MEASURER_AND_ACTUATOR_Data.EventFilters[4].Mask    = 0x0000;
    MEASURER_AND_ACTUATOR_Data.EventFilters[5].EventID = MEASURER_AND_ACTUATOR_LEN_ERR_EID;
    MEASURER_AND_ACTUATOR_Data.EventFilters[5].Mask    = 0x0000;
    MEASURER_AND_ACTUATOR_Data.EventFilters[6].EventID = MEASURER_AND_ACTUATOR_PIPE_ERR_EID;
    MEASURER_AND_ACTUATOR_Data.EventFilters[6].Mask    = 0x0000;
    

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(MEASURER_AND_ACTUATOR_Data.EventFilters, MEASURER_AND_ACTUATOR_EVENT_COUNTS, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Initialize housekeeping packet (clear user data area).
    */
    CFE_MSG_Init(&MEASURER_AND_ACTUATOR_Data.HkTlm.TlmHeader.Msg, CFE_SB_ValueToMsgId(MEASURER_AND_ACTUATOR_HK_TLM_MID),
                 sizeof(MEASURER_AND_ACTUATOR_Data.HkTlm));

    /*
    ** Create Software Bus message pipe.
    */
    status = CFE_SB_CreatePipe(&MEASURER_AND_ACTUATOR_Data.CommandPipe, MEASURER_AND_ACTUATOR_Data.PipeDepth, MEASURER_AND_ACTUATOR_Data.PipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Error creating pipe, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Subscribe to Housekeeping request commands
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(MEASURER_AND_ACTUATOR_SEND_HK_MID), MEASURER_AND_ACTUATOR_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Error Subscribing to HK request, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Subscribe to ground command packets
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(MEASURER_AND_ACTUATOR_CMD_MID), MEASURER_AND_ACTUATOR_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Error Subscribing to Command, RC = 0x%08lX\n", (unsigned long) status);

        return (status);
    }


    CFE_EVS_SendEvent(MEASURER_AND_ACTUATOR_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION, "MEASURER_AND_ACTUATOR App Initialized.%s",
                      MEASURER_AND_ACTUATOR_VERSION_STRING);

    /*
    ** Create Software Bus message pipe.
    */
    MEASURER_AND_ACTUATOR_Data.PipeDepth = MEASURER_AND_ACTUATOR_PIPE_DEPTH;

    strcpy(MEASURER_AND_ACTUATOR_Data.PipeName, "MEASURER_AND_ACTUATOR_COM_PIPE");
    CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] COM_PIPE Name = %s", MEASURER_AND_ACTUATOR_Data.PipeName);

    status = CFE_SB_CreatePipe(&MEASURER_AND_ACTUATOR_Data.CommandPipe, MEASURER_AND_ACTUATOR_Data.PipeDepth, MEASURER_AND_ACTUATOR_Data.PipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Error creating communication pipe,210, MEASURER_AND_ACTUATOR_app.c, RC = 0x%08lX\n",
                             (unsigned long)status);
    }
    CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Pipe creation successful");

    // Get App Id
    MEASURER_AND_ACTUATOR_FindAppId();

    // Call user init
    MEASURER_AND_ACTUATOR_User_Init();

    return (CFE_SUCCESS);

} /* End of MEASURER_AND_ACTUATOR_Init() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  MEASURER_AND_ACTUATOR_ProcessCommandPacket                                    */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the MEASURER_AND_ACTUATOR    */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void MEASURER_AND_ACTUATOR_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);

    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        case MEASURER_AND_ACTUATOR_CMD_MID:
            MEASURER_AND_ACTUATOR_ProcessGroundCommand(SBBufPtr);
            break;

        case MEASURER_AND_ACTUATOR_SEND_HK_MID:
            MEASURER_AND_ACTUATOR_ReportHousekeeping((CFE_MSG_CommandHeader_t *)SBBufPtr);
            break;

        default:
            CFE_EVS_SendEvent(MEASURER_AND_ACTUATOR_INVALID_MSGID_ERR_EID, CFE_EVS_EventType_ERROR,
                              "MEASURER_AND_ACTUATOR: invalid command packet,MID = 0x%x", (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;
    }

    return;

} /* End MEASURER_AND_ACTUATOR_ProcessCommandPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* MEASURER_AND_ACTUATOR_ProcessGroundCommand() -- MEASURER_AND_ACTUATOR ground commands                */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void MEASURER_AND_ACTUATOR_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_MSG_FcnCode_t CommandCode = 0;

    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);

    /*
    ** Process "known" MEASURER_AND_ACTUATOR app ground commands
    */
    switch (CommandCode)
    {
        case MEASURER_AND_ACTUATOR_NOOP_CC:
            if (MEASURER_AND_ACTUATOR_VerifyCmdLength(&SBBufPtr->Msg, sizeof(MEASURER_AND_ACTUATOR_NoopCmd_t)))
            {
                MEASURER_AND_ACTUATOR_Noop((MEASURER_AND_ACTUATOR_NoopCmd_t *)SBBufPtr);
            }

            break;

        case MEASURER_AND_ACTUATOR_RESET_COUNTERS_CC:
            if (MEASURER_AND_ACTUATOR_VerifyCmdLength(&SBBufPtr->Msg, sizeof(MEASURER_AND_ACTUATOR_ResetCountersCmd_t)))
            {
                MEASURER_AND_ACTUATOR_ResetCounters((MEASURER_AND_ACTUATOR_ResetCountersCmd_t *)SBBufPtr);
            }

            break;


        /* default case already found during FC vs length test */
        default:
            CFE_EVS_SendEvent(MEASURER_AND_ACTUATOR_COMMAND_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Invalid ground command code: CC = %d", CommandCode);
            break;
    }

    return;

} /* End of MEASURER_AND_ACTUATOR_ProcessGroundCommand() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  MEASURER_AND_ACTUATOR_ReportHousekeeping                                          */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 MEASURER_AND_ACTUATOR_ReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg)
{

    /*
    ** Get command execution counters...
    */
    MEASURER_AND_ACTUATOR_Data.HkTlm.Payload.CommandErrorCounter = MEASURER_AND_ACTUATOR_Data.ErrCounter;
    MEASURER_AND_ACTUATOR_Data.HkTlm.Payload.CommandCounter      = MEASURER_AND_ACTUATOR_Data.CmdCounter;

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg(&MEASURER_AND_ACTUATOR_Data.HkTlm.TlmHeader.Msg);
    CFE_SB_TransmitMsg(&MEASURER_AND_ACTUATOR_Data.HkTlm.TlmHeader.Msg, true);


    return CFE_SUCCESS;

} /* End of MEASURER_AND_ACTUATOR_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* MEASURER_AND_ACTUATOR_Noop -- MEASURER_AND_ACTUATOR NOOP commands                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 MEASURER_AND_ACTUATOR_Noop(const MEASURER_AND_ACTUATOR_NoopCmd_t *Msg)
{

    MEASURER_AND_ACTUATOR_Data.CmdCounter++;

    CFE_EVS_SendEvent(MEASURER_AND_ACTUATOR_COMMANDNOP_INF_EID, CFE_EVS_EventType_INFORMATION, "MEASURER_AND_ACTUATOR: NOOP command %s",
                      MEASURER_AND_ACTUATOR_VERSION);

    return CFE_SUCCESS;

} /* End of MEASURER_AND_ACTUATOR_Noop */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  MEASURER_AND_ACTUATOR_ResetCounters                                               */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 MEASURER_AND_ACTUATOR_ResetCounters(const MEASURER_AND_ACTUATOR_ResetCountersCmd_t *Msg)
{

    MEASURER_AND_ACTUATOR_Data.CmdCounter = 0;
    MEASURER_AND_ACTUATOR_Data.ErrCounter = 0;

    CFE_EVS_SendEvent(MEASURER_AND_ACTUATOR_COMMANDRST_INF_EID, CFE_EVS_EventType_INFORMATION, "MEASURER_AND_ACTUATOR: RESET command");

    return CFE_SUCCESS;

} /* End of MEASURER_AND_ACTUATOR_ResetCounters() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* MEASURER_AND_ACTUATOR_VerifyCmdLength() -- Verify command packet length                   */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
bool MEASURER_AND_ACTUATOR_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
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

        CFE_EVS_SendEvent(MEASURER_AND_ACTUATOR_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid Msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)FcnCode, (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);

        result = false;

        MEASURER_AND_ACTUATOR_Data.ErrCounter++;
    }

    return (result);

} /* End of MEASURER_AND_ACTUATOR_VerifyCmdLength() */




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* MEASURER_AND_ACTUATOR_Send_MGT_Data -- Send message         */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void MEASURER_AND_ACTUATOR_Send_MGT_Data() {
    // Initialize message MGT_Data
    MEASURER_AND_ACTUATOR_Data.MsgId_MGT_Data = MEASURER_AND_ACTUATOR_APP_COMMUNICATION_MID_16;
    MEASURER_AND_ACTUATOR_Data.Size_MGT_Data = asn1SccMyInteger_REQUIRED_BITS_FOR_ENCODING;

    status = CFE_MSG_Init(&MEASURER_AND_ACTUATOR_Data.MsgPtr_MGT_Data, MEASURER_AND_ACTUATOR_Data.MsgId_MGT_Data, MEASURER_AND_ACTUATOR_Data.Size_MGT_Data);

    if (status != CFE_SUCCESS) {
        MEASURER_AND_ACTUATOR_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Error creating the message, RC = %lu\n", (unsigned long) status);
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
    if (!asn1SccMyInteger_Encode(&MEASURER_AND_ACTUATOR_Data.OUT_pMGMData, &bitstream, &errorCode, TRUE))
    {
        CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Encode failed. Error code %d.\n", errorCode);
    }


    status = CFE_PSP_MemCpy(CFE_SB_GetUserData(&MEASURER_AND_ACTUATOR_Data.MsgPtr_MGT_Data), &perBuffer, sizeof(perBuffer));

    if (status != CFE_SUCCESS) {
        MEASURER_AND_ACTUATOR_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Error writing the struct, RC = %lu\n", (unsigned long) status);
    }

    status = CFE_SB_TransmitMsg(&MEASURER_AND_ACTUATOR_Data.MsgPtr_MGT_Data, false);

    if (status != CFE_SUCCESS) {
        MEASURER_AND_ACTUATOR_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Error sending the message, RC = %lu\n", (unsigned long) status);
    } else {
        // CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Message sent successfully");
    }

}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* MEASURER_AND_ACTUATOR_Send_Step_In -- Send message         */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void MEASURER_AND_ACTUATOR_Send_Step_In() {
    // Initialize message Step_In
    MEASURER_AND_ACTUATOR_Data.MsgId_Step_In = MEASURER_AND_ACTUATOR_APP_COMMUNICATION_MID_11;
    MEASURER_AND_ACTUATOR_Data.Size_Step_In = asn1SccMyInteger_REQUIRED_BITS_FOR_ENCODING;

    status = CFE_MSG_Init(&MEASURER_AND_ACTUATOR_Data.MsgPtr_Step_In, MEASURER_AND_ACTUATOR_Data.MsgId_Step_In, MEASURER_AND_ACTUATOR_Data.Size_Step_In);

    if (status != CFE_SUCCESS) {
        MEASURER_AND_ACTUATOR_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Error creating the message, RC = %lu\n", (unsigned long) status);
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
    if (!asn1SccMyInteger_Encode(&MEASURER_AND_ACTUATOR_Data.OUT_pStepIn, &bitstream, &errorCode, TRUE))
    {
        CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Encode failed. Error code %d.\n", errorCode);
    }


    status = CFE_PSP_MemCpy(CFE_SB_GetUserData(&MEASURER_AND_ACTUATOR_Data.MsgPtr_Step_In), &perBuffer, sizeof(perBuffer));

    if (status != CFE_SUCCESS) {
        MEASURER_AND_ACTUATOR_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Error writing the struct, RC = %lu\n", (unsigned long) status);
    }

    status = CFE_SB_TransmitMsg(&MEASURER_AND_ACTUATOR_Data.MsgPtr_Step_In, false);

    if (status != CFE_SUCCESS) {
        MEASURER_AND_ACTUATOR_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Error sending the message, RC = %lu\n", (unsigned long) status);
    } else {
        // CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Message sent successfully");
    }

}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* MEASURER_AND_ACTUATOR_Receive_MGM_Data -- Receive message   */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void MEASURER_AND_ACTUATOR_Receive_MGM_Data() {
    // CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Try number %i of receiving message", i);
    status = CFE_SB_ReceiveBuffer(&MEASURER_AND_ACTUATOR_Data.Buffer,
                           MEASURER_AND_ACTUATOR_Data.CommandPipe,
                           CFE_SB_PEND_FOREVER);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Error ReceiveBuffer, RC = 0x%08lX\n",
                             (unsigned long)status);
    } else
    {
        // Get buffer data
        byte *data = CFE_SB_GetUserData(&MEASURER_AND_ACTUATOR_Data.Buffer->Msg);

        // Declare bit stream
        BitStream bitstream;

        // Initialize bitstream
        BitStream_AttachBuffer(&bitstream, data, asn1SccMyInteger_REQUIRED_BITS_FOR_ENCODING);

        // Declare errorCode
        int errorCode;

        // Decode data
        if (!asn1SccMyInteger_Decode(&MEASURER_AND_ACTUATOR_Data.IN_pMGMData, &bitstream, &errorCode))
        {
            CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Decode failed. Error code: %d", errorCode);
        }




    }
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* MEASURER_AND_ACTUATOR_Receive_Step_Out -- Receive message   */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void MEASURER_AND_ACTUATOR_Receive_Step_Out() {
    // CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Try number %i of receiving message", i);
    status = CFE_SB_ReceiveBuffer(&MEASURER_AND_ACTUATOR_Data.Buffer,
                           MEASURER_AND_ACTUATOR_Data.CommandPipe,
                           CFE_SB_PEND_FOREVER);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Error ReceiveBuffer, RC = 0x%08lX\n",
                             (unsigned long)status);
    } else
    {
        // Get buffer data
        byte *data = CFE_SB_GetUserData(&MEASURER_AND_ACTUATOR_Data.Buffer->Msg);

        // Declare bit stream
        BitStream bitstream;

        // Initialize bitstream
        BitStream_AttachBuffer(&bitstream, data, asn1SccMyInteger_REQUIRED_BITS_FOR_ENCODING);

        // Declare errorCode
        int errorCode;

        // Decode data
        if (!asn1SccMyInteger_Decode(&MEASURER_AND_ACTUATOR_Data.IN_pStepOut, &bitstream, &errorCode))
        {
            CFE_ES_WriteToSysLog("[MEASURER_AND_ACTUATOR] Decode failed. Error code: %d", errorCode);
        }




    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* MEASURER_AND_ACTUATOR_FindAppId -- Get App Id and set it in App data   */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void MEASURER_AND_ACTUATOR_FindAppId(void)
{
    /*
    ** Search the ES Application table for the name that matches this app
    */
    for (int i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++ )
    {

        if (!strcmp(CFE_ES_Global.AppTable[i].AppName, MEASURER_AND_ACTUATOR_APP_NAME))
        {
            CFE_ES_GetAppIDByName(&MEASURER_AND_ACTUATOR_Data.AppId, CFE_ES_Global.AppTable[i].AppName);
        }

    } /* end for */
}



