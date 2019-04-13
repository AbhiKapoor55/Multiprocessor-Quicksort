FLAGS = -Wall -g -std=gnu99 
DEPENDENCIES = psort.c helper.c helper.h 

all: psort 

psort: psort.o helper.o  
	gcc -g -Wall -std=gnu99 -o $@ $^

%.o: %.c ${DEPENDENCIES}
	gcc ${FLAGS} -c $<

clean: 
	rm -f *.o psort 

