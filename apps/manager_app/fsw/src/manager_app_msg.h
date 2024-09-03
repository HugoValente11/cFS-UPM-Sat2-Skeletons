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
*******************************************************************************/

/**
 * @file
 *
 * Define SAMPLE App  Messages and info
 */

#ifndef MANAGER_MSG_H
#define MANAGER_MSG_H

/*
** SAMPLE App command codes
*/
#define MANAGER_NOOP_CC           0
#define MANAGER_RESET_COUNTERS_CC 1
#define MANAGER_PROCESS_CC        2

/*************************************************************************/

/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader; /**< \brief Command header */
} MANAGER_NoArgsCmd_t;

/*
** The following commands all share the "NoArgs" format
**
** They are each given their own type name matching the command name, which
** allows them to change independently in the future without changing the prototype
** of the handler function
*/
typedef MANAGER_NoArgsCmd_t MANAGER_NoopCmd_t;
typedef MANAGER_NoArgsCmd_t MANAGER_ResetCountersCmd_t;
typedef MANAGER_NoArgsCmd_t MANAGER_ProcessCmd_t;

/*************************************************************************/
/*
** Type definition (SAMPLE App housekeeping)
*/

typedef struct
{
    uint8 CommandErrorCounter;
    uint8 CommandCounter;
    uint8 spare[2];
} MANAGER_HkTlm_Payload_t;

typedef struct
{
    CFE_MSG_TelemetryHeader_t  TlmHeader; /**< \brief Telemetry header */
    MANAGER_HkTlm_Payload_t Payload;   /**< \brief Telemetry payload */
} MANAGER_HkTlm_t;

#endif /* MANAGER_MSG_H */
