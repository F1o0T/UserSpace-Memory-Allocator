#author Matthias Noack <Ma.Noack@tu-cottbus.de>

SRC_FILES = \
<<<<<<< HEAD
	DrawingWindow.cc \
	main.cc
=======
	gui/DrawingWindow.cc
	
>>>>>>> dustin

	
SRC_MAIN = main.cc

TMP_DIR = ./tmp/
BIN_DIR = ./
SRC_DIR = ../src/
INC_DIR = ../include/
DEP_DIR = $(TMP_DIR)
# configure file ending, .cc or .c++
CXX_FILE_ENDING = .cc

ASM	= g++
CXX	= g++
LD	= g++

CXXFLAGS = -g -Wall

INCLUDES = -I$(INC_DIR)

XLIBDIR   = /usr/X11R6/lib/
XLIBS   = -L$(XLIBDIR) -lX11
PTHREADLIBS = -lpthread
LIBS = $(XLIBS) $(PTHREADLIBS)

TARGET_MAIN = $(BIN_DIR)main

######## end of configureable part ########
SRCS = $(addprefix $(SRC_DIR), $(SRC_FILES))
OBJS = $(addprefix $(TMP_DIR), $(subst $(CXX_FILE_ENDING),.o, $(SRC_FILES)))
OBJS_MAIN = $(addprefix $(TMP_DIR), $(subst $(CXX_FILE_ENDING),.o, $(SRC_MAIN)))
DEPS = $(addprefix $(TMP_DIR), $(subst $(CXX_FILE_ENDING),.d, $(SRC_FILES)))
DEPS += $(addprefix $(TMP_DIR), $(subst $(CXX_FILE_ENDING),.d, $(SRC_MAIN))) 
TMP_SUBDIRS = $(sort $(dir $(OBJS)))
.PHONY: clean all depend

all: depend $(TARGET_MAIN)

$(TARGET_MAIN): $(OBJS) $(OBJS_MAIN)
	$(LD) $(LDFLAGS) $(OBJS) $(OBJS_MAIN) $(LIBS) -o $(TARGET_MAIN)

$(TMP_SUBDIRS) :
	mkdir -p $@

$(TMP_DIR)%.o : $(SRC_DIR)%$(CXX_FILE_ENDING) $(TMP_SUBDIRS)
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) -o $@ $<

depend: $(SRCS) $(TMP_SUBDIRS)
	$(CXX) -MD -E $(CXXFLAGS) $(SRCS) $(INCLUDES) > /dev/null
	mv *.d $(DEP_DIR)

main: $(TARGET_MAIN)

clean:
	rm -rf $(OBJS) $(OBJS_MAIN) $(TARGET_MAIN) $(DEPS) $(TMP_SUBDIRS)

# generated dependencies
-include $(DEPS)

