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

}


void DatabasesManagementController::onCreateClicked()
{

}


void DatabasesManagementController::onAddClicked()
{

}


void DatabasesManagementController::onCopyClicked()
{
    // Перечень индексов ячеек, которые были выбраны
    QModelIndexList indexes=view->selectionModel()->selectedIndexes();

    if(indexes.size() < 1)
        return;

    // Индексы выбранных ячеек идут не так как на экране, поэтому их нужно отсортировать
    std::sort( indexes.begin(), indexes.end() );

    // Размещаемый в буфере обмена текст
    QString selectedText;

    for (int i=0; i<indexes.size(); ++i)
    {
        QVariant data = model->data(indexes.at(i));
        QString text = data.toString();

        // Добавляется текст из ячейки
        selectedText.append(text);

        // Если не последняя ячейка
        if(i<indexes.size()-1)
        {
            if(indexes.at(i).row() == indexes.at(i+1).row()) // Если текущая ячейка на той же строке что и последующая ячейка
                selectedText.append('\t');
            else // Иначе последующая ячейка на другой строке
                selectedText.append('\n');
        }
    }

    QApplication::clipboard()->setText(selectedText);
}

