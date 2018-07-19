#ifndef _MIMETEX_EXPORT_H_
#define _MIMETEX_EXPORT_H_

#ifdef __cplusplus
extern "C" {
#endif


#ifdef Q_OS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

MY_EXPORT int mimetexMain( int argc, char *argv[]);
MY_EXPORT int CreateGifFromEq( char *expression, char *gifFileName );


#ifdef __cplusplus
}
#endif

#endif
