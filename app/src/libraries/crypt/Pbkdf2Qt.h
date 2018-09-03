#ifndef __PBKDF2QT_H__
#define __PBKDF2QT_H__

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <QString>
#include <QByteArray>

#define PBKDF2QT_VERSION "Pbkdf2Qt Ver. 1.06 / 30.05.2011"

#define PBKDF2QT_SHA1_BLOCK_LENGTH   64
#define PBKDF2QT_SHA1_DIGEST_LENGTH  20
#define PBKDF2QT_SALT_SIZE_MAX       1024


class Pbkdf2Qt
{

public:
 Pbkdf2Qt();
 virtual ~Pbkdf2Qt();

 const char *GetVersion(void);

 QByteArray Pbkdf2(QByteArray pass, 
                   QByteArray salt, 
                   unsigned int rounds,
                   unsigned int key_len);

private:

 unsigned int Min(unsigned int a, unsigned int b);
 
 QByteArray HmacSha1(QByteArray text, 
                     QByteArray key);

 int Pbkdf2_calculate(const char *pass, 
                      unsigned int pass_len, 
                      const char *salt, 
                      unsigned int salt_len,
                      char *key, 
                      unsigned int key_len, 
                      unsigned int rounds);

};

#endif // __PBKDF2QT_H__
