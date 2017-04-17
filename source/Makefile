#-
# ==========================================================================
# Copyright (c) 2011 Autodesk, Inc.
# All rights reserved.
# 
# These coded instructions, statements, and computer programs contain
# unpublished proprietary information written by Autodesk, Inc., and are
# protected by Federal copyright law. They may not be disclosed to third
# parties or copied or duplicated in any form, in whole or in part, without
# the prior written consent of Autodesk, Inc.
# ==========================================================================
#+

ifndef INCL_BUILDRULES

#
# Include platform specific build settings
#
TOP := ..
include $(TOP)/buildrules


#
# Always build the local plug-in when make is invoked from the
# directory.
#
all : plugins

endif

#
# Variable definitions
#

SRCDIR := $(TOP)/psdit
DSTDIR := $(TOP)/psdit

psdit_SOURCES  := $(TOP)/psdit/psditCmd.cpp 
psdit_OBJECTS  := $(TOP)/psdit/psditCmd.o 
psdit_PLUGIN   := $(DSTDIR)/psditCmd.$(EXT)
psdit_MAKEFILE := $(DSTDIR)/Makefile

#
# Include the optional per-plugin Makefile.inc
#
#    The file can contain macro definitions such as:
#       {pluginName}_EXTRA_CFLAGS
#       {pluginName}_EXTRA_C++FLAGS
#       {pluginName}_EXTRA_INCLUDES
#       {pluginName}_EXTRA_LIBS
-include $(SRCDIR)/Makefile.inc


#
# Set target specific flags.
#

$(psdit_OBJECTS): CFLAGS   := $(CFLAGS)   $(psdit_EXTRA_CFLAGS)
$(psdit_OBJECTS): C++FLAGS := $(C++FLAGS) $(psdit_EXTRA_C++FLAGS)
$(psdit_OBJECTS): INCLUDES := $(INCLUDES) $(psdit_EXTRA_INCLUDES)

depend_psdit:     INCLUDES := $(INCLUDES) $(psdit_EXTRA_INCLUDES)

$(psdit_PLUGIN):  LFLAGS   := $(LFLAGS) $(psdit_EXTRA_LFLAGS) 
$(psdit_PLUGIN):  LIBS     := $(LIBS)   -lOpenMaya -lOpenMayaAnim -lFoundation -lclew $(psdit_EXTRA_LIBS) 

#
# Rules definitions
#

.PHONY: depend_psdit clean_psdit Clean_psdit


$(psdit_PLUGIN): $(psdit_OBJECTS) 
	-rm -f $@
	$(LD) -o $@ $(LFLAGS) $^ $(LIBS)

depend_psdit :
	makedepend $(INCLUDES) $(MDFLAGS) -f$(DSTDIR)/Makefile $(psdit_SOURCES)

clean_psdit:
	-rm -f $(psdit_OBJECTS)

Clean_psdit:
	-rm -f $(psdit_MAKEFILE).bak $(psdit_OBJECTS) $(psdit_PLUGIN)


plugins: $(psdit_PLUGIN)
depend:	 depend_psdit
clean:	 clean_psdit
Clean:	 Clean_psdit

