SRC=$(shell ls *.cc)
OBJ=$(subst .cc,.o,$(SRC))

CPPFLAGS=-Wall -g3 -I. -I ../../.. -fpic
CPPLDFLAGS= -lpthread

LIB=libebiff_notifier_sox.so

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
