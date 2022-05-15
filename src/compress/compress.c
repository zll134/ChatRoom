#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <pthread.h>
#include <regex.h>
#include <sys/wait.h>
#include <limits.h>
#include <ctype.h>
#include <sys/sysinfo.h>
#include <stdint.h>
#include "zlib_wrapper.h"

static long long read_bytes(int fd, void *buff, long long bytes)
{
	long long res, count;

	for(count = 0; count < bytes; count += res) {
		int len = (bytes - count) > SSIZE_MAX ? SSIZE_MAX : bytes - count;

		res = read(fd, buff + count, len);
		if(res < 1) {
			if(res == 0)
				goto bytes_read;
			else if(errno != EINTR) {
				return -1;
			} else
				res = 0;
		}
	}

bytes_read:
	return count;
}
typedef struct {
	void *data;
	uint64_t size;
} data_out_t;

static data_out_t reader_read_file(const char *path)
{
    int fd = open(path, O_RDONLY);
	struct stat buf2;
	int res = fstat(fd, &buf2);
	long long size = buf2.st_size;
	void *buf = (void *)malloc(size);
	read_bytes(fd, buf, size);
	data_out_t out;
	out.data = buf;
	out.size = size;
	return out;
} 

int write_bytes(int fd, void *buff, long long bytes)
{
	long long res, count;

	for(count = 0; count < bytes; count += res) {
		int len = (bytes - count) > SSIZE_MAX ? SSIZE_MAX : bytes - count;

		res = write(fd, buff + count, len);
		if(res == -1) {
			if(errno != EINTR) {
				return -1;
			}
			res = 0;
		}
	}

	return 0;
}
#define BLOCK_SIZE (128 * 1024 * 1024)
int main(int argc, char **argv)
{
	if (argc != 4) {
		printf("./crmcomp <infile> <outfile> <c | d>\n");
		return -1;
	}
	const char *infile = argv[1];
	const char *outfile = argv[2];
	data_out_t in_data = reader_read_file(infile);


	void *d = malloc(BLOCK_SIZE);
	uint64_t outsize = 0;
	if (strcmp(argv[3], "c") == 0) {
		int error;
		outsize = gzip_compress(d, in_data.data, in_data.size, BLOCK_SIZE,
		&error);
	} else if (strcmp(argv[3], "d") ==0) {
		int error;
		outsize = gzip_uncompress(d, in_data.data, in_data.size, BLOCK_SIZE,
		&error);
	}
	if (outsize == -1) {
		printf("comress or decompress failed\n");;
		return -1;
	}
	int fd = open(outfile, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU);
	write_bytes(fd, d, outsize);
	free(d);
	free(in_data.data);
    return 0;
}