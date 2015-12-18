#ifndef PLACEMENTFORMATTER_H
#define PLACEMENTFORMATTER_H

#include "Formatter.h"

// Класс форматирования размещения текста (выравнивание, отступы и т. п.)


class PlacementFormatter : public Formatter
{
  Q_OBJECT

public:
  PlacementFormatter();

signals:

  void updateIndentsliderToActualFormat();
  void updateAlignButtonHiglight(bool);

public slots:

  // Действия в области редактирования
  void onIndentplusClicked(void);
  void onIndentminusClicked(void);

  void onAlignleftClicked(void);
  void onAligncenterClicked(void);
  void onAlignrightClicked(void);
  void onAlignwidthClicked(void);

protected:

  void alignText(Qt::AlignmentFlag mode);

};

#endif // PLACEMENTFORMATTER_H
