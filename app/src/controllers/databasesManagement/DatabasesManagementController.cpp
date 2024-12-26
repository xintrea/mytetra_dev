#include <QObject>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <algorithm>
#include <QModelIndexList>

#include "main.h"
#include "DatabasesManagementController.h"
#include "views/databasesManagement/DatabasesManagementTable.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/GlobalParameters.h"
#include "libraries/helpers/DiskHelper.h"
#include "views/dialog/ReduceMessageBox.h"


extern GlobalParameters globalParameters;
extern AppConfig mytetraConfig;


DatabasesManagementController::DatabasesManagementController(QObject *parent) : QObject(parent)
{
    // Создается область со списком записей лога
    view=new DatabasesManagementTable( qobject_cast<QWidget *>(parent) ); // Вид размещается внутри виджета Screen
    view->setObjectName("DatabasesManagementTable");
    view->setController(this);

    // Создание модели данных
    model=new DatabasesManagementModel(this);
    model->setObjectName("DatabasesManagementModel");

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

    int row=indexList[0].row();

    // Установка пометки выбора базы данных
    model->selectDatabase(row);

    // Выбор текущей строки
    const QModelIndex index = model->index(row, 0);
    view->setCurrentIndex(index);
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

    QString emptyDirPath;

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
            emptyDirPath=tetradirSelectDialog.directory().absolutePath();

            // Выход, если директория не пустая
            QDir dir(emptyDirPath);
            QStringList entries = dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
            if (entries.isEmpty()) {
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

    // Если добавляемая директория уже есть в списке баз данных
    // Ситуация возможна если пользователь самостоятельно удалил
    // в файловой системе содержимое директории,
    // которая была зарегистрирована в MyTetra
    if( model->isDbPathExists(emptyDirPath) )
    {
        QMessageBox msgBox;
        msgBox.setText(tr("This database directory already using "
                          "in databases list.\n"
                          "First, please delete corresponding database "
                          "from databases list"));
        msgBox.exec();
        return;
    }

    QString dbPath = emptyDirPath+"/data";
    QString trashPath = emptyDirPath+"/trash";

    // Подтверждение с информацией какие директории будут созданы
    QMessageBox applyBox;
    applyBox.setWindowTitle(title);
    applyBox.setText(tr("<b>The following database directories will be created</b>"));
    applyBox.setInformativeText(tr("Database data directory: ")+
                                dbPath+"\n\n"+
                                tr("Trash directory: ")+
                                trashPath);
    applyBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    applyBox.setDefaultButton(QMessageBox::Ok);
    applyBox.setIconPixmap( QIcon(":/resource/pic/dbmanagement_db_icon.svg").pixmap(QSize(32, 32)) );
    if( applyBox.exec()!=QMessageBox::Ok )
    {
        return;
    }

    // Здесь считается что все проверки пройдены, и можно создавать БД
    QDir dir;
    dir.mkpath(dbPath);
    dir.mkpath(trashPath);

    globalParameters.createFirstProgramFiles(dbPath);

    model->addDatabaseByUser(dbPath, trashPath);
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

    model->addDatabaseByUser(dbPath, trashPath);
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

    // Выбор режима удаления
    QMessageBox box;
    box.setWindowTitle(tr("Deleting a database"));
    box.setText(tr("<b>Select delete mode"));
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
    }
    if ( clickedButton == cancelButton )
    {
        return;
    }

    QModelIndex deleteIndex = indexList[0];

    QString dbPath = model->getCellValue(deleteIndex.row(), DBMANAGEMENT_COLUMN_DBPATH);
    QString trashPath = model->getCellValue(deleteIndex.row(), DBMANAGEMENT_COLUMN_TRASHPATH);

    // Удаление из модели
    model->deleteDatabaseByUser(dbPath, trashPath);

    // Удаление данных на диске
    if ( isFullDelete )
    {
        DiskHelper::removeDirectory(dbPath);
        DiskHelper::removeDirectory(trashPath);
    }
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

    QApplication::clipboard()->setText(selectedText);
}

