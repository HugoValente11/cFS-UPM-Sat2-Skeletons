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
** File: platform.c
**
** Purpose:
**   This file contains the source code for the Sample App.
**
*******************************************************************************/

/*
** Include Files:
*/
#include "platform_app_version.h"
#include "platform_app.h"
#include "dataview_init.h"


#include <string.h>

/* Global instance of PLATFORM_APP_Data */
PLATFORM_APP_Data_t PLATFORM_APP_Data;


void PLATFORM_User_Init(void)
{
    // User initialization here


}

void PLATFORM_User_Main(void)
{
    // User main loop here

}


// Functions

int32 PLATFORM_User_Receive_Sensor_data() {
    status = CFE_SUCCESS;

    // Get buffer data
    byte *data = CFE_SB_GetUserData(&PLATFORM_APP_Data.Buffer->Msg);

    // Declare bit stream
    BitStream bitstream;

    // Initialize bitstream
    BitStream_AttachBuffer(&bitstream, data, asn1SccMyInteger_REQUIRED_BITS_FOR_ENCODING);

    // Declare errorCode
    int errorCode;

    // Decode data
    if (!asn1SccMyInteger_Decode(&PLATFORM_APP_Data.IN_sensor, &bitstream, &errorCode))
    {
        CFE_ES_WriteToSysLog("[PLATFORM] Decode failed. Error code: %d", errorCode);
        status = CFE_SB_BAD_ARGUMENT;

    } else
    {
        // Message received stored in PLATFORM_APP_Data.IN_sensor
        // TODO handle on receive
        // CFE_ES_WriteToSysLog("[PLATFORM] Message received: %s", PLATFORM_APP_Data.IN_sensor);

    }
    return status;
}



int32 PLATFORM_User_Start_Pressure_sensorApp(void)
{
    // Build token list with startup params for Pressure_sensor app
    const char * TokenList[] = {"CFE_APP", // Object type
    "pressure_sensor_app", // Path/Filename
    "PRESSURE_SENSOR_APP_Main", // Entry point
    "PRESSURE_SENSOR_APP", // CFE_Name
    "100", // Priority
    "16384", // Stack size
    "0x0", // Load address
    "0" }; // Exception action, 0 - Just restart app, != 0 - cFE processor reset

    // Load app
    status = CFE_ES_ParseFileEntry(TokenList, 8);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[PLATFORM] Error adding the App, RC = %lu\n", (unsigned long)status);

    }
    return status;
}

int32 PLATFORM_User_Stop_Pressure_sensorApp(void)
{
    // Returns Pressure_sensor app id
    uint32 appId = PLATFORM_Find_Pressure_sensorAppId();

    // Stops application
    status = CFE_ES_DeleteApp(appId);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[PLATFORM] Error deleting the App, RC = %lu\n", (unsigned long)status);

    }
    return status;
}

int32 PLATFORM_Find_Pressure_sensorAppId(void)
{
    int32 appId = CFE_ES_APPID_UNDEFINED;
    CFE_ES_AppId_t TempAppId;
    /*
    ** Search the ES Application table for the name that matches PRESSURE_SENSOR_APP
    */
    for (int i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++ )
    {

        if (!strcmp(CFE_ES_Global.AppTable[i].AppName, "PRESSURE_SENSOR_APP"))
        {
            status = CFE_ES_GetAppIDByName(&TempAppId, CFE_ES_Global.AppTable[i].AppName);
            if (status == CFE_SUCCESS)
            {
                appId = TempAppId;
            }
            else
            {
                CFE_ES_WriteToSysLog("[PLATFORM] Error getting the App ID of PRESSURE_SENSOR_APP, RC = %lu\n", (unsigned long)status);
            }
            break;
        }

    } /* end for */

    return appId;

}




int32 PLATFORM_User_Start_Temperature_sensorApp(void)
{
    // Build token list with startup params for Temperature_sensor app
    const char * TokenList[] = {"CFE_APP", // Object type
    "temperature_sensor_app", // Path/Filename
    "TEMPERATURE_SENSOR_APP_Main", // Entry point
    "TEMPERATURE_SENSOR_APP", // CFE_Name
    "100", // Priority
    "16384", // Stack size
    "0x0", // Load address
    "0" }; // Exception action, 0 - Just restart app, != 0 - cFE processor reset

    // Load app
    status = CFE_ES_ParseFileEntry(TokenList, 8);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[PLATFORM] Error adding the App, RC = %lu\n", (unsigned long)status);

    }
    return status;
}

int32 PLATFORM_User_Stop_Temperature_sensorApp(void)
{
    // Returns Temperature_sensor app id
    uint32 appId = PLATFORM_Find_Temperature_sensorAppId();

    // Stops application
    status = CFE_ES_DeleteApp(appId);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[PLATFORM] Error deleting the App, RC = %lu\n", (unsigned long)status);

    }
    return status;
}

int32 PLATFORM_Find_Temperature_sensorAppId(void)
{
    int32 appId = CFE_ES_APPID_UNDEFINED;
    CFE_ES_AppId_t TempAppId;
    /*
    ** Search the ES Application table for the name that matches TEMPERATURE_SENSOR_APP
    */
    for (int i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++ )
    {

        if (!strcmp(CFE_ES_Global.AppTable[i].AppName, "TEMPERATURE_SENSOR_APP"))
        {
            status = CFE_ES_GetAppIDByName(&TempAppId, CFE_ES_Global.AppTable[i].AppName);
            if (status == CFE_SUCCESS)
            {
                appId = TempAppId;
            }
            else
            {
                CFE_ES_WriteToSysLog("[PLATFORM] Error getting the App ID of TEMPERATURE_SENSOR_APP, RC = %lu\n", (unsigned long)status);
            }
            break;
        }

    } /* end for */

    return appId;

}



/*
** Helper functions, user shouldn't need to change them
*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/* PLATFORM_APP_Main() -- Application entry point and main process loop         */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void PLATFORM_APP_Main(void)
{

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(PLATFORM_PERF_ID);

    /*
    ** Perform application specific initialization
    ** If the Initialization fails, set the RunStatus to
    ** CFE_ES_RunStatus_APP_ERROR and the App will not enter the RunLoop
    */
    status = PLATFORM_APP_Init();
    if (status != CFE_SUCCESS)
    {
        PLATFORM_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    // Main app loop
    while (CFE_ES_RunLoop(&PLATFORM_APP_Data.RunStatus) == true)
    {

        /* Pend on receipt of command packet */
        status = CFE_SB_ReceiveBuffer(&PLATFORM_APP_Data.Buffer, PLATFORM_APP_Data.CommandPipe, CFE_SB_PEND_FOREVER);

        if (status == CFE_SUCCESS)
        {
            PLATFORM_ProcessCommandPacket(PLATFORM_APP_Data.Buffer);
        } else
        {
            CFE_EVS_SendEvent(PLATFORM_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "PLATFORM APP: SB Pipe Read Error, App Will Exit");

            PLATFORM_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }


    }



    // Delete pipe to free resources
    status = CFE_SB_DeletePipe (PLATFORM_APP_Data.CommandPipe);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[PLATFORM] Error Deleting the pipe, RC = 0x%08lX\n",
                             (unsigned long)status);
    }


    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(PLATFORM_PERF_ID);

    CFE_ES_ExitApp(PLATFORM_APP_Data.RunStatus);

} /* End of PLATFORM_APP_Main() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* PLATFORM_APP_Init() --  initialization                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 PLATFORM_APP_Init(void)
{
    int32 status = CFE_SUCCESS; // Initialize status to success;

    PLATFORM_APP_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Initialize app command execution counters
    */
    PLATFORM_APP_Data.CmdCounter = 0;
    PLATFORM_APP_Data.ErrCounter = 0;

    /*
    ** Initialize app configuration data
    */
    PLATFORM_APP_Data.PipeDepth = PLATFORM_PIPE_DEPTH;

    (void)strncpy(PLATFORM_APP_Data.PipeName, "PLATFORM_CMD_PIPE", sizeof(PLATFORM_APP_Data.PipeName));
    PLATFORM_APP_Data.PipeName[sizeof(PLATFORM_APP_Data.PipeName) - 1U] = 0;

    /*
    ** Initialize event filter table...
    */
    PLATFORM_APP_Data.EventFilters[0].EventID = PLATFORM_STARTUP_INF_EID;
    PLATFORM_APP_Data.EventFilters[0].Mask    = 0x0000;
    PLATFORM_APP_Data.EventFilters[1].EventID = PLATFORM_COMMAND_ERR_EID;
    PLATFORM_APP_Data.EventFilters[1].Mask    = 0x0000;
    PLATFORM_APP_Data.EventFilters[2].EventID = PLATFORM_COMMANDNOP_INF_EID;
    PLATFORM_APP_Data.EventFilters[2].Mask    = 0x0000;
    PLATFORM_APP_Data.EventFilters[3].EventID = PLATFORM_COMMANDRST_INF_EID;
    PLATFORM_APP_Data.EventFilters[3].Mask    = 0x0000;
    PLATFORM_APP_Data.EventFilters[4].EventID = PLATFORM_INVALID_MSGID_ERR_EID;
    PLATFORM_APP_Data.EventFilters[4].Mask    = 0x0000;
    PLATFORM_APP_Data.EventFilters[5].EventID = PLATFORM_LEN_ERR_EID;
    PLATFORM_APP_Data.EventFilters[5].Mask    = 0x0000;
    PLATFORM_APP_Data.EventFilters[6].EventID = PLATFORM_PIPE_ERR_EID;
    PLATFORM_APP_Data.EventFilters[6].Mask    = 0x0000;
    

    /* Register the events */
    status = CFE_EVS_Register(PLATFORM_APP_Data.EventFilters, PLATFORM_EVENT_COUNTS, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[PLATFORM] Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
    }


    /*
    ** Initialize housekeeping packet (clear user data area).
    */
    if (status == CFE_SUCCESS)
    {
        status = CFE_MSG_Init(&PLATFORM_APP_Data.HkTlm.TlmHeader.Msg, CFE_SB_ValueToMsgId(PLATFORM_HK_TLM_MID),
                 sizeof(PLATFORM_APP_Data.HkTlm));

        if (status != CFE_SUCCESS)
        {
        CFE_ES_WriteToSysLog("[PLATFORM] Error initializing housekeeping packets, RC = 0x%08lX\n", (unsigned long)status);
        }
    }

    /*
    ** Create Software Bus message pipe.
    */
    if (status == CFE_SUCCESS)
    {
        status = CFE_SB_CreatePipe(&PLATFORM_APP_Data.CommandPipe, PLATFORM_APP_Data.PipeDepth, PLATFORM_APP_Data.PipeName);
        if (status != CFE_SUCCESS)
        {
        CFE_ES_WriteToSysLog("[PLATFORM] Error creating pipe, RC = 0x%08lX\n", (unsigned long)status);
        }
    }


    /*
    ** Subscribe to Housekeeping request commands
    */
    if (status == CFE_SUCCESS)
    {
        status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(PLATFORM_SEND_HK_MID), PLATFORM_APP_Data.CommandPipe);
        if (status != CFE_SUCCESS)
        {
        CFE_ES_WriteToSysLog("[PLATFORM] Error Subscribing to HK request, RC = 0x%08lX\n", (unsigned long)status);
        }
    }

    /*
    ** Subscribe to ground command packets
    */
    if (status == CFE_SUCCESS)
    {
        status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(PLATFORM_CMD_MID), PLATFORM_APP_Data.CommandPipe);
        if (status != CFE_SUCCESS)
        {
        CFE_ES_WriteToSysLog("[PLATFORM] Error Subscribing to Command, RC = 0x%08lX\n", (unsigned long) status);
        }
    }



    if (status == CFE_SUCCESS)
    {
    CFE_EVS_SendEvent(PLATFORM_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION, "PLATFORM App Initialized.%s",
                      PLATFORM_VERSION_STRING);
    }

    // Get App Id
    PLATFORM_FindAppId();



    
    if (status == CFE_SUCCESS)
{
    PLATFORM_Receive_Sensor_data_Init();
}



    // Call user init
    PLATFORM_User_Init();

    return status;

} /* End of PLATFORM_Init() */


int32 PLATFORM_Receive_Sensor_data_Init(void)
{
    status = CFE_SB_Subscribe(0x010,
                              PLATFORM_APP_Data.CommandPipe);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[PLATFORM] Error Subscribing to communication pipe, RC = 0x%08lX\n",
                             (unsigned long) status);
    }
    return status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* PLATFORM_FindAppId -- Get App Id and set it in App data   */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 PLATFORM_FindAppId(void)
{
    status = CFE_ES_ERR_NAME_NOT_FOUND;
    CFE_ES_AppId_t TempAppId;
    /*
    ** Search the ES Application table for the name that matches this app
    */
    for (int i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++ )
    {

        if (!strcmp(CFE_ES_Global.AppTable[i].AppName, PLATFORM_APP_NAME))
        {
            status = CFE_ES_GetAppIDByName(&TempAppId, CFE_ES_Global.AppTable[i].AppName);
            if (status == CFE_SUCCESS)
            {
                PLATFORM_APP_Data.AppId = TempAppId;
            }
            else
            {
                CFE_ES_WriteToSysLog("[PLATFORM] Error getting the App ID, RC = %lu\n", (unsigned long)status);
            }
            break;
        }

    } /* end for */
    return status;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  PLATFORM_ProcessCommandPacket                                    */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the PLATFORM    */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void PLATFORM_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);

    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        case PLATFORM_CMD_MID:
            PLATFORM_ProcessGroundCommand(SBBufPtr);
            break;

        case PLATFORM_SEND_HK_MID:
            PLATFORM_ReportHousekeeping((CFE_MSG_CommandHeader_t *)SBBufPtr);
            // Call user loop
            PLATFORM_User_Main();
            break;

        // Process Sensor_data
        case 0x010:
            PLATFORM_User_Receive_Sensor_data();

    break;

        default:
            CFE_EVS_SendEvent(PLATFORM_INVALID_MSGID_ERR_EID, CFE_EVS_EventType_ERROR,
                              "PLATFORM: invalid command packet,MID = 0x%x", (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;
    }

    return;

} /* End PLATFORM_ProcessCommandPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* PLATFORM_ProcessGroundCommand() -- PLATFORM ground commands                */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void PLATFORM_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_MSG_FcnCode_t CommandCode = 0;

    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);

    /*
    ** Process "known" PLATFORM app ground commands
    */
    switch (CommandCode)
    {
        case PLATFORM_NOOP_CC:
            if (PLATFORM_VerifyCmdLength(&SBBufPtr->Msg, sizeof(PLATFORM_NoopCmd_t)))
            {
                PLATFORM_Noop((PLATFORM_NoopCmd_t *)SBBufPtr);
            } else
            {
                CFE_ES_WriteToSysLog("[PLATFORM] Invalid command length\n");
            }

            break;

        case PLATFORM_RESET_COUNTERS_CC:
            if (PLATFORM_VerifyCmdLength(&SBBufPtr->Msg, sizeof(PLATFORM_ResetCountersCmd_t)))
            {
                PLATFORM_ResetCounters((PLATFORM_ResetCountersCmd_t *)SBBufPtr);
            } else
            {
                CFE_ES_WriteToSysLog("[PLATFORM] Invalid command length\n");
            }

            break;


        /* default case already found during FC vs length test */
        default:
            CFE_EVS_SendEvent(PLATFORM_COMMAND_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Invalid ground command code: CC = %d", CommandCode);
            break;
    }

    return;

} /* End of PLATFORM_ProcessGroundCommand() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  PLATFORM_ReportHousekeeping                                          */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 PLATFORM_ReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg)
{
    (void)Msg;

    /*
    ** Get command execution counters...
    */
    PLATFORM_APP_Data.HkTlm.Payload.CommandErrorCounter = PLATFORM_APP_Data.ErrCounter;
    PLATFORM_APP_Data.HkTlm.Payload.CommandCounter      = PLATFORM_APP_Data.CmdCounter;

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg(&PLATFORM_APP_Data.HkTlm.TlmHeader.Msg);
    CFE_SB_TransmitMsg(&PLATFORM_APP_Data.HkTlm.TlmHeader.Msg, true);


    return CFE_SUCCESS;

} /* End of PLATFORM_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* PLATFORM_Noop -- PLATFORM NOOP commands                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 PLATFORM_Noop(const PLATFORM_NoopCmd_t *Msg)
{
    (void)Msg;

    PLATFORM_APP_Data.CmdCounter++;

    CFE_EVS_SendEvent(PLATFORM_COMMANDNOP_INF_EID, CFE_EVS_EventType_INFORMATION, "PLATFORM: NOOP command %s",
                      PLATFORM_VERSION);

    return CFE_SUCCESS;

} /* End of PLATFORM_Noop */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  PLATFORM_ResetCounters                                               */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 PLATFORM_ResetCounters(const PLATFORM_ResetCountersCmd_t *Msg)
{
    (void)Msg;

    PLATFORM_APP_Data.CmdCounter = 0;
    PLATFORM_APP_Data.ErrCounter = 0;

    CFE_EVS_SendEvent(PLATFORM_COMMANDRST_INF_EID, CFE_EVS_EventType_INFORMATION, "PLATFORM: RESET command");

    return CFE_SUCCESS;

} /* End of PLATFORM_ResetCounters() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* PLATFORM_VerifyCmdLength() -- Verify command packet length                   */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
bool PLATFORM_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
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

        CFE_EVS_SendEvent(PLATFORM_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid Msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)FcnCode, (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);

        result = false;

        PLATFORM_APP_Data.ErrCounter++;
    }

    return (result);

} /* End of PLATFORM_VerifyCmdLength() */


