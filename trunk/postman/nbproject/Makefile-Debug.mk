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
	${OBJECTDIR}/BindSerialPorts.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/ImuThread.o \
	${OBJECTDIR}/SbotThread.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lcv -lhighgui -lcvaux -lml -lcxcore -lfann

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Debug.mk dist/Debug/GNU-Linux-x86/robot_controll

dist/Debug/GNU-Linux-x86/robot_controll: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/GNU-Linux-x86
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/robot_controll ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/BindSerialPorts.o: BindSerialPorts.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I../nn_lib -MMD -MP -MF $@.d -o ${OBJECTDIR}/BindSerialPorts.o BindSerialPorts.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I../nn_lib -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/ImuThread.o: ImuThread.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I../nn_lib -MMD -MP -MF $@.d -o ${OBJECTDIR}/ImuThread.o ImuThread.cpp

${OBJECTDIR}/SbotThread.o: SbotThread.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I../nn_lib -MMD -MP -MF $@.d -o ${OBJECTDIR}/SbotThread.o SbotThread.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} dist/Debug/GNU-Linux-x86/robot_controll

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
