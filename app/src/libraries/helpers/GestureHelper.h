#ifndef GESTUREHELPER_H
#define GESTUREHELPER_H

class QAbstractItemView;

class GestureHelper
{
public:
    GestureHelper();

    static void setKineticScrollArea(QAbstractItemView *object);
};

#endif // GESTUREHELPER_H
