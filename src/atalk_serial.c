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

#include "atalk_serial.h"


int atalk_serial_init(struct aTalk *atalk,char *device,
  int speed,uint16_t receive_timeout) {

  struct termios tty;
  struct aTalkSerial *serial;

  /* allocate aTalkSerial structure */
  serial = (struct aTalkSerial *)malloc(sizeof(struct aTalkSerial));
  atalk->data_link = serial;

  /* save arguments */
  serial->device = device;
  serial->speed = speed;

  /* set serial tx and rx functions */
  atalk->msgtx = atalk_serial_tx;
  atalk->msgrx = atalk_serial_rx;

  /* open serial device in blocking mode */
  serial->fd = open(serial->device, O_RDWR | O_NOCTTY);
  if (serial->fd < 0) {
    printf("Error opening %s: %s\n",serial->device,strerror(errno));
    return -1;
  }

  /* prepare serial read by select to have read timeout */
  FD_ZERO(&(serial->fd_set));
  FD_SET(serial->fd,&(serial->fd_set));
  if (receive_timeout >= 0) {
    serial->receive_timeout = malloc(sizeof(struct timespec));
    serial->receive_timeout->tv_sec = receive_timeout / 1000;
    serial->receive_timeout->tv_nsec = receive_timeout % 1000 * 1000000;
  }
  else
    serial->receive_timeout = NULL;

  /* get termios structure */
  if (tcgetattr(serial->fd, &tty) < 0) {
      printf("Error from tcgetattr: %s\n", strerror(errno));
      return -1;
  }

  /* set tx and rx baudrate */
  cfsetospeed(&tty, (speed_t)serial->speed);
  cfsetispeed(&tty, (speed_t)serial->speed);

  /* set no modem ctrl, 8 bit, no parity, 1 stop */
  tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8;         /* 8-bit characters */
  tty.c_cflag &= ~PARENB;     /* no parity bit */
  tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
  tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

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
  if (tcsetattr(serial->fd, TCSANOW, &tty) != 0) {
      printf("Error from tcsetattr: %s\n", strerror(errno));
      return -1;
  }

  /* empty serial buffers, both tx and rx */
  tcflush(serial->fd,TCIOFLUSH);

  return 0;
}


int atalk_serial_tx(struct aTalk *atalk) {

    struct aTalkSerial *serial;
    int msglen;

    /* send message body */
    serial = (struct aTalkSerial *)atalk->data_link;
    msglen = write(serial->fd,atalk->send_msg_encoded,
      strlen(atalk->send_msg_encoded));
    if (msglen < 0) return ATALK_SERIAL_WRITE_ERROR;

    /* append line terminator: needed by canonical mode. */
    if (write(serial->fd,"\n",1) < 0) return ATALK_SERIAL_WRITE_NL_ERROR;

    return msglen;
}


int atalk_serial_rx(struct aTalk *atalk) {

  struct aTalkSerial *serial;
  int retval;
  int msglen;

  /* wait for received data or for timeout */
  serial = (struct aTalkSerial *)atalk->data_link;
  retval = pselect(serial->fd+1,&(serial->fd_set),
      NULL,NULL,serial->receive_timeout,NULL);

  /* check for error or timeout */
  if (retval < 0)
    return ATALK_SERIAL_PSELECT_ERROR;
  else if (retval == 0)
    return ATALK_SERIAL_READ_TIMEOUT;

  /* there is enough data for a non block read: do read */
  msglen = read(serial->fd,atalk->receive_msg_encoded,
    atalk->receive_buf_enc_len);
  if (msglen < 0)
    return ATALK_SERIAL_READ_ERROR;

  /* strip line terminator character */
  atalk->receive_msg_encoded[msglen-1] = '\0';

  return msglen - 1;
}

/**** end ****/
