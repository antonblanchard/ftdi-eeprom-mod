#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Note: This doesn't work for FT230X chips */
static uint16_t checksum(uint8_t *eeprom, unsigned long eeprom_size)
{
	uint16_t checksum = 0xaaaa;

	for (unsigned long i = 0; i < eeprom_size/2-1; i++) {
		uint16_t value = eeprom[i*2];
		value += eeprom[(i*2)+1] << 8;

        	checksum = value^checksum;
        	checksum = (checksum << 1) | (checksum >> 15);
	}

	return checksum;
}

static void usage(void)
{
	fprintf(stderr, "Usage: ftdi-eeprom-mod [offset] [byte]\n");
}

int main(int argc, char *argv[])
{
	int fd;
	uint8_t *p;
	struct stat buf;
	uint64_t csum;
	unsigned long offset;
	uint8_t byte;

	if (argc != 4) {
		usage();
		exit(1);
	}

	offset = strtoul(argv[2], NULL, 0);
	byte = strtoul(argv[3], NULL, 0);

	fd = open(argv[1], O_RDWR);
	if (fd == -1) {
		perror("Could not open file\n");
		exit(1);
	}

	p = mmap(NULL, getpagesize(), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (p == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}

	if (fstat(fd, &buf)) {
		perror("fstat");
		exit(1);
	}

	p[offset] = byte;

	csum = checksum(p, buf.st_size);

	p[buf.st_size-2] = csum & 0xff;
	p[buf.st_size-1] = (csum >> 8) & 0xff;
}
