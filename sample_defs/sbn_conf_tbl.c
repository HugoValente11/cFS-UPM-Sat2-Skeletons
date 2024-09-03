#include "sbn_tbl.h"
#include "cfe_tbl_filedef.h"

SBN_ConfTbl_t SBN_ConfTbl = {.ProtocolModules = {
                                                },
                             .ProtocolCnt     = 0,
                             .Peers =
                                 {



                                     {
.ProcessorID  = 1 ,
                                      .SpacecraftID = 0x42,
                                      .NetNum       = ,
                                      .ProtocolName = "",
                                      .Address      = "",
                                      .TaskFlags    = SBN_TASK_POLL},

                                 },
                             .PeerCnt = 0};

CFE_TBL_FILEDEF(SBN_ConfTbl, SBN.SBN_ConfTbl, SBN Configuration Table, sbn_conf_tbl.tbl)
