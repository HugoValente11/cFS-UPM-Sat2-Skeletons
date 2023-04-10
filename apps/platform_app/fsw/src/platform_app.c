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
#include "platform_app_user.h"
#include "dataview_init.h"


/* The sample_lib module provides the SAMPLE_LIB_Function() prototype */
#include <string.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/* PLATFORM_AppMain() -- Application entry point and main process loop         */
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
    status = PLATFORM_Init();
    if (status != CFE_SUCCESS)
    {
        PLATFORM_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }


    // ---------   Send messages  ------------
    
    // --------- Receive messages ------------
    // Initialize pipe Sensor_data
    // Subscribe to Sensor_data     0x010 msg id
    status = CFE_SB_Subscribe(0x010,
                              PLATFORM_Data.CommandPipe);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[PLATFORM] Error Subscribing to communication pipe, RC = 0x%08lX\n",
                             (unsigned long) status);
    }

    // Main app loop
    while (CFE_ES_RunLoop(&PLATFORM_Data.RunStatus) == true)
    {
        // Call user loop
        PLATFORM_User_Main();
    }



    // Delete pipe to free resources
    status = CFE_SB_DeletePipe (PLATFORM_Data.CommandPipe);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[PLATFORM] Error Deleting the pipe, RC = 0x%08lX\n",
                             (unsigned long)status);
    }


    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(PLATFORM_PERF_ID);

    PLATFORM_Data.RunStatus = CFE_ES_RunStatus_APP_EXIT;

    CFE_ES_ExitApp(PLATFORM_Data.RunStatus);

} /* End of PLATFORM_APP_Main() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* PLATFORM_Init() --  initialization                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 PLATFORM_Init(void)
{
    int32 status;

    PLATFORM_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Initialize app command execution counters
    */
    PLATFORM_Data.CmdCounter = 0;
    PLATFORM_Data.ErrCounter = 0;

    /*
    ** Initialize app configuration data
    */
    PLATFORM_Data.PipeDepth = PLATFORM_PIPE_DEPTH;

    strncpy(PLATFORM_Data.PipeName, "PLATFORM_CMD_PIPE", sizeof(PLATFORM_Data.PipeName));
    PLATFORM_Data.PipeName[sizeof(PLATFORM_Data.PipeName) - 1] = 0;

    /*
    ** Initialize event filter table...
    */
    PLATFORM_Data.EventFilters[0].EventID = PLATFORM_STARTUP_INF_EID;
    PLATFORM_Data.EventFilters[0].Mask    = 0x0000;
    PLATFORM_Data.EventFilters[1].EventID = PLATFORM_COMMAND_ERR_EID;
    PLATFORM_Data.EventFilters[1].Mask    = 0x0000;
    PLATFORM_Data.EventFilters[2].EventID = PLATFORM_COMMANDNOP_INF_EID;
    PLATFORM_Data.EventFilters[2].Mask    = 0x0000;
    PLATFORM_Data.EventFilters[3].EventID = PLATFORM_COMMANDRST_INF_EID;
    PLATFORM_Data.EventFilters[3].Mask    = 0x0000;
    PLATFORM_Data.EventFilters[4].EventID = PLATFORM_INVALID_MSGID_ERR_EID;
    PLATFORM_Data.EventFilters[4].Mask    = 0x0000;
    PLATFORM_Data.EventFilters[5].EventID = PLATFORM_LEN_ERR_EID;
    PLATFORM_Data.EventFilters[5].Mask    = 0x0000;
    PLATFORM_Data.EventFilters[6].EventID = PLATFORM_PIPE_ERR_EID;
    PLATFORM_Data.EventFilters[6].Mask    = 0x0000;
    

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(PLATFORM_Data.EventFilters, PLATFORM_EVENT_COUNTS, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[PLATFORM] Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Initialize housekeeping packet (clear user data area).
    */
    CFE_MSG_Init(&PLATFORM_Data.HkTlm.TlmHeader.Msg, CFE_SB_ValueToMsgId(PLATFORM_HK_TLM_MID),
                 sizeof(PLATFORM_Data.HkTlm));

    /*
    ** Create Software Bus message pipe.
    */
    status = CFE_SB_CreatePipe(&PLATFORM_Data.CommandPipe, PLATFORM_Data.PipeDepth, PLATFORM_Data.PipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[PLATFORM] Error creating pipe, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Subscribe to Housekeeping request commands
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(PLATFORM_SEND_HK_MID), PLATFORM_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[PLATFORM] Error Subscribing to HK request, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Subscribe to ground command packets
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(PLATFORM_CMD_MID), PLATFORM_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[PLATFORM] Error Subscribing to Command, RC = 0x%08lX\n", (unsigned long) status);

        return (status);
    }


    CFE_EVS_SendEvent(PLATFORM_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION, "PLATFORM App Initialized.%s",
                      PLATFORM_VERSION_STRING);

    /*
    ** Create Software Bus message pipe.
    */
    PLATFORM_Data.PipeDepth = PLATFORM_PIPE_DEPTH;

    strcpy(PLATFORM_Data.PipeName, "PLATFORM_COM_PIPE");
    CFE_ES_WriteToSysLog("[PLATFORM] COM_PIPE Name = %s", PLATFORM_Data.PipeName);

    status = CFE_SB_CreatePipe(&PLATFORM_Data.CommandPipe, PLATFORM_Data.PipeDepth, PLATFORM_Data.PipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[PLATFORM] Error creating communication pipe,210, PLATFORM_app.c, RC = 0x%08lX\n",
                             (unsigned long)status);
    }
    CFE_ES_WriteToSysLog("[PLATFORM] Pipe creation successful");

    // Get App Id
    PLATFORM_FindAppId();

    // Call user init
    PLATFORM_User_Init();

    return (CFE_SUCCESS);

} /* End of PLATFORM_Init() */

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
            }

            break;

        case PLATFORM_RESET_COUNTERS_CC:
            if (PLATFORM_VerifyCmdLength(&SBBufPtr->Msg, sizeof(PLATFORM_ResetCountersCmd_t)))
            {
                PLATFORM_ResetCounters((PLATFORM_ResetCountersCmd_t *)SBBufPtr);
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

    /*
    ** Get command execution counters...
    */
    PLATFORM_Data.HkTlm.Payload.CommandErrorCounter = PLATFORM_Data.ErrCounter;
    PLATFORM_Data.HkTlm.Payload.CommandCounter      = PLATFORM_Data.CmdCounter;

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg(&PLATFORM_Data.HkTlm.TlmHeader.Msg);
    CFE_SB_TransmitMsg(&PLATFORM_Data.HkTlm.TlmHeader.Msg, true);


    return CFE_SUCCESS;

} /* End of PLATFORM_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* PLATFORM_Noop -- PLATFORM NOOP commands                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 PLATFORM_Noop(const PLATFORM_NoopCmd_t *Msg)
{

    PLATFORM_Data.CmdCounter++;

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

    PLATFORM_Data.CmdCounter = 0;
    PLATFORM_Data.ErrCounter = 0;

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

        PLATFORM_Data.ErrCounter++;
    }

    return (result);

} /* End of PLATFORM_VerifyCmdLength() */





/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* PLATFORM_Receive_Sensor_data -- Receive message   */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void PLATFORM_Receive_Sensor_data() {
    // CFE_ES_WriteToSysLog("[PLATFORM] Try number %i of receiving message", i);
    status = CFE_SB_ReceiveBuffer(&PLATFORM_Data.Buffer,
                           PLATFORM_Data.CommandPipe,
                           CFE_SB_PEND_FOREVER);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[PLATFORM] Error ReceiveBuffer, RC = 0x%08lX\n",
                             (unsigned long)status);
    } else
    {
        // Get buffer data
        byte *data = CFE_SB_GetUserData(&PLATFORM_Data.Buffer->Msg);

        // Declare bit stream
        BitStream bitstream;

        // Initialize bitstream
        BitStream_AttachBuffer(&bitstream, data, asn1SccMyInteger_REQUIRED_BITS_FOR_ENCODING);

        // Declare errorCode
        int errorCode;

        // Decode data
        if (!asn1SccMyInteger_Decode(&PLATFORM_Data.IN_pSensorData, &bitstream, &errorCode))
        {
            CFE_ES_WriteToSysLog("[PLATFORM] Decode failed. Error code: %d", errorCode);
        }




    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* PLATFORM_FindAppId -- Get App Id and set it in App data   */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void PLATFORM_FindAppId(void)
{
    /*
    ** Search the ES Application table for the name that matches this app
    */
    for (int i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++ )
    {

        if (!strcmp(CFE_ES_Global.AppTable[i].AppName, PLATFORM_APP_NAME))
        {
            CFE_ES_GetAppIDByName(&PLATFORM_Data.AppId, CFE_ES_Global.AppTable[i].AppName);
        }

    } /* end for */
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* PLATFORM_FindPressure_sensorAppId -- Return Pressure_sensor app Id   */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
uint32 PLATFORM_FindPressure_sensorAppId(void)
{
    uint32 appId = -1;
    /*
    ** Search the ES Application table for the name that matches PRESSURE_SENSOR_APP
    */
    for (int i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++ )
    {

        if (!strcmp(CFE_ES_Global.AppTable[i].AppName, "PRESSURE_SENSOR_APP"))
        {
            CFE_ES_GetAppIDByName(&appId, CFE_ES_Global.AppTable[i].AppName);
            return appId;
        }

    } /* end for */

    return appId;

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* PLATFORM_StartPressure_sensorApp -- Starts Pressure_sensor app   */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void PLATFORM_StartPressure_sensorApp(void)
{

        // Build token list with startup params for Pressure_sensor app
        const char * TokenList[] = {"CFE_APP", // Object type
        "pressure_sensor_app", // Path/Filename
        "PRESSURE_SENSOR_APP_Main", // Entry point
        "PRESSURE_SENSOR_APP", // CFE_Name
        "100", // Priority
        "16384", // Stack size
        "0x0", // Load address
        "0" }; // Exceptiona action, 0 - Just restart app, != 0 - cFE processor reset

        // Load app
        CFE_ES_ParseFileEntry(TokenList, 8);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* PLATFORM_StopPressure_sensorApp -- Stops Pressure_sensor app   */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void PLATFORM_StopPressure_sensorApp(void)
{
        // Returns Pressure_sensor app id
        uint32 appId = PLATFORM_FindPressure_sensorAppId();

        // Stops application
        CFE_ES_DeleteApp(appId);
}






/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* PLATFORM_FindTemperature_sensorAppId -- Return Temperature_sensor app Id   */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
uint32 PLATFORM_FindTemperature_sensorAppId(void)
{
    uint32 appId = -1;
    /*
    ** Search the ES Application table for the name that matches TEMPERATURE_SENSOR_APP
    */
    for (int i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++ )
    {

        if (!strcmp(CFE_ES_Global.AppTable[i].AppName, "TEMPERATURE_SENSOR_APP"))
        {
            CFE_ES_GetAppIDByName(&appId, CFE_ES_Global.AppTable[i].AppName);
            return appId;
        }

    } /* end for */

    return appId;

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* PLATFORM_StartTemperature_sensorApp -- Starts Temperature_sensor app   */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void PLATFORM_StartTemperature_sensorApp(void)
{

        // Build token list with startup params for Temperature_sensor app
        const char * TokenList[] = {"CFE_APP", // Object type
        "temperature_sensor_app", // Path/Filename
        "TEMPERATURE_SENSOR_APP_Main", // Entry point
        "TEMPERATURE_SENSOR_APP", // CFE_Name
        "100", // Priority
        "16384", // Stack size
        "0x0", // Load address
        "0" }; // Exceptiona action, 0 - Just restart app, != 0 - cFE processor reset

        // Load app
        CFE_ES_ParseFileEntry(TokenList, 8);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* PLATFORM_StopTemperature_sensorApp -- Stops Temperature_sensor app   */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void PLATFORM_StopTemperature_sensorApp(void)
{
        // Returns Temperature_sensor app id
        uint32 appId = PLATFORM_FindTemperature_sensorAppId();

        // Stops application
        CFE_ES_DeleteApp(appId);
}
