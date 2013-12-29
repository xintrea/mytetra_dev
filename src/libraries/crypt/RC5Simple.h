#ifndef __RC5SIMPLE_H__
#define __RC5SIMPLE_H__

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#include <iostream>
#include <vector>

using namespace std;

#define RC5_SIMPLE_VERSION "RC5Simple Ver. 1.30 / 11.12.2013"

#define RC5_FORMAT_VERSION_1 1
#define RC5_FORMAT_VERSION_2 2
#define RC5_FORMAT_VERSION_3 3
#define RC5_FORMAT_VERSION_CURRENT RC5_FORMAT_VERSION_3

#define RC5_SIMPLE_SIGNATURE "RC5SIMP" // Strong 7 bytes

// For detect 32 or 64 architecture, this code checked LONG_BIT definition.
// In *NIX GCC this definition is present.
// In Windows is uncertain. 
// Try, if need, set this definition at this place.
// Sample:
// #define LONG_BIT 64

// If undestand enviroments
#ifndef LONG_BIT

typedef unsigned long int RC5_TWORD;
#define RC5_ARCHITECTURE 32

#else
// Else definition LONG_BIT is presents

#if LONG_BIT==32
typedef unsigned long int RC5_TWORD;
#define RC5_ARCHITECTURE 32
#endif

#if LONG_BIT==64
typedef unsigned int RC5_TWORD;
#define RC5_ARCHITECTURE 64
#endif

#endif

#ifndef RC5_ARCHITECTURE
#error CAN NOT DETECT MICROPROCESSOR ARCHITECTURE
#endif


#define RC5_W    32             // word size in bits
#define RC5_R    12             // number of rounds
#define RC5_B    16             // number of bytes in key
#define RC5_C     4             // number words in key = ceil(8*b/rc5_w)
#define RC5_T    26             // size of table S = 2*(r+1) words

#define RC5_WORDS_IN_BLOCK 2                           // words in block
#define RC5_BLOCK_LEN     (RC5_W*RC5_WORDS_IN_BLOCK/8) // block size in bytes
#define RC5_WORD_LEN      (RC5_W/8)                    // word size in bytes

// Rotation operators. x must be unsigned, to get logical right shift
#define RC5_ROTL(x,y) (((x)<<(y&(RC5_W-1))) | ((x)>>(RC5_W-(y&(RC5_W-1)))))
#define RC5_ROTR(x,y) (((x)>>(y&(RC5_W-1))) | ((x)<<(RC5_W-(y&(RC5_W-1)))))

#define RC5_MODE_ENCODE 0
#define RC5_MODE_DECODE 1

#define RC5_ERROR_CODE_1 1 // Bad RC5 key length
#define RC5_ERROR_CODE_2 2 // Can't read input file
#define RC5_ERROR_CODE_3 3 // Input file is empty
#define RC5_ERROR_CODE_4 4 // Can't create output file
#define RC5_ERROR_CODE_5 5 // Can't encrypt null data
#define RC5_ERROR_CODE_6 6 // Can't decrypt null data
#define RC5_ERROR_CODE_7 7 // Incorrect data size for decrypt data

// Debugging log to console
// 0 - disable debug print
// 1 - enable debug print
#define RC5_ENABLE_DEBUG_PRINT 0

#if RC5_ENABLE_DEBUG_PRINT==1
#define RC5_LOG(X) RC5_LogPrint X
#else
#define RC5_LOG(X)
#endif


class RC5Simple
{

public:
 RC5Simple(bool enableRandomInit=false);
 virtual ~RC5Simple();

 const char *RC5_GetVersion(void);

 void RC5_EncryptBlock(RC5_TWORD *pt, RC5_TWORD *ct);
 void RC5_DecryptBlock(RC5_TWORD *ct, RC5_TWORD *pt);
 void RC5_Setup(unsigned char *key);

 void RC5_SetKey(vector<unsigned char> &key);
 void RC5_SetFormatVersionForce(unsigned char formatVersion);
 void RC5_Encrypt(vector<unsigned char> &in, vector<unsigned char> &out);
 void RC5_Decrypt(vector<unsigned char> &in, vector<unsigned char> &out);

 void RC5_EncryptFile(unsigned char *in_name, unsigned char *out_name);
 void RC5_EncryptFile(const char *in_name, const char *out_name);

 void RC5_DecryptFile(unsigned char *in_name, unsigned char *out_name);
 void RC5_DecryptFile(const char *in_name, const char *out_name);

 void RC5_EncDecFile(unsigned char *in_name, unsigned char *out_name, int mode);

 unsigned int RC5_GetErrorCode();

private:

 RC5_TWORD rc5_s[RC5_T]; // Expanded key table
 RC5_TWORD rc5_p; // Magic constants one
 RC5_TWORD rc5_q; // Magic constants two

 unsigned char rc5_key[RC5_B];
 unsigned char rc5_formatVersion;
 bool rc5_isSetFormatVersionForce; 

 unsigned int errorCode;

 inline unsigned char RC5_GetByteFromWord(RC5_TWORD w, int n);
 inline unsigned char RC5_GetByteFromInt(unsigned int l, int n);
 inline unsigned int RC5_GetIntFromByte(unsigned char b0,
                                        unsigned char b1, 
                                        unsigned char b2, 
                                        unsigned char b3);
 inline RC5_TWORD RC5_GetWordFromByte(unsigned char b0,
                                      unsigned char b1, 
                                      unsigned char b2, 
                                      unsigned char b3);

 void RC5_EncDec(vector<unsigned char> &in, vector<unsigned char> &out, int mode);

 unsigned int RC5_Rand(unsigned int from, unsigned int to);

 void RC5_LogPrint(char *lpszText, ...);
};

#endif // __RC5SIMPLE_H__
