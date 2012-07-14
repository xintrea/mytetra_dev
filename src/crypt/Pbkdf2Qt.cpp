/*
Pbkdf2Qt C++ library

The easy implementation of PBKDF2 algorythm

Copyright 2011 Sergey M. Stepanov

Contact: xintrea@gmail.com, www.webhamster.ru

License: GPL v.3 and BSD

Volgodonsk, 2011
*/
#include <QCryptographicHash>

#include "Pbkdf2Qt.h"


Pbkdf2Qt::Pbkdf2Qt(void)
{ 

}


Pbkdf2Qt::~Pbkdf2Qt(void)
{ 

}


const char *Pbkdf2Qt::GetVersion(void)
{
 static const char *lib_version=PBKDF2QT_VERSION;
 return(lib_version);
}

unsigned int Pbkdf2Qt::Min(unsigned int a, unsigned int b)
{
 return (a<b)?a:b;
}


// HMAC-SHA-1 (from RFC 2202)
QByteArray Pbkdf2Qt::HmacSha1(QByteArray text, 
                               QByteArray key)
{
 QCryptographicHash ctx(QCryptographicHash::Sha1);
 
 if(key.length() > PBKDF2QT_SHA1_BLOCK_LENGTH) 
 {
  ctx.reset();
  ctx.addData(key);

  key=ctx.result();
  key.truncate(PBKDF2QT_SHA1_DIGEST_LENGTH);
 }

 QByteArray digest;
 QByteArray k_pad(PBKDF2QT_SHA1_BLOCK_LENGTH, 0); 

 k_pad.fill(0);
 k_pad.replace(0, key.length(), key);
 for(int i = 0; i < PBKDF2QT_SHA1_BLOCK_LENGTH; i++)
  k_pad[i]=k_pad[i]^0x36;

 ctx.reset();
 ctx.addData(k_pad, PBKDF2QT_SHA1_BLOCK_LENGTH);
 ctx.addData(text);
 digest=ctx.result();

 k_pad.fill(0);
 k_pad.replace(0, key.length(), key);
 for(int i = 0; i < PBKDF2QT_SHA1_BLOCK_LENGTH; i++)
  k_pad[i]=k_pad[i]^0x5c;

 ctx.reset();
 ctx.addData(k_pad, PBKDF2QT_SHA1_BLOCK_LENGTH);
 ctx.addData(digest, PBKDF2QT_SHA1_DIGEST_LENGTH);
 digest=ctx.result();

 return digest;
}
 
 
// Password-Based Key Derivation Function 2 (PKCS #5 v2.0).
// Code based on IEEE Std 802.11-2007, Annex H.4.2.
int Pbkdf2Qt::Pbkdf2_calculate(const char *pass, 
                               unsigned int pass_len, 
                               const char *salt, 
                               unsigned int salt_len,
                               char *key, 
                               unsigned int key_len, 
                               unsigned int rounds)
{
 char *asalt, obuf[PBKDF2QT_SHA1_DIGEST_LENGTH];
 unsigned int i, j;
 unsigned int count;
 unsigned int r;
 
 if (rounds < 1 || key_len == 0) 
  return -1;
 if (salt_len == 0 || salt_len > PBKDF2QT_SALT_SIZE_MAX - 1)
  return -1;
 if ((asalt = (char*)malloc(salt_len + 4)) == NULL)
  return -1;

 memcpy(asalt, salt, salt_len);

 QByteArray d1;
 QByteArray d2;
 
 for(count = 1; key_len > 0; count++) 
  {
   asalt[salt_len + 0] = (count >> 24) & 0xff;
   asalt[salt_len + 1] = (count >> 16) & 0xff;
   asalt[salt_len + 2] = (count >> 8) & 0xff;
   asalt[salt_len + 3] = count & 0xff;

   QByteArray ba_asalt((const char *)asalt, salt_len+4);
   QByteArray ba_pass(pass, pass_len);
   d1=HmacSha1(ba_asalt, ba_pass);

   memcpy(obuf, d1.data(), sizeof(obuf));
   
   for(i = 1; i < rounds; i++) 
    {
     d2=HmacSha1(d1, ba_pass);

     d1.replace(0, d1.size(), d2);

     for(j = 0; j < sizeof(obuf); j++)
      obuf[j] ^= d1[j];
    }

   r = Min((unsigned int)key_len, (unsigned int)PBKDF2QT_SHA1_DIGEST_LENGTH);
   memcpy(key, obuf, r);
   key += r;
   key_len -= r;
  }

 memset(asalt, 0, salt_len + 4);
 free(asalt);

 memset(obuf, 0, sizeof(obuf));

 d1.fill(0);
 d2.fill(0);

 return 0;
}
 

QByteArray Pbkdf2Qt::Pbkdf2(QByteArray pass, 
                            QByteArray salt, 
                            unsigned int rounds,
                            unsigned int key_len)
{
 char *key;
 key=(char*)malloc(key_len);

 if(key==NULL)
  return QByteArray();

 int calculate_result=Pbkdf2_calculate(pass, 
                                       pass.length(), 
                                       salt,
                                       salt.length(),
                                       key,
                                       key_len,
                                       rounds);

 if(calculate_result==-1)
  return QByteArray();

 QByteArray result(key, key_len);

 memset(key, 0, key_len);
 free(key);

 return result;
}
