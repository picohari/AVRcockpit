
/*
 *  AVRcockpit Development System for Racing Simulators
 *
 *  Harald W. Leschner, DK6YF - 2015  H9-Laboratory Ltd. 
 *
 *     hari@h9l.net    or visit    labor.h9l.net
 *
 *
 *  Licensed under the GPL license, see license.txt for details.
 *
 *     All text above must be included in any redistribution.
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "datacodes.h"


#define BUFLEN 512			// Network buffer length
#define UDP_PORT 6788		// Listen on port for incoming UDP datagram


void setGear(long gear);
void setRPM(float rpm);
void setSpeed(float meterspersec);
void setHeadLights(bool status);
void setHighbeam(bool status);
void setBlinker(short status, char mode);

void setTEST(unsigned char cha, unsigned char chb, unsigned char chc, unsigned char chd);


/* I²C File descriptors */
int twifd;						// File description
char *fileName = "/dev/i2c-0";	// Name of the port we will be using
unsigned char buf[32];			// Buffer for data being read / written on the I²C bus

int address = 0x10;				// Address of receiving hardware I²C data (AVR or other MCU)


/* Network File descriptors */
struct sockaddr_in my_addr, cli_addr;
int sockfd, i;
socklen_t slen = sizeof(cli_addr);
char netbuf[BUFLEN];

int udp_port = UDP_PORT;


/* Command console options and arguments */
int aflag = 0;
int bflag = 0;
int i2cflag = 0;			// Disables I²C output
static int vlevel = 0;		// Enables Debug messages


/* Print help messages ans usage info */
void print_help(void)
{
	printf("\n");
	printf("**** I²C Server for CARsim ****\n");
	printf(" Version: " GM_VERSION " by DK6YF 2015\n");
	printf("\n");
	printf("Usage:\n");
	printf(" gaugemaster [options] \tstart hardware server \n");
	printf("\n");

	printf(" Must be run as root to enable I²C functionality!\n");
	printf("\n");

	printf("Options:\n");
	printf(" -p <number>\tuse UDP port <number>\n");
	printf(" -i\t\tdisable I²C transmissions\n");
	printf(" -v\t\tprint verbose debug (use -vv.. for depth)\n");

	printf("\n");

	printf("****    Have a nice day!   ****\n");
	printf("\n");

	exit(1);
}


/* Here we go... */
int main(int argc, char **argv)
{

	char *cvalue = NULL;
	int index;
	int c;

	opterr = 0;

	// Check options and arguments provided
	while ((c = getopt(argc, argv, "abcihp:v")) != -1) {
		switch (c) {
		case 'a':
			aflag = 1;
			break;

		case 'b':
			bflag = 1;
			break;

		case 'c':
			cvalue = optarg;
			break;

		case 'v':
			vlevel++;
			break;

		case 'i':
			i2cflag = 1;
			break;

		case 'p':
			udp_port = atoi(optarg);
			break;

		case 'h':
			print_help();
			break;

		case '?':
			if (optopt == 'c')
				fprintf(stderr,
					"Option -%c requires an argument.\n",
					optopt);
			else if (isprint(optopt))
				fprintf(stderr, "Unknown option `-%c'.\n",
					optopt);
			else
				fprintf(stderr,
					"Unknown option character `\\x%x'.\n",
					optopt);
			return 1;
		default:
			abort();
		}
	}

	for (index = optind; index < argc; index++)
		printf("Non-option argument %s\n", argv[index]);

	//printf ("aflag = %d, bflag = %d, cvalue = %s\n", aflag, bflag, cvalue);


	// Say hello...
	printf("\n");
	printf("**** I²C Server for CARsim ****\n");

	// Initialize socket as file descriptor
	printf(" Setup socket connection:\t");

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		printf("ERROR: socket() failed!\n");
	else
		printf("OK\n");

	// Bind to network port provided
	printf(" Bind to network port:\t\t");

	// Set to non-blocking socket
	//int flags = fcntl(sockfd, F_GETFL);
	//flags |= O_NONBLOCK;
	//fcntl(sockfd, F_SETFL, flags);

	// Bind connection to socket
	bzero(&my_addr, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(udp_port);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1)
		printf("ERROR: bind() failed!\n");
	else
		printf("%d\n", udp_port);

	// Print own IP
	printf(" Listening on ip address(es):\n");
	FILE *fp = popen("ifconfig", "r");
	if (fp) {
		char *p = NULL, *e;
		size_t n;
		while ((getline(&p, &n, fp) > 0) && p) {
			if (p = strstr(p, "inet ")) {
				p += 5;
				if (p = strchr(p, ':')) {
					++p;
					if (e = strchr(p, ' ')) {
						*e = '\0';
						printf("\t\t\t\t%s\n", p);
					}
				}
			}
		}
	}
	pclose(fp);


	//printf("\n");


	if (!i2cflag) {
		// Open port for reading and writing
		printf(" Open I²C port as master:\t");
		if ((twifd = open(fileName, O_RDWR)) < 0) {
			printf("ERROR: Failed to open I²C port! - try to run as 'sudo'...\n");
			exit(1);
		}

		if (ioctl(twifd, I2C_SLAVE, address) < 0) {	// Set the port options and set the address of the device we wish to speak to
			printf("ERROR: Unable to get I²C bus access to talk to slave!\n");
			exit(1);
		} else {
			printf("OK\n");
		}
	} else {
		printf("\n**** WARN: I²C DISABLED !! ****\n\n");
	}

	printf(" Server ready...\t\tGentlemen, start your engines!\n\n");

	/** 

	buf[0] = 0;									// This is the register we wish to read software version from
	
	if ((write(twifd, buf, 1)) != 1) {						// Send regiter to read from
		printf("Error writing to i2c slave\n");
		exit(1);
	}
	
	if (read(twifd, buf, 1) != 1) {							// Read back data into buf[]
		printf("Unable to read from slave\n");
		exit(1);
	}
	else {
		printf("Software v: %u\n",buf[0]);
	}
	**/


	int recv_result = 0;	// Length of received bytes
	int recv_counter = 0;	// Counter for parsing single Record Tuples
	int bufcnt = 0;			// Counter for parsing buffer bytes

	short data_sequence;

	short data_what;		// Holds the WHAT value (what is it - of data to be transmitted)
	char data_type;			// Holds the TYPE value (type of it - of data to be transmitted)


	bool t_bool = FALSE;		// temporary values for storing received VALUE of WHAT
	char t_char = 0;
	short t_short = 0;
	long t_long = 0;
	float t_float = 0;
	double t_double = 0;

	uint8_t t_fourbyte[4] = {0,0,0,0};


	while (1) {

		// Blocking function waiting for UPD datagram packet 
		recv_result = recvfrom(sockfd, netbuf, BUFLEN, 0, (struct sockaddr *)&cli_addr, &slen);

		if (recv_result == -1) {
			printf("ERROR: recvfrom() failed!\n");
			exit(1);
		}
		if (vlevel >= 2)
			printf(" Received packet from %s:%d\n Data: %s\n Length: %d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), netbuf, recv_result);

		if (netbuf[0] == BEGIN_TRANSMISSION) {	//This should be the first packet identification...

			memcpy(&data_sequence, &netbuf[1], 2);		// Read sequence number
			if (vlevel >= 1)
				printf("-- ID: %0x --\n", data_sequence);

			recv_counter = 0;	// Holds bytes read for VALUE (1 to 8)
			bufcnt = 3;			// Holds current buffer position

			while (bufcnt < recv_result) {	// Perform scan of received data

				if (netbuf[bufcnt] == RECORD_SEPARATOR) {	// RS byte signals start of data tuple

					memcpy(&data_what, &netbuf[bufcnt + 1], 2);	// Read value of WHAT
					if (vlevel >= 1)
						printf("W:%#0x ", data_what);

					memcpy(&data_type, &netbuf[bufcnt + 3], 1);	// Read value of TYPE
					if (vlevel >= 1)
						printf("T:%#0x ", data_type);

					// First let's fix the buffer counter and data length and check for TYPE
					switch (data_type) {

						case BOOL_T:
							memcpy(&t_bool, &netbuf[bufcnt + 4], sizeof(bool));		// Read bool number             
							recv_counter += sizeof(bool);
							if (vlevel >= 1)
								printf("B:%d ", t_bool);
							break;

						case CHAR_T:
							memcpy(&t_char, &netbuf[bufcnt + 4], sizeof(char));		// Read char number             
							recv_counter += sizeof(char);
							if (vlevel >= 1)
								printf("C:%d ", t_char);
							break;

						case SHORT_T:
							memcpy(&t_short, &netbuf[bufcnt + 4], sizeof(short));	// Read short number            
							recv_counter += sizeof(short);
							if (vlevel >= 1)
								printf("S:%d ", t_short);
							break;

						case LONG_T:
							memcpy(&t_long, &netbuf[bufcnt + 4], sizeof(long));		// Read long number             
							recv_counter += sizeof(long);
							if (vlevel >= 1)
								printf("L:%ld ", t_long);
							break;

						case FLOAT_T:
							memcpy(&t_float, &netbuf[bufcnt + 4], sizeof(float));	// Read float number            
							recv_counter += sizeof(float);
							if (vlevel >= 1)
								printf("F:%f ", t_float);
							break;

						case DOUBLE_T:
							memcpy(&t_double, &netbuf[bufcnt + 4], sizeof(double));	// Read double number           
							recv_counter += sizeof(double);
							if (vlevel >= 1)
								printf("D:%f ", t_double);
							break;


						// For general puprose use T=0x40
						case FOURBYTE_T:
							memcpy(&t_fourbyte, &netbuf[bufcnt + 4], 4);			// Read fourbyte packet           
							recv_counter += 4;
							if (vlevel >= 1)
								printf("T:%x %x %x %x ", t_fourbyte[0], t_fourbyte[1], t_fourbyte[2], t_fourbyte[3]);
							break;


						default:
							printf("ERROR: TYPE not identified!");
							break;
					}

					// Take count of data length received
					bufcnt = (bufcnt + recv_counter + 4);	// Add WHAT and TYPE bytes to counter plus one: we are looking at the next RS or last ETX byte in telegram....
					recv_counter = 0;

					if (vlevel >= 1)
						//printf("buf:%d ", bufcnt);
						printf("\n");


					// Okay, so we have a single data value here, let's check WHAT kind of data we do have received
					switch (data_what) {

						case GEAR:
							setGear(t_long);
							break;
						case ENGRPM:
							setRPM(t_float);
							break;
						case ENGMAXRPM:
							//setEngMaxRPM();
							break;
						case ENGWATERTEMP:
							//setEngWaterTemp();
							break;
						case ENGOILTEMP:
							//setEngOilTemp();
							break;
						case ENGOVERHEAT:
							//setEngOverheat();
							break;
						case FUELTANK:
							//setFuelTank();
							break;
						case METERSPERSEC:
							setSpeed(t_float);
							break;
						case HEADLIGHTS:
							setHeadLights(t_bool);
							break;
						case HIGHBEAM:
							setHighbeam(t_bool);
							break;
						case BLINKER:
							t_short = t_fourbyte[1] + (t_fourbyte[0] << 8);		// First two are the 'short' timing value in ms
							setBlinker(t_short, (char)t_fourbyte[2]);			// Third byte is MODE (blinker mode)
							break;

						default:
							printf("ERROR: DATA not identified!\n");
							break;
					}
					
					data_what = 0;
					data_type = 0;

					// Detect END code and check that all bytes have been read
					if ((netbuf[bufcnt] == END_TRANSMISSION) && ((bufcnt + 1) == recv_result)) {
						//printf("%d \n", bufcnt);
						break;

					}

				}



			} // end while buffer !empty

		}





	} // end while(1)

	close(sockfd);
	return 0;
}




/* Device functions for data manipulation and conversion */


void setGear(long gear)
{

	buf[0] = GEAR;		// register that stores the gear to be set to

	switch (gear) {
		case -1:				// Reverse
			buf[1] = 0x39;		
			break;
		case 0:					// Neutral
			buf[1] = 0x3A;		
			break;
		case 1:					// 1-st
			buf[1] = 0x31;		
			break;
		case 2:					// 2-nd
			buf[1] = 0x32;		
			break;
		case 3:					// 3-rd
			buf[1] = 0x33;		
			break;
		case 4:					// 4-th
			buf[1] = 0x34;		
			break;
		case 5:					// 5-th
			buf[1] = 0x35;		
			break;
		case 6:					// 6-th
			buf[1] = 0x36;		
			break;
		case 7:					// 7-th
			buf[1] = 0x37;		
			break;

		default:				// Everything else pass through
			buf[1] = gear;
	}


	if (!i2cflag) {
		if ((write(twifd, buf, 2)) != 2) {
			printf("ERROR: setGear() writing to i2c slave\n");
			exit(1);
		}
	}
}


void setSpeed(float meterspersec)
{

	if (vlevel == 3)
		printf("m/s: %f " , meterspersec);

	buf[0] = METERSPERSEC;		// register that stores the speed the timer1 is to be set to

	float freq = 0;
	freq = (meterspersec * 3.6);	// Convert to km/h

	// Speedometer is not very linear... We need some adjustments here
	if (freq > 300 ) { freq *= 0.71; } else
	if (freq > 290 ) { freq *= 0.71; } else
	if (freq > 280 ) { freq *= 0.71; } else
	if (freq > 270 ) { freq *= 0.71; } else
	if (freq > 260 ) { freq *= 0.71; } else
	if (freq > 240 ) { freq *= 0.71; } else
	if (freq > 220 ) { freq *= 0.71; } else
	if (freq > 200 ) { freq *= 0.71; } else
	if (freq > 180 ) { freq *= 0.71; } else
	if (freq > 160 ) { freq *= 0.72; } else
	if (freq > 140 ) { freq *= 0.72; } else
	if (freq > 120 ) { freq *= 0.72; } else
	if (freq > 100 ) { freq *= 0.72; } else
	if (freq >  80 ) { freq *= 0.77; } else
	if (freq >  60 ) { freq *= 0.79; } else
	if (freq >  50 ) { freq *= 0.81; } else
	if (freq >  40 ) { freq *= 0.85; } else
	if (freq >  35 ) { freq *= 0.88; } else
	if (freq >  30 ) { freq *= 0.89; } else
	if (freq >  25 ) { freq *= 0.90; } else
	if (freq >  20 ) { freq *= 0.92; } else
	if (freq >  15 ) { freq *= 0.95; } else
	if (freq >  10 ) { freq *= 1.10; } else
	if (freq >   5 ) { freq *= 1.20; } else
					 { freq *= 2.00; };

	float temporary = (16000000.000 / (1024.00 * freq)) -1;

	uint16_t ocrval = (uint16_t) temporary;

	if (vlevel == 3)
		printf("OCR1: %d\n" , ocrval);


	buf[1] = (uint8_t)((ocrval >> 8) & 0xFF);
	buf[2] = (uint8_t)((ocrval & 0xFF));

	if (!i2cflag) {
		if ((write(twifd, buf, 3)) != 3) {
			printf("ERROR: setSpeed() writing to i2c slave\n");
			exit(1);
		}
	}
}


void setRPM(float rpm)
{

	buf[0] = ENGRPM;		// register that stores the rpm the timer3 is to be set to

	float freq = (rpm / 100);

	float temporary = (16000000.000 / (1024.00 * freq * 2)) -1;		// Reversed conversion formula

	uint16_t ocrval = (uint16_t) temporary;

	if (vlevel == 3)
		printf("OCR3: %d\n" , ocrval);


	buf[1] = (uint8_t)((ocrval >> 8) & 0xFF);
	buf[2] = (uint8_t)((ocrval & 0xFF));


	if (!i2cflag) {
		if ((write(twifd, buf, 3)) != 3) {
			printf("ERROR: setRPM() writing to i2c slave\n");
			exit(1);
		}
	}

}


void setHeadLights(bool status)
{
	buf[0] = HEADLIGHTS;

	if (status == TRUE) {
		buf[1] = 1;
	} else {
		buf[1] = 0;
	}

	if (!i2cflag) {
		if ((write(twifd, buf, 2)) != 2) {
			printf("ERROR: setHeadLights() writing to i2c slave\n");
			exit(1);
		}
	}

}


void setHighbeam(bool status)
{
	buf[0] = HIGHBEAM;

	if (status == TRUE) {
		buf[1] = 1;
	} else {
		buf[1] = 0;
	}

	if (!i2cflag) {
		if ((write(twifd, buf, 2)) != 2) {
			printf("ERROR: setHighbeam() writing to i2c slave\n");
			exit(1);
		}
	}

}


void setBlinker(short timing, char mode)
{

	buf[0] = BLINKER;

	if (timing != 0) {
		buf[1] = (uint8_t)((timing >> 8) & 0xFF);		// First two bytes are the short value of blinking interval in ms
		buf[2] = (uint8_t)((timing & 0xFF));
	} else {
		buf[1] = 0;
		buf[2] = 0;
	}

	buf[3] = mode;										// Third byte is defined as MODE (warning, left- or right-hand)

	if (vlevel == 3)
		printf("Sent:%x %x %x %x \n", buf[0], buf[1], buf[2], buf[3]);

	if (!i2cflag) {
		if ((write(twifd, buf, 4)) != 4) {
			printf("ERROR: setBlinker() writing to i2c slave\n");
			exit(1);
		}
	}

}






void setTEST(unsigned char cha, unsigned char chb, unsigned char chc, unsigned char chd)
{

	buf[0] = 0x30;		// register that stores the speed the motor is to be set to
	buf[1] = cha;		// Sepped to be set
	buf[2] = chb;		// Sepped to be set
	buf[3] = chc;		// Sepped to be set
	buf[4] = chd;		// Sepped to be set

	if (!i2cflag) {
		if ((write(twifd, buf, 4)) != 4) {
			printf("ERROR: setRPM() writing to i2c slave\n");
			exit(1);
		}
	}
}