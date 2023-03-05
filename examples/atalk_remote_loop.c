/* .+

.context    : aTalk, real time data communication
.title      : test aTalk with remote loop
.kind       : c source
.author     : Fabrizio Pollastri <mxgbot@gmail.com>
.site       : Revello - Italy
.creation   : 16-Sep-2019
.copyright  : (c) 2019 Fabrizio Pollastri
.license    : GNU Lesser General Public License version 3

.description

This program sends blocks of random data with random size, the blocks are
trasmitted through a serial device to a remote host. The remote host must
run another instance of aTalk that echos back the data. The data received
by the local host is checked again the original trasmitted data for errors.

.- */

#define NUMBER_OF_TESTS 100
#define RANDOM_DATA_SIZE_MIN 1
#define RANDOM_DATA_SIZE_MAX 32
#define DATA_SIZE_MAX 32
#define MSG_ENCODER BASE16
#define LINK_DEVICE "/dev/ttyUSB0"
#define LINK_SPEED B9600
#define RECEIVE_TIMEOUT 2000

#include <stdio.h>
#include <time.h>

#include "../src/atalk/atalk.h"
#include "../src/atalk_serial.h"


int main()
{
    int wlen;

    struct aTalk *atalk;
    struct aTalkSerial *serial;
    char link_device[100];
    uint8_t random_data[RANDOM_DATA_SIZE_MAX];
    uint8_t random_data_size;
    uint8_t receive_data[RANDOM_DATA_SIZE_MAX];
    uint8_t receive_data_size;
    int error;
    time_t start;
    time_t now;

    /* init aTalk */
    atalk = atalk_init(DATA_SIZE_MAX,MSG_ENCODER);
    if (atalk == NULL) {
      printf("ERROR: atalk_init failed to allocate aTalk struc.\n");
      exit(1);
    }

    /* ask for device connecting Arduino */
    printf("device connecting Arduino [%s]: ",LINK_DEVICE);
    scanf("%99[^\n]",link_device);
    if (!link_device)
      strcpy(link_device,LINK_DEVICE);

    /* init serial link */
    if (atalk_serial_init(atalk,link_device,LINK_SPEED,RECEIVE_TIMEOUT)) {
      printf("ERROR: atalk_serial_init failed.\n");
      exit(2);
    };

    /* wait for serial link hardware to settle, required by arduino reset
     * triggered by serial control lines */
    sleep(2);

    /* repeat the test, each time changing data */
    for (int i=0; i < NUMBER_OF_TESTS; i++) {

        /* generate a buffer of random data, with random size in the range
	 * ATALK_DATA_SIZE_MIN - ATALK_DATA_SIZE_MAX */
        random_data_size = rand()%(RANDOM_DATA_SIZE_MAX - RANDOM_DATA_SIZE_MIN)
	  + RANDOM_DATA_SIZE_MIN;
        for (unsigned int r=0; r < random_data_size; r++) {
	    random_data[r] = rand();
        }

        /* send data */
	error = atalk_send(atalk,random_data,random_data_size);
	if (error < 0)
	  printf("ERROR: send error %d, errdat %d at test #%d\n",
	    error,atalk->errdat,i);
	else
	  printf("TX: %s\n",atalk->send_msg_encoded);

        /* receive data */
	error = atalk_receive(atalk,receive_data,&receive_data_size);
	if (error < 0)
	  printf("ERROR: receive error %d, errdat %d at test #%d.\n",
	    error,atalk->errdat,i);
	else
	  printf("RX: %s}\n",atalk->receive_msg_encoded);

        /* compare received data with transmitted data */
        for (int j=0; j < receive_data_size; j++) {
	  if (random_data[j] != receive_data[j]) {
            printf("ERROR on data at test #%d.\n", i);
	    printf("ERROR receive data size = %d\n",receive_data_size);
            printf("ERROR tx data[%d] = %x\n",j,random_data[j]);
            printf("ERROR rx data[%d] = %x\n",j,receive_data[j]);
	    //exit(1);
	  }
        }
    }
    serial = (struct aTalkSerial *)atalk->data_link;
    close(serial->fd);
    printf("TEST OK\n");
}

/**** END ****/
