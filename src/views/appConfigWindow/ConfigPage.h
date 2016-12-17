#ifndef _CONFIGPAGE_H_
#define _CONFIGPAGE_H_

#include <QWidget>


class ConfigPage : public QWidget
{
  Q_OBJECT

public:
  explicit ConfigPage(QWidget *parent = 0);

  virtual int applyChanges(void) = 0;
};


#endif // _CONFIGPAGE_H_
