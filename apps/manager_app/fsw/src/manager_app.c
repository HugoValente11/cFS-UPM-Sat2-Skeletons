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
** File: manager.c
**
** Purpose:
**   This file contains the source code for the Sample App.
**
*******************************************************************************/

/*
** Include Files:
*/
#include "manager_app_version.h"
#include "manager_app.h"
#include "dataview_init.h"


#include <string.h>

/* Global instance of MANAGER_APP_Data */
MANAGER_APP_Data_t MANAGER_APP_Data;


void MANAGER_User_Init(void)
{
    // User initialization here


}

void MANAGER_User_Main(void)
{
    // User main loop here

}


// Functions
int32 MANAGER_User_Send_Current_Mode() {
    // TODO Build structs here
    // MANAGER_APP_Data.OUT_currentmode;

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
    if (!asn1SccMyInteger_Encode(&MANAGER_APP_Data.OUT_currentmode, &bitstream, &errorCode, TRUE))
    {
        CFE_ES_WriteToSysLog("[MANAGER] Encode failed. Error code %d.\n", errorCode);
        status = CFE_SB_BAD_ARGUMENT;
        MANAGER_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    if (status == CFE_SUCCESS)
    {
        status = CFE_PSP_MemCpy(CFE_SB_GetUserData(&MANAGER_APP_Data.MsgPtr_Current_Mode), &perBuffer, sizeof(perBuffer));

        if (status != CFE_SUCCESS) {
            MANAGER_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
            CFE_ES_WriteToSysLog("[MANAGER] Error writing the struct, RC = %lu\n", (unsigned long) status);
        }
    }

    if (status == CFE_SUCCESS)
    {
        status = CFE_SB_TransmitMsg(&MANAGER_APP_Data.MsgPtr_Current_Mode, false);

        if (status != CFE_SUCCESS) {
            MANAGER_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
            CFE_ES_WriteToSysLog("[MANAGER] Error sending the message, RC = %lu\n", (unsigned long) status);
        }
    }
    return status;
}
int32 MANAGER_User_Receive_Process_TC() {
    status = CFE_SUCCESS;

    // Get buffer data
    byte *data = CFE_SB_GetUserData(&MANAGER_APP_Data.Buffer->Msg);

    // Declare bit stream
    BitStream bitstream;

    // Initialize bitstream
    BitStream_AttachBuffer(&bitstream, data, asn1SccMyInteger_REQUIRED_BITS_FOR_ENCODING);

    // Declare errorCode
    int errorCode;

    // Decode data
    if (!asn1SccMyInteger_Decode(&MANAGER_APP_Data.IN_processtc, &bitstream, &errorCode))
    {
        CFE_ES_WriteToSysLog("[MANAGER] Decode failed. Error code: %d", errorCode);
        status = CFE_SB_BAD_ARGUMENT;

    } else
    {
        // Message received stored in MANAGER_APP_Data.IN_processtc
        // TODO handle on receive
        // CFE_ES_WriteToSysLog("[MANAGER] Message received: %s", MANAGER_APP_Data.IN_processtc);

    }
    return status;
}





int32 MANAGER_User_TM_Error_Event(void) {
    status = CFE_EVS_SendEvent(MANAGER_TM_ERROR_EID, CFE_EVS_EventType_ERROR, "[MANAGER-ERROR] Error");
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[MANAGER] Error sending TM_Error event, RC = 0x%08lX\n",
                             (unsigned long)status);
    }
    return status;
}


int32 MANAGER_User_TM_Mode_Event(void) {
    status = CFE_EVS_SendEvent(MANAGER_TM_MODE_EID, CFE_EVS_EventType_INFORMATION, "[MANAGER-INFORMATION] Current mode");
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[MANAGER] Error sending TM_Mode event, RC = 0x%08lX\n",
                             (unsigned long)status);
    }
    return status;
}

/*
** Helper functions, user shouldn't need to change them
*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/* MANAGER_APP_Main() -- Application entry point and main process loop         */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void MANAGER_APP_Main(void)
{

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(MANAGER_PERF_ID);

    /*
    ** Perform application specific initialization
    ** If the Initialization fails, set the RunStatus to
    ** CFE_ES_RunStatus_APP_ERROR and the App will not enter the RunLoop
    */
    status = MANAGER_APP_Init();
    if (status != CFE_SUCCESS)
    {
        MANAGER_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    // Main app loop
    while (CFE_ES_RunLoop(&MANAGER_APP_Data.RunStatus) == true)
    {

        /* Pend on receipt of command packet */
        status = CFE_SB_ReceiveBuffer(&MANAGER_APP_Data.Buffer, MANAGER_APP_Data.CommandPipe, CFE_SB_PEND_FOREVER);

        if (status == CFE_SUCCESS)
        {
            MANAGER_ProcessCommandPacket(MANAGER_APP_Data.Buffer);
        } else
        {
            CFE_EVS_SendEvent(MANAGER_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "MANAGER APP: SB Pipe Read Error, App Will Exit");

            MANAGER_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }


    }

    // Restart application on error
    CFE_ES_RestartApp(MANAGER_APP_Data.AppId);


    // Delete pipe to free resources
    status = CFE_SB_DeletePipe (MANAGER_APP_Data.CommandPipe);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[MANAGER] Error Deleting the pipe, RC = 0x%08lX\n",
                             (unsigned long)status);
    }


    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(MANAGER_PERF_ID);

    CFE_ES_ExitApp(MANAGER_APP_Data.RunStatus);

} /* End of MANAGER_APP_Main() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* MANAGER_APP_Init() --  initialization                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 MANAGER_APP_Init(void)
{
    int32 status = CFE_SUCCESS; // Initialize status to success;

    MANAGER_APP_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Initialize app command execution counters
    */
    MANAGER_APP_Data.CmdCounter = 0;
    MANAGER_APP_Data.ErrCounter = 0;

    /*
    ** Initialize app configuration data
    */
    MANAGER_APP_Data.PipeDepth = MANAGER_PIPE_DEPTH;

    (void)strncpy(MANAGER_APP_Data.PipeName, "MANAGER_CMD_PIPE", sizeof(MANAGER_APP_Data.PipeName));
    MANAGER_APP_Data.PipeName[sizeof(MANAGER_APP_Data.PipeName) - 1U] = 0;

    /*
    ** Initialize event filter table...
    */
    MANAGER_APP_Data.EventFilters[0].EventID = MANAGER_STARTUP_INF_EID;
    MANAGER_APP_Data.EventFilters[0].Mask    = 0x0000;
    MANAGER_APP_Data.EventFilters[1].EventID = MANAGER_COMMAND_ERR_EID;
    MANAGER_APP_Data.EventFilters[1].Mask    = 0x0000;
    MANAGER_APP_Data.EventFilters[2].EventID = MANAGER_COMMANDNOP_INF_EID;
    MANAGER_APP_Data.EventFilters[2].Mask    = 0x0000;
    MANAGER_APP_Data.EventFilters[3].EventID = MANAGER_COMMANDRST_INF_EID;
    MANAGER_APP_Data.EventFilters[3].Mask    = 0x0000;
    MANAGER_APP_Data.EventFilters[4].EventID = MANAGER_INVALID_MSGID_ERR_EID;
    MANAGER_APP_Data.EventFilters[4].Mask    = 0x0000;
    MANAGER_APP_Data.EventFilters[5].EventID = MANAGER_LEN_ERR_EID;
    MANAGER_APP_Data.EventFilters[5].Mask    = 0x0000;
    MANAGER_APP_Data.EventFilters[6].EventID = MANAGER_PIPE_ERR_EID;
    MANAGER_APP_Data.EventFilters[6].Mask    = 0x0000;
    // Event_Name Event_Info  Event_Type Event_ID
// TM_Error, Error, ERROR, 8
MANAGER_APP_Data.EventFilters[8].EventID = MANAGER_TM_ERROR_EID;
    MANAGER_APP_Data.EventFilters[8].Mask    = 0x0000;


// Event_Name Event_Info  Event_Type Event_ID
// TM_Mode, Current mode, INFORMATION, 7
MANAGER_APP_Data.EventFilters[7].EventID = MANAGER_TM_MODE_EID;
    MANAGER_APP_Data.EventFilters[7].Mask    = 0x0000;

    /* Register the events */
    status = CFE_EVS_Register(MANAGER_APP_Data.EventFilters, MANAGER_EVENT_COUNTS, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[MANAGER] Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
    }


    /*
    ** Initialize housekeeping packet (clear user data area).
    */
    if (status == CFE_SUCCESS)
    {
        status = CFE_MSG_Init(&MANAGER_APP_Data.HkTlm.TlmHeader.Msg, CFE_SB_ValueToMsgId(MANAGER_HK_TLM_MID),
                 sizeof(MANAGER_APP_Data.HkTlm));

        if (status != CFE_SUCCESS)
        {
        CFE_ES_WriteToSysLog("[MANAGER] Error initializing housekeeping packets, RC = 0x%08lX\n", (unsigned long)status);
        }
    }

    /*
    ** Create Software Bus message pipe.
    */
    if (status == CFE_SUCCESS)
    {
        status = CFE_SB_CreatePipe(&MANAGER_APP_Data.CommandPipe, MANAGER_APP_Data.PipeDepth, MANAGER_APP_Data.PipeName);
        if (status != CFE_SUCCESS)
        {
        CFE_ES_WriteToSysLog("[MANAGER] Error creating pipe, RC = 0x%08lX\n", (unsigned long)status);
        }
    }


    /*
    ** Subscribe to Housekeeping request commands
    */
    if (status == CFE_SUCCESS)
    {
        status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(MANAGER_SEND_HK_MID), MANAGER_APP_Data.CommandPipe);
        if (status != CFE_SUCCESS)
        {
        CFE_ES_WriteToSysLog("[MANAGER] Error Subscribing to HK request, RC = 0x%08lX\n", (unsigned long)status);
        }
    }

    /*
    ** Subscribe to ground command packets
    */
    if (status == CFE_SUCCESS)
    {
        status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(MANAGER_CMD_MID), MANAGER_APP_Data.CommandPipe);
        if (status != CFE_SUCCESS)
        {
        CFE_ES_WriteToSysLog("[MANAGER] Error Subscribing to Command, RC = 0x%08lX\n", (unsigned long) status);
        }
    }



    if (status == CFE_SUCCESS)
    {
    CFE_EVS_SendEvent(MANAGER_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION, "MANAGER App Initialized.%s",
                      MANAGER_VERSION_STRING);
    }

    // Get App Id
    MANAGER_FindAppId();



    MANAGER_Send_Current_Mode_Init();
    if (status == CFE_SUCCESS)
{
    MANAGER_Receive_Process_TC_Init();
}



    // Call user init
    MANAGER_User_Init();

    return status;

} /* End of MANAGER_Init() */

int32 MANAGER_Send_Current_Mode_Init(void)
{
    // Initialize message Current_Mode
    int32 status = CFE_SUCCESS; // Initialize status to success;
    MANAGER_APP_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;
    MANAGER_APP_Data.MsgId_Current_Mode = MANAGER_APP_COMMUNICATION_MID_16;
    MANAGER_APP_Data.Size_Current_Mode = asn1SccMyInteger_REQUIRED_BITS_FOR_ENCODING;

    status = CFE_MSG_Init(&MANAGER_APP_Data.MsgPtr_Current_Mode, MANAGER_APP_Data.MsgId_Current_Mode, MANAGER_APP_Data.Size_Current_Mode);

    if (status != CFE_SUCCESS) {
        MANAGER_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        CFE_ES_WriteToSysLog("[MANAGER] Error creating the message, RC = %lu\n", (unsigned long) status);
    }
    return status;
}
int32 MANAGER_Receive_Process_TC_Init(void)
{
    status = CFE_SB_Subscribe(0x017,
                              MANAGER_APP_Data.CommandPipe);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[MANAGER] Error Subscribing to communication pipe, RC = 0x%08lX\n",
                             (unsigned long) status);
    }
    return status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* MANAGER_FindAppId -- Get App Id and set it in App data   */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 MANAGER_FindAppId(void)
{
    status = CFE_ES_ERR_NAME_NOT_FOUND;
    CFE_ES_AppId_t TempAppId;
    /*
    ** Search the ES Application table for the name that matches this app
    */
    for (int i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++ )
    {

        if (!strcmp(CFE_ES_Global.AppTable[i].AppName, MANAGER_APP_NAME))
        {
            status = CFE_ES_GetAppIDByName(&TempAppId, CFE_ES_Global.AppTable[i].AppName);
            if (status == CFE_SUCCESS)
            {
                MANAGER_APP_Data.AppId = TempAppId;
            }
            else
            {
                CFE_ES_WriteToSysLog("[MANAGER] Error getting the App ID, RC = %lu\n", (unsigned long)status);
            }
            break;
        }

    } /* end for */
    return status;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  MANAGER_ProcessCommandPacket                                    */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the MANAGER    */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void MANAGER_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);

    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        case MANAGER_CMD_MID:
            MANAGER_ProcessGroundCommand(SBBufPtr);
            break;

        case MANAGER_SEND_HK_MID:
            MANAGER_ReportHousekeeping((CFE_MSG_CommandHeader_t *)SBBufPtr);
            // Call user loop
            MANAGER_User_Main();
            break;

        // Process Process_TC
        case 0x017:
            MANAGER_User_Receive_Process_TC();

    break;

        default:
            CFE_EVS_SendEvent(MANAGER_INVALID_MSGID_ERR_EID, CFE_EVS_EventType_ERROR,
                              "MANAGER: invalid command packet,MID = 0x%x", (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;
    }

    return;

} /* End MANAGER_ProcessCommandPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* MANAGER_ProcessGroundCommand() -- MANAGER ground commands                */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void MANAGER_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_MSG_FcnCode_t CommandCode = 0;

    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);

    /*
    ** Process "known" MANAGER app ground commands
    */
    switch (CommandCode)
    {
        case MANAGER_NOOP_CC:
            if (MANAGER_VerifyCmdLength(&SBBufPtr->Msg, sizeof(MANAGER_NoopCmd_t)))
            {
                MANAGER_Noop((MANAGER_NoopCmd_t *)SBBufPtr);
            } else
            {
                CFE_ES_WriteToSysLog("[MANAGER] Invalid command length\n");
            }

            break;

        case MANAGER_RESET_COUNTERS_CC:
            if (MANAGER_VerifyCmdLength(&SBBufPtr->Msg, sizeof(MANAGER_ResetCountersCmd_t)))
            {
                MANAGER_ResetCounters((MANAGER_ResetCountersCmd_t *)SBBufPtr);
            } else
            {
                CFE_ES_WriteToSysLog("[MANAGER] Invalid command length\n");
            }

            break;


        /* default case already found during FC vs length test */
        default:
            CFE_EVS_SendEvent(MANAGER_COMMAND_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Invalid ground command code: CC = %d", CommandCode);
            break;
    }

    return;

} /* End of MANAGER_ProcessGroundCommand() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  MANAGER_ReportHousekeeping                                          */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 MANAGER_ReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg)
{
    (void)Msg;

    /*
    ** Get command execution counters...
    */
    MANAGER_APP_Data.HkTlm.Payload.CommandErrorCounter = MANAGER_APP_Data.ErrCounter;
    MANAGER_APP_Data.HkTlm.Payload.CommandCounter      = MANAGER_APP_Data.CmdCounter;

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg(&MANAGER_APP_Data.HkTlm.TlmHeader.Msg);
    CFE_SB_TransmitMsg(&MANAGER_APP_Data.HkTlm.TlmHeader.Msg, true);


    return CFE_SUCCESS;

} /* End of MANAGER_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* MANAGER_Noop -- MANAGER NOOP commands                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 MANAGER_Noop(const MANAGER_NoopCmd_t *Msg)
{
    (void)Msg;

    MANAGER_APP_Data.CmdCounter++;

    CFE_EVS_SendEvent(MANAGER_COMMANDNOP_INF_EID, CFE_EVS_EventType_INFORMATION, "MANAGER: NOOP command %s",
                      MANAGER_VERSION);

    return CFE_SUCCESS;

} /* End of MANAGER_Noop */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  MANAGER_ResetCounters                                               */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 MANAGER_ResetCounters(const MANAGER_ResetCountersCmd_t *Msg)
{
    (void)Msg;

    MANAGER_APP_Data.CmdCounter = 0;
    MANAGER_APP_Data.ErrCounter = 0;

    CFE_EVS_SendEvent(MANAGER_COMMANDRST_INF_EID, CFE_EVS_EventType_INFORMATION, "MANAGER: RESET command");

    return CFE_SUCCESS;

} /* End of MANAGER_ResetCounters() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* MANAGER_VerifyCmdLength() -- Verify command packet length                   */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
bool MANAGER_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
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

        CFE_EVS_SendEvent(MANAGER_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid Msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)FcnCode, (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);

        result = false;

        MANAGER_APP_Data.ErrCounter++;
    }

    return (result);

} /* End of MANAGER_VerifyCmdLength() */


