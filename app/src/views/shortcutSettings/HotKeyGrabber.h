#ifndef HOTKEYGRABBER_H
#define HOTKEYGRABBER_H

#include <QLineEdit>


class HotKeyGrabber: public QLineEdit
{
    Q_OBJECT

public:
    HotKeyGrabber( QWidget* pParent = nullptr);
    ~HotKeyGrabber(){}

protected:
    void keyPressEvent ( QKeyEvent * event );
};


#endif // HOTKEYGRABBER_H
