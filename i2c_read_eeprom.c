/*
 * Program for connecting to an Arduino running the i2c_read_eeprom.ino
 *
 * Reads the requested number of bytes from the eeprom and saves
 * them in a file with the given name
 *
 * Execute with --help to see instructions.
 *
 * =============================================================================
 *
 * Original author(s):
 *    Andre Richter, andre.o.richter a t gmail d o t com
 *    Lorenzo Cafaro, lorenzo@interstella.net
 *
 * =============================================================================
 *
 * This version by me:
 *    Diogo Rodrigues, https://github.com/DMRodrigues
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <getopt.h>

int ctrlc = 0;
int print_read = -1;
char *file_name = NULL;

void handle_sig(int signum)
{
	ctrlc = 1;
}

struct sigaction int_handler = {
	.sa_handler = handle_sig,
};

int to_termios_baudrate(unsigned int baudrate)
{
	switch (baudrate) {
		case 50:     return B50;
		case 75:     return B75;
		case 110:    return B110;
		case 134:    return B134;
		case 150:    return B150;
		case 200:    return B200;
		case 300:    return B300;
		case 600:    return B600;
		case 1200:   return B1200;
		case 1800:   return B1800;
		case 2400:   return B2400;
		case 4800:   return B4800;
		case 9600:   return B9600;
		case 19200:  return B19200;
		case 38400:  return B38400;
		case 57600:  return B57600;
		case 115200: return B115200;
		case 230400: return B230400;
		default:     return -1;
	}
}

static void print_usage(FILE *stream, const char *program_name)
{
	fprintf(stream, "Usage: %s [OPTIONS...]\n", program_name);
	fprintf(stream, "\n");
	fprintf(stream, "  -t, --tty=/dev/*         tty the arduino is connected to.\n");
	fprintf(stream, "  -b, --baudrate=N         baudrate of tty. Default sketch is 115200.\n");
	fprintf(stream, "  -n, --num_bytes=N[k]     the number of bytes or Kbytes to read from the tty.\n");
	fprintf(stream, "  -f, --format=format      output format (a = ascii, d = dec, h = hex). Default is hexadecimal.\n");
	fprintf(stream, "  -o, --output-file=FILE   name to save FILE. Default is \"eeprom.bin\".\n");
	fprintf(stream, "  -p, --print=y/n          print the read content to screen. Default is yes.\n");
	fprintf(stream, "  -h, --help               print this screen.\n");
	fprintf(stream, "\nExamples:\n");
	fprintf(stream, "  %s -t /dev/ttyACM0 -n 2k\n", program_name);
	fprintf(stream, "  %s -t /dev/ttyACM0 -n 512 -o teste.bin\n", program_name);
	exit(EXIT_FAILURE);
}

int transmit_num_bytes(int fd, unsigned long num_bytes)
{
	unsigned int i;
	unsigned char num_bytes_array[7];

	for (i = 0; i < 7; i++)
		num_bytes_array[i] = (unsigned char)((num_bytes >> (i * 8)) & 0xff);

	return write(fd, num_bytes_array, sizeof(num_bytes_array));
}

unsigned long get_bytes(char* bytes)
{
	unsigned long res = 0;
	if((bytes[strlen(bytes) - 1] == 'k') || (bytes[strlen(bytes) - 1] == 'K')) {
		char temp_bytes[strlen(bytes) - 1];
		memcpy(temp_bytes, bytes, strlen(bytes) - 1);
		res = atoi(temp_bytes) * 1024; /* convert to bytes */
	}
	else {
		res = atoi(bytes);
	}
	return res;
}

int print_control(char* option) {
	if((strcmp(option, "y") == 0) || (strcmp(option, "yes") == 0))
		return 1;
	return 0;
}

/* complex stuff not needed */
void simple_handshake(int fd) {
	int ajuda;
	char buff, buffer[4];
	while(1) {
		/* read until see A */
		for(read(fd, &buff, 1); buff != 'A'; read(fd, &buff, 1));
		buffer[(ajuda = 0)] = buff;
		read(fd, &buff, 1);
		if(buff == 'C') {
			ajuda = 1;
			buffer[ajuda++] = buff;
			break;
		}
		else if(buff == 'A') {
			ajuda = 0;
			buffer[ajuda++] = buff;
			break;
		}
	}
	/* ignore '\0' */
	while(strncmp(buffer, "ACK+", 4) != 0) {
		read(fd, &buff, 1);
		buffer[ajuda++] = buff;
	}
}

void eeprom_read(int fd, unsigned long num_bytes, char format)
{
	unsigned long i = 0;
	unsigned char byte;
	FILE *fp = fopen(file_name, "w");

	while (!ctrlc) {
		if (read(fd, &byte, sizeof(byte)) < 0) {
			if (errno == EINTR)
				printf("\nInterrupted by CTRL-C\n");
			else
				printf("\nInterrupted, errno: %d\n", errno);
		}
		else {
			if(print_read == 1) {
				switch (format) {
					case 'a':
						printf("0x%06lx: %c\n", i, byte);
						break;
					case 'd':
						printf("0x%06lx: %02d\n", i, byte);
						break;
					case 'h':
						printf("0x%06lx: 0x%02x\n", i, byte);
						break;
					default:
						printf("0x%06lx: 0x%02x\n", i, byte);
				}
			}
			fwrite(&byte, sizeof(byte), 1, fp);
			i++;
			if (i == num_bytes)
				break;
		}
	}
	fclose(fp);
	return;
}

int main(int argc, char *argv[])
{
	int fd, next_option;
	int baudrate = B115200, baudrate_by_user = 0;
	unsigned long num_bytes = 0;
	char format = 0;
	char *tty_name = NULL;
	
	struct termios tty_attr, tty_attr_orig;
	speed_t i_speed, o_speed, user_speed;

	extern char *optarg;
	const char* short_options = "t:b:f:n:o:p:h";
	const struct option long_options[] = {
		{ "tty",          required_argument, NULL, 't' },
		{ "baudrate",     required_argument, NULL, 'b' },
		{ "format",       required_argument, NULL, 'f' },
		{ "num_bytes",    required_argument, NULL, 'n' },
		{ "output_name",  required_argument, NULL, 'o' },
		{ "print",        required_argument, NULL, 'p' },
		{ "help",         no_argument,       NULL, 'h' },
		{ 0,              0,                 0,     0  }
	};

	/* no argument provided */
	if(argc == 1) {
		printf("\n	NO ARGUMENTS PROVIDED!\n\n");
		print_usage(stdout, argv[0]);
		return EXIT_FAILURE;
	}

	do {
		next_option = getopt_long(argc, argv, short_options, long_options, NULL);
		switch (next_option) {
			case 't':
				tty_name = optarg;
				break;
			case 'b':
				baudrate = to_termios_baudrate(atoi(optarg));
				baudrate_by_user = 1;
				break;
			case 'f':
				format = optarg[0];
				break;
			case 'n':
				num_bytes = get_bytes(optarg);
				break;
			case 'o':
				file_name = optarg;
				break;
			case 'p':
				print_read = print_control(optarg);
				break;
			case 'h':
				print_usage(stdout, argv[0]);
				break;
			case '?':
				print_usage(stderr, argv[0]);
				break;
			case -1:
				break;
			default:
				abort();
		}
	} while (next_option != -1);

	/* check if setup parameters given and valid */
	if (!tty_name) {
		printf("\nSorry, you did not provide the name of "
		       "your arduino's tty device.\n\n");
		print_usage(stdout, argv[0]);
	}

	if (baudrate < 0) {
		printf("\nSorry, you did not provide a valid baudrate.\n\n");
		print_usage(stdout, argv[0]);
	}
	else if (!baudrate_by_user)
		printf("You did not provide a baudrate, defaulting to 115200.\n");
	user_speed = (speed_t)baudrate;

	if (!format) {
		printf("You did not specified a output format, defaulting to "
		       "h = hexadecimal.\n");
		format = 'h';
	}
	else if (format != 'a' && format != 'd' && format != 'h') {
		printf("\nSorry, invalid output format specified.\n\n");
		print_usage(stdout, argv[0]);
	}

	if (!num_bytes) {
		printf("\nSorry, you did not provide how many bytes you "
		       "want to read out from the eeprom.\n\n");
		print_usage(stdout, argv[0]);
	}

	if (!file_name) {
		printf("You did not provide a output name, defaulting to "
		       "\"eeprom.bin\".\n");
		file_name = "eeprom.bin";
	}

	if (print_read == -1) {
		printf("You did not provide a print option, defaulting to "
		       "yes.\n");
		print_read = 1;
	}

	/* Install ctrl-c signal handler */
	sigaction(SIGINT, &int_handler, 0);

	/* Open tty */
	fd = open(tty_name, O_RDWR);
	if (fd == -1) {
		printf("Error opening %s: %s\n", tty_name, strerror(errno));
		return EXIT_FAILURE;
	}
	printf("%s opened.\n", tty_name);

	/* Get tty config and make backup */
	tcgetattr(fd, &tty_attr);
	tty_attr_orig = tty_attr;

	/* Configure tty */
	i_speed = cfgetispeed(&tty_attr);
	if (i_speed != user_speed)
		cfsetispeed(&tty_attr, user_speed);

	o_speed = cfgetospeed(&tty_attr);
	if (o_speed != user_speed)
		cfsetospeed(&tty_attr, user_speed);

	tty_attr.c_cc[VMIN]  = 1; /* Block reads until 1 byte is available */
	tty_attr.c_cc[VTIME] = 0; /* Never return from read due to timeout */

	tty_attr.c_iflag &= ~(ICRNL | IXON);
	tty_attr.c_oflag &= ~OPOST;
	tty_attr.c_lflag &= ~(ISIG | ICANON | IEXTEN | ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE);

	if (tcsetattr(fd, TCSANOW, &tty_attr) == -1) {
		printf("Error while setting %s options: %s\n", tty_name, strerror(errno));
		close(fd);
		return EXIT_FAILURE;
	}

	printf("%s successfully configured.\n", tty_name);

	sleep(3); /* Wait a second; Prevents that first byte send to arduino gets corrupted */
	/* More time because UN0 resets on opening device
	 * maybe using 10uF cap between reset and ground */

	if(print_read == 1)
		printf("Starting to read.\n\n");

	if ((transmit_num_bytes(fd, num_bytes)) < 0) {
		printf("Error while transfering dump size to arduino: %s\n", strerror(errno));
		tcsetattr(fd, TCSANOW, &tty_attr_orig);
		close(fd);
		return EXIT_FAILURE;
	}
	else {
		eeprom_read(fd, num_bytes, format);
	}

	/* Revert to original tty config */
	if(tcsetattr(fd, TCSANOW, &tty_attr_orig) == -1)
		printf("Error while reverting original tty config: %s\n", strerror(errno));

	close(fd);
	return EXIT_SUCCESS;
}
