SRC=../mdg_unix_main.c ../mdg_chat_client.c
CFLAGS=-Wall -I../../include -DMDG_DYNAMIC_LIBRARY
LDFLAGS=-L. -lmdglib -lm -lpthread -lrt
SO_FILES=libmdglib.so

default: launch-unix_main

launch-unix_main: unix_main
	echo Launching runix chat demo, dynamically linked:
	LD_LIBRARY_PATH=.:${LD_LIBRARY_PATH} ./unix_main

unix_main: $(SRC) $(SO_FILES)
	$(CC) -o $@ $(CFLAGS) $(SRC) $(LDFLAGS)

libmdglib.so:
	cp -p ../../binaries/sandbox/ubuntu/libmdglib.so .

clean:
	rm -f ./unix_main libmdglib.so
	rm -f chat_demo_private.key
