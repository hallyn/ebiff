SRC=$(shell ls *.cc)
OBJ=$(subst .cc,.o,$(SRC))

CPPFLAGS=-Wall -g3 -I. -I ../../.. -fpic $(shell pkg-config --cflags gtk+-2.0)
CPPLDFLAGS= $(shell pkg-config --libs gtk+-2.0 gthread-2.0)

LIB=libebiff_notifyer_gtk2.so

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
