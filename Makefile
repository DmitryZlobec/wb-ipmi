# http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -std=c++0x -MT $@ -MMD -MP -MF $(DEPDIR)/$(notdir $*.Td)

POSTCOMPILE = mv -f $(DEPDIR)/$(notdir $*.Td) $(DEPDIR)/$(notdir $*.d)

ifeq ($(DEB_TARGET_ARCH),armel)
CROSS_COMPILE=arm-linux-gnueabi-
endif

CXX=$(CROSS_COMPILE)g++
CXX_PATH := $(shell which $(CROSS_COMPILE)g++-4.7)

CC=$(CROSS_COMPILE)gcc
CC_PATH := $(shell which $(CROSS_COMPILE)gcc-4.7)

ifneq ($(CXX_PATH),)
	CXX=$(CROSS_COMPILE)g++-4.7
endif

ifneq ($(CC_PATH),)
	CC=$(CROSS_COMPILE)gcc-4.7
endif

#CFLAGS=
DEBUG_CFLAGS=-Wall -ggdb -std=c++0x -O0 -I.
NORMAL_CFLAGS=-Wall -std=c++0x -Os -I.
CFLAGS=$(if $(or $(DEBUG),$(filter test, $(MAKECMDGOALS))), $(DEBUG_CFLAGS),$(NORMAL_CFLAGS))
LDFLAGS= -pthread -lmosquittopp -lmosquitto -ljsoncpp -lwbmqtt

IPMI_BIN=wb-ipmi
IPMI_LIBS=
IPMI_SRCS=Exception.cpp exec.cpp main.cpp
IPMI_OBJS=$(IPMI_SRCS:.cpp=.o)
SRCS=$(IPMI_SRCS)

.PHONY: all clean test

all : $(IPMI_BIN)


%.o : %.cpp $(DEPDIR)/$(notdir %.d)
	${CXX} ${DEPFLAGS} -c $< -o $@ ${CFLAGS}
	$(POSTCOMPILE)

test/%.o : test/%.cpp $(DEPDIR)/$(notdir %.d)
	${CXX} ${DEPFLAGS} -c $< -o $@ ${CFLAGS}
	$(POSTCOMPILE)

$(IPMI_BIN) : main.o $(IPMI_OBJS)
	${CXX} $^ ${LDFLAGS} -o $@ $(IPMI_LIBS)


clean :
	-rm -rf *.o $(IPMI_BIN) $(DEPDIR)


install: all
	install -d $(DESTDIR)
	install -d $(DESTDIR)/etc
	install -d $(DESTDIR)/usr/bin
	install -m 0644  wb-ipmi.json $(DESTDIR)/etc/wb-ipmi.conf
	install -m 0755  $(IPMI_BIN) $(DESTDIR)/usr/bin/$(IPMI_BIN)

$(DEPDIR)/$(notdir %.d): ;

-include $(patsubst %,$(DEPDIR)/%.d,$(notdir $(basename $(SRCS))))


