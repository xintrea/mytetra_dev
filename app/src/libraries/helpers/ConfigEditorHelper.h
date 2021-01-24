#ifndef CONFIGEDITORHELPER_H
#define CONFIGEDITORHELPER_H

class QString;


class ConfigEditorHelper
{
public:
    ConfigEditorHelper();

    // Редактирование какого-либо конфиг-файла
    static void editConfigFile( QString fileName, float sizeCoeffitient );
};

#endif // CONFIGEDITORHELPER_H
