/*
 * Includes
 */

#include "platform_app_coveragetest_common.h"
#include "ut_platform_app.h"

typedef struct
{
    uint16      ExpectedEvent;
    uint32      MatchCount;
    const char *ExpectedFormat;
} UT_CheckEvent_t;

typedef struct
{
    size_t      ExpectedBlockSize;
    const char *ExpectedName;
    uint32      MatchCount;
} UT_CheckCDSReg_t;

/*
 * An example hook function to check for a specific event.
 */
static int32 UT_CheckEvent_Hook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context,
                                va_list va)
{
    UT_CheckEvent_t *State = UserObj;
    uint16           EventId;
    const char *     Spec;

    /*
     * The CFE_EVS_SendEvent stub passes the EventID as the
     * first context argument.
     */
    if (Context->ArgCount > 0)
    {
        EventId = UT_Hook_GetArgValueByName(Context, "EventID", uint16);
        if (EventId == State->ExpectedEvent)
        {
            if (State->ExpectedFormat != NULL)
            {
                Spec = UT_Hook_GetArgValueByName(Context, "Spec", const char *);
                if (Spec != NULL)
                {
                    /*
                     * Example of how to validate the full argument set.
                     * ------------------------------------------------
                     *
                     * If really desired one can call something like:
                     *
                     * char TestText[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
                     * vsnprintf(TestText, sizeof(TestText), Spec, va);
                     *
                     * And then compare the output (TestText) to the expected fully-rendered string.
                     *
                     * NOTE: While this can be done, use with discretion - This isn't really
                     * verifying that the FSW code unit generated the correct event text,
                     * rather it is validating what the system snprintf() library function
                     * produces when passed the format string and args.
                     *
                     * This type of check has been demonstrated to make tests very fragile,
                     * because it is influenced by many factors outside the control of the
                     * test case.
                     *
                     * __This derived string is not an actual output of the unit under test__
                     */
                    if (strcmp(Spec, State->ExpectedFormat) == 0)
                    {
                        ++State->MatchCount;
                    }
                }
            }
            else
            {
                ++State->MatchCount;
            }
        }
    }

    return 0;
}

static int32 UT_RegisterCDS_Hook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context)
{
    UT_CheckCDSReg_t *State = UserObj;
    size_t BlockSize = 0;
    const char *Name = NULL;
    CFE_ES_CDSHandle_t **HandlePtr = NULL;

    // Retrieve the BlockSize and Name parameters from the stub context.
    BlockSize = UT_Hook_GetArgValueByName(Context, "BlockSize", size_t);
    Name = UT_Hook_GetArgValueByName(Context, "Name", const char *);
    HandlePtr = UT_Hook_GetArgValueByName(Context, "CDSHandlePtr", CFE_ES_CDSHandle_t **);

    // Verify if the retrieved parameters match the expected ones.
    if (BlockSize == State->ExpectedBlockSize && strcmp(Name, State->ExpectedName) == 0)
    {
        ++State->MatchCount; // Increment match count if parameters match.

        // If a handle pointer is provided, set it to a mock value (e.g., 42).

        if (HandlePtr != NULL)
        {
            *HandlePtr = (CFE_ES_CDSHandle_t) 42;
        }
    }

    return StubRetcode; // Return the original stub return code.
}

/*
 * Helper function to set up for event checking
 * This attaches the hook function to CFE_EVS_SendEvent
 */
static void UT_CheckEvent_Setup(UT_CheckEvent_t *Evt, uint16 ExpectedEvent, const char *ExpectedFormat)
{
    memset(Evt, 0, sizeof(*Evt));
    Evt->ExpectedEvent  = ExpectedEvent;
    Evt->ExpectedFormat = ExpectedFormat;
    UT_SetVaHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_CheckEvent_Hook, Evt);
}

void Test_PLATFORM_APP_Main(void)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

    // Nominal
    PLATFORM_APP_Main();
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_ExitApp)) == 1, "CFE_ES_ExitApp was called once after nominal app execution, ensuring proper exit.");

    // Fail on initialization
    // Can't stub APP_Init so fail inside init
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, CFE_EVS_INVALID_PARAMETER);
    PLATFORM_APP_Main();
    UtAssert_True(PLATFORM_APP_Data.RunStatus == CFE_ES_RunStatus_APP_ERROR,
                  "PLATFORM_APP_Data.RunStatus (%lu) == CFE_ES_RunStatus_APP_ERROR",
                  (unsigned long)PLATFORM_APP_Data.RunStatus);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_ExitApp)) == 2, "CFE_ES_ExitApp was called due to initialization failure, indicating an immediate exit after setup error.");
    // Test multiple failures on entering while loop but failing on receive buffer
    UT_ResetState(0);
    CFE_Status_t failureReceivingBufferCodes[] = {
        CFE_SB_BAD_ARGUMENT, CFE_SB_TIME_OUT, CFE_SB_PIPE_RD_ERR, CFE_SB_NO_MESSAGE
        };
    int numFailures = sizeof(failureReceivingBufferCodes) / sizeof(failureReceivingBufferCodes[0]);
    for (int i = 0; i < numFailures; i++)
    {
        /* Reset the state before each test case */
        UT_ResetState(UT_KEY(CFE_EVS_SendEvent));
        UT_ResetState(UT_KEY(CFE_ES_WriteToSysLog));
        UT_ResetState(UT_KEY(CFE_ES_ExitApp));
        UT_CheckEvent_t EventTest;
        UT_CheckEvent_Setup(&EventTest, PLATFORM_PIPE_ERR_EID, "PLATFORM APP: SB Pipe Read Error, App Will Exit");
        UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);
        UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
        UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 1, failureReceivingBufferCodes[i]);
        PLATFORM_APP_Main();
        UtAssert_True(EventTest.MatchCount == 1, "Pipe error event event generated (%u)", (unsigned int)EventTest.MatchCount);
        UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 2, "CFE_EVS_SendEvent called");
        UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 0, "CFE_ES_WriteToSysLog should not be called");
        UtAssert_True(PLATFORM_APP_Data.RunStatus == CFE_ES_RunStatus_APP_ERROR, "PLATFORM_APP_Data.RunStatus (%lu) == CFE_ES_RunStatus_APP_ERROR", (unsigned long)PLATFORM_APP_Data.RunStatus);
        UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_ExitApp)) == 1, "CFE_ES_ExitApp was called after SB Receive Buffer encountered a simulated failure, leading to app error exit.");
    }

    // Enter while loop and succeed on receive buffer
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 1, CFE_SUCCESS);
    PLATFORM_APP_Main();
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_ExitApp)) == 2, "CFE_ES_ExitApp was called after successfully processing an SB Receive Buffer, confirming normal termination.");

    // Never enter while loop
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, false);
    PLATFORM_APP_Main();
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_ExitApp)) == 3, "CFE_ES_ExitApp was called without entering the main loop, likely due to a pre-loop termination condition.");


    // Fail on deleting pipe
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_DeletePipe), 1, CFE_SB_BAD_ARGUMENT);
    PLATFORM_APP_Main();
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1, "CFE_ES_WriteToSysLog() called");
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_ExitApp)) == 4, "CFE_ES_ExitApp was invoked after failing to delete a pipe, signaling cleanup issues during shutdown.");

}


void Test_PLATFORM_APP_Init(void)
{
    // Nominal and check event
    UT_CheckEvent_t EventTest;
    UT_CheckEvent_Setup(&EventTest, PLATFORM_STARTUP_INF_EID, "PLATFORM App Initialized.%s");
    UT_TEST_FUNCTION_RC(PLATFORM_APP_Init(), CFE_SUCCESS);
    UtAssert_True(EventTest.MatchCount == 1, "Startup event confirmed, indicating successful app initialization.");
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent)) == 1, "Exactly one startup event was dispatched, confirming app initialization success.");
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 0, "No SysLog messages were generated, indicating a clean initialization without errors or warnings.");

    // Test multiple failures on register events
     UT_ResetState(0);
     CFE_Status_t failureRegisterEventCodes[] = {
         CFE_EVS_APP_FILTER_OVERLOAD, CFE_EVS_UNKNOWN_FILTER, CFE_EVS_APP_ILLEGAL_APP_ID, CFE_ES_BAD_ARGUMENT
        };

    int numFailures = sizeof(failureRegisterEventCodes) / sizeof(failureRegisterEventCodes[0]);
    for (int i = 0; i < numFailures; i++)
    {
        /* Reset the state before each test case */
        UT_ResetState(UT_KEY(CFE_EVS_Register));
        UT_ResetState(UT_KEY(CFE_ES_WriteToSysLog));
        UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, failureRegisterEventCodes[i]);
        UT_TEST_FUNCTION_RC(PLATFORM_APP_Init(), failureRegisterEventCodes[i]);
        UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1, "Event registration error logged for failure code, verifying system log was written.");
        UtAssert_True(UT_GetStubCount(UT_KEY(CFE_EVS_Register)) == 1, "CFE_EVS_Register was correctly called, ensuring event registration attempt.");
    }

    // Fail in initializing housekeeping
    UT_SetDeferredRetcode(UT_KEY(CFE_MSG_Init), 1, CFE_MSG_BAD_ARGUMENT);
    UT_TEST_FUNCTION_RC(PLATFORM_APP_Init(), CFE_MSG_BAD_ARGUMENT);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 2, "Initialization failure logged due to housekeeping packet initialization with bad argument.");

    // Test multiple failures in creating software bus pipe
    UT_ResetState(0);
    CFE_Status_t failureCreatePipeCodes[] = {
        CFE_SB_BAD_ARGUMENT, CFE_SB_MAX_PIPES_MET, CFE_SB_PIPE_CR_ERR
        };


    numFailures = sizeof(failureCreatePipeCodes) / sizeof(failureCreatePipeCodes[0]);
    for (int i = 0; i < numFailures; i++)
    {
        /* Reset the state before each test case */
        UT_ResetState(UT_KEY(CFE_SB_CreatePipe));
        UT_ResetState(UT_KEY(CFE_ES_WriteToSysLog));
        UT_SetDeferredRetcode(UT_KEY(CFE_SB_CreatePipe), 1, failureCreatePipeCodes[i]);
        UT_TEST_FUNCTION_RC(PLATFORM_APP_Init(), failureCreatePipeCodes[i]);
        UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1, "Software bus pipe creation error logged, validating error handling in system log.");
        UtAssert_True(UT_GetStubCount(UT_KEY(CFE_SB_CreatePipe)) == 1, "CFE_SB_CreatePipe call confirmed, indicating an attempt to create a software bus pipe.");
    }

    // Test multiple failures in subscribing to house keeping requests
    UT_ResetState(0);
    CFE_Status_t failureSubscribeCodes[] = {
        CFE_SB_MAX_MSGS_MET, CFE_SB_MAX_DESTS_MET, CFE_SB_BAD_ARGUMENT, CFE_SB_BUF_ALOC_ERR
        };

    numFailures = sizeof(failureSubscribeCodes) / sizeof(failureSubscribeCodes[0]);
    for (int i = 0; i < numFailures; i++)
    {
        /* Reset the state before each test case */
        UT_ResetState(UT_KEY(CFE_SB_Subscribe));
        UT_ResetState(UT_KEY(CFE_ES_WriteToSysLog));
        UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 1, failureSubscribeCodes[i]);
        UT_TEST_FUNCTION_RC(PLATFORM_APP_Init(), failureSubscribeCodes[i]);
        UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1, "Subscription to housekeeping requests failure is logged, verifying the system log captured the error.");
        UtAssert_True(UT_GetStubCount(UT_KEY(CFE_SB_Subscribe)) == 1, "CFE_SB_Subscribe call count is correct, confirming a subscription attempt was made.");
    }

    // Test multiple failures in subscribing to ground command packets
    UT_ResetState(0);
    for (int i = 0; i < numFailures; i++)
    {
        /* Reset the state before each test case */
        UT_ResetState(UT_KEY(CFE_SB_Subscribe));
        UT_ResetState(UT_KEY(CFE_ES_WriteToSysLog));
        UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 2, failureSubscribeCodes[i]);
        UT_TEST_FUNCTION_RC(PLATFORM_APP_Init(), failureSubscribeCodes[i]);
        UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1, "Ground command packet subscription failure is accurately logged, ensuring error handling was triggered.");
        UtAssert_True(UT_GetStubCount(UT_KEY(CFE_SB_Subscribe)) == 2, "CFE_SB_Subscribe was correctly invoked twice, reflecting the attempted subscriptions to ground command packets.");
    }
}

void Test_PLATFORM_FindAppId(void)
{
    CFE_ES_AppId_t ExpectedAppId;

    // Fail on finding app
    // Initialize AppTable with non-matching names
    for (int i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++)
    {
    strcpy(CFE_ES_Global.AppTable[i].AppName, "NON_MATCHING_APP");
    }
    PLATFORM_APP_Data.AppId = CFE_ES_APPID_UNDEFINED;
    UT_TEST_FUNCTION_RC(PLATFORM_FindAppId(), CFE_ES_ERR_NAME_NOT_FOUND);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_GetAppIDByName)) == 0, "CFE_ES_GetAppIDByName was not invoked due to no matching app found in the table.");
    UtAssert_True(PLATFORM_APP_Data.AppId == CFE_ES_APPID_UNDEFINED, "PLATFORM_APP_Data.AppId should remain unchanged");

    // Success on finding app
    // Initialize AppTable with matching names
    ExpectedAppId = 42;
    strcpy(CFE_ES_Global.AppTable[0].AppName, "PLATFORM_APP");
    for (int i = 1; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++)
    {
    strcpy(CFE_ES_Global.AppTable[i].AppName, "NON_MATCHING_APP");
    }
    UT_SetDataBuffer(UT_KEY(CFE_ES_GetAppIDByName), &ExpectedAppId, sizeof(ExpectedAppId), false);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppIDByName), 1, CFE_SUCCESS);
    UT_TEST_FUNCTION_RC(PLATFORM_FindAppId(), CFE_SUCCESS);
    UtAssert_True(PLATFORM_APP_Data.AppId == ExpectedAppId, "App ID correctly identified and updated in app data structure.");
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_GetAppIDByName)) == 1, "CFE_ES_GetAppIDByName successfully invoked once for app ID retrieval.");

    // Success on finding app but es error bad argument on GetAppID
    UT_ResetState(0);
    strcpy(CFE_ES_Global.AppTable[0].AppName, "PLATFORM_APP");
    for (int i = 1; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++)
    {
    strcpy(CFE_ES_Global.AppTable[i].AppName, "NON_MATCHING_APP");
    }
    PLATFORM_APP_Data.AppId = CFE_ES_APPID_UNDEFINED;
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppIDByName), 1, CFE_ES_BAD_ARGUMENT);
    UT_TEST_FUNCTION_RC(PLATFORM_FindAppId(), CFE_ES_BAD_ARGUMENT);
    UtAssert_True(PLATFORM_APP_Data.AppId == CFE_ES_APPID_UNDEFINED, "App ID remains unchanged due to CFE_ES_GetAppIDByName returning an error.");
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1, "Error logged to system log due to bad argument in CFE_ES_GetAppIDByName.");
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_GetAppIDByName)) == 1, "CFE_ES_GetAppIDByName called once but resulted in error due to bad argument.");

    // Success on finding app but es error name not found on GetAppID
    UT_ResetState(0);
    strcpy(CFE_ES_Global.AppTable[0].AppName, "PLATFORM_APP");
    for (int i = 1; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++)
    {
    strcpy(CFE_ES_Global.AppTable[i].AppName, "NON_MATCHING_APP");
    }
    PLATFORM_APP_Data.AppId = CFE_ES_APPID_UNDEFINED;
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppIDByName), 1, CFE_ES_ERR_NAME_NOT_FOUND);
    UT_TEST_FUNCTION_RC(PLATFORM_FindAppId(), CFE_ES_ERR_NAME_NOT_FOUND);
    UtAssert_True(PLATFORM_APP_Data.AppId == CFE_ES_APPID_UNDEFINED, "App ID remains unchanged as the app name was not found.");
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1, "System log entry generated due to app name not found in CFE_ES_GetAppIDByName.");
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_GetAppIDByName)) == 1, "CFE_ES_GetAppIDByName invoked once but failed due to app name not found.");

    // Boundary test success if match is last app
    UT_ResetState(0);
    ExpectedAppId = 42; // Expected app ID
    for (int i = 1; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++)
    {
    strcpy(CFE_ES_Global.AppTable[i].AppName, "NON_MATCHING_APP");
    }
    strcpy(CFE_ES_Global.AppTable[CFE_PLATFORM_ES_MAX_APPLICATIONS - 1].AppName, "PLATFORM_APP");
    UT_SetDataBuffer(UT_KEY(CFE_ES_GetAppIDByName), &ExpectedAppId, sizeof(ExpectedAppId), false);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppIDByName), 1, CFE_SUCCESS);
    PLATFORM_FindAppId();
    UtAssert_True(PLATFORM_APP_Data.AppId == ExpectedAppId, "App ID successfully found even when the match is the last app in the table.");
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_GetAppIDByName)) == 1, "CFE_ES_GetAppIDByName called successfully for the last app in the table.");
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 0, "No system log entries generated for successful last app match.");


    // Empty table
    UT_ResetState(0);
    for (int i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++)
    {
        CFE_ES_Global.AppTable[i].AppName[0] = '\0'; // Assuming an empty string denotes an unused slot
    }
    PLATFORM_APP_Data.AppId = CFE_ES_APPID_UNDEFINED;
    PLATFORM_FindAppId();
    UtAssert_True(PLATFORM_APP_Data.AppId == CFE_ES_APPID_UNDEFINED, "App ID remains undefined when AppTable is empty, as expected.");
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_GetAppIDByName)) == 0, "CFE_ES_GetAppIDByName was not called due to an empty AppTable.");
}


void Test_PLATFORM_Receive_Sensor_data_Init(void)
{
    // Nominal case: Subscription success
    UT_TEST_FUNCTION_RC(PLATFORM_Receive_Sensor_data_Init(), CFE_SUCCESS);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_SB_Subscribe)) == 1, "CFE_SB_Subscribe should be called once for successful subscription");

    // Testing subscription failures
    CFE_Status_t failureCodes[] = {
        CFE_SB_MAX_MSGS_MET, CFE_SB_MAX_DESTS_MET, CFE_SB_BAD_ARGUMENT, CFE_SB_BUF_ALOC_ERR
    };
    for (int i = 0; i < sizeof(failureCodes)/sizeof(failureCodes[0]); ++i) {
        UT_ResetState(0);
        UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 1, failureCodes[i]);
        UT_TEST_FUNCTION_RC(PLATFORM_Receive_Sensor_data_Init(), failureCodes[i]);
        UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1, "Syslog should be written for subscription failure");
    }
}

void Test_PLATFORM_User_Receive_Sensor_data(void)
{
    // Mock buffer and message
    CFE_SB_Buffer_t mockBuffer;
    memset(&mockBuffer, 0, sizeof(CFE_SB_Buffer_t)); // Clear the buffer
    memset(mockBuffer.Msg.Byte, 42, sizeof(mockBuffer.Msg.Byte));

    PLATFORM_APP_Data.Buffer = &mockBuffer;

    // Fail on decoding
    UT_SetDeferredRetcode(UT_KEY(asn1SccMyInteger_Decode), 1, false);
    UT_TEST_FUNCTION_RC(PLATFORM_User_Receive_Sensor_data(), CFE_SB_BAD_ARGUMENT);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1, "Decode failure logged correctly.");

    // Success
    byte mockData[40];
    memset(mockData, 0, sizeof(mockData)); // Initialize the mock data as needed for the test
    UT_SetDataBuffer(UT_KEY(CFE_SB_ReceiveBuffer), &mockBuffer, sizeof(mockBuffer), false);
    UT_SetDataBuffer(UT_KEY(CFE_SB_GetUserData), mockData, sizeof(mockData), false);
    UT_SetDeferredRetcode(UT_KEY(asn1SccMyInteger_Decode), 1, true);
    UT_TEST_FUNCTION_RC(PLATFORM_User_Receive_Sensor_data(), CFE_SUCCESS);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1, "Successful reception and decoding of the message.");

}


void Test_PLATFORM_Find_Pressure_sensorAppId(void)
{
    uint32 appId;

    // Test case: Successfully find the application and correctly call CFE_ES_GetAppIDByName
    strcpy(CFE_ES_Global.AppTable[0].AppName, "PRESSURE_SENSOR_APP");
    appId = 42; // Mock App ID
    UT_SetDataBuffer(UT_KEY(CFE_ES_GetAppIDByName), &appId, sizeof(appId), false);
    UT_TEST_FUNCTION_RC(PLATFORM_Find_Pressure_sensorAppId(), appId);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_GetAppIDByName)) == 1, "Successfully called CFE_ES_GetAppIDByName with correct parameters.");
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 0, "No SysLog entries should be generated for successful app ID retrieval.");

    // Test case: Fail to find the target application due to non-matching application names
    UT_ResetState(0);
    for (int i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++) {
        strcpy(CFE_ES_Global.AppTable[i].AppName, "NON_MATCHING_APP");
    }
    UT_TEST_FUNCTION_RC(PLATFORM_Find_Pressure_sensorAppId(), CFE_ES_APPID_UNDEFINED);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_GetAppIDByName)) == 0, "CFE_ES_GetAppIDByName should not be invoked for non-matching app names.");
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 0, "No SysLog entries should be generated when the target app is not found.");

    // Test case: Successfully find the application but fail due to a bad argument in CFE_ES_GetAppIDByName
    UT_ResetState(0);
    strcpy(CFE_ES_Global.AppTable[0].AppName, "PRESSURE_SENSOR_APP");
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppIDByName), 1, CFE_ES_BAD_ARGUMENT);
    UT_TEST_FUNCTION_RC(PLATFORM_Find_Pressure_sensorAppId(), CFE_ES_APPID_UNDEFINED);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_GetAppIDByName)) == 1, "CFE_ES_GetAppIDByName was called despite an issue.");
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1, "A SysLog entry should be generated for the failure due to bad argument.");

    // Test case: Successfully find the application but CFE_ES_GetAppIDByName returns name not found
    UT_ResetState(0);
    strcpy(CFE_ES_Global.AppTable[0].AppName, "PRESSURE_SENSOR_APP");
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppIDByName), 1, CFE_ES_ERR_NAME_NOT_FOUND);
    UT_TEST_FUNCTION_RC(PLATFORM_Find_Pressure_sensorAppId(), CFE_ES_APPID_UNDEFINED);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_GetAppIDByName)) == 1, "CFE_ES_GetAppIDByName was called, indicating an attempt to find the app.");
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1, "A SysLog entry should be generated for the failure due to name not found.");
}

void Test_PLATFORM_User_Start_Pressure_sensorApp(void)
{
    // Test case: Failure in creating the application due to bad argument
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_ParseFileEntry), 1, CFE_ES_BAD_ARGUMENT);
    UT_TEST_FUNCTION_RC(PLATFORM_User_Start_Pressure_sensorApp(), CFE_ES_BAD_ARGUMENT);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1, "Log entry should be made for bad argument failure in app creation");

    // Test case: Failure in creating the application due to application create error
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_ParseFileEntry), 1, CFE_ES_ERR_APP_CREATE);
    UT_TEST_FUNCTION_RC(PLATFORM_User_Start_Pressure_sensorApp(), CFE_ES_ERR_APP_CREATE);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 2, "Log entry should be made for app creation failure");

    // Test case: Successful application start
    UT_TEST_FUNCTION_RC(PLATFORM_User_Start_Pressure_sensorApp(), CFE_SUCCESS);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 2, "No additional log entries should be made for a successful app start");
}

void Test_PLATFORM_User_Stop_Pressure_sensorApp(void)
{
    // Test case: Failure in stopping the application due to invalid resource ID
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_DeleteApp), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UT_TEST_FUNCTION_RC(PLATFORM_User_Stop_Pressure_sensorApp(), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1, "Log entry should be made for invalid resource ID failure in app stopping");

    // Test case: Successful application stop
    UT_TEST_FUNCTION_RC(PLATFORM_User_Stop_Pressure_sensorApp(), CFE_SUCCESS);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1, "No additional log entries should be made for a successful app stop");
}





void Test_PLATFORM_Find_Temperature_sensorAppId(void)
{
    uint32 appId;

    // Test case: Successfully find the application and correctly call CFE_ES_GetAppIDByName
    strcpy(CFE_ES_Global.AppTable[0].AppName, "TEMPERATURE_SENSOR_APP");
    appId = 42; // Mock App ID
    UT_SetDataBuffer(UT_KEY(CFE_ES_GetAppIDByName), &appId, sizeof(appId), false);
    UT_TEST_FUNCTION_RC(PLATFORM_Find_Temperature_sensorAppId(), appId);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_GetAppIDByName)) == 1, "Successfully called CFE_ES_GetAppIDByName with correct parameters.");
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 0, "No SysLog entries should be generated for successful app ID retrieval.");

    // Test case: Fail to find the target application due to non-matching application names
    UT_ResetState(0);
    for (int i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++) {
        strcpy(CFE_ES_Global.AppTable[i].AppName, "NON_MATCHING_APP");
    }
    UT_TEST_FUNCTION_RC(PLATFORM_Find_Temperature_sensorAppId(), CFE_ES_APPID_UNDEFINED);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_GetAppIDByName)) == 0, "CFE_ES_GetAppIDByName should not be invoked for non-matching app names.");
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 0, "No SysLog entries should be generated when the target app is not found.");

    // Test case: Successfully find the application but fail due to a bad argument in CFE_ES_GetAppIDByName
    UT_ResetState(0);
    strcpy(CFE_ES_Global.AppTable[0].AppName, "TEMPERATURE_SENSOR_APP");
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppIDByName), 1, CFE_ES_BAD_ARGUMENT);
    UT_TEST_FUNCTION_RC(PLATFORM_Find_Temperature_sensorAppId(), CFE_ES_APPID_UNDEFINED);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_GetAppIDByName)) == 1, "CFE_ES_GetAppIDByName was called despite an issue.");
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1, "A SysLog entry should be generated for the failure due to bad argument.");

    // Test case: Successfully find the application but CFE_ES_GetAppIDByName returns name not found
    UT_ResetState(0);
    strcpy(CFE_ES_Global.AppTable[0].AppName, "TEMPERATURE_SENSOR_APP");
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppIDByName), 1, CFE_ES_ERR_NAME_NOT_FOUND);
    UT_TEST_FUNCTION_RC(PLATFORM_Find_Temperature_sensorAppId(), CFE_ES_APPID_UNDEFINED);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_GetAppIDByName)) == 1, "CFE_ES_GetAppIDByName was called, indicating an attempt to find the app.");
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1, "A SysLog entry should be generated for the failure due to name not found.");
}

void Test_PLATFORM_User_Start_Temperature_sensorApp(void)
{
    // Test case: Failure in creating the application due to bad argument
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_ParseFileEntry), 1, CFE_ES_BAD_ARGUMENT);
    UT_TEST_FUNCTION_RC(PLATFORM_User_Start_Temperature_sensorApp(), CFE_ES_BAD_ARGUMENT);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1, "Log entry should be made for bad argument failure in app creation");

    // Test case: Failure in creating the application due to application create error
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_ParseFileEntry), 1, CFE_ES_ERR_APP_CREATE);
    UT_TEST_FUNCTION_RC(PLATFORM_User_Start_Temperature_sensorApp(), CFE_ES_ERR_APP_CREATE);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 2, "Log entry should be made for app creation failure");

    // Test case: Successful application start
    UT_TEST_FUNCTION_RC(PLATFORM_User_Start_Temperature_sensorApp(), CFE_SUCCESS);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 2, "No additional log entries should be made for a successful app start");
}

void Test_PLATFORM_User_Stop_Temperature_sensorApp(void)
{
    // Test case: Failure in stopping the application due to invalid resource ID
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_DeleteApp), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UT_TEST_FUNCTION_RC(PLATFORM_User_Stop_Temperature_sensorApp(), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1, "Log entry should be made for invalid resource ID failure in app stopping");

    // Test case: Successful application stop
    UT_TEST_FUNCTION_RC(PLATFORM_User_Stop_Temperature_sensorApp(), CFE_SUCCESS);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1, "No additional log entries should be made for a successful app stop");
}



void Test_PLATFORM_ProcessCommandPacket(void)
{
    /*
     * Test Case For:
     * void PLATFORM_ProcessCommandPacket
     */
    /* a buffer large enough for any command message */
    union
    {
        CFE_SB_Buffer_t      SBBuf;
        PLATFORM_NoopCmd_t Noop;
    } TestMsg;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;
    UT_CheckEvent_t   EventTest;

    memset(&TestMsg, 0, sizeof(TestMsg));
    UT_CheckEvent_Setup(&EventTest, PLATFORM_INVALID_MSGID_ERR_EID, "PLATFORM: invalid command packet,MID = 0x%x");

    /*
     * The CFE_MSG_GetMsgId() stub uses a data buffer to hold the
     * message ID values to return.
     */
    TestMsgId = CFE_SB_ValueToMsgId(PLATFORM_CMD_MID);
    FcnCode   = PLATFORM_NOOP_CC;
    MsgSize   = sizeof(TestMsg.Noop);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);
    PLATFORM_ProcessCommandPacket(&TestMsg.SBBuf);

    TestMsgId = CFE_SB_ValueToMsgId(PLATFORM_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    PLATFORM_ProcessCommandPacket(&TestMsg.SBBuf);

    // Sensor_data message id
    TestMsgId = CFE_SB_ValueToMsgId(0x010);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    PLATFORM_ProcessCommandPacket(&TestMsg.SBBuf);

    /* invalid message id */
    TestMsgId = CFE_SB_INVALID_MSG_ID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    PLATFORM_ProcessCommandPacket(&TestMsg.SBBuf);

    /*
     * Confirm that the event was generated only _once_
     */
    UtAssert_True(EventTest.MatchCount == 1, "PLATFORM_COMMAND_ERR_EID generated (%u)",
                  (unsigned int)EventTest.MatchCount);
}

void Test_PLATFORM_ProcessGroundCommand(void)
{
    /*
     * Test Setup for Function : PLATFORM_ProcessGroundCommand
     */
    CFE_MSG_FcnCode_t FcnCode;
    size_t Size;
    CFE_SB_MsgId_t    MsgId = CFE_SB_ValueToMsgId(1);

    // A buffer large enough for any command message
    union
    {
        CFE_SB_Buffer_t SBBuf;
        PLATFORM_NoopCmd_t Noop;
        PLATFORM_ResetCountersCmd_t Reset;
        PLATFORM_ProcessCmd_t Process;
    } TestMsg;

    memset(&TestMsg, 0, sizeof(TestMsg));  // Clear the message structure

    /* Test dispatch of NOOP */
    // Correct length
    FcnCode = PLATFORM_NOOP_CC;
    Size = sizeof(TestMsg.Noop);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    PLATFORM_ProcessGroundCommand(&TestMsg.SBBuf);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 0, "CFE_ES_WriteToSysLog() should not be called");

    // Incorrect length
    FcnCode = PLATFORM_NOOP_CC;
    Size = sizeof(TestMsg.Noop) -1;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    PLATFORM_ProcessGroundCommand(&TestMsg.SBBuf);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1, "CFE_ES_WriteToSysLog() called");


    /* Test dispatch of RESET */
    // Correct length
    FcnCode = PLATFORM_RESET_COUNTERS_CC;
    Size = sizeof(TestMsg.Reset);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    PLATFORM_ProcessGroundCommand(&TestMsg.SBBuf);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1, "CFE_ES_WriteToSysLog() called");

    // Incorrect length
    FcnCode = PLATFORM_RESET_COUNTERS_CC;
    Size = sizeof(TestMsg.Reset) -1;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    PLATFORM_ProcessGroundCommand(&TestMsg.SBBuf);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 2, "CFE_ES_WriteToSysLog() called");

    /* Test dispatch of an invalid command code */
    FcnCode = 999; // Use an undefined function code
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    PLATFORM_ProcessGroundCommand(&TestMsg.SBBuf);
}

void Test_PLATFORM_ReportHousekeeping(void)
{
    /*
     * Test Case For:
     * void PLATFORM_ReportHousekeeping( const CFE_SB_CmdHdr_t *Msg )
     */
    CFE_MSG_Message_t *MsgSend;
    CFE_MSG_Message_t *MsgTimestamp;
    CFE_SB_MsgId_t     MsgId = CFE_SB_ValueToMsgId(PLATFORM_SEND_HK_MID);

    /* Set message id to return so PLATFORM_Housekeeping will be called */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);

    /* Set up to capture send message address */
    UT_SetDataBuffer(UT_KEY(CFE_SB_TransmitMsg), &MsgSend, sizeof(MsgSend), false);

    /* Set up to capture timestamp message address */
    UT_SetDataBuffer(UT_KEY(CFE_SB_TimeStampMsg), &MsgTimestamp, sizeof(MsgTimestamp), false);

    /* Call unit under test, NULL pointer confirms command access is through APIs */
    PLATFORM_ProcessCommandPacket((CFE_SB_Buffer_t *)NULL);

    /* Confirm message sent*/
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_SB_TransmitMsg)) == 1, "CFE_SB_TransmitMsg() called once");
    UtAssert_True(MsgSend == &PLATFORM_APP_Data.HkTlm.TlmHeader.Msg, "CFE_SB_TransmitMsg() address matches expected");

    /* Confirm timestamp msg address */
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_SB_TimeStampMsg)) == 1, "CFE_SB_TimeStampMsg() called once");
    UtAssert_True(MsgTimestamp == &PLATFORM_APP_Data.HkTlm.TlmHeader.Msg,
                  "CFE_SB_TimeStampMsg() address matches expected");

}

void Test_PLATFORM_Noop(void)
{
    /*
     * Test Case For:
     * void PLATFORM_NoopCmd( const PLATFORM_Noop_t *Msg )
     */
    PLATFORM_NoopCmd_t TestMsg;
    UT_CheckEvent_t      EventTest;

    memset(&TestMsg, 0, sizeof(TestMsg));

    /* test dispatch of NOOP */
    UT_CheckEvent_Setup(&EventTest, PLATFORM_COMMANDNOP_INF_EID, NULL);

    UT_TEST_FUNCTION_RC(PLATFORM_Noop(&TestMsg), CFE_SUCCESS);

    /*
     * Confirm that the event was generated
     */
    UtAssert_True(EventTest.MatchCount == 1, "PLATFORM_COMMANDNOP_INF_EID generated (%u)",
                  (unsigned int)EventTest.MatchCount);
}

void Test_PLATFORM_ResetCounters(void)
{
    /*
     * Test Case For:
     * void PLATFORM_ResetCounters( const PLATFORM_ResetCounters_t *Msg )
     */
    PLATFORM_ResetCountersCmd_t TestMsg;
    UT_CheckEvent_t               EventTest;

    memset(&TestMsg, 0, sizeof(TestMsg));

    UT_CheckEvent_Setup(&EventTest, PLATFORM_COMMANDRST_INF_EID, "PLATFORM: RESET command");

    UT_TEST_FUNCTION_RC(PLATFORM_ResetCounters(&TestMsg), CFE_SUCCESS);

    /*
     * Confirm that the event was generated
     */
    UtAssert_True(EventTest.MatchCount == 1, "PLATFORM_COMMANDRST_INF_EID generated (%u)",
                  (unsigned int)EventTest.MatchCount);
}


void Test_PLATFORM_VerifyCmdLength(void)
{
    /*
     * Test Case For:
     * bool PLATFORM_VerifyCmdLength
     */
    UT_CheckEvent_t   EventTest;
    size_t            size    = 1;
    CFE_MSG_FcnCode_t fcncode = 2;
    CFE_SB_MsgId_t    msgid   = CFE_SB_ValueToMsgId(3);

    /*
     * test a match case
     */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &size, sizeof(size), false);
    UT_CheckEvent_Setup(&EventTest, PLATFORM_LEN_ERR_EID,
                        "Invalid Msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u");

    PLATFORM_VerifyCmdLength(NULL, size);

    /*
     * Confirm that the event was NOT generated
     */
    UtAssert_True(EventTest.MatchCount == 0, "PLATFORM_LEN_ERR_EID NOT generated (%u)",
                  (unsigned int)EventTest.MatchCount);

    /*
     * test a mismatch case
     */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &size, sizeof(size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &msgid, sizeof(msgid), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &fcncode, sizeof(fcncode), false);
    PLATFORM_VerifyCmdLength(NULL, size + 1);

    /*
     * Confirm that the event WAS generated
     */
    UtAssert_True(EventTest.MatchCount == 1, "PLATFORM_LEN_ERR_EID generated (%u)",
                  (unsigned int)EventTest.MatchCount);
}

/*
 * Setup function prior to every test
 */
void Platform_UT_Setup(void)
{
    UT_ResetState(0);
}

/*
 * Teardown function after every test
 */
void Platform_UT_TearDown(void) {}

/*
 * Register the test cases to execute with the unit test tool
 */
void UtTest_Setup(void)
{
    ADD_TEST(PLATFORM_APP_Init);
    ADD_TEST(PLATFORM_APP_Main);
    ADD_TEST(PLATFORM_FindAppId);
    
ADD_TEST(PLATFORM_Receive_Sensor_data_Init);
    ADD_TEST(PLATFORM_User_Receive_Sensor_data);
    ADD_TEST(PLATFORM_Find_Pressure_sensorAppId);
    ADD_TEST(PLATFORM_User_Start_Pressure_sensorApp);
    ADD_TEST(PLATFORM_User_Stop_Pressure_sensorApp);


ADD_TEST(PLATFORM_Find_Temperature_sensorAppId);
    ADD_TEST(PLATFORM_User_Start_Temperature_sensorApp);
    ADD_TEST(PLATFORM_User_Stop_Temperature_sensorApp);
    
    ADD_TEST(PLATFORM_ProcessCommandPacket);
    ADD_TEST(PLATFORM_ProcessGroundCommand);
    ADD_TEST(PLATFORM_ReportHousekeeping);
    ADD_TEST(PLATFORM_Noop);
    ADD_TEST(PLATFORM_ResetCounters);
    ADD_TEST(PLATFORM_VerifyCmdLength);

}
