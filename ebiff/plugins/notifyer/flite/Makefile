SRC=$(shell ls *.cc)
OBJ=$(subst .cc,.o,$(SRC))

CPPFLAGS=-Wall -g3 -I. -I ../../.. -fpic 
CPPLDFLAGS= -lflite -lflite_cmu_us_kal16 -lflite_cmulex -lflite_usenglish -lpthread

LIB=libebiff_notifyer_flite.so

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
