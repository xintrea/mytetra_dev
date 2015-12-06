#ifndef PLACEMENTFORMATTER_H
#define PLACEMENTFORMATTER_H

#include "Formatter.h"


class PlacementFormatter : public Formatter
{
public:
  PlacementFormatter();

public slots:

  // Действия в области редактирования
  void onIndentplusClicked(void);
  void onIndentminusClicked(void);

  void onAlignleftClicked(void);
  void onAligncenterClicked(void);
  void onAlignrightClicked(void);
  void onAlignwidthClicked(void);
};

#endif // PLACEMENTFORMATTER_H
