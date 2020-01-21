# CFLAGS += -Wall -W -O3 -flto
CFLAGS += -Wall -W -Wno-unused-parameter -O0 -g
CPPFLAGS += -D_GNU_SOURCE
CPPFLAGS += -D_XOPEN_SOURCE=700
PKGS += tinfo
