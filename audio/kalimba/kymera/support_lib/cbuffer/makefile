############################################################################
# CONFIDENTIAL
# Copyright (c) 2008 - 2017 Qualcomm Technologies International, Ltd.
#
############################################################################
#
# COMPONENT:    makefile
# MODULE:       CBuffer
# DESCRIPTION:  Library makefile.
#
############################################################################


#########################################################################
# Define root directory (relative so we can be installed anywhere)
#########################################################################

BUILD_ROOT = ../../build
include $(BUILD_ROOT)/roots.mkf


#########################################################################
# Enter source files and directories and header directories here.
#
# Makerules will add the standard interface paths
#########################################################################

# Assembler files needed for downloadable capabilities are different from
# other builds
S_SRC += $(if $(findstring $(KALSIM_CONFIG), True),cbuffer_ex_asm.asm ,$(if $(findstring $(CAPABILITY_DOWNLOAD_BUILD), true),dsl_cbuffer_ex_asm.asm, cbuffer_ex_asm.asm))

# C files included for non-downloadable capability builds
#

TARGET_EXE_ROOT =
TARGET_LIB_ROOT = cbuffer

#########################################################################
# Include the standard definitions and rules
#########################################################################

include $(BUILD_ROOT)/makerules.mkf
