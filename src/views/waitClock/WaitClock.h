#ifndef _WAITCLOCK_H_
#define _WAITCLOCK_H_

#include <QWidget>
#include <QHBoxLayout>
#include <QIcon>
#include <QTimer>
#include <QLabel>
#include <QPixmap>


class WaitClock : public QWidget
{
 Q_OBJECT

public:
 WaitClock(QWidget *parent=0);
 ~WaitClock();

private slots:

 void iconUpdate();

private:

 QHBoxLayout *centralLayout;
 
 QLabel iconLabel;

 QPixmap kadr_00;
 QPixmap kadr_01;
 QPixmap kadr_02;

 int currentIconNum;

 QTimer timer;

};

#endif /* _WAITCLOCK_H_ */
