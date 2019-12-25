/* .+

.context    : aTalk, real time data communication
.title      : aTalk, asynchronous serial link driver
.kind       : c source
.author     : Fabrizio Pollastri <mxgbot@gmail.com>
.site       : Revello - Italy
.creation   : 18-Oct-2019
.copyright  : (c) 2019 Fabrizio Pollastri
.license    : GNU Lesser General Public License version 3

.description

Implementation of an asynchronous serial link driver for the aTalk
software.

.- */

#ifndef ATALK_SERIAL_H
#define ATALK_SERIAL_H

#include <errno.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/select.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "atalk/atalk.h"


/* return error codes */
enum aTalkSerialErrors {
  ATALK_SERIAL_WRITE_ERROR = -1,
  ATALK_SERIAL_WRITE_NL_ERROR = -2,
  ATALK_SERIAL_PSELECT_ERROR = -3,
  ATALK_SERIAL_READ_TIMEOUT = -4,
  ATALK_SERIAL_READ_ERROR = -5
};


/* data definitions */

struct aTalkSerial {
  char * device;
  int speed;
  int fd;
  fd_set fd_set;
  struct timespec *receive_timeout;
};


/* functions prototipes */

int atalk_serial_init(struct aTalk *atalk,char *device,int speed,
  uint16_t receive_timeout);

int atalk_serial_tx(struct aTalk *atalk);

int atalk_serial_rx(struct aTalk *atalk);

#endif	/* ATALK_SERIAL_H */

/**** end ****/
