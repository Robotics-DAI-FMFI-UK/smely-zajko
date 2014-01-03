#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/VisionBase.o \
	${OBJECTDIR}/VisionRegionReduced.o \
	${OBJECTDIR}/VisionRegion.o \
	${OBJECTDIR}/VisionHistogram.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m32
CXXFLAGS=-m32

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Debug.mk dist/Debug/GNU-Linux-x86/libnn_lib.a

dist/Debug/GNU-Linux-x86/libnn_lib.a: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/GNU-Linux-x86
	${RM} dist/Debug/GNU-Linux-x86/libnn_lib.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libnn_lib.a ${OBJECTFILES} 
	$(RANLIB) dist/Debug/GNU-Linux-x86/libnn_lib.a

${OBJECTDIR}/VisionBase.o: VisionBase.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include -I/usr/include/opencv -I/usr/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/VisionBase.o VisionBase.cpp

${OBJECTDIR}/VisionRegionReduced.o: VisionRegionReduced.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include -I/usr/include/opencv -I/usr/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/VisionRegionReduced.o VisionRegionReduced.cpp

${OBJECTDIR}/VisionRegion.o: VisionRegion.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include -I/usr/include/opencv -I/usr/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/VisionRegion.o VisionRegion.cpp

${OBJECTDIR}/VisionHistogram.o: VisionHistogram.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I/usr/include -I/usr/include/opencv -I/usr/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/VisionHistogram.o VisionHistogram.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} dist/Debug/GNU-Linux-x86/libnn_lib.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
