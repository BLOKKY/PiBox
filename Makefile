#
# falinux 라이블러리를 사용하는	어플용 Makefile
#

#CROSS_PREFIX	= mipsel-linux
#CROSS_PREFIX	= arm-linux
CROSS_PREFIX   =

## 사용자 소스
TARGET			= pb
SRCS			= $(wildcard src/*.cpp)
SRCS			+= $(wildcard src/hardware/*.cpp)
SRCS			+= $(wildcard src/util/*.cpp)

FONT = font

#---------------------------------------------------------------------
## 소스레벨의 라이브러리
DIR_GXLIB		= ../lib/gxlib/

SRCS_GXLIB		= $(DIR_GXLIB)gx.cpp  $(DIR_GXLIB)gxbmp.cpp $(DIR_GXLIB)gxbdf.cpp $(DIR_GXLIB)fontinfo.cpp $(DIR_GXLIB)ksc5601.cpp $(DIR_GXLIB)gxmosaic.cpp $(DIR_GXLIB)gxlayer.cpp
SRCS					+= $(SRCS_GXLIB) 

## 인클루드	디렉토리
INCLUDEDIRS		+= -I. -I../include/gxlib

OBJS					= $(patsubst %.cpp, %.o, $(SRCS))

CFLAGS				= $(INCLUDEDIRS) -W -Wall -O2 -fpermissive
LIBS						= -lpthread -lwiringPi

#---------------------------------------------------------------------
CC				=	$(CROSS_PREFIX)-gcc
CXX				=	$(CROSS_PREFIX)-g++
AR				=	$(CROSS_PREFIX)-ar rc
AR2				=	$(CROSS_PREFIX)-ranlib
RANLIB		=	$(CROSS_PREFIX)-ranlib
LD				=	$(CROSS_PREFIX)-ld
NM				=	$(CROSS_PREFIX)-nm
STRIP			=	$(CROSS_PREFIX)-strip
OBJCOPY	=	$(CROSS_PREFIX)-objcopy
CP	= cp
MV	= mv
#--------------------------------------------------------------------

all	: $(TARGET)

$(TARGET) :	dep $(OBJS)
	@echo "\033[32mBuild final file\033[0m"
	@$(CXX) $(INCLUDEDIRS) $(LDFLAGS) $(OBJS) -o	$@ $(LIBS)
	@$(NM) $(TARGET)	> $(TARGET).map

%.o:%.c
	@echo "\033[32mCompiling $<...\033[0m"
	@$(CC) -c $(CFLAGS) -o $@ $<

%.o:%.cc
	@echo "\033[32mC++ compiling $<...\033[0m"
	@$(CXX) -c $(CFLAGS)	$(CXXFLAGS) -std=c++0x	-o $@ $<
	
%.o:%.cpp
	@echo "\033[32mC++(cpp) compiling $<...\033[0m"
	@$(CXX) -c $(CFLAGS)	$(CXXFLAGS)	-o $@ $<

dep	:
	@echo "\033[32mMaking .depend file...\033[0m"
	@$(CXX) -M $(INCLUDEDIRS)  $(SRCS)	> .depend

clean:
	rm -f *.bak
	rm -f *.map
	rm -f *.o
	rm -f $(DIR_GXLIB)*.o
	rm -f $(TARGET)	core

distclean: clean
	rm -rf .depend
	
git:
	git add font* pb src/* Makefile
	git commit -m "PiBox"
	git push -u origin master


ifeq (.depend,$(wildcard .depend))
include	.depend
endif
