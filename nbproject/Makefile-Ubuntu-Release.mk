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
CND_CONF=Ubuntu-Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/BoundingBox.o \
	${OBJECTDIR}/BufferedImage.o \
	${OBJECTDIR}/Downloader.o \
	${OBJECTDIR}/File.o \
	${OBJECTDIR}/HOGFeature.o \
	${OBJECTDIR}/HOGProcessor.o \
	${OBJECTDIR}/Image.o \
	${OBJECTDIR}/ImageExtractor.o \
	${OBJECTDIR}/ImageIO.o \
	${OBJECTDIR}/Matfile.o \
	${OBJECTDIR}/MatfileReader.o \
	${OBJECTDIR}/OpenCLUtils.o \
	${OBJECTDIR}/PedestrianDetectionBenchmark.o \
	${OBJECTDIR}/PerformanceLap.o \
	${OBJECTDIR}/ReferenceSubImage.o \
	${OBJECTDIR}/SVMClassifier.o \
	${OBJECTDIR}/SeqFileHeader.o \
	${OBJECTDIR}/SeqFileReader.o \
	${OBJECTDIR}/StringUtils.o \
	${OBJECTDIR}/XWindow.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-O3 -fopenmp
CXXFLAGS=-O3 -fopenmp

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/local/cuda-10.1/targets/x86_64-linux/lib/ -lX11 -lpng -ljpeg -lsvm -lOpenCL

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/pedestriandetectionbenchmark

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/pedestriandetectionbenchmark: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/pedestriandetectionbenchmark ${OBJECTFILES} ${LDLIBSOPTIONS} -O3 -fopenmp

${OBJECTDIR}/BoundingBox.o: BoundingBox.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/libsvm -I/usr/local/cuda/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/BoundingBox.o BoundingBox.cpp

${OBJECTDIR}/BufferedImage.o: BufferedImage.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/libsvm -I/usr/local/cuda/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/BufferedImage.o BufferedImage.cpp

${OBJECTDIR}/Downloader.o: Downloader.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/libsvm -I/usr/local/cuda/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Downloader.o Downloader.cpp

${OBJECTDIR}/File.o: File.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/libsvm -I/usr/local/cuda/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/File.o File.cpp

${OBJECTDIR}/HOGFeature.o: HOGFeature.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/libsvm -I/usr/local/cuda/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/HOGFeature.o HOGFeature.cpp

${OBJECTDIR}/HOGProcessor.o: HOGProcessor.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/libsvm -I/usr/local/cuda/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/HOGProcessor.o HOGProcessor.cpp

${OBJECTDIR}/Image.o: Image.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/libsvm -I/usr/local/cuda/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Image.o Image.cpp

${OBJECTDIR}/ImageExtractor.o: ImageExtractor.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/libsvm -I/usr/local/cuda/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ImageExtractor.o ImageExtractor.cpp

${OBJECTDIR}/ImageIO.o: ImageIO.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/libsvm -I/usr/local/cuda/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ImageIO.o ImageIO.cpp

${OBJECTDIR}/Matfile.o: Matfile.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/libsvm -I/usr/local/cuda/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Matfile.o Matfile.cpp

${OBJECTDIR}/MatfileReader.o: MatfileReader.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/libsvm -I/usr/local/cuda/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MatfileReader.o MatfileReader.cpp

${OBJECTDIR}/OpenCLUtils.o: OpenCLUtils.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/libsvm -I/usr/local/cuda/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OpenCLUtils.o OpenCLUtils.cpp

${OBJECTDIR}/PedestrianDetectionBenchmark.o: PedestrianDetectionBenchmark.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/libsvm -I/usr/local/cuda/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PedestrianDetectionBenchmark.o PedestrianDetectionBenchmark.cpp

${OBJECTDIR}/PerformanceLap.o: PerformanceLap.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/libsvm -I/usr/local/cuda/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PerformanceLap.o PerformanceLap.cpp

${OBJECTDIR}/ReferenceSubImage.o: ReferenceSubImage.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/libsvm -I/usr/local/cuda/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ReferenceSubImage.o ReferenceSubImage.cpp

${OBJECTDIR}/SVMClassifier.o: SVMClassifier.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/libsvm -I/usr/local/cuda/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SVMClassifier.o SVMClassifier.cpp

${OBJECTDIR}/SeqFileHeader.o: SeqFileHeader.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/libsvm -I/usr/local/cuda/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SeqFileHeader.o SeqFileHeader.cpp

${OBJECTDIR}/SeqFileReader.o: SeqFileReader.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/libsvm -I/usr/local/cuda/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SeqFileReader.o SeqFileReader.cpp

${OBJECTDIR}/StringUtils.o: StringUtils.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/libsvm -I/usr/local/cuda/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/StringUtils.o StringUtils.cpp

${OBJECTDIR}/XWindow.o: XWindow.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/libsvm -I/usr/local/cuda/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/XWindow.o XWindow.cpp

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/libsvm -I/usr/local/cuda/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
