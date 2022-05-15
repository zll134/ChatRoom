#ifndef ZLIB_WRAPPER_H
#define ZLIB_WRAPPER_H

int gzip_compress(void *d, void *s, int size, int block_size,
		int *error);

int gzip_uncompress(void *d, void *s, int size, int outsize, int *error);
#endif