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
	${OBJECTDIR}/ContextHistogram.o \
	${OBJECTDIR}/ContextProbableRectangle.o \
	${OBJECTDIR}/ModKMeans.o \
	${OBJECTDIR}/VisionBaseNew.o \
	${OBJECTDIR}/VisionHistogram.o \
	${OBJECTDIR}/VisionRegion.o \
	${OBJECTDIR}/VisionRegionReduced.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=-march=corei7 -I/usr/include -I/usr/include/opencv -I/usr/local/include -O3 -ftree-parallelize-loops=4 -floop-parallelize-all -MMD -MP -MF "$@.d"

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libnn_lib.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libnn_lib.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libnn_lib.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libnn_lib.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libnn_lib.a

${OBJECTDIR}/ContextHistogram.o: ContextHistogram.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -o ${OBJECTDIR}/ContextHistogram.o ContextHistogram.cpp

${OBJECTDIR}/ContextProbableRectangle.o: ContextProbableRectangle.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -o ${OBJECTDIR}/ContextProbableRectangle.o ContextProbableRectangle.cpp

${OBJECTDIR}/ModKMeans.o: ModKMeans.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -o ${OBJECTDIR}/ModKMeans.o ModKMeans.cpp

${OBJECTDIR}/VisionBaseNew.o: VisionBaseNew.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -o ${OBJECTDIR}/VisionBaseNew.o VisionBaseNew.cpp

${OBJECTDIR}/VisionHistogram.o: VisionHistogram.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -o ${OBJECTDIR}/VisionHistogram.o VisionHistogram.cpp

${OBJECTDIR}/VisionRegion.o: VisionRegion.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -o ${OBJECTDIR}/VisionRegion.o VisionRegion.cpp

${OBJECTDIR}/VisionRegionReduced.o: VisionRegionReduced.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -o ${OBJECTDIR}/VisionRegionReduced.o VisionRegionReduced.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libnn_lib.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
