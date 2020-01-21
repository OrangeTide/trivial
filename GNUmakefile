#
# Universal Build System: Convention over configuration
#
#### DETECT
ARCH := $(shell uname -m | tr A-Z a-z)
ifeq ($(ARCH),x86_64)
ARCH := amd64
endif
ifeq ($(OS),)
OS := $(shell uname -s | tr A-Z a-z)
endif
ifeq ($(OS),Windows_NT)
OS := windows
UNIX :=
endif
ifeq ($(OS),linux)
UNIX = unix
endif
ifeq ($(OS),freebsd)
UNIX = unix
endif
#### COLLECT SOURCE
EXEC := $(notdir $(realpath .))
s := $(wildcard *.c)
s_ := $(wildcard *_*.c)
SRCS := $(filter-out ${s_},$s) $(filter %_$(ARCH).c %_$(OS).c %_$(ARCH)_$(OS).c,$s)
# all posix/unix environments also get *_unix*.c files
ifneq ($(UNIX),)
SRCS += $(filter %_unix.c %_$(ARCH)_unix.c,$s)
endif
OBJS := $(patsubst %.c,%.o,$(SRCS))
#### RULES
all : reportconfig $(EXEC)
reportconfig :
	@echo Configuration
	@echo -------------
	@echo OS=$(OS)
	@echo ARCH=$(ARCH)
	@echo EXEC=${EXEC}
	@echo SRCS=${SRCS}
	@echo OBJS=${OBJS}
	@echo -------------
clean : ; $(RM) $(EXEC) $(OBJS)
.PHONY : all clean reportconfig
$(EXEC) : $(OBJS) ; $(CC) $(LDFLAGS) $(TARGET_ARCH) $^ $(LDLIBS) $(if $(PKGS),$(shell pkg-config --libs $(PKGS))) -o $@
%.o : %.c ; $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) $(if $(PKGS),$(shell pkg-config --cflags $(PKGS)))-c -o $@ $<
# $(EXEC) : CFLAGS = -Wall -W -O0 -g
## TODO: build tool to extract comments
## TODO: use extracted comments to generate CFLAGS for each .o
-include config.mk
