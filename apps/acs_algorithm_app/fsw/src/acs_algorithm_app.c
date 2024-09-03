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
** File: acs_algorithm.c
**
** Purpose:
**   This file contains the source code for the Sample App.
**
*******************************************************************************/

/*
** Include Files:
*/
#include "acs_algorithm_app_version.h"
#include "acs_algorithm_app.h"
#include "dataview_init.h"


#include <string.h>

/* Global instance of ACS_ALGORITHM_APP_Data */
ACS_ALGORITHM_APP_Data_t ACS_ALGORITHM_APP_Data;


void ACS_ALGORITHM_User_Init(void)
{
    // User initialization here


}

void ACS_ALGORITHM_User_Main(void)
{
    // User main loop here

}


// Functions
int32 ACS_ALGORITHM_User_Send_Step_Out() {
    // TODO Build structs here
    // ACS_ALGORITHM_APP_Data.OUT_stepout;

    // Define buffer where stream will be written to
    byte perBuffer[asn1SccMyInteger_REQUIRED_BYTES_FOR_ENCODING];

    // Define bitstream
    BitStream bitstream;

    // Initialize bitstream
    BitStream_Init(&bitstream, perBuffer, asn1SccMyInteger_REQUIRED_BYTES_FOR_ENCODING);

    // Initialize error code
    int errorCode;

    status = CFE_SUCCESS;

    // Encode
    if (!asn1SccMyInteger_Encode(&ACS_ALGORITHM_APP_Data.OUT_stepout, &bitstream, &errorCode, TRUE))
    {
        CFE_ES_WriteToSysLog("[ACS_ALGORITHM] Encode failed. Error code %d.\n", errorCode);
        status = CFE_SB_BAD_ARGUMENT;
        ACS_ALGORITHM_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    if (status == CFE_SUCCESS)
    {
        status = CFE_PSP_MemCpy(CFE_SB_GetUserData(&ACS_ALGORITHM_APP_Data.MsgPtr_Step_Out), &perBuffer, sizeof(perBuffer));

        if (status != CFE_SUCCESS) {
            ACS_ALGORITHM_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
            CFE_ES_WriteToSysLog("[ACS_ALGORITHM] Error writing the struct, RC = %lu\n", (unsigned long) status);
        }
    }

    if (status == CFE_SUCCESS)
    {
        status = CFE_SB_TransmitMsg(&ACS_ALGORITHM_APP_Data.MsgPtr_Step_Out, false);

        if (status != CFE_SUCCESS) {
            ACS_ALGORITHM_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
            CFE_ES_WriteToSysLog("[ACS_ALGORITHM] Error sending the message, RC = %lu\n", (unsigned long) status);
        }
    }
    return status;
}
int32 ACS_ALGORITHM_User_Receive_Step_In() {
    status = CFE_SUCCESS;

    // Get buffer data
    byte *data = CFE_SB_GetUserData(&ACS_ALGORITHM_APP_Data.Buffer->Msg);

    // Declare bit stream
    BitStream bitstream;

    // Initialize bitstream
    BitStream_AttachBuffer(&bitstream, data, asn1SccMyInteger_REQUIRED_BITS_FOR_ENCODING);

    // Declare errorCode
    int errorCode;

    // Decode data
    if (!asn1SccMyInteger_Decode(&ACS_ALGORITHM_APP_Data.IN_stepin, &bitstream, &errorCode))
    {
        CFE_ES_WriteToSysLog("[ACS_ALGORITHM] Decode failed. Error code: %d", errorCode);
        status = CFE_SB_BAD_ARGUMENT;

    } else
    {
        // Message received stored in ACS_ALGORITHM_APP_Data.IN_stepin
        // TODO handle on receive
        // CFE_ES_WriteToSysLog("[ACS_ALGORITHM] Message received: %s", ACS_ALGORITHM_APP_Data.IN_stepin);

    }
    return status;
}







/*
** Helper functions, user shouldn't need to change them
*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/* ACS_ALGORITHM_APP_Main() -- Application entry point and main process loop         */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void ACS_ALGORITHM_APP_Main(void)
{

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(ACS_ALGORITHM_PERF_ID);

    /*
    ** Perform application specific initialization
    ** If the Initialization fails, set the RunStatus to
    ** CFE_ES_RunStatus_APP_ERROR and the App will not enter the RunLoop
    */
    status = ACS_ALGORITHM_APP_Init();
    if (status != CFE_SUCCESS)
    {
        ACS_ALGORITHM_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    // Main app loop
    while (CFE_ES_RunLoop(&ACS_ALGORITHM_APP_Data.RunStatus) == true)
    {

        /* Pend on receipt of command packet */
        status = CFE_SB_ReceiveBuffer(&ACS_ALGORITHM_APP_Data.Buffer, ACS_ALGORITHM_APP_Data.CommandPipe, CFE_SB_PEND_FOREVER);

        if (status == CFE_SUCCESS)
        {
            ACS_ALGORITHM_ProcessCommandPacket(ACS_ALGORITHM_APP_Data.Buffer);
        } else
        {
            CFE_EVS_SendEvent(ACS_ALGORITHM_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "ACS_ALGORITHM APP: SB Pipe Read Error, App Will Exit");

            ACS_ALGORITHM_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }


    }



    // Delete pipe to free resources
    status = CFE_SB_DeletePipe (ACS_ALGORITHM_APP_Data.CommandPipe);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[ACS_ALGORITHM] Error Deleting the pipe, RC = 0x%08lX\n",
                             (unsigned long)status);
    }


    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(ACS_ALGORITHM_PERF_ID);

    CFE_ES_ExitApp(ACS_ALGORITHM_APP_Data.RunStatus);

} /* End of ACS_ALGORITHM_APP_Main() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* ACS_ALGORITHM_APP_Init() --  initialization                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 ACS_ALGORITHM_APP_Init(void)
{
    int32 status = CFE_SUCCESS; // Initialize status to success;

    ACS_ALGORITHM_APP_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Initialize app command execution counters
    */
    ACS_ALGORITHM_APP_Data.CmdCounter = 0;
    ACS_ALGORITHM_APP_Data.ErrCounter = 0;

    /*
    ** Initialize app configuration data
    */
    ACS_ALGORITHM_APP_Data.PipeDepth = ACS_ALGORITHM_PIPE_DEPTH;

    (void)strncpy(ACS_ALGORITHM_APP_Data.PipeName, "ACS_ALGORITHM_CMD_PIPE", sizeof(ACS_ALGORITHM_APP_Data.PipeName));
    ACS_ALGORITHM_APP_Data.PipeName[sizeof(ACS_ALGORITHM_APP_Data.PipeName) - 1U] = 0;

    /*
    ** Initialize event filter table...
    */
    ACS_ALGORITHM_APP_Data.EventFilters[0].EventID = ACS_ALGORITHM_STARTUP_INF_EID;
    ACS_ALGORITHM_APP_Data.EventFilters[0].Mask    = 0x0000;
    ACS_ALGORITHM_APP_Data.EventFilters[1].EventID = ACS_ALGORITHM_COMMAND_ERR_EID;
    ACS_ALGORITHM_APP_Data.EventFilters[1].Mask    = 0x0000;
    ACS_ALGORITHM_APP_Data.EventFilters[2].EventID = ACS_ALGORITHM_COMMANDNOP_INF_EID;
    ACS_ALGORITHM_APP_Data.EventFilters[2].Mask    = 0x0000;
    ACS_ALGORITHM_APP_Data.EventFilters[3].EventID = ACS_ALGORITHM_COMMANDRST_INF_EID;
    ACS_ALGORITHM_APP_Data.EventFilters[3].Mask    = 0x0000;
    ACS_ALGORITHM_APP_Data.EventFilters[4].EventID = ACS_ALGORITHM_INVALID_MSGID_ERR_EID;
    ACS_ALGORITHM_APP_Data.EventFilters[4].Mask    = 0x0000;
    ACS_ALGORITHM_APP_Data.EventFilters[5].EventID = ACS_ALGORITHM_LEN_ERR_EID;
    ACS_ALGORITHM_APP_Data.EventFilters[5].Mask    = 0x0000;
    ACS_ALGORITHM_APP_Data.EventFilters[6].EventID = ACS_ALGORITHM_PIPE_ERR_EID;
    ACS_ALGORITHM_APP_Data.EventFilters[6].Mask    = 0x0000;
    

    /* Register the events */
    status = CFE_EVS_Register(ACS_ALGORITHM_APP_Data.EventFilters, ACS_ALGORITHM_EVENT_COUNTS, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[ACS_ALGORITHM] Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
    }


    /*
    ** Initialize housekeeping packet (clear user data area).
    */
    if (status == CFE_SUCCESS)
    {
        status = CFE_MSG_Init(&ACS_ALGORITHM_APP_Data.HkTlm.TlmHeader.Msg, CFE_SB_ValueToMsgId(ACS_ALGORITHM_HK_TLM_MID),
                 sizeof(ACS_ALGORITHM_APP_Data.HkTlm));

        if (status != CFE_SUCCESS)
        {
        CFE_ES_WriteToSysLog("[ACS_ALGORITHM] Error initializing housekeeping packets, RC = 0x%08lX\n", (unsigned long)status);
        }
    }

    /*
    ** Create Software Bus message pipe.
    */
    if (status == CFE_SUCCESS)
    {
        status = CFE_SB_CreatePipe(&ACS_ALGORITHM_APP_Data.CommandPipe, ACS_ALGORITHM_APP_Data.PipeDepth, ACS_ALGORITHM_APP_Data.PipeName);
        if (status != CFE_SUCCESS)
        {
        CFE_ES_WriteToSysLog("[ACS_ALGORITHM] Error creating pipe, RC = 0x%08lX\n", (unsigned long)status);
        }
    }


    /*
    ** Subscribe to Housekeeping request commands
    */
    if (status == CFE_SUCCESS)
    {
        status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(ACS_ALGORITHM_SEND_HK_MID), ACS_ALGORITHM_APP_Data.CommandPipe);
        if (status != CFE_SUCCESS)
        {
        CFE_ES_WriteToSysLog("[ACS_ALGORITHM] Error Subscribing to HK request, RC = 0x%08lX\n", (unsigned long)status);
        }
    }

    /*
    ** Subscribe to ground command packets
    */
    if (status == CFE_SUCCESS)
    {
        status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(ACS_ALGORITHM_CMD_MID), ACS_ALGORITHM_APP_Data.CommandPipe);
        if (status != CFE_SUCCESS)
        {
        CFE_ES_WriteToSysLog("[ACS_ALGORITHM] Error Subscribing to Command, RC = 0x%08lX\n", (unsigned long) status);
        }
    }



    if (status == CFE_SUCCESS)
    {
    CFE_EVS_SendEvent(ACS_ALGORITHM_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION, "ACS_ALGORITHM App Initialized.%s",
                      ACS_ALGORITHM_VERSION_STRING);
    }

    // Get App Id
    ACS_ALGORITHM_FindAppId();



    ACS_ALGORITHM_Send_Step_Out_Init();
    if (status == CFE_SUCCESS)
{
    ACS_ALGORITHM_Receive_Step_In_Init();
}



    // Call user init
    ACS_ALGORITHM_User_Init();

    return status;

} /* End of ACS_ALGORITHM_Init() */

int32 ACS_ALGORITHM_Send_Step_Out_Init(void)
{
    // Initialize message Step_Out
    int32 status = CFE_SUCCESS; // Initialize status to success;
    ACS_ALGORITHM_APP_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;
    ACS_ALGORITHM_APP_Data.MsgId_Step_Out = ACS_ALGORITHM_APP_COMMUNICATION_MID_12;
    ACS_ALGORITHM_APP_Data.Size_Step_Out = asn1SccMyInteger_REQUIRED_BITS_FOR_ENCODING;

    status = CFE_MSG_Init(&ACS_ALGORITHM_APP_Data.MsgPtr_Step_Out, ACS_ALGORITHM_APP_Data.MsgId_Step_Out, ACS_ALGORITHM_APP_Data.Size_Step_Out);

    if (status != CFE_SUCCESS) {
        ACS_ALGORITHM_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        CFE_ES_WriteToSysLog("[ACS_ALGORITHM] Error creating the message, RC = %lu\n", (unsigned long) status);
    }
    return status;
}
int32 ACS_ALGORITHM_Receive_Step_In_Init(void)
{
    status = CFE_SB_Subscribe(0x011,
                              ACS_ALGORITHM_APP_Data.CommandPipe);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[ACS_ALGORITHM] Error Subscribing to communication pipe, RC = 0x%08lX\n",
                             (unsigned long) status);
    }
    return status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* ACS_ALGORITHM_FindAppId -- Get App Id and set it in App data   */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 ACS_ALGORITHM_FindAppId(void)
{
    status = CFE_ES_ERR_NAME_NOT_FOUND;
    CFE_ES_AppId_t TempAppId;
    /*
    ** Search the ES Application table for the name that matches this app
    */
    for (int i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++ )
    {

        if (!strcmp(CFE_ES_Global.AppTable[i].AppName, ACS_ALGORITHM_APP_NAME))
        {
            status = CFE_ES_GetAppIDByName(&TempAppId, CFE_ES_Global.AppTable[i].AppName);
            if (status == CFE_SUCCESS)
            {
                ACS_ALGORITHM_APP_Data.AppId = TempAppId;
            }
            else
            {
                CFE_ES_WriteToSysLog("[ACS_ALGORITHM] Error getting the App ID, RC = %lu\n", (unsigned long)status);
            }
            break;
        }

    } /* end for */
    return status;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  ACS_ALGORITHM_ProcessCommandPacket                                    */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the ACS_ALGORITHM    */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void ACS_ALGORITHM_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);

    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        case ACS_ALGORITHM_CMD_MID:
            ACS_ALGORITHM_ProcessGroundCommand(SBBufPtr);
            break;

        case ACS_ALGORITHM_SEND_HK_MID:
            ACS_ALGORITHM_ReportHousekeeping((CFE_MSG_CommandHeader_t *)SBBufPtr);
            // Call user loop
            ACS_ALGORITHM_User_Main();
            break;

        // Process Step_In
        case 0x011:
            ACS_ALGORITHM_User_Receive_Step_In();

    break;

        default:
            CFE_EVS_SendEvent(ACS_ALGORITHM_INVALID_MSGID_ERR_EID, CFE_EVS_EventType_ERROR,
                              "ACS_ALGORITHM: invalid command packet,MID = 0x%x", (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;
    }

    return;

} /* End ACS_ALGORITHM_ProcessCommandPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* ACS_ALGORITHM_ProcessGroundCommand() -- ACS_ALGORITHM ground commands                */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void ACS_ALGORITHM_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_MSG_FcnCode_t CommandCode = 0;

    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);

    /*
    ** Process "known" ACS_ALGORITHM app ground commands
    */
    switch (CommandCode)
    {
        case ACS_ALGORITHM_NOOP_CC:
            if (ACS_ALGORITHM_VerifyCmdLength(&SBBufPtr->Msg, sizeof(ACS_ALGORITHM_NoopCmd_t)))
            {
                ACS_ALGORITHM_Noop((ACS_ALGORITHM_NoopCmd_t *)SBBufPtr);
            } else
            {
                CFE_ES_WriteToSysLog("[ACS_ALGORITHM] Invalid command length\n");
            }

            break;

        case ACS_ALGORITHM_RESET_COUNTERS_CC:
            if (ACS_ALGORITHM_VerifyCmdLength(&SBBufPtr->Msg, sizeof(ACS_ALGORITHM_ResetCountersCmd_t)))
            {
                ACS_ALGORITHM_ResetCounters((ACS_ALGORITHM_ResetCountersCmd_t *)SBBufPtr);
            } else
            {
                CFE_ES_WriteToSysLog("[ACS_ALGORITHM] Invalid command length\n");
            }

            break;


        /* default case already found during FC vs length test */
        default:
            CFE_EVS_SendEvent(ACS_ALGORITHM_COMMAND_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Invalid ground command code: CC = %d", CommandCode);
            break;
    }

    return;

} /* End of ACS_ALGORITHM_ProcessGroundCommand() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  ACS_ALGORITHM_ReportHousekeeping                                          */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 ACS_ALGORITHM_ReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg)
{
    (void)Msg;

    /*
    ** Get command execution counters...
    */
    ACS_ALGORITHM_APP_Data.HkTlm.Payload.CommandErrorCounter = ACS_ALGORITHM_APP_Data.ErrCounter;
    ACS_ALGORITHM_APP_Data.HkTlm.Payload.CommandCounter      = ACS_ALGORITHM_APP_Data.CmdCounter;

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg(&ACS_ALGORITHM_APP_Data.HkTlm.TlmHeader.Msg);
    CFE_SB_TransmitMsg(&ACS_ALGORITHM_APP_Data.HkTlm.TlmHeader.Msg, true);


    return CFE_SUCCESS;

} /* End of ACS_ALGORITHM_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* ACS_ALGORITHM_Noop -- ACS_ALGORITHM NOOP commands                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 ACS_ALGORITHM_Noop(const ACS_ALGORITHM_NoopCmd_t *Msg)
{
    (void)Msg;

    ACS_ALGORITHM_APP_Data.CmdCounter++;

    CFE_EVS_SendEvent(ACS_ALGORITHM_COMMANDNOP_INF_EID, CFE_EVS_EventType_INFORMATION, "ACS_ALGORITHM: NOOP command %s",
                      ACS_ALGORITHM_VERSION);

    return CFE_SUCCESS;

} /* End of ACS_ALGORITHM_Noop */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  ACS_ALGORITHM_ResetCounters                                               */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 ACS_ALGORITHM_ResetCounters(const ACS_ALGORITHM_ResetCountersCmd_t *Msg)
{
    (void)Msg;

    ACS_ALGORITHM_APP_Data.CmdCounter = 0;
    ACS_ALGORITHM_APP_Data.ErrCounter = 0;

    CFE_EVS_SendEvent(ACS_ALGORITHM_COMMANDRST_INF_EID, CFE_EVS_EventType_INFORMATION, "ACS_ALGORITHM: RESET command");

    return CFE_SUCCESS;

} /* End of ACS_ALGORITHM_ResetCounters() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* ACS_ALGORITHM_VerifyCmdLength() -- Verify command packet length                   */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
bool ACS_ALGORITHM_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
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

        CFE_EVS_SendEvent(ACS_ALGORITHM_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid Msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)FcnCode, (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);

        result = false;

        ACS_ALGORITHM_APP_Data.ErrCounter++;
    }

    return (result);

} /* End of ACS_ALGORITHM_VerifyCmdLength() */


