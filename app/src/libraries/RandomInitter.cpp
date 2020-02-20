#include <cmath>

#include <QDebug>
#include <QDateTime>

#include "RandomInitter.h"


RandomInitter::RandomInitter()
{

}


void RandomInitter::init(long humanGenerateSeedShift)
{
    long seed1=getMilliCount();
    srand( static_cast<unsigned int>( seed1+humanGenerateSeedShift+rand() ) );

    long delay=rand()%1000;
    long r=0;
    for(long i=0; i<delay; i++) r=r+rand();

    seed1=seed1+r+getMilliCount();

    long divMod=rand();
    divMod = divMod==0 ? 1 : divMod; // Защита от деления на 0
    long seed2=QDateTime::currentMSecsSinceEpoch() > divMod ? QDateTime::currentMSecsSinceEpoch()/divMod : divMod/QDateTime::currentMSecsSinceEpoch();

    long seed3=seed1+seed2;
    long seed=seed3;

    // qDebug() << "Random generator seed: " << seed;

    srand( static_cast<unsigned int>( seed ) );

    // qDebug() << "Random number: " << rand();
}


long RandomInitter::getMilliCount()
{
    #if TARGET_OS!=ANDROID_OS
    return static_cast<long>(QDateTime::currentMSecsSinceEpoch());
    #else
    long   ms; // Milliseconds
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds

    return static_cast<long>(ms);
    #endif
}

