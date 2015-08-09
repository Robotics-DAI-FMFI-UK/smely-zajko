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
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/EvalDireciton.o \
	${OBJECTDIR}/HokuyoThread.o \
	${OBJECTDIR}/BindSerialPorts.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/Coordinate.o \
	${OBJECTDIR}/GpsThread.o \
	${OBJECTDIR}/ImuThread.o \
	${OBJECTDIR}/SbotThread.o \
	${OBJECTDIR}/SensorManagement.o \
	${OBJECTDIR}/Subroutines.o \
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
LDLIBSOPTIONS=-L/usr/lib/i386-linux-gnu -L../../fann-2.1.0/src -lopencv_core -lopencv_highgui -lopencv_ml ../nn_lib/dist/Debug/GNU-Linux-x86/libnn_lib.a -lfann -lpthread -lopencv_imgproc -lopencv_legacy

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/robot_controll

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/robot_controll: ../nn_lib/dist/Debug/GNU-Linux-x86/libnn_lib.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/robot_controll: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/robot_controll ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/EvalDireciton.o: EvalDireciton.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I../nn_lib -I../../fann-2.1.0/src/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/EvalDireciton.o EvalDireciton.cpp

${OBJECTDIR}/HokuyoThread.o: HokuyoThread.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I../nn_lib -I../../fann-2.1.0/src/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/HokuyoThread.o HokuyoThread.cpp

${OBJECTDIR}/BindSerialPorts.o: BindSerialPorts.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I../nn_lib -I../../fann-2.1.0/src/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/BindSerialPorts.o BindSerialPorts.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I../nn_lib -I../../fann-2.1.0/src/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/Coordinate.o: Coordinate.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I../nn_lib -I../../fann-2.1.0/src/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Coordinate.o Coordinate.cpp

${OBJECTDIR}/GpsThread.o: GpsThread.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I../nn_lib -I../../fann-2.1.0/src/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/GpsThread.o GpsThread.cpp

${OBJECTDIR}/ImuThread.o: ImuThread.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I../nn_lib -I../../fann-2.1.0/src/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/ImuThread.o ImuThread.cpp

${OBJECTDIR}/SbotThread.o: SbotThread.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I../nn_lib -I../../fann-2.1.0/src/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/SbotThread.o SbotThread.cpp

${OBJECTDIR}/SensorManagement.o: SensorManagement.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I../nn_lib -I../../fann-2.1.0/src/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/SensorManagement.o SensorManagement.cpp

${OBJECTDIR}/Subroutines.o: Subroutines.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I../nn_lib -I../../fann-2.1.0/src/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Subroutines.o Subroutines.cpp

${OBJECTDIR}/LocalizationAndPlaning.o: LocalizationAndPlaning.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../rapidxml-1.13 -I/usr/include/opencv -I../nn_lib -I../../fann-2.1.0/src/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/LocalizationAndPlaning.o LocalizationAndPlaning.cpp

# Subprojects
.build-subprojects:
	cd ../nn_lib && ${MAKE}  -f Makefile CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/robot_controll

# Subprojects
.clean-subprojects:
	cd ../nn_lib && ${MAKE}  -f Makefile CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
