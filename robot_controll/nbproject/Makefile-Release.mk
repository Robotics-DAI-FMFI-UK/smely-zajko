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
	${OBJECTDIR}/_ext/1bea6ee6/Config.o \
	${OBJECTDIR}/_ext/1bea6ee6/EvalDirection.o \
	${OBJECTDIR}/_ext/1bea6ee6/HokuyoThread.o \
	${OBJECTDIR}/BindSerialPorts.o \
	${OBJECTDIR}/Coordinate.o \
	${OBJECTDIR}/GpsThread.o \
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
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/robot_controll

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/robot_controll: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/robot_controll ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/1bea6ee6/Config.o: /home/navigation/src/smely-zajko/robot_controll/Config.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1bea6ee6
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1bea6ee6/Config.o /home/navigation/src/smely-zajko/robot_controll/Config.cpp

${OBJECTDIR}/_ext/1bea6ee6/EvalDirection.o: /home/navigation/src/smely-zajko/robot_controll/EvalDirection.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1bea6ee6
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1bea6ee6/EvalDirection.o /home/navigation/src/smely-zajko/robot_controll/EvalDirection.cpp

${OBJECTDIR}/_ext/1bea6ee6/HokuyoThread.o: /home/navigation/src/smely-zajko/robot_controll/HokuyoThread.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1bea6ee6
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1bea6ee6/HokuyoThread.o /home/navigation/src/smely-zajko/robot_controll/HokuyoThread.cpp

${OBJECTDIR}/BindSerialPorts.o: BindSerialPorts.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/BindSerialPorts.o BindSerialPorts.cpp

${OBJECTDIR}/Coordinate.o: Coordinate.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Coordinate.o Coordinate.cpp

${OBJECTDIR}/GpsThread.o: GpsThread.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/GpsThread.o GpsThread.cpp

${OBJECTDIR}/ImuThread.o: ImuThread.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ImuThread.o ImuThread.cpp

${OBJECTDIR}/LocalizationAndPlaning.o: LocalizationAndPlaning.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/LocalizationAndPlaning.o LocalizationAndPlaning.cpp

${OBJECTDIR}/SbotThread.o: SbotThread.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SbotThread.o SbotThread.cpp

${OBJECTDIR}/SensorManagement.o: SensorManagement.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SensorManagement.o SensorManagement.cpp

${OBJECTDIR}/Subroutines.o: Subroutines.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Subroutines.o Subroutines.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

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
