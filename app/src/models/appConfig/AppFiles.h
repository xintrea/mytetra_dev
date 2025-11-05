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

    //! Создание файлов стилей
    //! Файлы стилей могут создаваться и после развертывания начальных файлов MyTetra
    //! Так как в более старых версиях MyTetra их могло не быть
    void createThemesFiles(const QString &dirName, const QString &themeName="");

    void createStandartProgramFiles(void);
    void createPortableProgramFiles(void);

};

#endif // APPFILES_H
