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
** File: temperature_sensor.c
**
** Purpose:
**   This file contains the source code for the Sample App.
**
*******************************************************************************/

/*
** Include Files:
*/
#include "temperature_sensor_app_version.h"
#include "temperature_sensor_app.h"
#include "dataview_init.h"


#include <string.h>

/* Global instance of TEMPERATURE_SENSOR_APP_Data */
TEMPERATURE_SENSOR_APP_Data_t TEMPERATURE_SENSOR_APP_Data;


void TEMPERATURE_SENSOR_User_Init(void)
{
    // User initialization here


}

void TEMPERATURE_SENSOR_User_Main(void)
{
    // User main loop here

}


// Functions
int32 TEMPERATURE_SENSOR_User_Send_Sensor_data() {
    // TODO Build structs here
    // TEMPERATURE_SENSOR_APP_Data.OUT_sensor;

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
    if (!asn1SccMyInteger_Encode(&TEMPERATURE_SENSOR_APP_Data.OUT_sensor, &bitstream, &errorCode, TRUE))
    {
        CFE_ES_WriteToSysLog("[TEMPERATURE_SENSOR] Encode failed. Error code %d.\n", errorCode);
        status = CFE_SB_BAD_ARGUMENT;
        TEMPERATURE_SENSOR_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    if (status == CFE_SUCCESS)
    {
        status = CFE_PSP_MemCpy(CFE_SB_GetUserData(&TEMPERATURE_SENSOR_APP_Data.MsgPtr_Sensor_data), &perBuffer, sizeof(perBuffer));

        if (status != CFE_SUCCESS) {
            TEMPERATURE_SENSOR_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
            CFE_ES_WriteToSysLog("[TEMPERATURE_SENSOR] Error writing the struct, RC = %lu\n", (unsigned long) status);
        }
    }

    if (status == CFE_SUCCESS)
    {
        status = CFE_SB_TransmitMsg(&TEMPERATURE_SENSOR_APP_Data.MsgPtr_Sensor_data, false);

        if (status != CFE_SUCCESS) {
            TEMPERATURE_SENSOR_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
            CFE_ES_WriteToSysLog("[TEMPERATURE_SENSOR] Error sending the message, RC = %lu\n", (unsigned long) status);
        }
    }
    return status;
}








/*
** Helper functions, user shouldn't need to change them
*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/* TEMPERATURE_SENSOR_APP_Main() -- Application entry point and main process loop         */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void TEMPERATURE_SENSOR_APP_Main(void)
{

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(TEMPERATURE_SENSOR_PERF_ID);

    /*
    ** Perform application specific initialization
    ** If the Initialization fails, set the RunStatus to
    ** CFE_ES_RunStatus_APP_ERROR and the App will not enter the RunLoop
    */
    status = TEMPERATURE_SENSOR_APP_Init();
    if (status != CFE_SUCCESS)
    {
        TEMPERATURE_SENSOR_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    // Main app loop
    while (CFE_ES_RunLoop(&TEMPERATURE_SENSOR_APP_Data.RunStatus) == true)
    {

        /* Pend on receipt of command packet */
        status = CFE_SB_ReceiveBuffer(&TEMPERATURE_SENSOR_APP_Data.Buffer, TEMPERATURE_SENSOR_APP_Data.CommandPipe, CFE_SB_PEND_FOREVER);

        if (status == CFE_SUCCESS)
        {
            TEMPERATURE_SENSOR_ProcessCommandPacket(TEMPERATURE_SENSOR_APP_Data.Buffer);
        } else
        {
            CFE_EVS_SendEvent(TEMPERATURE_SENSOR_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "TEMPERATURE_SENSOR APP: SB Pipe Read Error, App Will Exit");

            TEMPERATURE_SENSOR_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }


    }



    // Delete pipe to free resources
    status = CFE_SB_DeletePipe (TEMPERATURE_SENSOR_APP_Data.CommandPipe);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[TEMPERATURE_SENSOR] Error Deleting the pipe, RC = 0x%08lX\n",
                             (unsigned long)status);
    }


    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(TEMPERATURE_SENSOR_PERF_ID);

    CFE_ES_ExitApp(TEMPERATURE_SENSOR_APP_Data.RunStatus);

} /* End of TEMPERATURE_SENSOR_APP_Main() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* TEMPERATURE_SENSOR_APP_Init() --  initialization                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 TEMPERATURE_SENSOR_APP_Init(void)
{
    int32 status = CFE_SUCCESS; // Initialize status to success;

    TEMPERATURE_SENSOR_APP_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Initialize app command execution counters
    */
    TEMPERATURE_SENSOR_APP_Data.CmdCounter = 0;
    TEMPERATURE_SENSOR_APP_Data.ErrCounter = 0;

    /*
    ** Initialize app configuration data
    */
    TEMPERATURE_SENSOR_APP_Data.PipeDepth = TEMPERATURE_SENSOR_PIPE_DEPTH;

    (void)strncpy(TEMPERATURE_SENSOR_APP_Data.PipeName, "TEMPERATURE_SENSOR_CMD_PIPE", sizeof(TEMPERATURE_SENSOR_APP_Data.PipeName));
    TEMPERATURE_SENSOR_APP_Data.PipeName[sizeof(TEMPERATURE_SENSOR_APP_Data.PipeName) - 1U] = 0;

    /*
    ** Initialize event filter table...
    */
    TEMPERATURE_SENSOR_APP_Data.EventFilters[0].EventID = TEMPERATURE_SENSOR_STARTUP_INF_EID;
    TEMPERATURE_SENSOR_APP_Data.EventFilters[0].Mask    = 0x0000;
    TEMPERATURE_SENSOR_APP_Data.EventFilters[1].EventID = TEMPERATURE_SENSOR_COMMAND_ERR_EID;
    TEMPERATURE_SENSOR_APP_Data.EventFilters[1].Mask    = 0x0000;
    TEMPERATURE_SENSOR_APP_Data.EventFilters[2].EventID = TEMPERATURE_SENSOR_COMMANDNOP_INF_EID;
    TEMPERATURE_SENSOR_APP_Data.EventFilters[2].Mask    = 0x0000;
    TEMPERATURE_SENSOR_APP_Data.EventFilters[3].EventID = TEMPERATURE_SENSOR_COMMANDRST_INF_EID;
    TEMPERATURE_SENSOR_APP_Data.EventFilters[3].Mask    = 0x0000;
    TEMPERATURE_SENSOR_APP_Data.EventFilters[4].EventID = TEMPERATURE_SENSOR_INVALID_MSGID_ERR_EID;
    TEMPERATURE_SENSOR_APP_Data.EventFilters[4].Mask    = 0x0000;
    TEMPERATURE_SENSOR_APP_Data.EventFilters[5].EventID = TEMPERATURE_SENSOR_LEN_ERR_EID;
    TEMPERATURE_SENSOR_APP_Data.EventFilters[5].Mask    = 0x0000;
    TEMPERATURE_SENSOR_APP_Data.EventFilters[6].EventID = TEMPERATURE_SENSOR_PIPE_ERR_EID;
    TEMPERATURE_SENSOR_APP_Data.EventFilters[6].Mask    = 0x0000;
    

    /* Register the events */
    status = CFE_EVS_Register(TEMPERATURE_SENSOR_APP_Data.EventFilters, TEMPERATURE_SENSOR_EVENT_COUNTS, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[TEMPERATURE_SENSOR] Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
    }


    /*
    ** Initialize housekeeping packet (clear user data area).
    */
    if (status == CFE_SUCCESS)
    {
        status = CFE_MSG_Init(&TEMPERATURE_SENSOR_APP_Data.HkTlm.TlmHeader.Msg, CFE_SB_ValueToMsgId(TEMPERATURE_SENSOR_HK_TLM_MID),
                 sizeof(TEMPERATURE_SENSOR_APP_Data.HkTlm));

        if (status != CFE_SUCCESS)
        {
        CFE_ES_WriteToSysLog("[TEMPERATURE_SENSOR] Error initializing housekeeping packets, RC = 0x%08lX\n", (unsigned long)status);
        }
    }

    /*
    ** Create Software Bus message pipe.
    */
    if (status == CFE_SUCCESS)
    {
        status = CFE_SB_CreatePipe(&TEMPERATURE_SENSOR_APP_Data.CommandPipe, TEMPERATURE_SENSOR_APP_Data.PipeDepth, TEMPERATURE_SENSOR_APP_Data.PipeName);
        if (status != CFE_SUCCESS)
        {
        CFE_ES_WriteToSysLog("[TEMPERATURE_SENSOR] Error creating pipe, RC = 0x%08lX\n", (unsigned long)status);
        }
    }


    /*
    ** Subscribe to Housekeeping request commands
    */
    if (status == CFE_SUCCESS)
    {
        status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(TEMPERATURE_SENSOR_SEND_HK_MID), TEMPERATURE_SENSOR_APP_Data.CommandPipe);
        if (status != CFE_SUCCESS)
        {
        CFE_ES_WriteToSysLog("[TEMPERATURE_SENSOR] Error Subscribing to HK request, RC = 0x%08lX\n", (unsigned long)status);
        }
    }

    /*
    ** Subscribe to ground command packets
    */
    if (status == CFE_SUCCESS)
    {
        status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(TEMPERATURE_SENSOR_CMD_MID), TEMPERATURE_SENSOR_APP_Data.CommandPipe);
        if (status != CFE_SUCCESS)
        {
        CFE_ES_WriteToSysLog("[TEMPERATURE_SENSOR] Error Subscribing to Command, RC = 0x%08lX\n", (unsigned long) status);
        }
    }



    if (status == CFE_SUCCESS)
    {
    CFE_EVS_SendEvent(TEMPERATURE_SENSOR_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION, "TEMPERATURE_SENSOR App Initialized.%s",
                      TEMPERATURE_SENSOR_VERSION_STRING);
    }

    // Get App Id
    TEMPERATURE_SENSOR_FindAppId();



    TEMPERATURE_SENSOR_Send_Sensor_data_Init();
    



    // Call user init
    TEMPERATURE_SENSOR_User_Init();

    return status;

} /* End of TEMPERATURE_SENSOR_Init() */

int32 TEMPERATURE_SENSOR_Send_Sensor_data_Init(void)
{
    // Initialize message Sensor_data
    int32 status = CFE_SUCCESS; // Initialize status to success;
    TEMPERATURE_SENSOR_APP_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;
    TEMPERATURE_SENSOR_APP_Data.MsgId_Sensor_data = TEMPERATURE_SENSOR_APP_COMMUNICATION_MID_10;
    TEMPERATURE_SENSOR_APP_Data.Size_Sensor_data = asn1SccMyInteger_REQUIRED_BITS_FOR_ENCODING;

    status = CFE_MSG_Init(&TEMPERATURE_SENSOR_APP_Data.MsgPtr_Sensor_data, TEMPERATURE_SENSOR_APP_Data.MsgId_Sensor_data, TEMPERATURE_SENSOR_APP_Data.Size_Sensor_data);

    if (status != CFE_SUCCESS) {
        TEMPERATURE_SENSOR_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        CFE_ES_WriteToSysLog("[TEMPERATURE_SENSOR] Error creating the message, RC = %lu\n", (unsigned long) status);
    }
    return status;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TEMPERATURE_SENSOR_FindAppId -- Get App Id and set it in App data   */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 TEMPERATURE_SENSOR_FindAppId(void)
{
    status = CFE_ES_ERR_NAME_NOT_FOUND;
    CFE_ES_AppId_t TempAppId;
    /*
    ** Search the ES Application table for the name that matches this app
    */
    for (int i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++ )
    {

        if (!strcmp(CFE_ES_Global.AppTable[i].AppName, TEMPERATURE_SENSOR_APP_NAME))
        {
            status = CFE_ES_GetAppIDByName(&TempAppId, CFE_ES_Global.AppTable[i].AppName);
            if (status == CFE_SUCCESS)
            {
                TEMPERATURE_SENSOR_APP_Data.AppId = TempAppId;
            }
            else
            {
                CFE_ES_WriteToSysLog("[TEMPERATURE_SENSOR] Error getting the App ID, RC = %lu\n", (unsigned long)status);
            }
            break;
        }

    } /* end for */
    return status;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  TEMPERATURE_SENSOR_ProcessCommandPacket                                    */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the TEMPERATURE_SENSOR    */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void TEMPERATURE_SENSOR_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);

    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        case TEMPERATURE_SENSOR_CMD_MID:
            TEMPERATURE_SENSOR_ProcessGroundCommand(SBBufPtr);
            break;

        case TEMPERATURE_SENSOR_SEND_HK_MID:
            TEMPERATURE_SENSOR_ReportHousekeeping((CFE_MSG_CommandHeader_t *)SBBufPtr);
            // Call user loop
            TEMPERATURE_SENSOR_User_Main();
            break;

        

        default:
            CFE_EVS_SendEvent(TEMPERATURE_SENSOR_INVALID_MSGID_ERR_EID, CFE_EVS_EventType_ERROR,
                              "TEMPERATURE_SENSOR: invalid command packet,MID = 0x%x", (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;
    }

    return;

} /* End TEMPERATURE_SENSOR_ProcessCommandPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* TEMPERATURE_SENSOR_ProcessGroundCommand() -- TEMPERATURE_SENSOR ground commands                */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void TEMPERATURE_SENSOR_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_MSG_FcnCode_t CommandCode = 0;

    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);

    /*
    ** Process "known" TEMPERATURE_SENSOR app ground commands
    */
    switch (CommandCode)
    {
        case TEMPERATURE_SENSOR_NOOP_CC:
            if (TEMPERATURE_SENSOR_VerifyCmdLength(&SBBufPtr->Msg, sizeof(TEMPERATURE_SENSOR_NoopCmd_t)))
            {
                TEMPERATURE_SENSOR_Noop((TEMPERATURE_SENSOR_NoopCmd_t *)SBBufPtr);
            } else
            {
                CFE_ES_WriteToSysLog("[TEMPERATURE_SENSOR] Invalid command length\n");
            }

            break;

        case TEMPERATURE_SENSOR_RESET_COUNTERS_CC:
            if (TEMPERATURE_SENSOR_VerifyCmdLength(&SBBufPtr->Msg, sizeof(TEMPERATURE_SENSOR_ResetCountersCmd_t)))
            {
                TEMPERATURE_SENSOR_ResetCounters((TEMPERATURE_SENSOR_ResetCountersCmd_t *)SBBufPtr);
            } else
            {
                CFE_ES_WriteToSysLog("[TEMPERATURE_SENSOR] Invalid command length\n");
            }

            break;


        /* default case already found during FC vs length test */
        default:
            CFE_EVS_SendEvent(TEMPERATURE_SENSOR_COMMAND_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Invalid ground command code: CC = %d", CommandCode);
            break;
    }

    return;

} /* End of TEMPERATURE_SENSOR_ProcessGroundCommand() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  TEMPERATURE_SENSOR_ReportHousekeeping                                          */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 TEMPERATURE_SENSOR_ReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg)
{
    (void)Msg;

    /*
    ** Get command execution counters...
    */
    TEMPERATURE_SENSOR_APP_Data.HkTlm.Payload.CommandErrorCounter = TEMPERATURE_SENSOR_APP_Data.ErrCounter;
    TEMPERATURE_SENSOR_APP_Data.HkTlm.Payload.CommandCounter      = TEMPERATURE_SENSOR_APP_Data.CmdCounter;

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg(&TEMPERATURE_SENSOR_APP_Data.HkTlm.TlmHeader.Msg);
    CFE_SB_TransmitMsg(&TEMPERATURE_SENSOR_APP_Data.HkTlm.TlmHeader.Msg, true);


    return CFE_SUCCESS;

} /* End of TEMPERATURE_SENSOR_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* TEMPERATURE_SENSOR_Noop -- TEMPERATURE_SENSOR NOOP commands                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 TEMPERATURE_SENSOR_Noop(const TEMPERATURE_SENSOR_NoopCmd_t *Msg)
{
    (void)Msg;

    TEMPERATURE_SENSOR_APP_Data.CmdCounter++;

    CFE_EVS_SendEvent(TEMPERATURE_SENSOR_COMMANDNOP_INF_EID, CFE_EVS_EventType_INFORMATION, "TEMPERATURE_SENSOR: NOOP command %s",
                      TEMPERATURE_SENSOR_VERSION);

    return CFE_SUCCESS;

} /* End of TEMPERATURE_SENSOR_Noop */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  TEMPERATURE_SENSOR_ResetCounters                                               */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 TEMPERATURE_SENSOR_ResetCounters(const TEMPERATURE_SENSOR_ResetCountersCmd_t *Msg)
{
    (void)Msg;

    TEMPERATURE_SENSOR_APP_Data.CmdCounter = 0;
    TEMPERATURE_SENSOR_APP_Data.ErrCounter = 0;

    CFE_EVS_SendEvent(TEMPERATURE_SENSOR_COMMANDRST_INF_EID, CFE_EVS_EventType_INFORMATION, "TEMPERATURE_SENSOR: RESET command");

    return CFE_SUCCESS;

} /* End of TEMPERATURE_SENSOR_ResetCounters() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* TEMPERATURE_SENSOR_VerifyCmdLength() -- Verify command packet length                   */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
bool TEMPERATURE_SENSOR_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
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

        CFE_EVS_SendEvent(TEMPERATURE_SENSOR_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid Msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)FcnCode, (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);

        result = false;

        TEMPERATURE_SENSOR_APP_Data.ErrCounter++;
    }

    return (result);

} /* End of TEMPERATURE_SENSOR_VerifyCmdLength() */


