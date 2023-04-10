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
** File: ttc.c
**
** Purpose:
**   This file contains the source code for the Sample App.
**
*******************************************************************************/

/*
** Include Files:
*/
#include "ttc_app_version.h"
#include "ttc_app.h"
#include "ttc_app_user.h"
#include "dataview_init.h"


/* The sample_lib module provides the SAMPLE_LIB_Function() prototype */
#include <string.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/* TTC_AppMain() -- Application entry point and main process loop         */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void TTC_APP_Main(void)
{

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(TTC_PERF_ID);

    /*
    ** Perform application specific initialization
    ** If the Initialization fails, set the RunStatus to
    ** CFE_ES_RunStatus_APP_ERROR and the App will not enter the RunLoop
    */
    status = TTC_Init();
    if (status != CFE_SUCCESS)
    {
        TTC_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }


    // ---------   Send messages  ------------
    // uint32 period =  1000; // ms



    // uint32 period =  1000; // ms
    // --------- Receive messages ------------
    // Initialize pipe Current_Mode
    // Subscribe to Current_Mode     0x031 msg id
    status = CFE_SB_Subscribe(0x031,
                              TTC_Data.CommandPipe);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[TTC] Error Subscribing to communication pipe, RC = 0x%08lX\n",
                             (unsigned long) status);
    }



    // Initialize pipe Get_Out
    // Subscribe to Get_Out     0x022 msg id
    status = CFE_SB_Subscribe(0x022,
                              TTC_Data.CommandPipe);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[TTC] Error Subscribing to communication pipe, RC = 0x%08lX\n",
                             (unsigned long) status);
    }



    // Initialize pipe Last
    // Subscribe to Last     0x017 msg id
    status = CFE_SB_Subscribe(0x017,
                              TTC_Data.CommandPipe);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[TTC] Error Subscribing to communication pipe, RC = 0x%08lX\n",
                             (unsigned long) status);
    }

    // Main app loop
    while (CFE_ES_RunLoop(&TTC_Data.RunStatus) == true)
    {
        // Call user loop
        TTC_User_Main();
    }



    // Delete pipe to free resources
    status = CFE_SB_DeletePipe (TTC_Data.CommandPipe);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[TTC] Error Deleting the pipe, RC = 0x%08lX\n",
                             (unsigned long)status);
    }


    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(TTC_PERF_ID);

    TTC_Data.RunStatus = CFE_ES_RunStatus_APP_EXIT;

    CFE_ES_ExitApp(TTC_Data.RunStatus);

} /* End of TTC_APP_Main() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* TTC_Init() --  initialization                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 TTC_Init(void)
{
    int32 status;

    TTC_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Initialize app command execution counters
    */
    TTC_Data.CmdCounter = 0;
    TTC_Data.ErrCounter = 0;

    /*
    ** Initialize app configuration data
    */
    TTC_Data.PipeDepth = TTC_PIPE_DEPTH;

    strncpy(TTC_Data.PipeName, "TTC_CMD_PIPE", sizeof(TTC_Data.PipeName));
    TTC_Data.PipeName[sizeof(TTC_Data.PipeName) - 1] = 0;

    /*
    ** Initialize event filter table...
    */
    TTC_Data.EventFilters[0].EventID = TTC_STARTUP_INF_EID;
    TTC_Data.EventFilters[0].Mask    = 0x0000;
    TTC_Data.EventFilters[1].EventID = TTC_COMMAND_ERR_EID;
    TTC_Data.EventFilters[1].Mask    = 0x0000;
    TTC_Data.EventFilters[2].EventID = TTC_COMMANDNOP_INF_EID;
    TTC_Data.EventFilters[2].Mask    = 0x0000;
    TTC_Data.EventFilters[3].EventID = TTC_COMMANDRST_INF_EID;
    TTC_Data.EventFilters[3].Mask    = 0x0000;
    TTC_Data.EventFilters[4].EventID = TTC_INVALID_MSGID_ERR_EID;
    TTC_Data.EventFilters[4].Mask    = 0x0000;
    TTC_Data.EventFilters[5].EventID = TTC_LEN_ERR_EID;
    TTC_Data.EventFilters[5].Mask    = 0x0000;
    TTC_Data.EventFilters[6].EventID = TTC_PIPE_ERR_EID;
    TTC_Data.EventFilters[6].Mask    = 0x0000;
    

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(TTC_Data.EventFilters, TTC_EVENT_COUNTS, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[TTC] Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Initialize housekeeping packet (clear user data area).
    */
    CFE_MSG_Init(&TTC_Data.HkTlm.TlmHeader.Msg, CFE_SB_ValueToMsgId(TTC_HK_TLM_MID),
                 sizeof(TTC_Data.HkTlm));

    /*
    ** Create Software Bus message pipe.
    */
    status = CFE_SB_CreatePipe(&TTC_Data.CommandPipe, TTC_Data.PipeDepth, TTC_Data.PipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[TTC] Error creating pipe, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Subscribe to Housekeeping request commands
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(TTC_SEND_HK_MID), TTC_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[TTC] Error Subscribing to HK request, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Subscribe to ground command packets
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(TTC_CMD_MID), TTC_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[TTC] Error Subscribing to Command, RC = 0x%08lX\n", (unsigned long) status);

        return (status);
    }


    CFE_EVS_SendEvent(TTC_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION, "TTC App Initialized.%s",
                      TTC_VERSION_STRING);

    /*
    ** Create Software Bus message pipe.
    */
    TTC_Data.PipeDepth = TTC_PIPE_DEPTH;

    strcpy(TTC_Data.PipeName, "TTC_COM_PIPE");
    CFE_ES_WriteToSysLog("[TTC] COM_PIPE Name = %s", TTC_Data.PipeName);

    status = CFE_SB_CreatePipe(&TTC_Data.CommandPipe, TTC_Data.PipeDepth, TTC_Data.PipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[TTC] Error creating communication pipe,210, TTC_app.c, RC = 0x%08lX\n",
                             (unsigned long)status);
    }
    CFE_ES_WriteToSysLog("[TTC] Pipe creation successful");

    // Get App Id
    TTC_FindAppId();

    // Call user init
    TTC_User_Init();

    return (CFE_SUCCESS);

} /* End of TTC_Init() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  TTC_ProcessCommandPacket                                    */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the TTC    */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void TTC_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);

    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        case TTC_CMD_MID:
            TTC_ProcessGroundCommand(SBBufPtr);
            break;

        case TTC_SEND_HK_MID:
            TTC_ReportHousekeeping((CFE_MSG_CommandHeader_t *)SBBufPtr);
            break;

        default:
            CFE_EVS_SendEvent(TTC_INVALID_MSGID_ERR_EID, CFE_EVS_EventType_ERROR,
                              "TTC: invalid command packet,MID = 0x%x", (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;
    }

    return;

} /* End TTC_ProcessCommandPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* TTC_ProcessGroundCommand() -- TTC ground commands                */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void TTC_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_MSG_FcnCode_t CommandCode = 0;

    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);

    /*
    ** Process "known" TTC app ground commands
    */
    switch (CommandCode)
    {
        case TTC_NOOP_CC:
            if (TTC_VerifyCmdLength(&SBBufPtr->Msg, sizeof(TTC_NoopCmd_t)))
            {
                TTC_Noop((TTC_NoopCmd_t *)SBBufPtr);
            }

            break;

        case TTC_RESET_COUNTERS_CC:
            if (TTC_VerifyCmdLength(&SBBufPtr->Msg, sizeof(TTC_ResetCountersCmd_t)))
            {
                TTC_ResetCounters((TTC_ResetCountersCmd_t *)SBBufPtr);
            }

            break;


        /* default case already found during FC vs length test */
        default:
            CFE_EVS_SendEvent(TTC_COMMAND_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Invalid ground command code: CC = %d", CommandCode);
            break;
    }

    return;

} /* End of TTC_ProcessGroundCommand() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  TTC_ReportHousekeeping                                          */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 TTC_ReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg)
{

    /*
    ** Get command execution counters...
    */
    TTC_Data.HkTlm.Payload.CommandErrorCounter = TTC_Data.ErrCounter;
    TTC_Data.HkTlm.Payload.CommandCounter      = TTC_Data.CmdCounter;

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg(&TTC_Data.HkTlm.TlmHeader.Msg);
    CFE_SB_TransmitMsg(&TTC_Data.HkTlm.TlmHeader.Msg, true);


    return CFE_SUCCESS;

} /* End of TTC_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* TTC_Noop -- TTC NOOP commands                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 TTC_Noop(const TTC_NoopCmd_t *Msg)
{

    TTC_Data.CmdCounter++;

    CFE_EVS_SendEvent(TTC_COMMANDNOP_INF_EID, CFE_EVS_EventType_INFORMATION, "TTC: NOOP command %s",
                      TTC_VERSION);

    return CFE_SUCCESS;

} /* End of TTC_Noop */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  TTC_ResetCounters                                               */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 TTC_ResetCounters(const TTC_ResetCountersCmd_t *Msg)
{

    TTC_Data.CmdCounter = 0;
    TTC_Data.ErrCounter = 0;

    CFE_EVS_SendEvent(TTC_COMMANDRST_INF_EID, CFE_EVS_EventType_INFORMATION, "TTC: RESET command");

    return CFE_SUCCESS;

} /* End of TTC_ResetCounters() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* TTC_VerifyCmdLength() -- Verify command packet length                   */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
bool TTC_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
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

        CFE_EVS_SendEvent(TTC_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid Msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)FcnCode, (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);

        result = false;

        TTC_Data.ErrCounter++;
    }

    return (result);

} /* End of TTC_VerifyCmdLength() */




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TTC_Send_Get_In -- Send message         */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TTC_Send_Get_In() {
    // Initialize message Get_In
    TTC_Data.MsgId_Get_In = TTC_APP_COMMUNICATION_MID_21;
    TTC_Data.Size_Get_In = asn1SccMyInteger_REQUIRED_BITS_FOR_ENCODING;

    status = CFE_MSG_Init(&TTC_Data.MsgPtr_Get_In, TTC_Data.MsgId_Get_In, TTC_Data.Size_Get_In);

    if (status != CFE_SUCCESS) {
        TTC_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        CFE_ES_WriteToSysLog("[TTC] Error creating the message, RC = %lu\n", (unsigned long) status);
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
    if (!asn1SccMyInteger_Encode(&TTC_Data.OUT_pGetIn, &bitstream, &errorCode, TRUE))
    {
        CFE_ES_WriteToSysLog("[TTC] Encode failed. Error code %d.\n", errorCode);
    }


    status = CFE_PSP_MemCpy(CFE_SB_GetUserData(&TTC_Data.MsgPtr_Get_In), &perBuffer, sizeof(perBuffer));

    if (status != CFE_SUCCESS) {
        TTC_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        CFE_ES_WriteToSysLog("[TTC] Error writing the struct, RC = %lu\n", (unsigned long) status);
    }

    status = CFE_SB_TransmitMsg(&TTC_Data.MsgPtr_Get_In, false);

    if (status != CFE_SUCCESS) {
        TTC_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        CFE_ES_WriteToSysLog("[TTC] Error sending the message, RC = %lu\n", (unsigned long) status);
    } else {
        // CFE_ES_WriteToSysLog("[TTC] Message sent successfully");
    }

}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TTC_Send_Process_TC -- Send message         */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TTC_Send_Process_TC() {
    // Initialize message Process_TC
    TTC_Data.MsgId_Process_TC = TTC_APP_COMMUNICATION_MID_30;
    TTC_Data.Size_Process_TC = asn1SccMyInteger_REQUIRED_BITS_FOR_ENCODING;

    status = CFE_MSG_Init(&TTC_Data.MsgPtr_Process_TC, TTC_Data.MsgId_Process_TC, TTC_Data.Size_Process_TC);

    if (status != CFE_SUCCESS) {
        TTC_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        CFE_ES_WriteToSysLog("[TTC] Error creating the message, RC = %lu\n", (unsigned long) status);
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
    if (!asn1SccMyInteger_Encode(&TTC_Data.OUT_pTC, &bitstream, &errorCode, TRUE))
    {
        CFE_ES_WriteToSysLog("[TTC] Encode failed. Error code %d.\n", errorCode);
    }


    status = CFE_PSP_MemCpy(CFE_SB_GetUserData(&TTC_Data.MsgPtr_Process_TC), &perBuffer, sizeof(perBuffer));

    if (status != CFE_SUCCESS) {
        TTC_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        CFE_ES_WriteToSysLog("[TTC] Error writing the struct, RC = %lu\n", (unsigned long) status);
    }

    status = CFE_SB_TransmitMsg(&TTC_Data.MsgPtr_Process_TC, false);

    if (status != CFE_SUCCESS) {
        TTC_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        CFE_ES_WriteToSysLog("[TTC] Error sending the message, RC = %lu\n", (unsigned long) status);
    } else {
        // CFE_ES_WriteToSysLog("[TTC] Message sent successfully");
    }

}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TTC_Receive_Current_Mode -- Receive message   */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TTC_Receive_Current_Mode() {
    // CFE_ES_WriteToSysLog("[TTC] Try number %i of receiving message", i);
    status = CFE_SB_ReceiveBuffer(&TTC_Data.Buffer,
                           TTC_Data.CommandPipe,
                           CFE_SB_PEND_FOREVER);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[TTC] Error ReceiveBuffer, RC = 0x%08lX\n",
                             (unsigned long)status);
    } else
    {
        // Get buffer data
        byte *data = CFE_SB_GetUserData(&TTC_Data.Buffer->Msg);

        // Declare bit stream
        BitStream bitstream;

        // Initialize bitstream
        BitStream_AttachBuffer(&bitstream, data, asn1SccMyInteger_REQUIRED_BITS_FOR_ENCODING);

        // Declare errorCode
        int errorCode;

        // Decode data
        if (!asn1SccMyInteger_Decode(&TTC_Data.IN_pMode, &bitstream, &errorCode))
        {
            CFE_ES_WriteToSysLog("[TTC] Decode failed. Error code: %d", errorCode);
        }




    }
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TTC_Receive_Get_Out -- Receive message   */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TTC_Receive_Get_Out() {
    // CFE_ES_WriteToSysLog("[TTC] Try number %i of receiving message", i);
    status = CFE_SB_ReceiveBuffer(&TTC_Data.Buffer,
                           TTC_Data.CommandPipe,
                           CFE_SB_PEND_FOREVER);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[TTC] Error ReceiveBuffer, RC = 0x%08lX\n",
                             (unsigned long)status);
    } else
    {
        // Get buffer data
        byte *data = CFE_SB_GetUserData(&TTC_Data.Buffer->Msg);

        // Declare bit stream
        BitStream bitstream;

        // Initialize bitstream
        BitStream_AttachBuffer(&bitstream, data, asn1SccMyInteger_REQUIRED_BITS_FOR_ENCODING);

        // Declare errorCode
        int errorCode;

        // Decode data
        if (!asn1SccMyInteger_Decode(&TTC_Data.IN_pGetOut, &bitstream, &errorCode))
        {
            CFE_ES_WriteToSysLog("[TTC] Decode failed. Error code: %d", errorCode);
        }




    }
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TTC_Receive_Last -- Receive message   */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TTC_Receive_Last() {
    // CFE_ES_WriteToSysLog("[TTC] Try number %i of receiving message", i);
    status = CFE_SB_ReceiveBuffer(&TTC_Data.Buffer,
                           TTC_Data.CommandPipe,
                           CFE_SB_PEND_FOREVER);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[TTC] Error ReceiveBuffer, RC = 0x%08lX\n",
                             (unsigned long)status);
    } else
    {
        // Get buffer data
        byte *data = CFE_SB_GetUserData(&TTC_Data.Buffer->Msg);

        // Declare bit stream
        BitStream bitstream;

        // Initialize bitstream
        BitStream_AttachBuffer(&bitstream, data, asn1SccMyInteger_REQUIRED_BITS_FOR_ENCODING);

        // Declare errorCode
        int errorCode;

        // Decode data
        if (!asn1SccMyInteger_Decode(&TTC_Data.IN_pLast, &bitstream, &errorCode))
        {
            CFE_ES_WriteToSysLog("[TTC] Decode failed. Error code: %d", errorCode);
        }




    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TTC_FindAppId -- Get App Id and set it in App data   */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TTC_FindAppId(void)
{
    /*
    ** Search the ES Application table for the name that matches this app
    */
    for (int i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++ )
    {

        if (!strcmp(CFE_ES_Global.AppTable[i].AppName, TTC_APP_NAME))
        {
            CFE_ES_GetAppIDByName(&TTC_Data.AppId, CFE_ES_Global.AppTable[i].AppName);
        }

    } /* end for */
}



