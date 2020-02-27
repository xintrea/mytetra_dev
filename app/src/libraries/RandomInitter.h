#ifndef RANDOMINITTER_H
#define RANDOMINITTER_H


class RandomInitter
{
public:
    RandomInitter();

    static void init(long humanGenerateSeedShift);

private:
    static long getMilliCount();

};

#endif // RANDOMINITTER_H
