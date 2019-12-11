# .+ 
#
# .context    : aTalkLinux, real time data communication
# .title      : examples make file
# .kind       : make file
# .author     : Fabrizio Pollastri <mxgbot@gmail.com>
# .site       : Revello - Italy
# .creation   : 17-Nov-2019
# .copyright  : (c) 2019 Fabrizio Pollastri
# .license    : GNU Lesser General Public License version 3
# 
# .-

.PHONY: all clean 


all: atalk_remote_loop


# aTalk remote loop example
atalk_remote_loop: atalk_remote_loop.c ../src/atalk_serial.o ../src/atalk/libatalk.a
	cc -o atalk_remote_loop atalk_remote_loop.c ../src/atalk_serial.o -L../src/atalk -latalk


atalk_serial.o:
	$(MAKE) -C ../src atalk_serial.o

../src/atalk/libatalk.a:
	$(MAKE) -C ../src/atalk libatalk.a

clean:
	rm -f *.o *.a
	rm -f atalk_remote_loop

#### END ####
