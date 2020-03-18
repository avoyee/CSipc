

CFLAGS = -g -fPIC -rdynamic -funwind-tables -D_GNU_SOURCE -D__WITH_MURMUR
TOOL_CHAIN = arm-linux-gnueabihf-
#TOOL_CHAIN = 
CC := $(TOOL_CHAIN)gcc
AR = $(patsubst %gcc,%,$(CC))ar


libinc = `pwd`/build/include
libpath = `pwd`/build/lib
libsrc = $(wildcard src/*.c)
libobj = $(patsubst %.c,%.o,$(libsrc))

do_clean = $(foreach tar,$(wildcard $(1)),$(shell rm -r $(tar)))

all:lib test
	@echo all done!
lib_install:
ifeq ($(wildcard build),)
	@-mkdir -p build/include
	@-mkdir -p build/lib
endif
	@$(AR) csq $(libpath)/libipc.a $(libobj)
	@$(CC) -shared -o $(libpath)/libipc.so $(libobj)
header_install:
	@cp src/server.h src/client.h src/ipc.h $(libinc)

lib:$(libobj) lib_install header_install
	@echo lib done!
test:
	@echo $(CC) main
	@$(CC) -I $(libinc) -L$(libpath)   -o main main.c  -lrt -pthread  -Wl,-Bdynamic -lipc
%.o:%.c 
	@echo $(CC) $@
	@$(CC) -c -o  $@ $^  $(CFLAGS)
clean:
	$(call do_clean,main)
	$(call do_clean,build)
	$(call do_clean,$(libobj))
	@echo clean done!

