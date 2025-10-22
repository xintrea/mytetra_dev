#ifndef APPFILES_H
#define APPFILES_H

#include <QString>

//! \brief
//! Объект для управления стандартными файлами, необходимыми
//! для работы программы
//! Объект работает в условиях, когда рабочая директория уже была определена

class AppFiles
{

public:

    // Флаги создания новой рабочей директории
    enum CreateFirstAppFilesFlags
    {
        DB         = 1 << 0, // Создание директории с файлами БД
        TRASH      = 1 << 1, // Создание директории корзины
        APP_CONFIG = 1 << 2  // Создание конфиг-файлов рабочей директории
    };


    AppFiles();

    //! Создание файлов новой БД в указанной директории
    void createFirstAppFiles(QString dirName,
                             unsigned int flags =
                                 CreateFirstAppFilesFlags::DB |
                                 CreateFirstAppFilesFlags::TRASH |
                                 CreateFirstAppFilesFlags::APP_CONFIG );

    //! Создание файла стилей
    //! Файл стилей может создаваться и после развертывания начальных файлов MyTetra
    //! Так как в более старых версиях MyTetra его еще не было
    void createThemesFiles(QString dirName, QString themeName=NULL);

    void createStandartProgramFiles(void);
    void createPortableProgramFiles(void);

};

#endif // APPFILES_H
