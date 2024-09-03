#include "cfe.h"

// Define a simplified version of the CFE_ES_Global structure
typedef struct
{
    CFE_ES_AppId_t            AppId;                    /* The actual AppID of this entry, or undefined */
    char                      AppName[OS_MAX_API_NAME]; /* The name of the app */

} CFE_ES_AppRecord_t;

typedef struct {
    CFE_ES_AppRecord_t AppTable[CFE_PLATFORM_ES_MAX_APPLICATIONS];

} CFE_ES_Global_t;

// Initialize a mock global variable
CFE_ES_Global_t CFE_ES_Global = {0};
