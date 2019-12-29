#ifndef EDITORTOOLBARUSEDTOOLBUTTONSMODEL
#define EDITORTOOLBARUSEDTOOLBUTTONSMODEL

#include <QObject>
#include <QStandardItemModel>

#include "libraries/GlobalParameters.h"
#include "EditorToolbarSettingsAbstractModel.h"

// Модель списка используемых команд
// на выбранной строке панели инструментов редактора


class EditorConfig;

class EditorToolbarSettingsUsedToolsModel : public EditorToolbarSettingsAbstractModel
{
    Q_OBJECT

public:

    EditorToolbarSettingsUsedToolsModel(QObject *parent = nullptr);

    // Первичное наполнение модели
    void init(GlobalParameters::EditorToolbar tb);

};

#endif // EDITORTOOLBARUSEDTOOLBUTTONSMODEL
