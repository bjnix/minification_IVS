MASEXEC=DGRAssimpMaster
SLVEXEC=DGRAssimpSlave
CC=g++
FLAGS=-O2 -g -Wall -fpermissive

GLEW_INC=
GLEW_LIB=-lGLEW

MAS_SRC=DGRAssimpMaster.cpp
SLV_SRC=DGRAssimpSlave.cpp camera.cpp glut_backend.cpp lighting_technique.cpp math_3d.cpp mesh.cpp pipeline.cpp technique.cpp texture.cpp

# Hardcode path to GLEW since it is installed in a non-standard location.
ifeq "$(HOSTNAME)" "ivs.research.mtu.edu"
GLEW_INC=-I/opt/viz/include
GLEW_LIB=-Wl,-rpath,/opt/viz/lib -L/opt/viz/lib -lGLEW
endif

ifeq "$(HOSTNAME)" "ccsr.ee.mtu.edu"
GLEW_INC=-I/usr/local/glew/1.9.0/include
GLEW_LIB=-Wl,-rpath,/usr/local/glew/1.9.0/lib -L/usr/local/glew/1.9.0/lib -lGLEW
endif
VICON_LIB = -L./vicon-libs -Wl,-rpath,./vicon-libs -lViconDataStreamSDK_CPP
ASSIMP_DIR=/research/bjnix/assimp-lib
ASSIMP_INC=-I$(ASSIMP_DIR)/include/assimp
ASSIMP_LIB=-Wl,-rpath,$(ASSIMP_DIR)/lib -L$(ASSIMP_DIR)/lib -Wl,-Bstatic -lassimp -Wl,-Bdynamic -lz

MAGICK_INC=-I/usr/include/ImageMagick
MAGICK_LIB=-lMagick++

ALL_INC=${GLEW_INC} ${ASSIMP_INC} ${MAGICK_INC}
ALL_LIB=${GLEW_LIB} ${ASSIMP_LIB} ${MAGICK_LIB} ${VICON_LIB}-lglut -lX11 -lGL -lGLU -lstdc++ -lc -pthread -L/opt/intel/fce/COMPILER_VERSION/lib

all: $(MASEXEC) $(SLVEXEC)

$(MASEXEC): ${MAS_SRC}
	${CC} ${FLAGS} ${ALL_INC} -o ${MASEXEC} ${MAS_SRC} ${ALL_LIB}

$(SLVEXEC): ${SLV_SRC}
	${CC} ${FLAGS} ${ALL_INC} -o ${SLVEXEC} ${SLV_SRC} ${ALL_LIB}

clean:
	$(RM) $(MASEXEC) $(SLVEXEC)
