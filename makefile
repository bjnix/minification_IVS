
UNAME := $(shell uname)

#+_+_+_+_+_+_+_+_+_+_+_+_+_
ifeq ($(UNAME), Linux)

ifeq "$(HOSTNAME)" "ivs.research.mtu.edu"

GLEW_INC=-I/opt/viz/include
GLEW_LIB=-L/opt/viz/lib -lGLEW

endif #end IVS

# We statically link GLEW on CCSR with our binaries so when we
# transfer the binaries to the IVS tiles, glew works properly. CCSR
# and IVS have different glew versions installed in different places
# with different library filenames.
ifeq "$(HOSTNAME)" "ccsr.ee.mtu.edu"

GLEW_INC=-I/usr/local/glew/1.9.0/include
GLEW_LIB=-L/usr/local/glew/1.9.0/lib -Wl,-Bstatic -lGLEW -Wl,-Bdynamic

endif #end CCSR

GLEW_LIB=-lGLEW

OS_LIBS=${GLEW_LIB} -lglut -lGL -lGLU -lX11

endif #end Linux
#--------------------------

#+_+_+_+_+_+_+_+_+_+_+_+_+_
ifeq ($(UNAME), Darwin)

OS_LIBS=-framework GLUT -framework OpenGL -framework Cocoa -L/usr/X11/lib

endif #end Darwin
#--------------------------

MASEXEC=DGRMaster
SLVEXEC=DGRSlave
RLYEXEC=DGRRelay

HEADERS=

GLEW_INC=
GLEW_LIB=-lGLEW



ALL_INC=${GLEW_INC}
ALL_LIB= -lstdc++ -lc -pthread ${OS_LIBS}

MUTSOURCE=testing_around.cpp DGR_framework.cpp
RLYSOURCE=DGRRelay.cpp

CC=g++

FLAGS=-std=c++0x -fpermissive -g -Wall -O2 


all: $(SLVEXEC) $(RLYEXEC) $(MASEXEC)

$(MASEXEC): $(MUTSOURCE) $(HEADERS)
	@echo "=== COMPILING MASTER ==="
	$(CC) -DDGR_MASTER=1 $(FLAGS) $(MUTSOURCE) -o $(MASEXEC) $(ALL_INC) $(ALL_LIB)
	@echo "=== COMPILING SLAVE ==="
	$(CC) $(FLAGS) $(MUTSOURCE) -o $(SLVEXEC) $(ALL_INC) $(ALL_LIB)

$(SLVEXEC): $(MASEXEC)

$(RLYEXEC): $(RLYSOURCE)
	@echo "=== COMPILING RELAY ==="
	$(CC) $(FLAGS) $(RLYSOURCE) -o $(RLYEXEC) $(ALL_INC) $(ALL_LIB)

clean:
	rm -f $(SLVEXEC) $(RLYEXEC) $(MASEXEC) *.o
