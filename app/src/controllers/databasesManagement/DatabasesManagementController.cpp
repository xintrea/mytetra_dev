#include <QObject>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <algorithm>
#include <QModelIndexList>
#include <QInputDialog>
#include <QApplication>
#include <QClipboard>

#include "DatabasesManagementController.h"
#include "views/databasesManagement/DatabasesManagementTable.h"
#include "models/databasesManagement/DatabasesManagementModel.h"
#include "models/appConfig/AppConfig.h"
#include "models/appConfig/AppFiles.h"
#include "libraries/GlobalParameters.h"
#include "libraries/helpers/DiskHelper.h"
#include "views/tree/KnowTreeView.h"
#include "models/tree/KnowTreeModel.h"
#include "libraries/helpers/ObjectHelper.h"
#include "models/dataBaseConfig/DataBaseConfig.h"
#include "libraries/IconSelectDialog.h"
#include "controllers/recordTable/RecordTableController.h"
#include "views/consoleEmulator/CommandRunner.h"


extern GlobalParameters globalParameters;
extern AppConfig mytetraConfig;
extern AppFiles mytetraFiles;
extern DataBaseConfig dataBaseConfig;


DatabasesManagementController::DatabasesManagementController(QObject *parent) : QObject(parent)
{
    // Создается область со списком записей лога
    view=new DatabasesManagementTable( qobject_cast<QWidget *>(parent) ); // Вид размещается внутри виджета Screen
    view->setObjectName("databasesManagementTable");
    view->setController(this);

    // Создание модели данных
    model=new DatabasesManagementModel(this);
    model->setObjectName("databasesManagementModel");

    // Модель данных задается для вида
    view->setModel(model);
}


DatabasesManagementController::~DatabasesManagementController()
{
    delete view;
    delete model;
}


DatabasesManagementTable *DatabasesManagementController::getView(void)
{
    return view;
}


void DatabasesManagementController::onSelectClicked()
{
    QModelIndexList indexList = view->selectionModel()->selectedRows();

    if(indexList.size()!=1)
    {
        return;
    }

    // Переключение возможно только если не идет синхронизация
    if ( globalParameters.getSyncroCommandRunnner()->isRun() )
    {
        // Диалог информирования о том что в момент синхронизации переключение невозможно
        QMessageBox box;
        box.setWindowTitle(tr("Can't switch database"));
        box.setText(tr("It is not possible to switch to another database because synchronization is running."));
        box.setInformativeText(tr("Wait for the synchronization to complete and try again."));
        box.setStandardButtons(QMessageBox::Ok);
        box.setIcon( QMessageBox::Warning );
        box.exec();
        return;
    }

    int row=indexList[0].row(); // Номер выбранной строки

    // Переключение на выбранную БД
    QString dbPath    = model->getCellValue( row, DBMANAGEMENT_COLUMN_DBPATH );
    QString trashPath = model->getCellValue( row, DBMANAGEMENT_COLUMN_TRASHPATH );
    bool result = this->switchToDatabase(dbPath, trashPath);

    if (result)
    {
        // Эти действия нужны если окно при выборе не закрывается
        /*
        // Установка пометки выбора базы данных
        model->selectDatabase(row);

        // Индекс первого элемента выбранной строки
        const QModelIndex index = model->index(row, 0);

        // Выбор текущей строки
        view->setCurrentIndex(index);
        */

        // Обнуляется ключ шифрования, чтобы в переключенной БД MyTetra
        // не пыталась расшифровывать данные ключем от другой базы.
        // Это значит, что при переключении баз данных, если есть зашифрованные ветки,
        // при каждом переключении на другую БД надо будет заново вводить пароль

        // todo: подумать, возможно надо в памяти хранить QMap из пути к базе
        // и ключа шифрования, и автоматически их запоминать и применять
        // в пределах одной сессии запуска MyTetra
        globalParameters.setCryptKey( QByteArray() );

        // Закрытие окна управления БД
        static_cast<QWidget *>( parent() )->close();
    }
    else
    {
        // Диалог информирования о проблеме
        QMessageBox box;
        box.setWindowTitle(tr("Select database"));
        box.setText(tr("Errors detected when switching to database with\npath '%1'\nand trash path '%2'").
                    arg(dbPath).
                    arg(trashPath));
        box.setStandardButtons(QMessageBox::Ok);
        box.setIcon( QMessageBox::Critical );
        box.exec();
    }
}


bool DatabasesManagementController::switchToDatabase(const QString &dbPath,
                                                     const QString &trashPath)
{
    if ( !model->isDbDirectory(dbPath) )
    {
        QMessageBox box;
        box.setText(tr("The selected database directory '%1' is not a database directory").
                    arg(dbPath));
        box.setStandardButtons(QMessageBox::Ok);
        box.setIcon( QMessageBox::Critical );
        box.exec();

        return false;
    }

    if ( !model->isTrashDirectory(trashPath) )
    {
        QMessageBox box;
        box.setText(tr("It is not possible to use the directory '%1' as a trash directory").
                    arg(trashPath));
        box.setStandardButtons(QMessageBox::Ok);
        box.setIcon( QMessageBox::Critical );
        box.exec();

        return false;
    }


    // Обнуляется модель дерева
    KnowTreeModel *knowTreeModel=static_cast<KnowTreeModel*>(find_object<KnowTreeView>("knowTreeView")->model());
    knowTreeModel->clear();

    // Устанавливаются пустые данные в таблицу конечных записей
    find_object<RecordTableController>("recordTableController")->setTableData(nullptr);

    // Изменяются пути к БД в conf.ini файле
    mytetraConfig.set_tetradir(dbPath);
    mytetraConfig.set_trashdir(trashPath);

    // Инициализация переменных, отвечающих за хранилище данных
    dataBaseConfig.init();

    // Проверяется наличие коллекции прикрепляемых к веткам иконок (и иконки создаются если они отсутствуют)
    IconSelectDialog::iconsCollectionCheck();

    // Заполняется модель дерева
    knowTreeModel->initFromXML(dbPath+"/mytetra.xml");

    return true;
}


void DatabasesManagementController::onCreateClicked()
{
    QString title=tr("Create new database");

    // Диалог, поясняющий что нужно выбрать пустую директорию
    QMessageBox firstBox;
    firstBox.setWindowTitle(title);
    firstBox.setText(tr("<b>Select an empty directory</b>"));
    firstBox.setInformativeText(tr("To create a new database, "
                                   "please specify an existing "
                                   "empty directory."));
    firstBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    firstBox.setDefaultButton(QMessageBox::Ok);
    firstBox.button(QMessageBox::Ok)->setText(tr("I understand"));
    firstBox.setIconPixmap( QIcon(":/resource/pic/dbmanagement_db_icon.svg").pixmap(QSize(32, 32)) );
    if( firstBox.exec()!=QMessageBox::Ok )
    {
        return;
    }

    QString workPath;

    // Диалог выбора пустой директории для создания новой БД
    QFileDialog tetradirSelectDialog;
    tetradirSelectDialog.setFileMode(QFileDialog::Directory);
    tetradirSelectDialog.setWindowTitle(tr("Select empty directory for new DB"));
    tetradirSelectDialog.setDirectory("");
    if( tetradirSelectDialog.exec()==QDialog::Accepted )
    {
        if( !tetradirSelectDialog.directory().absolutePath().isEmpty() )
        {
            // Запоминается выбранный пользователем путь
            workPath=tetradirSelectDialog.directory().absolutePath();

            // Выход, если директория не пустая
            QDir dir(workPath);
            QStringList entries = dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
            if ( !entries.isEmpty() )
            {
                QMessageBox msgBox;
                msgBox.setText(tr("It is not possible to create a new database.\n"
                                  "The specified directory is not empty."));
                msgBox.exec();
                return;
            }
        }
        else
        {
            return; // Выход, если был установлен пустой путь
        }
    }
    else
    {
        return; // Выход, если была нажата отмена в диалоге выбора директории
    }

    QString dbPath = workPath+"/data";
    QString trashPath = workPath+"/trash";

    // Если добавляемая директория уже есть в списке баз данных
    // Ситуация возможна если пользователь самостоятельно удалил
    // в файловой системе содержимое директории,
    // которая была зарегистрирована в MyTetra
    if( model->isDbPathExists(dbPath) )
    {
        QMessageBox msgBox;
        msgBox.setText(tr("This database directory already using "
                          "in databases list.\n"
                          "First, please delete corresponding database "
                          "from databases list, and try again"));
        msgBox.exec();
        return;
    }

    // Подтверждение с информацией какие директории будут созданы
    QMessageBox applyBox;
    applyBox.setWindowTitle(title);
    applyBox.setTextFormat(Qt::RichText);
    applyBox.setText(tr("<b>The following database directories will be created</b><br><br>")+
                     tr("Database data directory:<br>")+
                     "<span style='font-family: monospace;'>"+dbPath+"</span><br><br>"+
                     tr("Trash directory:<br>")+
                     "<span style='font-family: monospace;'>"+trashPath+"</span>");
    applyBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    applyBox.setDefaultButton(QMessageBox::Ok);
    applyBox.setIconPixmap( QIcon(":/resource/pic/dbmanagement_db_icon.svg").pixmap(QSize(32, 32)) );
    if( applyBox.exec()!=QMessageBox::Ok )
    {
        return;
    }

    // Здесь считается что все проверки пройдены, и можно создавать БД
    // Внутри функции будут созданы подкаталоги data и trash
    // относительно пути workPath
    mytetraFiles.createFirstAppFiles(workPath,
                                     AppFiles::CreateFirstAppFilesFlags::DB |
                                     AppFiles::CreateFirstAppFilesFlags::TRASH);

    this->addDatabase(dbPath, trashPath);
}


// Добавление существующей базы
void DatabasesManagementController::onAddClicked()
{
    QString title=tr("Append exists database");

    // Диалог, поясняющий что нужно выбрать директорию с существующей БД
    QMessageBox firstBox;
    firstBox.setWindowTitle(title);
    firstBox.setText(tr("<b>First step:</b> select database directiory"));
    firstBox.setInformativeText(tr("To add an existing database,\n"
                                   "please select the directory\n"
                                   "where it is located.\n"
                                   "This directory should contain the file mytetra.xml."));
    firstBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    firstBox.setDefaultButton(QMessageBox::Ok);
    firstBox.button(QMessageBox::Ok)->setText(tr("I understand"));
    firstBox.setIconPixmap( QIcon(":/resource/pic/dbmanagement_db_icon.svg").pixmap(QSize(32, 32)) );
    if ( firstBox.exec()!=QMessageBox::Ok )
    {
        return;
    }

    QString dbPath;
    QString trashPath;

    // Диалог выбора директории с существующей БД
    QFileDialog tetradirSelectDialog;
    tetradirSelectDialog.setFileMode(QFileDialog::Directory);
    tetradirSelectDialog.setWindowTitle(tr("Select directory with an existing database"));
    tetradirSelectDialog.setDirectory("");
    if ( tetradirSelectDialog.exec()==QDialog::Accepted )
    {
        if ( !tetradirSelectDialog.directory().absolutePath().isEmpty() )
        {
            // Запоминается выбранный пользователем путь
            dbPath=tetradirSelectDialog.directory().absolutePath();

            // Выход, если в директории нет файла mytetra.xml
            QFileInfo checkFile(dbPath+"/mytetra.xml");
            if ( !(checkFile.exists() && checkFile.isFile()) )
            {
                QMessageBox msgBox;
                msgBox.setText(tr("Can not find file mytetra.xml in this directory"));
                msgBox.exec();
                return;
            }
        }
        else
        {
            return; // Выход, если был установлен пустой путь
        }
    }
    else
    {
        return; // Выход, если была нажата отмена в диалоге выбора директории
    }

    // Если добавляемая директория уже есть в списке баз данных
    if ( model->isDbPathExists(dbPath) )
    {
        QMessageBox msgBox;
        msgBox.setText(tr("This database directory already using in databses list"));
        msgBox.exec();
        return;
    }


    // Диалог, поясняющий что нужно выбрать директорию корзины
    QMessageBox secondBox;
    secondBox.setWindowTitle(title);
    secondBox.setText(tr("<b>Second step:</b> select trash directiory"));
    secondBox.setInformativeText(tr("For database append please select "
                                    "trash directory.\n"
                                    "MyTetra will put changed notes "
                                    "file copies and changed item tree copies "
                                    "to this directory"));
    secondBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    secondBox.setDefaultButton(QMessageBox::Ok);
    secondBox.button(QMessageBox::Ok)->setText(tr("I understand"));
    secondBox.setIconPixmap( QIcon(":/resource/pic/dbmanagement_trash_icon.svg").pixmap(QSize(32, 32)) );
    if ( secondBox.exec()!=QMessageBox::Ok )
    {
        return;
    }

    // Диалог выбора директории с корзиной
    QFileDialog trashdirSelectDialog;
    trashdirSelectDialog.setFileMode(QFileDialog::Directory);
    trashdirSelectDialog.setWindowTitle(tr("Select trash directory"));
    trashdirSelectDialog.setDirectory("");
    if ( trashdirSelectDialog.exec()==QDialog::Accepted )
    {
        if ( !trashdirSelectDialog.directory().absolutePath().isEmpty() )
        {
            // Запоминается выбранный пользователем путь к корзине
            trashPath=trashdirSelectDialog.directory().absolutePath();
        }
        else
        {
            return; // Выход, если был установлен пустой путь
        }
    }
    else
    {
        return; // Выход, если была нажата отмена
    }


    // Если пользователь ошибся и выбрал в качестве директории корзины
    // ту же директорию что и директория базы данных
    if ( dbPath==trashPath )
    {
        QMessageBox msgBox;
        msgBox.setText(tr("You cannot add a trash directory in database directory."));
        msgBox.exec();
        return;
    }

    this->addDatabase(dbPath, trashPath);
}


void DatabasesManagementController::onEditClicked()
{
    QModelIndexList indexList = view->selectionModel()->selectedRows();

    // Должна быть выбрана только одна строка
    if (indexList.size()!=1)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("You can select only one database for description edit."));
        msgBox.exec();

        return;
    }

    // Указатель на редактируемую строку в виде
    QModelIndex editIndex = indexList[0];

    // Пути, прописанные в выбранной строке
    QString dbPath = model->getCellValue(editIndex.row(),
                                         DBMANAGEMENT_COLUMN_DBPATH,
                                         Qt::UserRole);
    QString trashPath = model->getCellValue(editIndex.row(),
                                            DBMANAGEMENT_COLUMN_TRASHPATH,
                                            Qt::UserRole);

    // Запрещено редактировать описание баз даных, которые не прописаны в knownbases.ini
    if ( !model->isDbInKnownBasesConfig(dbPath, trashPath) )
    {
        QMessageBox msgBox;
        msgBox.setText(tr("It is allowed to edit descriptions only for manually added databases."));
        msgBox.exec();

        return;
    }

    // Существующее описание
    QString description = model->getCellValue(editIndex.row(),
                                              DBMANAGEMENT_COLUMN_DESCRIPT,
                                              Qt::UserRole);

    // Редактирование
    bool ok;
    QString text = QInputDialog::getText(
        nullptr,
        tr("Description edit"),
        tr("Description:"),
        QLineEdit::Normal,
        description,
        &ok);

    if (ok)
    {
        // Сохранение изменений
        bool result = model->editDatabaseDescript(editIndex.row(), text);

        if ( !result )
        {
            QMessageBox msgBox;
            msgBox.setText(tr("Error when applying the changes."));
            msgBox.exec();

            return;
        }
    }
}


void DatabasesManagementController::onDeleteClicked()
{
    QModelIndexList indexList = view->selectionModel()->selectedRows();

    // Должна быть выбрана только одна строка
    if (indexList.size()!=1)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("You can select only one database for deletion."));
        msgBox.exec();

        return;
    }

    // Указатель на удаляемою строку в виде
    QModelIndex deleteIndex = indexList[0];

    // Если это текущая рабочая БД, ее удалять нельзя
    if ( model->getCellValue(deleteIndex.row(),
                             DBMANAGEMENT_COLUMN_ISSELECT,
                             Qt::UserRole) == "true" )
    {
        QMessageBox msgBox;
        msgBox.setText(tr("You cannot delete the current working database."));
        msgBox.exec();

        return;
    }

    // Выбор режима удаления
    QMessageBox box;
    box.setWindowTitle(tr("Deleting a database"));
    box.setText(tr("<b>Select delete mode</b>"));
    box.setInformativeText(tr("Select the mode in which you want to delete"));
    QPushButton *softDelete = box.addButton(tr("Remove from list"), QMessageBox::ActionRole);
    QPushButton *fullDelete = box.addButton(tr("Delete with the data"), QMessageBox::ActionRole);
    QPushButton *cancelButton = box.addButton(tr("Cancel"), QMessageBox::RejectRole);
    box.setDefaultButton(softDelete);
    box.setEscapeButton(cancelButton);
    box.setIcon( QMessageBox::Warning );

    box.exec();

    bool isFullDelete = false;
    QAbstractButton *clickedButton = box.clickedButton();

    if ( clickedButton == softDelete )
    {
        isFullDelete = false; // Только исключение из списка
    }

    if ( clickedButton == fullDelete )
    {
        isFullDelete = true; // Удаление вместе с данными

        bool ok;
        QString text = QInputDialog::getText(
            nullptr,
            tr("Delete a database with all its contents"),
            tr("Write \"yes\" to confirm:"),
            QLineEdit::Normal,
            "",
            &ok);

        if ( ! (ok && text=="yes"))
        {
            QMessageBox msgBox;
            msgBox.setText(tr("You did not write \"yes\", the database deletion is canceled."));
            msgBox.exec();

            return;
        }
    }

    if ( clickedButton == cancelButton )
    {
        return;
    }

    int row = deleteIndex.row();

    QString dbPath = model->getCellValue(row, DBMANAGEMENT_COLUMN_DBPATH);
    QString trashPath = model->getCellValue(row, DBMANAGEMENT_COLUMN_TRASHPATH);

    // Удаление из модели
    model->deleteDatabaseByUser(dbPath, trashPath);

    // Удаление данных на диске
    if ( isFullDelete )
    {
        DiskHelper::removeDirectory(dbPath);
        DiskHelper::removeDirectory(trashPath);
    }

    // Выделение предыдущей строки перед удаляемой после удаления
    int selectRowAfterDelete = (row - 1) < 0 ? 0 : row - 1;
    view->selectRow(selectRowAfterDelete);
}


void DatabasesManagementController::onCopyClicked()
{
    // Перечень индексов ячеек, которые были выбраны
    QModelIndexList indexes=view->selectionModel()->selectedIndexes();

    if (indexes.size() < 1)
        return;

    // Индексы выбранных ячеек идут не так как на экране, поэтому их нужно отсортировать
    std::sort( indexes.begin(), indexes.end() );

    // Размещаемый в буфере обмена текст
    QString selectedText;

    for (int i = 0; i < indexes.size(); ++i)
    {
        QVariant data = model->data(indexes.at(i));
        QString text = data.toString();

        // Добавляется текст из ячейки
        selectedText.append(text);

        // Если не последняя ячейка
        if (i < indexes.size()-1)
        {
            if (indexes.at(i).row() == indexes.at(i+1).row()) // Если текущая ячейка на той же строке что и последующая ячейка
                selectedText.append('\t');
            else // Иначе последующая ячейка на другой строке
                selectedText.append('\n');
        }
    }

    // Выделенный текст вставляется в системный буфоер обмена
    QApplication::clipboard()->setText(selectedText);
}


void DatabasesManagementController::onDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
    {
        return;
    }

    // Вызывается выбор БД
    // Не нужно передавать никаких параметров, например номера строки, потому что
    // при первом клике курсор сам перейдет на выбираемую строку, а она
    // автоматически определится внутри метода onSelectClicked()
    this->onSelectClicked();
}


void DatabasesManagementController::addDatabase(const QString &dbPath,
                                                const QString &trashPath)
{
    model->addDatabaseByUser(dbPath, trashPath);

    // Индекс последней строки
    int newRow = model->rowCount() - 1;

    // Устанавливается выделение на новую строку
    view->selectRow(newRow);
}



