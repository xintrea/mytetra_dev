#ifndef _CONFIGPAGE_H_
#define _CONFIGPAGE_H_

#include <QWidget>


class ConfigPage : public QWidget
{
 Q_OBJECT

public:
  ConfigPage(QWidget *parent = 0);
  virtual ~ConfigPage();

  virtual int applyChanges(void);
};


#endif // _CONFIGPAGE_H_
