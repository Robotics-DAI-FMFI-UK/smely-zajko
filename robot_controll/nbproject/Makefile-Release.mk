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
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/BindSerialPorts.o \
	${OBJECTDIR}/Coordinate.o \
	${OBJECTDIR}/EvalDireciton.o \
	${OBJECTDIR}/GpsThread.o \
	${OBJECTDIR}/HokuyoThread.o \
	${OBJECTDIR}/ImuThread.o \
	${OBJECTDIR}/LocalizationAndPlaning.o \
	${OBJECTDIR}/SbotThread.o \
	${OBJECTDIR}/SensorManagement.o \
	${OBJECTDIR}/Subroutines.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=-march=corei7 -I../rapidxml-1.13 -I/usr/include/opencv -I../nn_lib -I../../fann-2.1.0/src/include -O3 -ftree-parallelize-loops=4 -floop-parallelize-all -MMD -MP -MF "$@.d"

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/lib/x86_64-linux-gnu -L/usr/lib ../nn_lib/dist/Release/GNU-Linux/libnn_lib.a -lopencv_core -lopencv_highgui -lopencv_ml -lfann -lpthread -lopencv_imgproc -lopencv_legacy -march=corei7


# Build Targets
.build-conf:	${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/robot_controll

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/robot_controll: ${OBJECTFILES} ../nn_lib/dist/Release/GNU-Linux/libnn_lib.a
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/robot_controll ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/BindSerialPorts.o: BindSerialPorts.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -o ${OBJECTDIR}/BindSerialPorts.o BindSerialPorts.cpp

${OBJECTDIR}/Coordinate.o: Coordinate.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -o ${OBJECTDIR}/Coordinate.o Coordinate.cpp

${OBJECTDIR}/EvalDireciton.o: EvalDireciton.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -o ${OBJECTDIR}/EvalDireciton.o EvalDireciton.cpp

${OBJECTDIR}/GpsThread.o: GpsThread.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -o ${OBJECTDIR}/GpsThread.o GpsThread.cpp

${OBJECTDIR}/HokuyoThread.o: HokuyoThread.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -o ${OBJECTDIR}/HokuyoThread.o HokuyoThread.cpp

${OBJECTDIR}/ImuThread.o: ImuThread.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -o ${OBJECTDIR}/ImuThread.o ImuThread.cpp

${OBJECTDIR}/LocalizationAndPlaning.o: LocalizationAndPlaning.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -o ${OBJECTDIR}/LocalizationAndPlaning.o LocalizationAndPlaning.cpp

${OBJECTDIR}/SbotThread.o: SbotThread.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -o ${OBJECTDIR}/SbotThread.o SbotThread.cpp

${OBJECTDIR}/SensorManagement.o: SensorManagement.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -o ${OBJECTDIR}/SensorManagement.o SensorManagement.cpp

${OBJECTDIR}/Subroutines.o: Subroutines.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -o ${OBJECTDIR}/Subroutines.o Subroutines.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/robot_controll

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
