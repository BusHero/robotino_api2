LIBPATH = ../../../../lib
INCPATH = ../../../../include

CFLAGS = -Dlinux32 -Wall -I$(INCPATH)
LDFLAGS += -Wl,-rpath,$(LIBPATH),-rpath -L$(LIBPATH) -lrec_core -lpthread -lboost_thread-gcc42-mt-1_41 -lrec_dataexchange 

CC = g++
OBJ = main.o

TARGET = udp

all: $(TARGET)

clean:
	rm -f *.o $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $? -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $? -o $@
