SRC=$(shell ls *.cc)
OBJ=$(subst .cc,.o,$(SRC))

CPPFLAGS=-Wall -g3 -I. -I ../../.. -fpic $(shell sdl-config --cflags)
CPPLDFLAGS= $(shell sdl-config --libs)

LIB=libebiff_notifyer_sdlaudio.so

all: $(LIB)

clean:
	rm -f *.o $(LIB) $(SRC:%.cc=.%.d)

$(LIB):$(OBJ)
	gcc -shared  -o $(LIB) $(OBJ) $(CPPLDFLAGS)

%.o:%.cc
	g++ $(CPPFLAGS) -c $< 

.%.d:%.cc 
	g++ $(CPPFLAGS) -MM $< > $@

include $(SRC:%.cc=.%.d)
