# .+ 
#
# .context    : aTalk, real time data communication
# .title      : src make file
# .kind       : make file
# .author     : Fabrizio Pollastri <mxgbot@gmail.com>
# .site       : Revello - Italy
# .creation   : 17-Nov-2019
# .copyright  : (c) 2019 Fabrizio Pollastri
# .license    : GNU Lesser General Public License version 3
# 
# .-

.PHONY: all clean


all: libatalk.a atalk_loop.o


## aTalk static library

atalk.o: atalk.c atalk.h
	cc -c -o atalk.o atalk.c

base16.o: base16.c base16.h
	cc -c -o base16.o base16.c

base64.o: base64.c base64.h
	cc -c -o base64.o base64.c

crc_ccitt.o: crc_ccitt.c crc_ccitt.h
	cc -c -o crc_ccitt.o crc_ccitt.c

queue.o: queue.c queue.h
	cc -c -o queue.o queue.c

libobjs = atalk.o base16.o base64.o crc_ccitt.o

libatalk.a: ${libobjs}
	ar rcs libatalk.a ${libobjs}


## aTalk drivers

atalk_loop.o: atalk_loop.c atalk_loop.h
	cc -c -o atalk_loop.o atalk_loop.c


clean:
	rm -f *.o *.a

#### END ####
