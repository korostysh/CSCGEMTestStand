##
#
# This is the TriDAS/emu/emuDCS/TAMUTestStand Package Makefile
#
##

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)
include $(BUILD_HOME)/emu/rpm_version

# change the optimization level for debugging
CCFlags = -g -O0 -Wall -fPIC
#.PHONY: flag_check
#flag_check:
#	@echo "Checking g++ flags:  $(CCFlags)"

# Packages to be built
#
Project=emu
Package=emuDCS/CSCGEMTestStand
PackageName=CSCGEMTestStand
Description="CSC-GEM Test Stand"


Sources=\
	CSCGEMTestApplication.cc \
	commands.cpp \
	eth_lib.cpp \
	utils.cpp \
	CLCT.cc \
	CSCConstants.cpp \
	Set.cpp
#	GEMPatternGen.cc \
	PatternGen.cc \
	pattern_convert.cc \
	Yuriy_CLCT_PatternGen.cc

IncludeDirs = \
	$(XDAQ_ROOT)/include \
	$(BUILD_HOME)/emu/emuDCS/CSCGEMTestStand/include \
	$(BUILD_HOME)/emu/emuDCS/CSCGEMTestStand/include/emu/pc \
	$(BUILD_HOME)/emu/emuDCS/OnlineDB/include \
	$(BUILD_HOME)/emu/base/include \
	$(BUILD_HOME)/emu/soap/include \
	$(BUILD_HOME)/emu/emuDCS/PeripheralCore/include \
	$(BUILD_HOME)/emu/emuDCS/PeripheralApps/include

TestLibraryDirs = \
	$(XDAQ_ROOT)/lib 

UserCFlags  = 
UserCCFlags = 
UserDynamicLinkFlags = -lboost_regex -lboost_filesystem
UserStaticLinkFlags = 
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = xerces-c xdaq xdata log4cplus toolbox xoap cgicc xcept xgi peer

TestLibraries = xerces-c xdaq xdata log4cplus toolbox xoap cgicc xcept xgi peer \
                mimetic logxmlappender logudpappender asyncresolv config \
                uuid xalan-c xalanMsg xoapfilter tstoreutils tstoreclient tstore \
                b2innub executive pthttp ptfifo xrelay hyperdaq occi nnz11 clntsh ociei \
                emubase emusoap EmuOnlineDB EmuUtils EmuPeripheralCore EmuPeripheralApps EmuConfigDB

#
# Compile the source files and create a shared library
#
DynamicLibrary= EmuCSCGEMTestStand
StaticLibrary= 

Executables=
TestExecutables= 
#	testTableDefinitions.cc \

include $(XDAQ_ROOT)/config/Makefile.rules
include $(XDAQ_ROOT)/config/mfRPM.rules
