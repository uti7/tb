# configured for Linux fy7 4.19.128-microsoft-standard #1 SMP Tue Jun 23 12:58:10 UTC 2020 x86_64 x86_64 x86_64 GNU/Linux
### MAKEFILE

########################################################################
# suffies
########################################################################
.SUFFIXES: .cxx .c .o

########################################################################
# programs
########################################################################
CC=gcc
CXX=g++
BTTA_LD=g++
TTBA_LD=gcc
#CTAGS=ctags -VR --c-types=+c+d+e+f+g+m+p+s+t+C
CTAGS=ctags
########################################################################
# flags
########################################################################
INC=-I. 
INCS= @.h ArgParser.h Boolean.h GenArg.h Generic.h InputStream.h \
	Member.h Message.h Ptr.h Record.h Resource.h StringOf.h Vector.h \
	xnew.h StdinputStream.h
DEFS=-DSP_HAVE_BOOL -DSP_API= -DSP_ANSI_LIB
CFLAGS=-O  $(INC) $(DEFS)
CXXFLAGS=-fno-implicit-templates -Wno-deprecated \
          $(CFLAGS)
CPPFLAGS=-P -Sd $(CFLAGS)
LDFLAGS=
# CXXFLAGS+=-Diswprint="0 & "
########################################################################
# targets
########################################################################
PKGNAME=tb14b
EXT=
ASSERTED_TARGET=btta$(EXT)
TARGET=btta.Linux$(EXT)
INSTALL_DIR=/usr/local
INSTALL_BIN_DIR=$(INSTALL_DIR)/bin
COMPLETE_TARGET=$(INSTALL_BIN_DIR)/$(ASSERTED_TARGET)

TTBA_ASSERTED_TARGET=ttba$(EXT)
TTBA_TARGET=ttba.Linux$(EXT)
TTBA_COMPLETE_TARGET=$(INSTALL_BIN_DIR)/$(TTBA_ASSERTED_TARGET)
########################################################################
# sources
########################################################################
SRC=Message.cxx \
	InputStream.cxx \
	ArgParser.cxx \
	Member.cxx \
	btta.cxx \
	btta_inst.cxx 
TTBA_SRC=ttba.c

########################################################################
# objects
########################################################################
OBJ=$(SRC:.cxx=.o)
TTBA_OBJ=$(TTBA_SRC:.c=.o)

########################################################################
# libs
########################################################################
LIB= \
 \


########################################################################
# rules
########################################################################
all : $(TARGET) $(TTBA_TARGET)

$(TARGET): $(OBJ)
	$(BTTA_LD) $(LDFLAGS) -o $(TARGET) $(OBJ) $(LIB)

$(TTBA_TARGET) : $(TTBA_OBJ)
	$(TTBA_LD) $(LDFLAGS) -o $(TTBA_TARGET) $(TTBA_OBJ) $(LIB)

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $<

.c.o:
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f $(OBJ) $(TTBA_OBJ) ./core $(TARGET) $(TTBA_TARGET)

depend:
	$(CXX) $(CXXFLAGS) -MM $(SRC) > Makefile.dep
	$(CC) $(CFLAGS) -MM $(TTBA_SRC) >> Makefile.dep

#cpp:
#	cpp32 $(CPPFLAGS) dummy.cpp

cpp:
	$(CXX) -E $(CXXFLAGS) $(SRC) | awk 'NF>0' > $(TARGET).cppout

tags: $(SRC) $(INCS)
	$(CTAGS) $(SRC) $(INCS)

dist: clean
	rm -f @.h Makefile Makefile.dep
	sh -c "(cd ..; tar cvfz $(PKGNAME).tar.gz $(PKGNAME))"

install:
	mkdir -p $(INSTALL_BIN_DIR)
	strip $(TARGET)
	rm -f $(COMPLETE_TARGET)
	cp -p $(TARGET) $(COMPLETE_TARGET)
	strip $(TTBA_TARGET)
	rm -f $(TTBA_COMPLETE_TARGET)
	cp -p $(TTBA_TARGET) $(TTBA_COMPLETE_TARGET)

install_fear:
	mkdir -p $(INSTALL_BIN_DIR)
	strip $(TARGET)
	cp -p $(TARGET) $(INSTALL_BIN_DIR)
	strip $(TTBA_TARGET)
	cp -p $(TTBA_TARGET) $(INSTALL_BIN_DIR)

########################################################################
include Makefile.dep
### ELIFEKAM
