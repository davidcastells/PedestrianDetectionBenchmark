# Environment 
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin


all: benchmark
	

# Building and Cleaning subprojects are done by default, but can be controlled with the SUB
# macro. If SUB=no, subprojects will not be built or cleaned. The following macro
# statements set BUILD_SUB-CONF and CLEAN_SUB-CONF to .build-reqprojects-conf
# and .clean-reqprojects-conf unless SUB has the value 'no'
SUB_no=NO
SUBPROJECTS=${SUB_${SUB}}
BUILD_SUBPROJECTS_=.build-subprojects
BUILD_SUBPROJECTS_NO=
BUILD_SUBPROJECTS=${BUILD_SUBPROJECTS_${SUBPROJECTS}}
CLEAN_SUBPROJECTS_=.clean-subprojects
CLEAN_SUBPROJECTS_NO=
CLEAN_SUBPROJECTS=${CLEAN_SUBPROJECTS_${SUBPROJECTS}}


# Project Name
PROJECTNAME=PedestrianDetectionBenchmark

# Active Configuration
DEFAULTCONF=Ubuntu-Debug
CONF=${DEFAULTCONF}

# All Configurations
ALLCONFS=Cygwin-Debug Cygwin-Release Ubuntu-Debug Ubuntu-Release 






# include project make variables
#
# Generated - do not edit!
#
# NOCDDL
#
CND_BASEDIR=`pwd`
CND_BUILDDIR=build
CND_DISTDIR=dist
# Cygwin-Debug configuration
CND_PLATFORM_Cygwin-Debug=GNU-Linux
CND_ARTIFACT_DIR_Cygwin-Debug=dist/Cygwin-Debug/GNU-Linux
CND_ARTIFACT_NAME_Cygwin-Debug=pedestriandetectionbenchmark
CND_ARTIFACT_PATH_Cygwin-Debug=dist/Cygwin-Debug/GNU-Linux/pedestriandetectionbenchmark
CND_PACKAGE_DIR_Cygwin-Debug=dist/Cygwin-Debug/GNU-Linux/package
CND_PACKAGE_NAME_Cygwin-Debug=pedestriandetectionbenchmark.tar
CND_PACKAGE_PATH_Cygwin-Debug=dist/Cygwin-Debug/GNU-Linux/package/pedestriandetectionbenchmark.tar
# Cygwin-Release configuration
CND_PLATFORM_Cygwin-Release=GNU-Linux
CND_ARTIFACT_DIR_Cygwin-Release=dist/Cygwin-Release/GNU-Linux
CND_ARTIFACT_NAME_Cygwin-Release=pedestriandetectionbenchmark
CND_ARTIFACT_PATH_Cygwin-Release=dist/Cygwin-Release/GNU-Linux/pedestriandetectionbenchmark
CND_PACKAGE_DIR_Cygwin-Release=dist/Cygwin-Release/GNU-Linux/package
CND_PACKAGE_NAME_Cygwin-Release=pedestriandetectionbenchmark.tar
CND_PACKAGE_PATH_Cygwin-Release=dist/Cygwin-Release/GNU-Linux/package/pedestriandetectionbenchmark.tar
# Ubuntu-Debug configuration
CND_PLATFORM_Ubuntu-Debug=GNU-Linux
CND_ARTIFACT_DIR_Ubuntu-Debug=dist/Ubuntu-Debug/GNU-Linux
CND_ARTIFACT_NAME_Ubuntu-Debug=pedestriandetectionbenchmark
CND_ARTIFACT_PATH_Ubuntu-Debug=dist/Ubuntu-Debug/GNU-Linux/pedestriandetectionbenchmark
CND_PACKAGE_DIR_Ubuntu-Debug=dist/Ubuntu-Debug/GNU-Linux/package
CND_PACKAGE_NAME_Ubuntu-Debug=pedestriandetectionbenchmark.tar
CND_PACKAGE_PATH_Ubuntu-Debug=dist/Ubuntu-Debug/GNU-Linux/package/pedestriandetectionbenchmark.tar
# Ubuntu-Release configuration
CND_PLATFORM_Ubuntu-Release=GNU-Linux
CND_ARTIFACT_DIR_Ubuntu-Release=dist/Ubuntu-Release/GNU-Linux
CND_ARTIFACT_NAME_Ubuntu-Release=pedestriandetectionbenchmark
CND_ARTIFACT_PATH_Ubuntu-Release=dist/Ubuntu-Release/GNU-Linux/pedestriandetectionbenchmark
CND_PACKAGE_DIR_Ubuntu-Release=dist/Ubuntu-Release/GNU-Linux/package
CND_PACKAGE_NAME_Ubuntu-Release=pedestriandetectionbenchmark.tar
CND_PACKAGE_PATH_Ubuntu-Release=dist/Ubuntu-Release/GNU-Linux/package/pedestriandetectionbenchmark.tar


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
CND_CONF=Ubuntu-Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile

# Object Directory
OBJECTDIR=build

#OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
COMMONOBJECTFILES= \
	${OBJECTDIR}/BoundingBox.o \
	${OBJECTDIR}/BufferedImage.o \
	${OBJECTDIR}/Downloader.o \
	${OBJECTDIR}/File.o \
	${OBJECTDIR}/HOGFeature.o \
	${OBJECTDIR}/HOGProcessor.o \
	${OBJECTDIR}/FPGAHOGProcessor.o \
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
	${OBJECTDIR}/XWindow.o 
	

BENCHMARKOBJECTFILE= ${OBJECTDIR}/main.o

KERNELTESTOBJECTFILE= ${OBJECTDIR}/kernel_test.o



clean:
	rm -fr ${OBJECTDIR}/*

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-fopenmp -O0 -g
CXXFLAGS=-fopenmp -O0 -g

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/lib/libsvm -L/usr/local/cuda-10.1/targets/x86_64-linux/lib/ -lX11 -ljpeg -lpng -lsvm -lOpenCL

# Build Targets
benchmark: ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/pedestriandetectionbenchmark

kernel_test: ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/kernel_test
	
${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/pedestriandetectionbenchmark: ${COMMONOBJECTFILES} $(BENCHMARKOBJECTFILE)
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/pedestriandetectionbenchmark $(BENCHMARKOBJECTFILE) ${COMMONOBJECTFILES} ${LDLIBSOPTIONS} -fopenmp

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/kernel_test: ${COMMONOBJECTFILES} $(KERNELTESTOBJECTFILE)
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/kernel_test $(KERNELTESTOBJECTFILE) ${COMMONOBJECTFILES} ${LDLIBSOPTIONS} -fopenmp

${OBJECTDIR}/BoundingBox.o: BoundingBox.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/libsvm -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/BoundingBox.o BoundingBox.cpp

${OBJECTDIR}/BufferedImage.o: BufferedImage.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/libsvm -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/BufferedImage.o BufferedImage.cpp

${OBJECTDIR}/Downloader.o: Downloader.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/libsvm -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Downloader.o Downloader.cpp

${OBJECTDIR}/File.o: File.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/libsvm -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/File.o File.cpp

${OBJECTDIR}/HOGFeature.o: HOGFeature.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/libsvm -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/HOGFeature.o HOGFeature.cpp

${OBJECTDIR}/HOGProcessor.o: HOGProcessor.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/libsvm -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/HOGProcessor.o HOGProcessor.cpp

${OBJECTDIR}/FPGAHOGProcessor.o: FPGAHOGProcessor.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/libsvm -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/FPGAHOGProcessor.o FPGAHOGProcessor.cpp

${OBJECTDIR}/Image.o: Image.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/libsvm -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Image.o Image.cpp

${OBJECTDIR}/ImageExtractor.o: ImageExtractor.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/libsvm -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ImageExtractor.o ImageExtractor.cpp

${OBJECTDIR}/ImageIO.o: ImageIO.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/libsvm -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ImageIO.o ImageIO.cpp

${OBJECTDIR}/Matfile.o: Matfile.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/libsvm -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Matfile.o Matfile.cpp

${OBJECTDIR}/MatfileReader.o: MatfileReader.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/libsvm -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MatfileReader.o MatfileReader.cpp

${OBJECTDIR}/OpenCLUtils.o: OpenCLUtils.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/libsvm -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OpenCLUtils.o OpenCLUtils.cpp

${OBJECTDIR}/PedestrianDetectionBenchmark.o: PedestrianDetectionBenchmark.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/libsvm -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PedestrianDetectionBenchmark.o PedestrianDetectionBenchmark.cpp

${OBJECTDIR}/PerformanceLap.o: PerformanceLap.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/libsvm -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PerformanceLap.o PerformanceLap.cpp

${OBJECTDIR}/ReferenceSubImage.o: ReferenceSubImage.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/libsvm -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ReferenceSubImage.o ReferenceSubImage.cpp

${OBJECTDIR}/SVMClassifier.o: SVMClassifier.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/libsvm -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SVMClassifier.o SVMClassifier.cpp

${OBJECTDIR}/SeqFileHeader.o: SeqFileHeader.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/libsvm -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SeqFileHeader.o SeqFileHeader.cpp

${OBJECTDIR}/SeqFileReader.o: SeqFileReader.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/libsvm -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SeqFileReader.o SeqFileReader.cpp

${OBJECTDIR}/StringUtils.o: StringUtils.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/libsvm -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/StringUtils.o StringUtils.cpp

${OBJECTDIR}/XWindow.o: XWindow.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/libsvm -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/XWindow.o XWindow.cpp

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/libsvm -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp


${OBJECTDIR}/kernel_test.o: kernel_test.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/libsvm -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/kernel_test.o kernel_test.cpp

