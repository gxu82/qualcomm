/**************************************************************************/
/* Copyright (c) 2018-2018 Qualcomm Technologies International, Ltd.      */
/**************************************************************************/

/* These public C functions, declared in aov_interface.h, are resolved
 * to absolute addresses exported from the patch build.
 */

#include "subsys3_patch0_fw000012B2_map_public.h"

#ifdef DISABLE_PATCH_BUILD_ID_CHECK
.const $_aov_request_notify_trigger     PATCH_ENTRY_AOV_REQUEST_NOTIFY_TRIGGER;
#else /* DISABLE_PATCH_BUILD_ID_CHECK */

.MODULE $M.aov_interface.aov_request_notify_trigger;
.CODESEGMENT PM;
.MINIM;

$_aov_request_notify_trigger:
    rMAC = M[$_patched_fw_version];
    Null = rMAC - PATCH_BUILD_ID;
    if EQ jump  PATCH_ENTRY_AOV_REQUEST_NOTIFY_TRIGGER; 

L_pb_mismatch:
    /* Stub: return false; */
    r0 = 0;
    rts;

.ENDMODULE;

#endif /* DISABLE_PATCH_BUILD_ID_CHECK */

