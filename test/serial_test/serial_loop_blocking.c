/* .+

.context    : aTalk, real time data communication
.title      : test serial with remote loop
.kind       : c source
.author     : Fabrizio Pollastri <mxgbot@gmail.com>
.site       : Revello - Italy
.creation   : 16-Sep-2019
.copyright  : (c) 2019 Fabrizio Pollastri
.license    : GNU Lesser General Public License version 3

.description

This program sends lines of random printable characters through
a serial device to a remote host. The remote host must echos back all
characters. The character received by the local host is checked against
the original trasmitted one for errors. This is the version using a
blocking read.

.- */

#define NUMBER_OF_LINES 100
#define LINK_DEVICE "/dev/ttyUSB0"
#define LINK_SPEED B9600
#define LINE_LEN 100


#include <errno.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>


int set_serial_attribs(int fd, int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    //tty.c_iflag &= ~( BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_iflag = 0;

    /* canonical mode: one line at a time */
    tty.c_lflag |= ICANON | ISIG;
    tty.c_lflag &= ~(ECHO | ECHOE | ECHONL | IEXTEN);

    /* input control */
    tty.c_iflag &= ~IGNCR;  /* preserve carriage return */
    tty.c_iflag &= ~INPCK;  /* no parity checking */
    tty.c_iflag &= ~INLCR;  /* no NL to CR traslation */
    tty.c_iflag &= ~ICRNL;  /* no CR to NL traslation */
    tty.c_iflag &= ~IUCLC;  /* no upper to lower case mapping */
    tty.c_iflag &= ~IMAXBEL;/* no ring bell at rx buffer full */
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);/* no SW flowcontrol */

    /* no output remapping, no char dependent delays */
    tty.c_oflag = 0;

    /* no additional EOL chars, confirm EOF to be 0x04 */
    tty.c_cc[VEOL] = 0x00;
    tty.c_cc[VEOL2] = 0x00;
    tty.c_cc[VEOF] = 0x04;

    /* set changed attributes really */
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

    int fd;


int main()
{
    char *portname = LINK_DEVICE;
    uint8_t wrlen;
    uint8_t rdlen;

    uint8_t random_data[LINE_LEN + 2];
    uint8_t receive_data[LINE_LEN + 2];
    int error;

    /* set line terminator and string terminator */
    random_data[LINE_LEN] = '\n';
    random_data[LINE_LEN+1] = '\0';

    /* open serial device */
    fd = open(portname, O_RDWR | O_NOCTTY );
    if (fd < 0) {
        printf("Error opening %s: %s\n", portname, strerror(errno));
        return -1;
    }

    /* set baudrate, 8 bits, no parity, 1 stop bit */
    set_serial_attribs(fd,LINK_SPEED);

    /* wait for hardware to settle, required by arduino reset triggered
     * by serial control lines */
    sleep(2);

    /* empty serial buffers, both tx and rx */
    tcflush(fd,TCIOFLUSH);

    /* repeat the test, each time changing data */
    for (int i=0; i < NUMBER_OF_LINES; i++) {

        /* generate a random printable chars */
	for (int j=0; j < LINE_LEN; j++)
	  random_data[j] = rand() % 95 + 32;

        /* send random data */
        wrlen = write(fd,random_data,LINE_LEN+1);
        if (wrlen != LINE_LEN + 1 ) {
            printf("Error from write: %d, %d\n", wrlen, errno);
	}

        /* receive data */
        rdlen = read(fd,receive_data,LINE_LEN+1);

        if (rdlen > 0) {

          /* add string terminator to received data */
          receive_data[LINE_LEN+1] = '\0';

        } else if (rdlen < 0) {
            printf("Error from read: %d: %s\n", rdlen, strerror(errno));
        } else {  /* rdlen == 0 */
            printf("Timeout from read\n");
        }    

        /* compare received data with transmitted data */	
	if (random_data[0] != receive_data[0]) {
            printf("ERROR on data at test #%d.\n", i);
            printf("ERROR tx data = %x, %c\n",random_data[0],random_data[0]);
	    printf("ERROR rx data = %x, %c\n",receive_data[0],receive_data[0]);
	    exit(1);
        }
    }
    printf("TEST OK\n");
}

/**** END ****/
