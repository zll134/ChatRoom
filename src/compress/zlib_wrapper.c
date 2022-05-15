
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zlib.h>

#define GZIP_DEFAULT_COMPRESSION_LEVEL 9
#define GZIP_DEFAULT_WINDOW_SIZE 15

struct gzip_strategy {
	int strategy;
	int length;
	void *buffer;
};

struct gzip_stream {
	z_stream stream;
	int strategies;
	struct gzip_strategy strategy[0];
};

static void *gzip_init()
{
	int i, j, res;
	struct gzip_stream *stream;

	stream = malloc(sizeof(*stream) + sizeof(struct gzip_strategy));
	if(stream == NULL)
		return NULL;

	stream->strategies = 1;
	stream->strategy[0].strategy = Z_DEFAULT_STRATEGY;
		
	stream->stream.zalloc = Z_NULL;
	stream->stream.zfree = Z_NULL;
	stream->stream.opaque = 0;
    int compression_level = GZIP_DEFAULT_COMPRESSION_LEVEL;
    short window_size = GZIP_DEFAULT_WINDOW_SIZE;
	res = deflateInit2(&stream->stream, compression_level, Z_DEFLATED,
		window_size, 8, stream->strategy[0].strategy);
	if(res != Z_OK)
		return NULL;

	return stream;
}

int gzip_compress(void *d, void *s, int size, int block_size,
		int *error)
{
	int i, res;
	struct gzip_stream *stream = gzip_init();
    if (stream == NULL) {
        return -1;
    }
    int compression_level = GZIP_DEFAULT_COMPRESSION_LEVEL;
	stream->strategy[0].buffer = d;

	for(i = 0; i < stream->strategies; i++) {
		struct gzip_strategy *strategy = &stream->strategy[i];

		res = deflateReset(&stream->stream);
		if(res != Z_OK)
			goto failed;

		stream->stream.next_in = s;
		stream->stream.avail_in = size;
		stream->stream.next_out = strategy->buffer;
		stream->stream.avail_out = block_size;

		if(stream->strategies > 1) {
			res = deflateParams(&stream->stream,
				compression_level, strategy->strategy);
			if(res != Z_OK)
				goto failed;
		}

		res = deflate(&stream->stream, Z_FINISH);
		strategy->length = stream->stream.total_out;
		if(res == Z_STREAM_END) {
			
		} else if(res != Z_OK)
			goto failed;
	}

	return (int) stream->stream.total_out;

failed:
	/*
	 * All other errors return failure, with the compressor
	 * specific error code in *error
	 */
	*error = res;
	return -1;
}

int gzip_uncompress(void *d, void *s, int size, int outsize, int *error)
{
	int res;
	unsigned long bytes = outsize;

	res = uncompress(d, &bytes, s, size);

	if(res == Z_OK)
		return (int) bytes;
	else {
		*error = res;
		return -1;
	}
}