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
** File: storage.c
**
** Purpose:
**   This file contains the source code for the Sample App.
**
*******************************************************************************/

/*
** Include Files:
*/
#include "storage_app_version.h"
#include "storage_app.h"
#include "storage_app_user.h"
#include "dataview_init.h"


/* The sample_lib module provides the SAMPLE_LIB_Function() prototype */
#include <string.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/* STORAGE_AppMain() -- Application entry point and main process loop         */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void STORAGE_APP_Main(void)
{

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(STORAGE_PERF_ID);

    /*
    ** Perform application specific initialization
    ** If the Initialization fails, set the RunStatus to
    ** CFE_ES_RunStatus_APP_ERROR and the App will not enter the RunLoop
    */
    status = STORAGE_Init();
    if (status != CFE_SUCCESS)
    {
        STORAGE_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }


    // ---------   Send messages  ------------
    // uint32 period =  1000; // ms



    // uint32 period =  1000; // ms
    // --------- Receive messages ------------
    // Initialize pipe Get_In
    // Subscribe to Get_In     0x021 msg id
    status = CFE_SB_Subscribe(0x021,
                              STORAGE_Data.CommandPipe);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[STORAGE] Error Subscribing to communication pipe, RC = 0x%08lX\n",
                             (unsigned long) status);
    }



    // Initialize pipe Sensor_data
    // Subscribe to Sensor_data     0x010 msg id
    status = CFE_SB_Subscribe(0x010,
                              STORAGE_Data.CommandPipe);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[STORAGE] Error Subscribing to communication pipe, RC = 0x%08lX\n",
                             (unsigned long) status);
    }

    // Main app loop
    while (CFE_ES_RunLoop(&STORAGE_Data.RunStatus) == true)
    {
        // Call user loop
        STORAGE_User_Main();
    }



    // Delete pipe to free resources
    status = CFE_SB_DeletePipe (STORAGE_Data.CommandPipe);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[STORAGE] Error Deleting the pipe, RC = 0x%08lX\n",
                             (unsigned long)status);
    }


    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(STORAGE_PERF_ID);

    STORAGE_Data.RunStatus = CFE_ES_RunStatus_APP_EXIT;

    CFE_ES_ExitApp(STORAGE_Data.RunStatus);

} /* End of STORAGE_APP_Main() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* STORAGE_Init() --  initialization                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 STORAGE_Init(void)
{
    int32 status;

    STORAGE_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Initialize app command execution counters
    */
    STORAGE_Data.CmdCounter = 0;
    STORAGE_Data.ErrCounter = 0;

    /*
    ** Initialize app configuration data
    */
    STORAGE_Data.PipeDepth = STORAGE_PIPE_DEPTH;

    strncpy(STORAGE_Data.PipeName, "STORAGE_CMD_PIPE", sizeof(STORAGE_Data.PipeName));
    STORAGE_Data.PipeName[sizeof(STORAGE_Data.PipeName) - 1] = 0;

    /*
    ** Initialize event filter table...
    */
    STORAGE_Data.EventFilters[0].EventID = STORAGE_STARTUP_INF_EID;
    STORAGE_Data.EventFilters[0].Mask    = 0x0000;
    STORAGE_Data.EventFilters[1].EventID = STORAGE_COMMAND_ERR_EID;
    STORAGE_Data.EventFilters[1].Mask    = 0x0000;
    STORAGE_Data.EventFilters[2].EventID = STORAGE_COMMANDNOP_INF_EID;
    STORAGE_Data.EventFilters[2].Mask    = 0x0000;
    STORAGE_Data.EventFilters[3].EventID = STORAGE_COMMANDRST_INF_EID;
    STORAGE_Data.EventFilters[3].Mask    = 0x0000;
    STORAGE_Data.EventFilters[4].EventID = STORAGE_INVALID_MSGID_ERR_EID;
    STORAGE_Data.EventFilters[4].Mask    = 0x0000;
    STORAGE_Data.EventFilters[5].EventID = STORAGE_LEN_ERR_EID;
    STORAGE_Data.EventFilters[5].Mask    = 0x0000;
    STORAGE_Data.EventFilters[6].EventID = STORAGE_PIPE_ERR_EID;
    STORAGE_Data.EventFilters[6].Mask    = 0x0000;
    

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(STORAGE_Data.EventFilters, STORAGE_EVENT_COUNTS, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[STORAGE] Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Initialize housekeeping packet (clear user data area).
    */
    CFE_MSG_Init(&STORAGE_Data.HkTlm.TlmHeader.Msg, CFE_SB_ValueToMsgId(STORAGE_HK_TLM_MID),
                 sizeof(STORAGE_Data.HkTlm));

    /*
    ** Create Software Bus message pipe.
    */
    status = CFE_SB_CreatePipe(&STORAGE_Data.CommandPipe, STORAGE_Data.PipeDepth, STORAGE_Data.PipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[STORAGE] Error creating pipe, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Subscribe to Housekeeping request commands
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(STORAGE_SEND_HK_MID), STORAGE_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[STORAGE] Error Subscribing to HK request, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Subscribe to ground command packets
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(STORAGE_CMD_MID), STORAGE_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[STORAGE] Error Subscribing to Command, RC = 0x%08lX\n", (unsigned long) status);

        return (status);
    }


    CFE_EVS_SendEvent(STORAGE_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION, "STORAGE App Initialized.%s",
                      STORAGE_VERSION_STRING);

    /*
    ** Create Software Bus message pipe.
    */
    STORAGE_Data.PipeDepth = STORAGE_PIPE_DEPTH;

    strcpy(STORAGE_Data.PipeName, "STORAGE_COM_PIPE");
    CFE_ES_WriteToSysLog("[STORAGE] COM_PIPE Name = %s", STORAGE_Data.PipeName);

    status = CFE_SB_CreatePipe(&STORAGE_Data.CommandPipe, STORAGE_Data.PipeDepth, STORAGE_Data.PipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[STORAGE] Error creating communication pipe,210, STORAGE_app.c, RC = 0x%08lX\n",
                             (unsigned long)status);
    }
    CFE_ES_WriteToSysLog("[STORAGE] Pipe creation successful");

    // Get App Id
    STORAGE_FindAppId();

    // Call user init
    STORAGE_User_Init();

    return (CFE_SUCCESS);

} /* End of STORAGE_Init() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  STORAGE_ProcessCommandPacket                                    */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the STORAGE    */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void STORAGE_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);

    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        case STORAGE_CMD_MID:
            STORAGE_ProcessGroundCommand(SBBufPtr);
            break;

        case STORAGE_SEND_HK_MID:
            STORAGE_ReportHousekeeping((CFE_MSG_CommandHeader_t *)SBBufPtr);
            break;

        default:
            CFE_EVS_SendEvent(STORAGE_INVALID_MSGID_ERR_EID, CFE_EVS_EventType_ERROR,
                              "STORAGE: invalid command packet,MID = 0x%x", (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;
    }

    return;

} /* End STORAGE_ProcessCommandPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* STORAGE_ProcessGroundCommand() -- STORAGE ground commands                */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void STORAGE_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_MSG_FcnCode_t CommandCode = 0;

    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);

    /*
    ** Process "known" STORAGE app ground commands
    */
    switch (CommandCode)
    {
        case STORAGE_NOOP_CC:
            if (STORAGE_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STORAGE_NoopCmd_t)))
            {
                STORAGE_Noop((STORAGE_NoopCmd_t *)SBBufPtr);
            }

            break;

        case STORAGE_RESET_COUNTERS_CC:
            if (STORAGE_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STORAGE_ResetCountersCmd_t)))
            {
                STORAGE_ResetCounters((STORAGE_ResetCountersCmd_t *)SBBufPtr);
            }

            break;


        /* default case already found during FC vs length test */
        default:
            CFE_EVS_SendEvent(STORAGE_COMMAND_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Invalid ground command code: CC = %d", CommandCode);
            break;
    }

    return;

} /* End of STORAGE_ProcessGroundCommand() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  STORAGE_ReportHousekeeping                                          */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 STORAGE_ReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg)
{

    /*
    ** Get command execution counters...
    */
    STORAGE_Data.HkTlm.Payload.CommandErrorCounter = STORAGE_Data.ErrCounter;
    STORAGE_Data.HkTlm.Payload.CommandCounter      = STORAGE_Data.CmdCounter;

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg(&STORAGE_Data.HkTlm.TlmHeader.Msg);
    CFE_SB_TransmitMsg(&STORAGE_Data.HkTlm.TlmHeader.Msg, true);


    return CFE_SUCCESS;

} /* End of STORAGE_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* STORAGE_Noop -- STORAGE NOOP commands                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 STORAGE_Noop(const STORAGE_NoopCmd_t *Msg)
{

    STORAGE_Data.CmdCounter++;

    CFE_EVS_SendEvent(STORAGE_COMMANDNOP_INF_EID, CFE_EVS_EventType_INFORMATION, "STORAGE: NOOP command %s",
                      STORAGE_VERSION);

    return CFE_SUCCESS;

} /* End of STORAGE_Noop */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  STORAGE_ResetCounters                                               */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 STORAGE_ResetCounters(const STORAGE_ResetCountersCmd_t *Msg)
{

    STORAGE_Data.CmdCounter = 0;
    STORAGE_Data.ErrCounter = 0;

    CFE_EVS_SendEvent(STORAGE_COMMANDRST_INF_EID, CFE_EVS_EventType_INFORMATION, "STORAGE: RESET command");

    return CFE_SUCCESS;

} /* End of STORAGE_ResetCounters() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* STORAGE_VerifyCmdLength() -- Verify command packet length                   */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
bool STORAGE_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
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

        CFE_EVS_SendEvent(STORAGE_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid Msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)FcnCode, (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);

        result = false;

        STORAGE_Data.ErrCounter++;
    }

    return (result);

} /* End of STORAGE_VerifyCmdLength() */



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* STORAGE_Datastore_Init -- Initialize datastore           */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 STORAGE_Datastore_Init(void)
{
    int32 status;

    char cDSName[] = "storageCDS";

    // Register CDS
    status = CFE_ES_RegisterCDS(&STORAGE_Data.CDSHandlePtr, STORAGE_DATASTORE_SIZE, cDSName);

    if (status == CFE_ES_CDS_ALREADY_EXISTS)
    {
        CFE_ES_WriteToSysLog("[STORAGE] CDS Already exists");

        // Critical Data Store already exists,
        // get a copy of its current contents and store it in STORAGE_Data.CDSData
        // to see if we can work use it
        status = CFE_ES_RestoreFromCDS(&STORAGE_Data.CDSData, STORAGE_Data.CDSHandlePtr);

        if (status == CFE_SUCCESS)
        {
            return CFE_ES_CDS_ALREADY_EXISTS;
        }

        status = CFE_SUCCESS;
    }

    if (status != CFE_SUCCESS) {
        CFE_ES_WriteToSysLog("[STORAGE] Error registering CDS, RC = 0x%08lX\n", (unsigned long) status);
    } else {
        CFE_ES_WriteToSysLog("[STORAGE] Successful CDS creation");
    }

    return status;

} /* End of STORAGE_Datastore_Init() */

int32 STORAGE_Datastore_Store(const void *dataToStore)
{
    int32 status;

    // Copy data to CDS
    status = CFE_ES_CopyToCDS(STORAGE_Data.CDSHandlePtr, dataToStore);

    if (status != CFE_SUCCESS) {
        CFE_ES_WriteToSysLog("[STORAGE] Error copying to CDS, RC = 0x%08lX\n", (unsigned long) status);
    } else {
        CFE_ES_WriteToSysLog("[STORAGE] Successful copy to CDS");
    }

    return status;

} /* End of STORAGE_Datastore_Store() */



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* STORAGE_Send_Get_Out -- Send message         */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void STORAGE_Send_Get_Out() {
    // Initialize message Get_Out
    STORAGE_Data.MsgId_Get_Out = STORAGE_APP_COMMUNICATION_MID_22;
    STORAGE_Data.Size_Get_Out = asn1SccMyInteger_REQUIRED_BITS_FOR_ENCODING;

    status = CFE_MSG_Init(&STORAGE_Data.MsgPtr_Get_Out, STORAGE_Data.MsgId_Get_Out, STORAGE_Data.Size_Get_Out);

    if (status != CFE_SUCCESS) {
        STORAGE_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        CFE_ES_WriteToSysLog("[STORAGE] Error creating the message, RC = %lu\n", (unsigned long) status);
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
    if (!asn1SccMyInteger_Encode(&STORAGE_Data.OUT_pGetOut, &bitstream, &errorCode, TRUE))
    {
        CFE_ES_WriteToSysLog("[STORAGE] Encode failed. Error code %d.\n", errorCode);
    }


    status = CFE_PSP_MemCpy(CFE_SB_GetUserData(&STORAGE_Data.MsgPtr_Get_Out), &perBuffer, sizeof(perBuffer));

    if (status != CFE_SUCCESS) {
        STORAGE_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        CFE_ES_WriteToSysLog("[STORAGE] Error writing the struct, RC = %lu\n", (unsigned long) status);
    }

    status = CFE_SB_TransmitMsg(&STORAGE_Data.MsgPtr_Get_Out, false);

    if (status != CFE_SUCCESS) {
        STORAGE_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        CFE_ES_WriteToSysLog("[STORAGE] Error sending the message, RC = %lu\n", (unsigned long) status);
    } else {
        // CFE_ES_WriteToSysLog("[STORAGE] Message sent successfully");
    }

}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* STORAGE_Send_Last -- Send message         */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void STORAGE_Send_Last() {
    // Initialize message Last
    STORAGE_Data.MsgId_Last = STORAGE_APP_COMMUNICATION_MID_17;
    STORAGE_Data.Size_Last = asn1SccMyInteger_REQUIRED_BITS_FOR_ENCODING;

    status = CFE_MSG_Init(&STORAGE_Data.MsgPtr_Last, STORAGE_Data.MsgId_Last, STORAGE_Data.Size_Last);

    if (status != CFE_SUCCESS) {
        STORAGE_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        CFE_ES_WriteToSysLog("[STORAGE] Error creating the message, RC = %lu\n", (unsigned long) status);
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
    if (!asn1SccMyInteger_Encode(&STORAGE_Data.OUT_pLast, &bitstream, &errorCode, TRUE))
    {
        CFE_ES_WriteToSysLog("[STORAGE] Encode failed. Error code %d.\n", errorCode);
    }


    status = CFE_PSP_MemCpy(CFE_SB_GetUserData(&STORAGE_Data.MsgPtr_Last), &perBuffer, sizeof(perBuffer));

    if (status != CFE_SUCCESS) {
        STORAGE_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        CFE_ES_WriteToSysLog("[STORAGE] Error writing the struct, RC = %lu\n", (unsigned long) status);
    }

    status = CFE_SB_TransmitMsg(&STORAGE_Data.MsgPtr_Last, false);

    if (status != CFE_SUCCESS) {
        STORAGE_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        CFE_ES_WriteToSysLog("[STORAGE] Error sending the message, RC = %lu\n", (unsigned long) status);
    } else {
        // CFE_ES_WriteToSysLog("[STORAGE] Message sent successfully");
    }

}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* STORAGE_Receive_Get_In -- Receive message   */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void STORAGE_Receive_Get_In() {
    // CFE_ES_WriteToSysLog("[STORAGE] Try number %i of receiving message", i);
    status = CFE_SB_ReceiveBuffer(&STORAGE_Data.Buffer,
                           STORAGE_Data.CommandPipe,
                           CFE_SB_PEND_FOREVER);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[STORAGE] Error ReceiveBuffer, RC = 0x%08lX\n",
                             (unsigned long)status);
    } else
    {
        // Get buffer data
        byte *data = CFE_SB_GetUserData(&STORAGE_Data.Buffer->Msg);

        // Declare bit stream
        BitStream bitstream;

        // Initialize bitstream
        BitStream_AttachBuffer(&bitstream, data, asn1SccMyInteger_REQUIRED_BITS_FOR_ENCODING);

        // Declare errorCode
        int errorCode;

        // Decode data
        if (!asn1SccMyInteger_Decode(&STORAGE_Data.IN_pGetIn, &bitstream, &errorCode))
        {
            CFE_ES_WriteToSysLog("[STORAGE] Decode failed. Error code: %d", errorCode);
        }




    }
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* STORAGE_Receive_Sensor_data -- Receive message   */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void STORAGE_Receive_Sensor_data() {
    // CFE_ES_WriteToSysLog("[STORAGE] Try number %i of receiving message", i);
    status = CFE_SB_ReceiveBuffer(&STORAGE_Data.Buffer,
                           STORAGE_Data.CommandPipe,
                           CFE_SB_PEND_FOREVER);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[STORAGE] Error ReceiveBuffer, RC = 0x%08lX\n",
                             (unsigned long)status);
    } else
    {
        // Get buffer data
        byte *data = CFE_SB_GetUserData(&STORAGE_Data.Buffer->Msg);

        // Declare bit stream
        BitStream bitstream;

        // Initialize bitstream
        BitStream_AttachBuffer(&bitstream, data, asn1SccMyInteger_REQUIRED_BITS_FOR_ENCODING);

        // Declare errorCode
        int errorCode;

        // Decode data
        if (!asn1SccMyInteger_Decode(&STORAGE_Data.IN_pSensorData, &bitstream, &errorCode))
        {
            CFE_ES_WriteToSysLog("[STORAGE] Decode failed. Error code: %d", errorCode);
        }




    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* STORAGE_FindAppId -- Get App Id and set it in App data   */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void STORAGE_FindAppId(void)
{
    /*
    ** Search the ES Application table for the name that matches this app
    */
    for (int i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++ )
    {

        if (!strcmp(CFE_ES_Global.AppTable[i].AppName, STORAGE_APP_NAME))
        {
            CFE_ES_GetAppIDByName(&STORAGE_Data.AppId, CFE_ES_Global.AppTable[i].AppName);
        }

    } /* end for */
}



