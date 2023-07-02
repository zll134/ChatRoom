/* ********************************
 * Author:       Zhanglele
 * Description:  压缩管理模块
 * create time:  2023.05.07
 ********************************/

#include "toylz.h"

#include <stdint.h>

#include "pub_def.h"
#include "dict.h"

/**
 * 1、压缩文件格式：
 *    +========+=======+     +=======+
 *    | Header | block | ... | block |
 *    +========+=======+     +=======+
 * 
 * 2、block的总体格式：
 *      +=======+============+
 *      | token | block data |
 *      +=======+============+
 *
 *  block分为literal和match两类.
 *  2.1 Literal
 *    Literal 的总体格式如下，M1M0 表示L1~Ln+5的占用字节,L1~Ln+5表示
 *    表示block data的长度。
 *        +-----------------+-+ ... +-+============+
 *        | 0M1M0 Ln+5~Ln+1 |  Ln~L1  | block data |
 *        +-----------------+-+ ... +-+============+
 *    例子1: Literal 长度在1b~31b的长度
 *        +-----------+============+
 *        | 000 L5-L1 | block data |
 *        +-----------+============+
 *    例子2: Literal长度在32b~8k之间
 *        +------------+-------+============+
 *        | 001 L13-L9 | L8-L1 | block data |
 *        +------------+-------+============+
 *    例子3: Literal长度在8k~2M之间
 *        +-------------+--------+-------+============+
 *        | 001 L21-L17 | L16-L9 | L8-L1 | block data |
 *        +-------------+--------+-------+============+
 *  2.1 Match
 *    Match类型的格式
 *      +----+
 *      | 11 
 *      +
 */

/**
 *  不同level下的滑窗大小：
 *   level  ~  sliding windows
 *     0    ~       4k
 *     1    ~       8k
 *     2    ~       16k
 *     3    ~       32k
 *     4    ~       64k
 *     5    ~       128k
 *     6    ~       256k
 *     7    ~       512k
 *     8    ~       1M
 *     9    ~       2M
 */
int lz_option_preset(lz_options_t *option, int level)
{
    if (level > LZ_MAX_COMPRESS_LEVEL ||
        level < LZ_MIN_COMPRESS_LEVEL) {
      return TOY_ERR_LZ_LEVEL_INVALID;
    }

    uint8_t win_size_pow2[] = {
      12, 13, 14, 15, 16, 17, 18, 19, 20, 21
    };

    option->sliding_win_size = ((uint32_t)1) << win_size_pow2[level];
    return TOY_OK;
}

#define MAX_COPY 32
#define MAX_LEN 264 /* 256 + 8 */
#define MAX_L1_DISTANCE 8192
#define MAX_L2_DISTANCE 8191
#define MAX_FARDISTANCE (65535 + MAX_L2_DISTANCE - 1)

#define HASH_LOG 13
#define HASH_SIZE (1 << HASH_LOG)
#define HASH_MASK (HASH_SIZE - 1)

static uint16_t flz_hash(uint32_t v) {
  uint32_t h = (v * 2654435769LL) >> (32 - HASH_LOG);
  return h & HASH_MASK;
}

/* special case of memcpy: at most MAX_COPY bytes */
static void flz_smallcopy(uint8_t* dest, const uint8_t* src, uint32_t count) {
#if defined(FLZ_ARCH64)
  if (count >= 4) {
    const uint32_t* p = (const uint32_t*)src;
    uint32_t* q = (uint32_t*)dest;
    while (count > 4) {
      *q++ = *p++;
      count -= 4;
      dest += 4;
      src += 4;
    }
  }
#endif
  fastlz_memcpy(dest, src, count);
}

/* special case of memcpy: exactly MAX_COPY bytes */
static void flz_maxcopy(void* dest, const void* src) {
#if defined(FLZ_ARCH64)
  const uint32_t* p = (const uint32_t*)src;
  uint32_t* q = (uint32_t*)dest;
  *q++ = *p++;
  *q++ = *p++;
  *q++ = *p++;
  *q++ = *p++;
  *q++ = *p++;
  *q++ = *p++;
  *q++ = *p++;
  *q++ = *p++;
#else
  fastlz_memcpy(dest, src, MAX_COPY);
#endif
}

static uint8_t* flz_literals(uint32_t runs, const uint8_t* src, uint8_t* dest) {
  while (runs >= MAX_COPY) {
    *dest++ = MAX_COPY - 1;
    flz_maxcopy(dest, src);
    src += MAX_COPY;
    dest += MAX_COPY;
    runs -= MAX_COPY;
  }
  if (runs > 0) {
    *dest++ = runs - 1;
    flz_smallcopy(dest, src, runs);
    dest += runs;
  }
  return dest;
}

static uint8_t* flz1_match(uint32_t len, uint32_t distance, uint8_t* op) {
  --distance;
  if (FASTLZ_UNLIKELY(len > MAX_LEN - 2))
    while (len > MAX_LEN - 2) {
      *op++ = (7 << 5) + (distance >> 8);
      *op++ = MAX_LEN - 2 - 7 - 2;
      *op++ = (distance & 255);
      len -= MAX_LEN - 2;
    }
  if (len < 7) {
    *op++ = (len << 5) + (distance >> 8);
    *op++ = (distance & 255);
  } else {
    *op++ = (7 << 5) + (distance >> 8);
    *op++ = len - 7;
    *op++ = (distance & 255);
  }
  return op;
}

#define FASTLZ_BOUND_CHECK(cond) \
  if (FASTLZ_UNLIKELY(!(cond))) return 0;

int fastlz1_compress(const void* input, int length, void* output) {
  const uint8_t* ip = (const uint8_t*)input;
  const uint8_t* ip_start = ip;
  const uint8_t* ip_bound = ip + length - 4; /* because readU32 */
  const uint8_t* ip_limit = ip + length - 12 - 1;
  uint8_t* op = (uint8_t*)output;

  uint32_t htab[HASH_SIZE];
  uint32_t seq, hash;

  /* initializes hash table */
  for (hash = 0; hash < HASH_SIZE; ++hash) htab[hash] = 0;

  /* we start with literal copy */
  const uint8_t* anchor = ip;
  ip += 2;

  /* main loop */
  while (FASTLZ_LIKELY(ip < ip_limit)) {
    const uint8_t* ref;
    uint32_t distance, cmp;

    /* find potential match */
    do {
      seq = flz_readu32(ip) & 0xffffff;
      hash = flz_hash(seq);
      ref = ip_start + htab[hash];
      htab[hash] = ip - ip_start;
      distance = ip - ref;
      cmp = FASTLZ_LIKELY(distance < MAX_L1_DISTANCE) ? flz_readu32(ref) & 0xffffff : 0x1000000;
      if (FASTLZ_UNLIKELY(ip >= ip_limit)) break;
      ++ip;
    } while (seq != cmp);

    if (FASTLZ_UNLIKELY(ip >= ip_limit)) break;
    --ip;

    if (FASTLZ_LIKELY(ip > anchor)) {
      op = flz_literals(ip - anchor, anchor, op);
    }

    uint32_t len = flz_cmp(ref + 3, ip + 3, ip_bound);
    op = flz1_match(len, distance, op);

    /* update the hash at match boundary */
    ip += len;
    seq = flz_readu32(ip);
    hash = flz_hash(seq & 0xffffff);
    htab[hash] = ip++ - ip_start;
    seq >>= 8;
    hash = flz_hash(seq);
    htab[hash] = ip++ - ip_start;

    anchor = ip;
  }

  uint32_t copy = (uint8_t*)input + length - anchor;
  op = flz_literals(copy, anchor, op);

  return op - (uint8_t*)output;
}

int fastlz1_decompress(const void* input, int length, void* output, int maxout) {
  const uint8_t* ip = (const uint8_t*)input;
  const uint8_t* ip_limit = ip + length;
  const uint8_t* ip_bound = ip_limit - 2;
  uint8_t* op = (uint8_t*)output;
  uint8_t* op_limit = op + maxout;
  uint32_t ctrl = (*ip++) & 31;

  while (1) {
    if (ctrl >= 32) {
      uint32_t len = (ctrl >> 5) - 1;
      uint32_t ofs = (ctrl & 31) << 8;
      const uint8_t* ref = op - ofs - 1;
      if (len == 7 - 1) {
        FASTLZ_BOUND_CHECK(ip <= ip_bound);
        len += *ip++;
      }
      ref -= *ip++;
      len += 3;
      FASTLZ_BOUND_CHECK(op + len <= op_limit);
      FASTLZ_BOUND_CHECK(ref >= (uint8_t*)output);
      fastlz_memmove(op, ref, len);
      op += len;
    } else {
      ctrl++;
      FASTLZ_BOUND_CHECK(op + ctrl <= op_limit);
      FASTLZ_BOUND_CHECK(ip + ctrl <= ip_limit);
      fastlz_memcpy(op, ip, ctrl);
      ip += ctrl;
      op += ctrl;
    }

    if (FASTLZ_UNLIKELY(ip > ip_bound)) break;
    ctrl = *ip++;
  }

  return op - (uint8_t*)output;
}

static uint8_t* flz2_match(uint32_t len, uint32_t distance, uint8_t* op) {
  --distance;
  if (distance < MAX_L2_DISTANCE) {
    if (len < 7) {
      *op++ = (len << 5) + (distance >> 8);
      *op++ = (distance & 255);
    } else {
      *op++ = (7 << 5) + (distance >> 8);
      for (len -= 7; len >= 255; len -= 255) *op++ = 255;
      *op++ = len;
      *op++ = (distance & 255);
    }
  } else {
    /* far away, but not yet in the another galaxy... */
    if (len < 7) {
      distance -= MAX_L2_DISTANCE;
      *op++ = (len << 5) + 31;
      *op++ = 255;
      *op++ = distance >> 8;
      *op++ = distance & 255;
    } else {
      distance -= MAX_L2_DISTANCE;
      *op++ = (7 << 5) + 31;
      for (len -= 7; len >= 255; len -= 255) *op++ = 255;
      *op++ = len;
      *op++ = 255;
      *op++ = distance >> 8;
      *op++ = distance & 255;
    }
  }
  return op;
}


int fastlz2_decompress(const void* input, int length, void* output, int maxout) {
  const uint8_t* ip = (const uint8_t*)input;
  const uint8_t* ip_limit = ip + length;
  const uint8_t* ip_bound = ip_limit - 2;
  uint8_t* op = (uint8_t*)output;
  uint8_t* op_limit = op + maxout;
  uint32_t ctrl = (*ip++) & 31;

  while (1) {
    if (ctrl >= 32) {
      uint32_t len = (ctrl >> 5) - 1;
      uint32_t ofs = (ctrl & 31) << 8;
      const uint8_t* ref = op - ofs - 1;

      uint8_t code;
      if (len == 7 - 1) do {
          FASTLZ_BOUND_CHECK(ip <= ip_bound);
          code = *ip++;
          len += code;
        } while (code == 255);
      code = *ip++;
      ref -= code;
      len += 3;

      /* match from 16-bit distance */
      if (FASTLZ_UNLIKELY(code == 255))
        if (FASTLZ_LIKELY(ofs == (31 << 8))) {
          FASTLZ_BOUND_CHECK(ip < ip_bound);
          ofs = (*ip++) << 8;
          ofs += *ip++;
          ref = op - ofs - MAX_L2_DISTANCE - 1;
        }

      FASTLZ_BOUND_CHECK(op + len <= op_limit);
      FASTLZ_BOUND_CHECK(ref >= (uint8_t*)output);
      fastlz_memmove(op, ref, len);
      op += len;
    } else {
      ctrl++;
      FASTLZ_BOUND_CHECK(op + ctrl <= op_limit);
      FASTLZ_BOUND_CHECK(ip + ctrl <= ip_limit);
      fastlz_memcpy(op, ip, ctrl);
      ip += ctrl;
      op += ctrl;
    }

    if (FASTLZ_UNLIKELY(ip >= ip_limit)) break;
    ctrl = *ip++;
  }

  return op - (uint8_t*)output;
}

int fastlz_decompress(const void* input, int length, void* output, int maxout) {
  /* magic identifier for compression level */
  int level = ((*(const uint8_t*)input) >> 5) + 1;

  if (level == 1) return fastlz1_decompress(input, length, output, maxout);
  if (level == 2) return fastlz2_decompress(input, length, output, maxout);

  /* unknown level, trigger error */
  return 0;
}


int fastlz2_compress(const void* input, int length, void* output) {
  /* ip应该是input */
  const uint8_t* ip = (const uint8_t*)input;
  const uint8_t* ip_start = ip;
  const uint8_t* ip_bound = ip + length - 4; /* because readU32 */
  const uint8_t* ip_limit = ip + length - 12 - 1;
  uint8_t* op = (uint8_t*)output;

  uint32_t htab[HASH_SIZE];
  uint32_t seq, hash;

  /* initializes hash table */
  for (hash = 0; hash < HASH_SIZE; ++hash) htab[hash] = 0;

  /* we start with literal copy */
  const uint8_t* anchor = ip;
  ip += 2;

  /* main loop */
  while (FASTLZ_LIKELY(ip < ip_limit)) {
    const uint8_t* ref;
    uint32_t distance, cmp;

    /* find potential match */
    do {
      /* seq表示ip读取32位的值，并计算hash值 */
      seq = flz_readu32(ip) & 0xffffff;
      hash = flz_hash(seq);

      /* htab[hash]表示到start的距离，ref表示引用的指针位置 */
      ref = ip_start + htab[hash];
      htab[hash] = ip - ip_start;

      /* distance 表示当前位置到重复子串的距离 */
      distance = ip - ref;
      cmp = FASTLZ_LIKELY(distance < MAX_FARDISTANCE) ? flz_readu32(ref) & 0xffffff : 0x1000000;
      if (FASTLZ_UNLIKELY(ip >= ip_limit)) break;
      ++ip;
    } while (seq != cmp);

    if (FASTLZ_UNLIKELY(ip >= ip_limit)) break;

    --ip;

    /* far, needs at least 5-byte match */
    if (distance >= MAX_L2_DISTANCE) {
      if (ref[3] != ip[3] || ref[4] != ip[4]) {
        ++ip;
        continue;
      }
    }

    if (FASTLZ_LIKELY(ip > anchor)) {
      op = flz_literals(ip - anchor, anchor, op);
    }

    uint32_t len = flz_cmp(ref + 3, ip + 3, ip_bound);
    op = flz2_match(len, distance, op);

    /* update the hash at match boundary */
    ip += len;
    seq = flz_readu32(ip);
    hash = flz_hash(seq & 0xffffff);
    htab[hash] = ip++ - ip_start;
    seq >>= 8;
    hash = flz_hash(seq);
    htab[hash] = ip++ - ip_start;

    anchor = ip;
  }

  uint32_t copy = (uint8_t*)input + length - anchor;
  op = flz_literals(copy, anchor, op);

  /* marker for fastlz2 */
  *(uint8_t*)output |= (1 << 5);

  return op - (uint8_t*)output;
}

int lz_compress(const void *in, uint32_t in_len, void *out, uint32_t out_len,
    lz_options_t *option);
{

}

