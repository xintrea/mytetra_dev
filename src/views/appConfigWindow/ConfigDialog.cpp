#include "ConfigDialog.h"
#include "ui_ConfigDialog.h"
#include "ConfigPage.h"

#include <QMessageBox>


ConfigDialog::ConfigDialog(QWidget *parent)
  : QDialog(parent)
  , ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);
    setupSignals();
}

ConfigDialog::~ConfigDialog()
{
  delete ui;
}


void ConfigDialog::setupSignals(void)
{
    connect(ui->confirmButtons, SIGNAL(accepted()), this, SLOT(applyChanges()));
    connect(ui->confirmButtons, SIGNAL(rejected()), this, SLOT(reject()));
    connect(ui->contentsWidget,
            SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
            this, SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));
}


QListWidgetItem *ConfigDialog::addWidget(QWidget *inswidget, QString name)
{
    ui->pagesWidget->addWidget(inswidget);

    return createItems(name);
}


// Создаются пункты для вызова нужных конфигурирующих виджетов
QListWidgetItem *ConfigDialog::createItems(QString name)
{
    QListWidgetItem *item = new QListWidgetItem(ui->contentsWidget);
    item->setText(name);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    return item;
}


// Приватный слот, переключение виджета настройки при клике по списку настроечных виджетов
void ConfigDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

    ui->pagesWidget->setCurrentIndex(ui->contentsWidget->row(current));
}

void ConfigDialog::externalChangePage(QListWidgetItem *item)
{
    ui->contentsWidget->setCurrentItem(item, QItemSelectionModel::ClearAndSelect);
}


void ConfigDialog::updateListWidth(void)
{
    ui->contentsWidget->updateGeometry();
    ui->contentsWidget->update();
}


void ConfigDialog::applyChanges(void)
{
    int difficult_flag = 0;

    // Перебираются виджеты настройки
    for (int i = 0; i < ui->pagesWidget->count(); i++)
    {
        // Выясняется указатель на виджет
        ConfigPage *cnpg = qobject_cast<ConfigPage *>(ui->pagesWidget->widget(i));

        // Вызывается метод apply_changes() для текущего перебираемого виджета
        if (cnpg->applyChanges() == 1)
            difficult_flag = 1;
    }

    // Если требуется перезапустить программу для принятия изменений
    if (difficult_flag == 1)
    {
        QMessageBox::warning(this, tr("Warning"),
                             tr("The program will have to be restarted for changes to take effect."),
                             QMessageBox::Ok);
        exit(0);
    }

    // Диалог настройки закрывается
    close();
}
