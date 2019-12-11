# .+ 
#
# .context    : aTalkLinux, real time data communication
# .title      : src make file
# .kind       : make file
# .author     : Fabrizio Pollastri <mxgbot@gmail.com>
# .site       : Revello - Italy
# .creation   : 19-Nov-2019
# .copyright  : (c) 2019 Fabrizio Pollastri
# .license    : GNU Lesser General Public License version 3
# 
# .-

.PHONY: clean pull


atalk_serial.o: atalk_serial.c atalk_serial.h
	cc -c -o atalk_serial.o atalk_serial.c


clean:
	rm -f *.o *.a
	$(MAKE) -C atalk clean
	
pull:
	rm -rf ./atalk
	cp -rdp ../../aTalk/src atalk

#### END ####
