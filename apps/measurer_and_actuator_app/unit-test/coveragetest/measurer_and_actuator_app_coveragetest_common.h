#ifndef MEASURER_AND_ACTUATOR_APP_COVERAGETEST_COMMON_H
#define MEASURER_AND_ACTUATOR_APP_COVERAGETEST_COMMON_H

/*
 * Includes
 */

#include "utassert.h"
#include "uttest.h"
#include "utstubs.h"

#include "cfe.h"
#include "measurer_and_actuator_app_events.h"
#include "measurer_and_actuator_app.h"
#include "measurer_and_actuator_app_user.h"

/*
 * Macro to call a function and check its int32 return code
 */
#define UT_TEST_FUNCTION_RC(func, exp)                                                                \
    {                                                                                                 \
        int32 rcexp = exp;                                                                            \
        int32 rcact = func;                                                                           \
        UtAssert_True(rcact == rcexp, "%s (%ld) == %s (%ld)", #func, (long)rcact, #exp, (long)rcexp); \
    }

/*
 * Macro to add a test case to the list of tests to execute
 */
#define ADD_TEST(test) UtTest_Add((Test_##test), Measurer_and_Actuator_UT_Setup, Measurer_and_Actuator_UT_TearDown, #test)

/*
 * Setup function prior to every test
 */
void Measurer_and_Actuator_UT_Setup(void);

/*
 * Teardown function after every test
 */
void Measurer_and_Actuator_UT_TearDown(void);

#endif /* MEASURER_AND_ACTUATOR_APP_COVERAGETEST_COMMON_H */
