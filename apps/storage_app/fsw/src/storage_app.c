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
#include "dataview_init.h"


#include <string.h>

/* Global instance of STORAGE_APP_Data */
STORAGE_APP_Data_t STORAGE_APP_Data;


void STORAGE_User_Init(void)
{
    // User initialization here


}

void STORAGE_User_Main(void)
{
    // User main loop here

}


int32 STORAGE_User_Datastore_Init()
{
    // Check if there's data in Datastore,
    // if there is recover the data and store it in STORAGE_APP_Data.CDSData
    int32 status = CFE_SUCCESS;

    char cDSName[] = "storageCDS";

    // Register CDS
    status = CFE_ES_RegisterCDS(&STORAGE_APP_Data.CDSHandlePtr, STORAGE_DATASTORE_SIZE, cDSName);

    if (status == CFE_ES_CDS_ALREADY_EXISTS)
    {
        CFE_ES_WriteToSysLog("[STORAGE] CDS Already exists");

        // Critical Data Store already exists,
        // get a copy of its current contents and store it in STORAGE_APP_Data.CDSData
        // to see if we can work use it
        status = CFE_ES_RestoreFromCDS(&STORAGE_APP_Data.CDSData, STORAGE_APP_Data.CDSHandlePtr);

        if (status != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("[SUB] Error restoring CDS, RC = 0x%08lX\n", (unsigned long) status);
        }
    }

    else if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[STORAGE] Error registering CDS, RC = 0x%08lX\n", (unsigned long) status);
    } else
    {
        // Success. Only here to be MISRA compliant since all else ifs need to terminated by an else
    }

    return status;
}

int32 STORAGE_User_Datastore_Store(const void *dataToStore)
{
    int32 status;

    // Copy data to CDS
    status = CFE_ES_CopyToCDS(STORAGE_APP_Data.CDSHandlePtr, dataToStore);

    if (status != CFE_SUCCESS) {
        CFE_ES_WriteToSysLog("[STORAGE] Error copying to CDS, RC = 0x%08lX\n", (unsigned long) status);
    }

    return status;

} /* End of STORAGE_User_Datastore_Store() */

int32 STORAGE_User_Datastore_Load(uint8 load[STORAGE_DATASTORE_SIZE])
{
    // Read data from CDS
    status = CFE_ES_RestoreFromCDS(load, STORAGE_APP_Data.CDSHandlePtr);
    if (status != CFE_SUCCESS) {
        CFE_ES_WriteToSysLog("[STORAGE] Error restoring from CDS, RC = 0x%08lX\n", (unsigned long) status);
    }
    return status;
}


// Functions
int32 STORAGE_User_Send_Get_Res() {
    // TODO Build structs here
    // STORAGE_APP_Data.OUT_getres;

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
    if (!asn1SccMyInteger_Encode(&STORAGE_APP_Data.OUT_getres, &bitstream, &errorCode, TRUE))
    {
        CFE_ES_WriteToSysLog("[STORAGE] Encode failed. Error code %d.\n", errorCode);
        status = CFE_SB_BAD_ARGUMENT;
        STORAGE_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    if (status == CFE_SUCCESS)
    {
        status = CFE_PSP_MemCpy(CFE_SB_GetUserData(&STORAGE_APP_Data.MsgPtr_Get_Res), &perBuffer, sizeof(perBuffer));

        if (status != CFE_SUCCESS) {
            STORAGE_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
            CFE_ES_WriteToSysLog("[STORAGE] Error writing the struct, RC = %lu\n", (unsigned long) status);
        }
    }

    if (status == CFE_SUCCESS)
    {
        status = CFE_SB_TransmitMsg(&STORAGE_APP_Data.MsgPtr_Get_Res, false);

        if (status != CFE_SUCCESS) {
            STORAGE_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
            CFE_ES_WriteToSysLog("[STORAGE] Error sending the message, RC = %lu\n", (unsigned long) status);
        }
    }
    return status;
}



int32 STORAGE_User_Send_Last() {
    // TODO Build structs here
    // STORAGE_APP_Data.OUT_last;

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
    if (!asn1SccMyInteger_Encode(&STORAGE_APP_Data.OUT_last, &bitstream, &errorCode, TRUE))
    {
        CFE_ES_WriteToSysLog("[STORAGE] Encode failed. Error code %d.\n", errorCode);
        status = CFE_SB_BAD_ARGUMENT;
        STORAGE_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    if (status == CFE_SUCCESS)
    {
        status = CFE_PSP_MemCpy(CFE_SB_GetUserData(&STORAGE_APP_Data.MsgPtr_Last), &perBuffer, sizeof(perBuffer));

        if (status != CFE_SUCCESS) {
            STORAGE_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
            CFE_ES_WriteToSysLog("[STORAGE] Error writing the struct, RC = %lu\n", (unsigned long) status);
        }
    }

    if (status == CFE_SUCCESS)
    {
        status = CFE_SB_TransmitMsg(&STORAGE_APP_Data.MsgPtr_Last, false);

        if (status != CFE_SUCCESS) {
            STORAGE_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
            CFE_ES_WriteToSysLog("[STORAGE] Error sending the message, RC = %lu\n", (unsigned long) status);
        }
    }
    return status;
}
int32 STORAGE_User_Receive_Get_Req() {
    status = CFE_SUCCESS;

    // Get buffer data
    byte *data = CFE_SB_GetUserData(&STORAGE_APP_Data.Buffer->Msg);

    // Declare bit stream
    BitStream bitstream;

    // Initialize bitstream
    BitStream_AttachBuffer(&bitstream, data, asn1SccMyInteger_REQUIRED_BITS_FOR_ENCODING);

    // Declare errorCode
    int errorCode;

    // Decode data
    if (!asn1SccMyInteger_Decode(&STORAGE_APP_Data.IN_getreq, &bitstream, &errorCode))
    {
        CFE_ES_WriteToSysLog("[STORAGE] Decode failed. Error code: %d", errorCode);
        status = CFE_SB_BAD_ARGUMENT;

    } else
    {
        // Message received stored in STORAGE_APP_Data.IN_getreq
        // TODO handle on receive
        // CFE_ES_WriteToSysLog("[STORAGE] Message received: %s", STORAGE_APP_Data.IN_getreq);

    }
    return status;
}



int32 STORAGE_User_Receive_Sensor_data() {
    status = CFE_SUCCESS;

    // Get buffer data
    byte *data = CFE_SB_GetUserData(&STORAGE_APP_Data.Buffer->Msg);

    // Declare bit stream
    BitStream bitstream;

    // Initialize bitstream
    BitStream_AttachBuffer(&bitstream, data, asn1SccMyInteger_REQUIRED_BITS_FOR_ENCODING);

    // Declare errorCode
    int errorCode;

    // Decode data
    if (!asn1SccMyInteger_Decode(&STORAGE_APP_Data.IN_sensor, &bitstream, &errorCode))
    {
        CFE_ES_WriteToSysLog("[STORAGE] Decode failed. Error code: %d", errorCode);
        status = CFE_SB_BAD_ARGUMENT;

    } else
    {
        // Message received stored in STORAGE_APP_Data.IN_sensor
        // TODO handle on receive
        // CFE_ES_WriteToSysLog("[STORAGE] Message received: %s", STORAGE_APP_Data.IN_sensor);

    }
    return status;
}







/*
** Helper functions, user shouldn't need to change them
*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/* STORAGE_APP_Main() -- Application entry point and main process loop         */
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
    status = STORAGE_APP_Init();
    if (status != CFE_SUCCESS)
    {
        STORAGE_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    // Main app loop
    while (CFE_ES_RunLoop(&STORAGE_APP_Data.RunStatus) == true)
    {

        /* Pend on receipt of command packet */
        status = CFE_SB_ReceiveBuffer(&STORAGE_APP_Data.Buffer, STORAGE_APP_Data.CommandPipe, CFE_SB_PEND_FOREVER);

        if (status == CFE_SUCCESS)
        {
            STORAGE_ProcessCommandPacket(STORAGE_APP_Data.Buffer);
        } else
        {
            CFE_EVS_SendEvent(STORAGE_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "STORAGE APP: SB Pipe Read Error, App Will Exit");

            STORAGE_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }


    }



    // Delete pipe to free resources
    status = CFE_SB_DeletePipe (STORAGE_APP_Data.CommandPipe);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[STORAGE] Error Deleting the pipe, RC = 0x%08lX\n",
                             (unsigned long)status);
    }


    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(STORAGE_PERF_ID);

    CFE_ES_ExitApp(STORAGE_APP_Data.RunStatus);

} /* End of STORAGE_APP_Main() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* STORAGE_APP_Init() --  initialization                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 STORAGE_APP_Init(void)
{
    int32 status = CFE_SUCCESS; // Initialize status to success;

    STORAGE_APP_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Initialize app command execution counters
    */
    STORAGE_APP_Data.CmdCounter = 0;
    STORAGE_APP_Data.ErrCounter = 0;

    /*
    ** Initialize app configuration data
    */
    STORAGE_APP_Data.PipeDepth = STORAGE_PIPE_DEPTH;

    (void)strncpy(STORAGE_APP_Data.PipeName, "STORAGE_CMD_PIPE", sizeof(STORAGE_APP_Data.PipeName));
    STORAGE_APP_Data.PipeName[sizeof(STORAGE_APP_Data.PipeName) - 1U] = 0;

    /*
    ** Initialize event filter table...
    */
    STORAGE_APP_Data.EventFilters[0].EventID = STORAGE_STARTUP_INF_EID;
    STORAGE_APP_Data.EventFilters[0].Mask    = 0x0000;
    STORAGE_APP_Data.EventFilters[1].EventID = STORAGE_COMMAND_ERR_EID;
    STORAGE_APP_Data.EventFilters[1].Mask    = 0x0000;
    STORAGE_APP_Data.EventFilters[2].EventID = STORAGE_COMMANDNOP_INF_EID;
    STORAGE_APP_Data.EventFilters[2].Mask    = 0x0000;
    STORAGE_APP_Data.EventFilters[3].EventID = STORAGE_COMMANDRST_INF_EID;
    STORAGE_APP_Data.EventFilters[3].Mask    = 0x0000;
    STORAGE_APP_Data.EventFilters[4].EventID = STORAGE_INVALID_MSGID_ERR_EID;
    STORAGE_APP_Data.EventFilters[4].Mask    = 0x0000;
    STORAGE_APP_Data.EventFilters[5].EventID = STORAGE_LEN_ERR_EID;
    STORAGE_APP_Data.EventFilters[5].Mask    = 0x0000;
    STORAGE_APP_Data.EventFilters[6].EventID = STORAGE_PIPE_ERR_EID;
    STORAGE_APP_Data.EventFilters[6].Mask    = 0x0000;
    

    /* Register the events */
    status = CFE_EVS_Register(STORAGE_APP_Data.EventFilters, STORAGE_EVENT_COUNTS, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[STORAGE] Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
    }


    /*
    ** Initialize housekeeping packet (clear user data area).
    */
    if (status == CFE_SUCCESS)
    {
        status = CFE_MSG_Init(&STORAGE_APP_Data.HkTlm.TlmHeader.Msg, CFE_SB_ValueToMsgId(STORAGE_HK_TLM_MID),
                 sizeof(STORAGE_APP_Data.HkTlm));

        if (status != CFE_SUCCESS)
        {
        CFE_ES_WriteToSysLog("[STORAGE] Error initializing housekeeping packets, RC = 0x%08lX\n", (unsigned long)status);
        }
    }

    /*
    ** Create Software Bus message pipe.
    */
    if (status == CFE_SUCCESS)
    {
        status = CFE_SB_CreatePipe(&STORAGE_APP_Data.CommandPipe, STORAGE_APP_Data.PipeDepth, STORAGE_APP_Data.PipeName);
        if (status != CFE_SUCCESS)
        {
        CFE_ES_WriteToSysLog("[STORAGE] Error creating pipe, RC = 0x%08lX\n", (unsigned long)status);
        }
    }


    /*
    ** Subscribe to Housekeeping request commands
    */
    if (status == CFE_SUCCESS)
    {
        status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(STORAGE_SEND_HK_MID), STORAGE_APP_Data.CommandPipe);
        if (status != CFE_SUCCESS)
        {
        CFE_ES_WriteToSysLog("[STORAGE] Error Subscribing to HK request, RC = 0x%08lX\n", (unsigned long)status);
        }
    }

    /*
    ** Subscribe to ground command packets
    */
    if (status == CFE_SUCCESS)
    {
        status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(STORAGE_CMD_MID), STORAGE_APP_Data.CommandPipe);
        if (status != CFE_SUCCESS)
        {
        CFE_ES_WriteToSysLog("[STORAGE] Error Subscribing to Command, RC = 0x%08lX\n", (unsigned long) status);
        }
    }



    if (status == CFE_SUCCESS)
    {
    CFE_EVS_SendEvent(STORAGE_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION, "STORAGE App Initialized.%s",
                      STORAGE_VERSION_STRING);
    }

    // Get App Id
    STORAGE_FindAppId();

    // Call datastore initialization with 80 bytes
    STORAGE_User_Datastore_Init();


    STORAGE_Send_Get_Res_Init();


STORAGE_Send_Last_Init();
    if (status == CFE_SUCCESS)
{
    STORAGE_Receive_Get_Req_Init();
}


if (status == CFE_SUCCESS)
{
    STORAGE_Receive_Sensor_data_Init();
}



    // Call user init
    STORAGE_User_Init();

    return status;

} /* End of STORAGE_Init() */

int32 STORAGE_Send_Get_Res_Init(void)
{
    // Initialize message Get_Res
    int32 status = CFE_SUCCESS; // Initialize status to success;
    STORAGE_APP_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;
    STORAGE_APP_Data.MsgId_Get_Res = STORAGE_APP_COMMUNICATION_MID_23;
    STORAGE_APP_Data.Size_Get_Res = asn1SccMyInteger_REQUIRED_BITS_FOR_ENCODING;

    status = CFE_MSG_Init(&STORAGE_APP_Data.MsgPtr_Get_Res, STORAGE_APP_Data.MsgId_Get_Res, STORAGE_APP_Data.Size_Get_Res);

    if (status != CFE_SUCCESS) {
        STORAGE_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        CFE_ES_WriteToSysLog("[STORAGE] Error creating the message, RC = %lu\n", (unsigned long) status);
    }
    return status;
}



int32 STORAGE_Send_Last_Init(void)
{
    // Initialize message Last
    int32 status = CFE_SUCCESS; // Initialize status to success;
    STORAGE_APP_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;
    STORAGE_APP_Data.MsgId_Last = STORAGE_APP_COMMUNICATION_MID_20;
    STORAGE_APP_Data.Size_Last = asn1SccMyInteger_REQUIRED_BITS_FOR_ENCODING;

    status = CFE_MSG_Init(&STORAGE_APP_Data.MsgPtr_Last, STORAGE_APP_Data.MsgId_Last, STORAGE_APP_Data.Size_Last);

    if (status != CFE_SUCCESS) {
        STORAGE_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        CFE_ES_WriteToSysLog("[STORAGE] Error creating the message, RC = %lu\n", (unsigned long) status);
    }
    return status;
}
int32 STORAGE_Receive_Get_Req_Init(void)
{
    status = CFE_SB_Subscribe(0x021,
                              STORAGE_APP_Data.CommandPipe);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[STORAGE] Error Subscribing to communication pipe, RC = 0x%08lX\n",
                             (unsigned long) status);
    }
    return status;
}



int32 STORAGE_Receive_Sensor_data_Init(void)
{
    status = CFE_SB_Subscribe(0x010,
                              STORAGE_APP_Data.CommandPipe);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("[STORAGE] Error Subscribing to communication pipe, RC = 0x%08lX\n",
                             (unsigned long) status);
    }
    return status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* STORAGE_FindAppId -- Get App Id and set it in App data   */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 STORAGE_FindAppId(void)
{
    status = CFE_ES_ERR_NAME_NOT_FOUND;
    CFE_ES_AppId_t TempAppId;
    /*
    ** Search the ES Application table for the name that matches this app
    */
    for (int i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++ )
    {

        if (!strcmp(CFE_ES_Global.AppTable[i].AppName, STORAGE_APP_NAME))
        {
            status = CFE_ES_GetAppIDByName(&TempAppId, CFE_ES_Global.AppTable[i].AppName);
            if (status == CFE_SUCCESS)
            {
                STORAGE_APP_Data.AppId = TempAppId;
            }
            else
            {
                CFE_ES_WriteToSysLog("[STORAGE] Error getting the App ID, RC = %lu\n", (unsigned long)status);
            }
            break;
        }

    } /* end for */
    return status;
}


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
            // Call user loop
            STORAGE_User_Main();
            break;

        // Process Get_Req
        case 0x021:
            STORAGE_User_Receive_Get_Req();

    break;


    // Process Sensor_data
        case 0x010:
            STORAGE_User_Receive_Sensor_data();

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
            } else
            {
                CFE_ES_WriteToSysLog("[STORAGE] Invalid command length\n");
            }

            break;

        case STORAGE_RESET_COUNTERS_CC:
            if (STORAGE_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STORAGE_ResetCountersCmd_t)))
            {
                STORAGE_ResetCounters((STORAGE_ResetCountersCmd_t *)SBBufPtr);
            } else
            {
                CFE_ES_WriteToSysLog("[STORAGE] Invalid command length\n");
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
    (void)Msg;

    /*
    ** Get command execution counters...
    */
    STORAGE_APP_Data.HkTlm.Payload.CommandErrorCounter = STORAGE_APP_Data.ErrCounter;
    STORAGE_APP_Data.HkTlm.Payload.CommandCounter      = STORAGE_APP_Data.CmdCounter;

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg(&STORAGE_APP_Data.HkTlm.TlmHeader.Msg);
    CFE_SB_TransmitMsg(&STORAGE_APP_Data.HkTlm.TlmHeader.Msg, true);


    return CFE_SUCCESS;

} /* End of STORAGE_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* STORAGE_Noop -- STORAGE NOOP commands                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 STORAGE_Noop(const STORAGE_NoopCmd_t *Msg)
{
    (void)Msg;

    STORAGE_APP_Data.CmdCounter++;

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
    (void)Msg;

    STORAGE_APP_Data.CmdCounter = 0;
    STORAGE_APP_Data.ErrCounter = 0;

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

        STORAGE_APP_Data.ErrCounter++;
    }

    return (result);

} /* End of STORAGE_VerifyCmdLength() */


