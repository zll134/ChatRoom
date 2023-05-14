/* ********************************
 * Author:       Zhanglele
 * Description:  压缩管理模块
 * create time:  2023.05.07
 ********************************/
#ifndef TOYLZ_H
#define TOYLZ_H

#define FASTLZ_VERSION 0x000500

#define FASTLZ_VERSION_MAJOR 0
#define FASTLZ_VERSION_MINOR 5
#define FASTLZ_VERSION_REVISION 0

#define FASTLZ_VERSION_STRING "0.5.0"

int lz_decompress(const void* input, int length, void* output, int maxout);

int lz_compress(const void* input, int length, void* output);

#endif