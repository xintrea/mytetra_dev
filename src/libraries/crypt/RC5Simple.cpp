/*
RC5Simple C++ library

The easy implementation of RC5 32/12/16 algorythm
with user fiendly interface.

Copyright 2011 Sergey M. Stepanov

Contact: xintrea@gmail.com, www.webhamster.ru

License: GPL v.3 and BSD

Volgodonsk, 2011-2013
*/

#include "RC5Simple.h"


// 1.15 - Первая публичная версия

// 1.16 - Основана на версии 1.15
//      - Исправлены предупреждения о сравнении знакового и беззнакового
//        числа в методах
//        RC5_Setup() и
//        RC5_EncDecFile()
//        возникающие при строгих опциях компилятора
//      - В прототип конструктора добавлен аргумент enableRandomInit,
//        который по-умолчанию равен false. Это необходимо для того,
//        чтобы пользователь мог управлять инициализацией генератора
//        случайных чисел. Если в основной программе сделана инициализация,
//        то инициализация в данном классе не требуется, 
//        и по-умолчанию теперь отключена

// 1.17 - 1.19 - Промежуточные отладочные версии

// 1.20 - Основана на версии 1.19
//      - Изменена реализация механизма сцепления блоков шифротекста с упрощенного, 
//        предложенного в Reference implementation of RC5-32/12/16 in C
//        RSA Data Security, Inc, 1995 на полный CBC, описанный в Wikipedia
//        http://ru.wikipedia.org/wiki/Режим_сцепления_блоков_шифротекста
//      - Скорость шифрации/дешифрации данных увеличена в 2,5 раз
//      - Добавлен каталог /doc. В нем размещено описание формата данных
//        и HOWTO по использованию библиотеки RC5Simple

// 1.21 - Основана на версии 1.20
//      - Внесены исправления, позволяющие добиться павильной работы на 
//        64-х битовых архитектурах. Протестировано на Cent OS x86_64, Intel 64

// 1.22 - Основана на версии 1.21
//      - Удалена проверка на невозможность работы под x86_64 
//        и сообщение об ошибке в конструкторе
//      - Подтверждена работа блоков шифрования и дешифровки на 
//        миропроцессорах AMD

// 1.23 - Основана на версии 1.22
//      - Исправлен код, приводящий к появлению предупреждений 
//        в релиз режиме компиляции в методе RC5_SetKey()
//      - В заголовочном файле исправлена сбившаяся кодировка в определениях
//        разрядности микропроцессора, получившаяся вследствие копирования
//        из окна терминала виртуальной машины с другой кодировкой

// 1.27 - Основана на версиях 1.24 - 1.26
//      - Сделана поддержка форматов хранения данных.
//        Формат 1 - это первый формат, использующийся вплоть до версии 1.23
//        Формат 2 - это новый формат с усиленным форматом шифрования
//      - В формате 2 (и всех последующих) присутсвует сигнатура RC5SIMP
//        после которой следует один байт версии формата данных
//      - По умолчанию идет шифрация в формат 2
//      - В формате 2, помимо вектора инициализации, добавлен дополнительный
//        блок случайных данных, размещаемый в шифрованном тексте. Данная
//        мера предотвращает атаку при наличии нескольких образцов шифрованного 
//        текста по заранее известному началу открытого текста. Пользоваться 
//        устаревшей версией формата 1 строго не рекомендуется
//      - Сделана возможность принудительного выбора формата шифрации/дешифрации
//        для сохранения совместимости с предыдущей версией. Выбор формата
//        осуществляется функцией
//        void RC5_SetFormatVersionForce(unsigned char formatVersion);
//      - Добавлено несколько дополнительных отладочных сообщений

// 1.28 - Основана на версии 1.27
//      - Проведены тесты упаковки-распаковки форматов 1 и 2
//      - Прописаны правильные смещения для корректной распаковки формата 1
//      - Добавлено свойство, хранящее последнюю ошибку
//      - Добавлен метод получения последней ошибки RC5_GetErrorCode()
//      - Вывод сообщения в консоль при возникновении ошибки заменен запоминанием
//        кода последней ошибки
//      - В заголовочном файле прописаны коды ошибок RC5_ERROR_CODE_X и
//        их описания

// 1.29 - Основана на версии 1.28
//      - Внесены исправления для правильной компиляции под Windows, в которой
//        отсутсвует предустановленное определение LONG_BIT. Теперь, в случае 
//        отсутсвия данного определения, компиляция произвдится на 32-х битную
//        платформу

// Отвлекли: когда будешь ложиться спать, закрой у ребенка окно

// 1.30 - Основана на версии 1.29
//      - Добавлен формат 3. В предыдущем формате 2 обнаружено
//        необоснованное занижение количества первичных случайных данных
//        на половину длинны ключа шифрования. В формате 3 первичные
//        случайные данные имеют полную ширину ключа шифрования
//      - По умолчанию идет шифрация в формат 3
//      - В метод RC5_Encrypt добавлена установка кода ошибки при попытке
//        зашифровать пустые данные
//      - В метод RC5_Decrypt добавлена установка кода ошибки при попытке
//        расшифровать пустые данные
//      - В метод RC5_Decrypt добавлена установка кода ошибки при попытке
//        расшифровать данные с некорректной длинной
//      - В методах расшифровки проставлены правильные константы для 
//        смещений данных в разных форматах хранения
//      - Произведена проверка через valgrind упаковки/распаковки в каждом
//        из форматов. Везде имеем отчет
//        ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
//      - Фиксация номера версии

// 1.31 - Основана на версии 1.30
//      - Исправлено незакрытие файла при шифрации/дешифровке в случае
//        возникновения ошибки из-за нуливого размера файла
//        в методе RC5_EncDecFile()


RC5Simple::RC5Simple(bool enableRandomInit)
{ 
 // Set magic constants
 rc5_p = 0xb7e15163;
 rc5_q = 0x9e3779b9;

 // Cleaning user key
 for(int i=0; i<RC5_B; i++)
  rc5_key[i]=0;

 rc5_formatVersion=RC5_FORMAT_VERSION_CURRENT;
 rc5_isSetFormatVersionForce=false;

 errorCode=0;

 // Init random generator
 if(enableRandomInit)
  srand( time(NULL) );
}


RC5Simple::~RC5Simple(void)
{ 
 // Cleaning user key
 for(int i=0; i<RC5_B; i++)
  rc5_key[i]=0;
}


const char *RC5Simple::RC5_GetVersion(void)
{
 static const char *lib_version=RC5_SIMPLE_VERSION;
 return(lib_version);
}


void RC5Simple::RC5_SetFormatVersionForce(unsigned char formatVersionForce)
{ 
 rc5_formatVersion=formatVersionForce;
 rc5_isSetFormatVersionForce=true;
}


// Отвлекли: папа, бабушка нам дала адрес бабушки Люды
// Отвлекли: таймер, готовить макароны
// Отвлекли: сканируй письмо
// Encrypt data block
// Parameters: 
// pt - input data
// ct - encrypt data
void RC5Simple::RC5_EncryptBlock(RC5_TWORD *pt, RC5_TWORD *ct)
{ 
  RC5_TWORD i; 
  RC5_TWORD a=pt[0]+rc5_s[0];
  RC5_TWORD b=pt[1]+rc5_s[1];

  for(i=1; i<=RC5_R; i++)
    { 
      a = RC5_ROTL(a^b, b)+rc5_s[2*i]; 
      b = RC5_ROTL(b^a, a)+rc5_s[2*i+1]; 
    }

  ct[0] = a; 
  ct[1] = b;  
} 


// Decrypt data block
// Parameters:
// pt - input data
// ct - decrypt data
void RC5Simple::RC5_DecryptBlock(RC5_TWORD *ct, RC5_TWORD *pt) 
{ 
  RC5_TWORD i;
  RC5_TWORD b=ct[1];
  RC5_TWORD a=ct[0];

  for(i=RC5_R; i>0; i--) 
    { 
      b = RC5_ROTR(b-rc5_s[2*i+1], a)^a;
      a = RC5_ROTR(a-rc5_s[2*i], b)^b; 
    }

  pt[1] = b-rc5_s[1]; 
  pt[0] = a-rc5_s[0]; 
} 


// Setup secret key
// Parameters: 
// key - secret input key[RC5_B] 
void RC5Simple::RC5_Setup(unsigned char *key)
{
   RC5_LOG(( "RC5_Setup, set key to: " ));
   for(int i=0; i<RC5_B; i++)
    RC5_LOG(( "%.2X", key[i] ));
   RC5_LOG(( "\n" ));


   int i, j, k;
   RC5_TWORD u=RC5_W/8, a, b, l[RC5_C]; 

   // Отвлекли: потренируй мне складку
   // Initialize l[], then rc5_s[], then mix key into rc5_s[]
   for(i=RC5_B-1, l[RC5_C-1]=0; i!=-1; i--)
    l[i/u] = (l[i/u]<<8)+key[i];

   RC5_LOG(( "RC5_Setup, l[]: " ));
   for(int i=0; i<RC5_C; i++)
    RC5_LOG(( "%.2X", l[i] ));
   RC5_LOG(( "\n" ));


   for(rc5_s[0]=rc5_p,i=1; i<RC5_T; i++) 
    rc5_s[i] = rc5_s[i-1]+rc5_q;

   RC5_LOG(( "RC5_Setup, rc5_s[]: " ));
   for(int i=0; i<RC5_T; i++)
    RC5_LOG(( "%.2X", rc5_s[i] ));
   RC5_LOG(( "\n" ));


   // 3*t > 3*c
   for(a=b=i=j=k=0; k<3*RC5_T; k++, i=(i+1)%RC5_T, j=(j+1)%RC5_C)   
     { 
       a = rc5_s[i] = RC5_ROTL(rc5_s[i]+(a+b),3);  
       b = l[j] = RC5_ROTL(l[j]+(a+b),(a+b)); 
     } 

   RC5_LOG(( "RC5_Setup, mix rc5_s[]: " ));
   for(int i=0; i<RC5_T; i++)
    RC5_LOG(( "%.2X", rc5_s[i] ));
   RC5_LOG(( "\n" ));
}


// Отвлекли: смотри как я кувыркаюсь
// Set secret key
void RC5Simple::RC5_SetKey(vector<unsigned char> &key)
{
 if(key.size()!=RC5_B)
  {
   errorCode=RC5_ERROR_CODE_1;
   return;
  }
  
 for(int i=0; i<RC5_B; i++)
  rc5_key[i]=key[i];
}


// Encrypt data in vector
void RC5Simple::RC5_Encrypt(vector<unsigned char> &in, vector<unsigned char> &out)
{
 // Clear output vector
 out.clear();
 
 // Отвлекли: помой ребенка, потри его мочалкой а то он сам не может
 // Отвлекли: смотри какое платье на кукле
 // No crypt null data
 if(in.size()==0) 
  {
   errorCode=RC5_ERROR_CODE_5;
   return;
  }

 
 // Save input data size
 unsigned int clean_data_size=in.size();
 RC5_LOG(( "Input data size: %d\n", clean_data_size ));


 // IV block
 unsigned char iv[RC5_BLOCK_LEN];
 for(int i=0; i<RC5_BLOCK_LEN; i++)
  iv[i]=(unsigned char)RC5_Rand(0, 0xFF);


 // Block with data size
 unsigned char data_size[RC5_BLOCK_LEN];

 for(int i=0; i<RC5_BLOCK_LEN; i++)
  {
   data_size[i]=RC5_GetByteFromInt( clean_data_size, i );
   RC5_LOG(( "Data size byte %d: %.2X\n", i, data_size[i] ));
  }


 // Insert data size to begin data
 in.insert( in.begin(), RC5_BLOCK_LEN, 0);
 for(int i=0; i<RC5_BLOCK_LEN; i++)
  in[i]=data_size[i];

 // Отвлекли: почисть ребенку зубы
 // Add firsted random data
 unsigned int firsRandomDataBlocks=0;
 if(rc5_formatVersion==RC5_FORMAT_VERSION_2)
  firsRandomDataBlocks=1; // At start at format version 2, in begin data add one block with random data

 if(rc5_formatVersion==RC5_FORMAT_VERSION_3)
  firsRandomDataBlocks=2; // At start at format version 3, in begin data add two blocks with random data (full width is 128 bit)

 if(firsRandomDataBlocks>0) 
  for(unsigned int n=1; n<=firsRandomDataBlocks; n++)
   {
    in.insert( in.begin(), RC5_BLOCK_LEN, 0);
    for(int i=0; i<RC5_BLOCK_LEN; i++)
     in[i]=(unsigned char)RC5_Rand(0, 0xFF);
   }


 // Align end of data to block size
 int last_unalign_len=clean_data_size%RC5_BLOCK_LEN;
 RC5_LOG(( "Last unalign len: %d\n", last_unalign_len ));

 if(last_unalign_len>0)
  {
   // Add random data to end for align to block size
   for(int i=0; i<(RC5_BLOCK_LEN-last_unalign_len); i++)
    {
     RC5_LOG(( "Add byte: %d\n", i ));
     in.push_back( (unsigned char)RC5_Rand(0, 0xFF) );
    }
  }

 #if RC5_ENABLE_DEBUG_PRINT==1
  RC5_LOG(( "Data size after crypt setup: %d\n", in.size() ));
  RC5_LOG(( "Plain byte after crypt setup: " ));
  for(int i=0; i<in.size(); i++)
   RC5_LOG(( "%.2X ", in[i] ));
  RC5_LOG(( "\n" ));
 #endif


 // ------  
 // Encode
 // ------  

 // Create and fill cell in output vector

 unsigned int notCryptDataSize=0;

 // In format version 1 save only IV as open data in block 0
 if(rc5_formatVersion==RC5_FORMAT_VERSION_1)
  {
   out.resize(in.size()+RC5_BLOCK_LEN, 0); 

   // Save start IV to block 0 in output data
   for(int i=0; i<RC5_BLOCK_LEN; i++)
    out[i]=iv[i];

   notCryptDataSize=RC5_BLOCK_LEN;
  }

 // Отвлекли: иди поешь
 // In format version 2 or higth save format header (block 0) and IV (block 1)
 if(rc5_formatVersion>=RC5_FORMAT_VERSION_2)
  {
   out.resize(in.size()+RC5_BLOCK_LEN*2, 0);

   // Signature bytes in header
   const char *signature=RC5_SIMPLE_SIGNATURE;
   for(int i=0; i<(RC5_BLOCK_LEN-1); i++)
    out[i]=signature[i];

   // Format version byte in header
   if(rc5_isSetFormatVersionForce)
    out[RC5_BLOCK_LEN-1]=rc5_formatVersion;
   else
    out[RC5_BLOCK_LEN-1]=RC5_FORMAT_VERSION_CURRENT;

   // Save start IV to second block in output data
   for(int i=0; i<RC5_BLOCK_LEN; i++)
    out[i+RC5_BLOCK_LEN]=iv[i];

   notCryptDataSize=RC5_BLOCK_LEN*2;
  }


 // Set secret key for encrypt
 RC5_Setup(rc5_key);


 // Encode by blocks
 unsigned int block=0;
 while( (RC5_BLOCK_LEN*(block+1))<=in.size() )
  {
   unsigned int shift=block*RC5_BLOCK_LEN;

   // Temp input buffer for dont modify input data
   unsigned char temp_in[RC5_BLOCK_LEN];

   // XOR block with current IV
   for(int i=0; i<RC5_BLOCK_LEN; i++)
    temp_in[i]=in[shift+i] ^ iv[i];

   RC5_TWORD temp_word_1;
   RC5_TWORD temp_word_2;
   RC5_TWORD pt[RC5_WORDS_IN_BLOCK];
   RC5_TWORD ct[RC5_WORDS_IN_BLOCK];

   RC5_LOG(( "Block num %d, shift %d\n", block, shift ));

   temp_word_1=RC5_GET_WORD_FROM_BYTE(temp_in[0], temp_in[1], temp_in[2], temp_in[3]);

   temp_word_2=RC5_GET_WORD_FROM_BYTE(temp_in[RC5_WORD_LEN+0], temp_in[RC5_WORD_LEN+1], temp_in[RC5_WORD_LEN+2], temp_in[RC5_WORD_LEN+3]);

   pt[0]=temp_word_1; 
   pt[1]=temp_word_2;


   // Encode
   RC5_EncryptBlock(pt, ct);


   #if RC5_ENABLE_DEBUG_PRINT==1
    RC5_LOG(( "Pt %.8X, %.8X\n", pt[0], pt[1] ));
    RC5_LOG(( "Ct %.8X, %.8X\n", ct[0], ct[1] ));
   #endif

   // Отвлекли: надо покушать, кончай питаться йогуртами
   // Save crypt data
   for(int i=0; i<RC5_WORD_LEN; i++)
    {
     // Save crypt data with shift to RC5_BLOCK_LEN
     // btw. in first block putted IV 
     out[notCryptDataSize+shift+i]=RC5_GET_BYTE_FROM_WORD(ct[0], i);
     out[notCryptDataSize+shift+RC5_WORD_LEN+i]=RC5_GET_BYTE_FROM_WORD(ct[1], i);
    }


   // Generate next IV for Cipher Block Chaining (CBC)
   for(int i=0; i<RC5_BLOCK_LEN; i++)
    iv[i]=out[notCryptDataSize+shift+i];


   block++;
  }


 // Cleaning IV
 for(int i=0; i<RC5_BLOCK_LEN; i++)
  iv[i]=0;


 // -----------------------------------
 // Return input vector to firsted data
 // -----------------------------------

 // Отвлекли: сходи в магазин Магнит, у нас есть нечего
 // Отвлекли: почитай ребенку
 // Remove size block (for all formats)
 in.erase(in.begin(), in.begin()+RC5_BLOCK_LEN);

 // Отвлекли: давай поиграем в Shatter
 // Remove random data blocks
 if(firsRandomDataBlocks>0) 
  for(unsigned int n=1; n<=firsRandomDataBlocks; n++)
   in.erase(in.begin(), in.begin()+RC5_BLOCK_LEN);

 // Отвлекли: звонок, проверь ребенку зрение
 // Отвлекли: напечатай таблицу проверки зрения
 // Отвлекли: проверь зрение у ребенка и у себя
 // Remove from input vector last random byte for aligning
 if(in.size()>clean_data_size)
  in.erase(in.begin()+clean_data_size, in.end());

}


// Decrypt data in vector
void RC5Simple::RC5_Decrypt(vector<unsigned char> &in, vector<unsigned char> &out)
{ 
 RC5_LOG(( "\nDecrypt\n" ));

 RC5_LOG(( "\nInput data size: %d\n", in.size() ));

 // Отвлекли: иди покушай
 // No decrypt null data
 if(in.size()==0) 
  {
   errorCode=RC5_ERROR_CODE_6;
   return;
  }


 // Detect format version
 unsigned int formatVersion=0;
 if(rc5_isSetFormatVersionForce==true)
  formatVersion=rc5_formatVersion; // If format set force, format not autodetected
 else
  {
   // Signature bytes
   const char *signature=RC5_SIMPLE_SIGNATURE;

   // Detect signature
   bool isSignatureCorrect=true;
   for(int i=0; i<(RC5_BLOCK_LEN-1); i++)
    if(in[i]!=signature[i])
     isSignatureCorrect=false;

   // If not detect signature
   if(isSignatureCorrect==false)
    formatVersion=RC5_FORMAT_VERSION_1; // In first version can't signature
   else
    {
     // Get format version
     unsigned char readFormatVersion=in[RC5_BLOCK_LEN-1];

     // If format version correct
     if(readFormatVersion>=RC5_FORMAT_VERSION_2 && readFormatVersion<=RC5_FORMAT_VERSION_CURRENT)
      formatVersion=readFormatVersion;
     else
      formatVersion=RC5_FORMAT_VERSION_1; // If version not correct, may be it format 1 ( probability 0.[hrendesyatih]1 )
    }
  }


 unsigned int ivShift=0;
 unsigned int firstDataBlock=0;
 unsigned int removeBlocksFromOutput=0;
 unsigned int blockWithDataSize=0;


 if(formatVersion==RC5_FORMAT_VERSION_1)
  {
   ivShift=0;
   firstDataBlock=1; // Start decode from block with index 1 (from block 0 read IV)
   removeBlocksFromOutput=1;
   blockWithDataSize=1;
  }

 if(formatVersion==RC5_FORMAT_VERSION_2)
  {
   ivShift=RC5_BLOCK_LEN;
   firstDataBlock=2; // Start decode from block with index 2 (0 - signature and version, 1 - IV)
   removeBlocksFromOutput=2; // Random data block and size data block
   blockWithDataSize=3;
  }

 if(formatVersion==RC5_FORMAT_VERSION_3)
  {
   ivShift=RC5_BLOCK_LEN;
   firstDataBlock=2; // Start decode from block with index 2 (0 - signature and version, 1 - IV)
   removeBlocksFromOutput=3; // Two random data block and size data block
   blockWithDataSize=4;
  }


 // Get IV
 unsigned char iv[RC5_BLOCK_LEN];
 for(int i=0; i<RC5_BLOCK_LEN; i++)
  iv[i]=in[i+ivShift];


 // Set secret key for decrypt
 RC5_Setup(rc5_key);


 // Cleaning output vector
 out.clear();
 out.resize(in.size(), 0);


 // Decode by blocks from started data block
 unsigned int data_size=0;
 unsigned int block=firstDataBlock;
 while( (RC5_BLOCK_LEN*(block+1))<=in.size() )
  {
   unsigned int shift=block*RC5_BLOCK_LEN;

   RC5_TWORD temp_word_1;
   RC5_TWORD temp_word_2;
   RC5_TWORD pt[RC5_WORDS_IN_BLOCK];
   RC5_TWORD ct[RC5_WORDS_IN_BLOCK];

   RC5_LOG(( "Block num %d, shift %d\n", block, shift ));

   temp_word_1=RC5_GET_WORD_FROM_BYTE(in[shift], in[shift+1], in[shift+2], in[shift+3]);

   temp_word_2=RC5_GET_WORD_FROM_BYTE(in[shift+RC5_WORD_LEN], in[shift+RC5_WORD_LEN+1], in[shift+RC5_WORD_LEN+2], in[shift+RC5_WORD_LEN+3]);
              
   pt[0]=temp_word_1; 
   pt[1]=temp_word_2;

   RC5_LOG(( "Block data. Word 1: %.2X, Word 2: %.2X\n", temp_word_1, temp_word_2 ));


   // Decode
   RC5_DecryptBlock(pt, ct);


   // ---------------------------
   // Un XOR block with IV vector
   // ---------------------------

   // Convert block words to plain array
   unsigned char ct_part[RC5_BLOCK_LEN];

   for(int i=0; i<RC5_WORD_LEN; i++)
    { 
     ct_part[i]=RC5_GET_BYTE_FROM_WORD(ct[0], i);
     ct_part[i+RC5_WORD_LEN]=RC5_GET_BYTE_FROM_WORD(ct[1], i);
    }

   // Un XOR
   for(int i=0; i<RC5_BLOCK_LEN; i++)
    ct_part[i]^=iv[i];  

   // Отвлекли: попугаи наверно голодные, кто бы их кормил, надо покормить
   if(block==blockWithDataSize)
    {
     data_size=RC5_GetIntFromByte(ct_part[0], ct_part[1], ct_part[2], ct_part[3]);

     RC5_LOG(( "Decrypt data size: %d\n", data_size ));

     // Uncorrect decrypt data size
     if((unsigned int)data_size>(unsigned int)in.size()) 
      {
       // RC5_LOG(( "Incorrect data size. Decrypt data size: %d, estimate data size: ~%d\n", data_size,  in.size() ));
       errorCode=RC5_ERROR_CODE_7;
       return;
      }
    }

   #if RC5_ENABLE_DEBUG_PRINT==1
    RC5_LOG(( "Pt %.8X, %.8X\n", pt[0], pt[1] ));
    RC5_LOG(( "Ct %.8X, %.8X\n", ct[0], ct[1] ));
   #endif


   // Generate next IV for Cipher Block Chaining (CBC)
   for(int i=0; i<RC5_BLOCK_LEN; i++)
    iv[i]=in[shift+i];


   // Save decrypt data
   for(int i=0; i<RC5_BLOCK_LEN; i++)
    {
     RC5_LOG(( "Put decrypt data to vector out[%d] = %.2X\n", shift-(removeBlocksFromOutput*RC5_BLOCK_LEN)+i, ct_part[i] ));
     out[shift-(firstDataBlock*RC5_BLOCK_LEN)+i]=ct_part[i]; 
    }

   block++;
  }


 // Cleaning IV
 for(int i=0; i<RC5_BLOCK_LEN; i++)
  iv[i]=0;

 // Отвлекли: Пошли за ребенком
 // Remove from output a blocks with technical data (random blocks, size, etc...)
 out.erase(out.begin(), out.begin()+removeBlocksFromOutput*RC5_BLOCK_LEN);

 // Remove from output a last byte with random byte for aligning
 if(out.size()>data_size)
  out.erase(out.begin()+data_size, out.end());
}


void RC5Simple::RC5_EncryptFile(unsigned char *in_name, unsigned char *out_name)
{
 RC5_EncDecFile(in_name, out_name, RC5_MODE_ENCODE);
}


// Отвлекли: что это за пакет? Кухня, Бюджетный вариант?
// Отвлекли: позвонил дедушка
void RC5Simple::RC5_EncryptFile(const char *in_name, const char *out_name)
{
 // Отвлекли: виниловая наклейка как мечь, смотри
 RC5_EncDecFile((unsigned char *)in_name, (unsigned char *)out_name, RC5_MODE_ENCODE);
}


void RC5Simple::RC5_DecryptFile(unsigned char *in_name, unsigned char *out_name)
{
 RC5_EncDecFile(in_name, out_name, RC5_MODE_DECODE);
}


void RC5Simple::RC5_DecryptFile(const char *in_name, const char *out_name)
{
 RC5_EncDecFile((unsigned char *)in_name, (unsigned char *)out_name, RC5_MODE_DECODE);
}


void RC5Simple::RC5_EncDecFile(unsigned char *in_name, unsigned char *out_name, int mode)
{
 FILE *in_file;
 FILE *out_file;

 if((in_file=fopen( (const char*)in_name, "rb") )==NULL)
  {
   errorCode=RC5_ERROR_CODE_2;
   return;
  }

 // Get size of input file
 fseek(in_file, 0, SEEK_END);
 unsigned int in_file_length=ftell(in_file);
 RC5_LOG(( "Input file name: %s, size: %d\n", in_name, in_file_length));

 if(in_file_length==0)
  {
   fclose(in_file);
   errorCode=RC5_ERROR_CODE_3;
   return;
  }

 // Return to begin file
 fseek(in_file, 0, SEEK_SET);

 // Create data vectors
 vector<unsigned char> in(in_file_length);
 vector<unsigned char> out;

 // Fill input data vector
 unsigned int byte=0;
 while(byte<in_file_length)
  in[byte++]=fgetc(in_file);

 fclose(in_file);

 if(mode==RC5_MODE_ENCODE)
  RC5_Encrypt(in, out); // Encrypt data vector
 else
  RC5_Decrypt(in, out); // Decrypt data vector

 // Create output file
 if((out_file=fopen( (const char*)out_name, "wb") )==NULL)
  {
   errorCode=RC5_ERROR_CODE_4;
   return;
  }

 // Fill output file
 for(unsigned int i=0; i<out.size(); i++)
  {
   RC5_LOG(( "File byte %d : %.2X \n", i, out[i] )); 
   fputc(out[i], out_file);
  } 

 fclose(out_file);

}


/*
inline unsigned char RC5Simple::RC5_GET_BYTE_FROM_WORD(RC5_TWORD w, int n)
{
 unsigned char b=0;

 switch (n) {                                                                                  
   case 0:
       b=(w & 0x000000FF);
       break; 
   case 1:
       b=(w & 0x0000FF00) >> 8;
       break; 
   case 2:
       b=(w & 0x00FF0000) >> 16;
       break; 
   case 3:
       b=(w & 0xFF000000) >> 24;
 }

 // RC5_LOG(( "GetByteFromWord(%.8X, %d)=%.2X\n", w, n, b ));

 return b;
}          
*/


inline unsigned char RC5Simple::RC5_GetByteFromInt(unsigned int w, int n)
{
 unsigned char b=0;

 switch (n) {                                                                                  
   case 0:
       b=(w & 0x000000FF);
       break; 
   case 1:
       b=(w & 0x0000FF00) >> 8;
       break; 
   case 2:
       b=(w & 0x00FF0000) >> 16;
       break; 
   case 3:
       b=(w & 0xFF000000) >> 24;
 }

 // RC5_LOG(( "GetByteFromWord(%.8X, %d)=%.2X\n", w, n, b ));

 return b;
}          


inline unsigned int RC5Simple::RC5_GetIntFromByte(unsigned char b0,
                                                  unsigned char b1, 
                                                  unsigned char b2, 
                                                  unsigned char b3)
{
 return b0+
        (b1 << 8)+
        (b2 << 16)+
        (b3 << 24);
}


/*
inline RC5_TWORD RC5Simple::RC5_GET_WORD_FROM_BYTE(unsigned char b0,
                                                unsigned char b1, 
                                                unsigned char b2, 
                                                unsigned char b3)
{
 return b0+
        (b1 << 8)+
        (b2 << 16)+
        (b3 << 24);
}
*/


// Random generator, from - to inclusive
unsigned int RC5Simple::RC5_Rand(unsigned int from, unsigned int to)
{
 unsigned int len=to-from+1;

 return (rand()%len)+from;
}


unsigned int RC5Simple::RC5_GetErrorCode()
{
 return errorCode;
}


void RC5Simple::RC5_LogPrint(char *lpszText, ...)
{
 va_list argList;

 va_start(argList, lpszText);

 vprintf(lpszText, argList);

 va_end(argList);

 return;
}
