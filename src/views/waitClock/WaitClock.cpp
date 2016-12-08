#include <QHBoxLayout>
#include <QIcon>

#include "WaitClock.h"
#include "main.h"


WaitClock::WaitClock(QWidget *parent) : QWidget(parent)
{
 currentIconNum=0;

 kadr_00=QIcon(":/resource/pic/waitclock_00.svg").pixmap(32, 32);
 kadr_01=QIcon(":/resource/pic/waitclock_01.svg").pixmap(32, 32);
 kadr_02=QIcon(":/resource/pic/waitclock_02.svg").pixmap(32, 32);

 centralLayout=new QHBoxLayout(this);

 // iconLabel.setPixmap(kadr_02);

 centralLayout->addWidget(&iconLabel);

 setLayout(centralLayout);

 connect(&timer, SIGNAL(timeout()), this, SLOT(iconUpdate()));

 iconUpdate();

 // timer.start(650);
}


WaitClock::~WaitClock()
{

}


void WaitClock::iconUpdate(void)
{
 timer.stop();

 if(currentIconNum==0)
  {
   iconLabel.setPixmap(kadr_00);
   timer.start(550);
  }

 if(currentIconNum==1)
  {
   iconLabel.setPixmap(kadr_01);
   timer.start(100);
  }
 
 if(currentIconNum==2)
  {
   iconLabel.setPixmap(kadr_02);
   timer.start(550);
  }

 if(currentIconNum==3)
  {
   iconLabel.setPixmap(kadr_01);
   timer.start(100);
  }

 currentIconNum++;
 if(currentIconNum==4)
  currentIconNum=0;
}

