###################################################
# Output execution file definition                #
###################################################
PROGRAM = mkimage

###################################################
# Compiler definition                             #
###################################################
CC = gcc
CFLAGS = -Wall -g -std=c99
LD = gcc
DEPENDFLAG = -MM
#LIBS = -lpthread
LIBS = -lm
INCLUDES = 


###################################################
# Source file block definition                    #
###################################################
SRCDIRS = src
SRCS = $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.c))

###################################################
# Object file definition                          #
###################################################
OBJS = $(SRCS:.c=.o)


###################################################
# Include definition                              #
###################################################
INCLUDES += -I/usr/include
INCLUDES += -Iinclude

CFLAGS := $(CFLAGS) $(INCLUDES)
LDFLAGS := $(LIBS)

DEPENDS = $(SRCS:.c=.d)


all: $(PROGRAM)
	cp $< ../../bin

$(PROGRAM): $(OBJS)
	$(CC) $(filter %.o ,$+) $(LDFLAGS) -o $@

%.d:%.c
	$(CC) $(DEPENDFLAG) $(CFLAGS) $< |\
	sed "s?\\(.*\\):?$(basename $<).o $(basename $<).d :?g" \
	> $@ || $(RM) $@

-include $(DEPENDS)

clean:
	rm -f $(DEPENDS) $(OBJS) $(PROGRAM)
