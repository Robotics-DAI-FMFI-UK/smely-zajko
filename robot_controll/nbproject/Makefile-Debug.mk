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
	${OBJECTDIR}/EvalDireciton.o \
	${OBJECTDIR}/BindSerialPorts.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/Coordinate.o \
	${OBJECTDIR}/GpsThread.o \
	${OBJECTDIR}/PhoneThread.o \
	${OBJECTDIR}/JoystickThread.o \
	${OBJECTDIR}/ImuThread.o \
	${OBJECTDIR}/SbotThread.o \
	${OBJECTDIR}/LocalizationAndPlaning.o


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
LDLIBSOPTIONS=-lcv -lhighgui -lcvaux -lml -lcxcore ../nn_lib/dist/Debug/GNU-Linux-x86/libnn_lib.a -lfann

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Debug.mk dist/Debug/GNU-Linux-x86/robot_controll

dist/Debug/GNU-Linux-x86/robot_controll: ../nn_lib/dist/Debug/GNU-Linux-x86/libnn_lib.a

dist/Debug/GNU-Linux-x86/robot_controll: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/GNU-Linux-x86
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/robot_controll ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/EvalDireciton.o: EvalDireciton.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I../nn_lib -MMD -MP -MF $@.d -o ${OBJECTDIR}/EvalDireciton.o EvalDireciton.cpp

${OBJECTDIR}/BindSerialPorts.o: BindSerialPorts.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I../nn_lib -MMD -MP -MF $@.d -o ${OBJECTDIR}/BindSerialPorts.o BindSerialPorts.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I../nn_lib -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/Coordinate.o: Coordinate.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I../nn_lib -MMD -MP -MF $@.d -o ${OBJECTDIR}/Coordinate.o Coordinate.cpp

${OBJECTDIR}/GpsThread.o: GpsThread.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I../nn_lib -MMD -MP -MF $@.d -o ${OBJECTDIR}/GpsThread.o GpsThread.cpp

${OBJECTDIR}/PhoneThread.o: PhoneThread.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I../nn_lib -MMD -MP -MF $@.d -o ${OBJECTDIR}/PhoneThread.o PhoneThread.cpp

${OBJECTDIR}/JoystickThread.o: JoystickThread.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I../nn_lib -MMD -MP -MF $@.d -o ${OBJECTDIR}/JoystickThread.o JoystickThread.cpp

${OBJECTDIR}/ImuThread.o: ImuThread.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I../nn_lib -MMD -MP -MF $@.d -o ${OBJECTDIR}/ImuThread.o ImuThread.cpp

${OBJECTDIR}/SbotThread.o: SbotThread.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I../nn_lib -MMD -MP -MF $@.d -o ${OBJECTDIR}/SbotThread.o SbotThread.cpp

${OBJECTDIR}/LocalizationAndPlaning.o: LocalizationAndPlaning.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I../nn_lib -MMD -MP -MF $@.d -o ${OBJECTDIR}/LocalizationAndPlaning.o LocalizationAndPlaning.cpp

# Subprojects
.build-subprojects:
	cd ../nn_lib && ${MAKE}  -f Makefile CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} dist/Debug/GNU-Linux-x86/robot_controll

# Subprojects
.clean-subprojects:
	cd ../nn_lib && ${MAKE}  -f Makefile CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc