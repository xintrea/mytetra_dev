#include <QStackedWidget>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QMessageBox>

#include "ConfigDialog.h"
#include "ConfigPage.h"


ConfigDialog::ConfigDialog()
{
 setup_ui();
 setup_signals();
 assembly();
}


void ConfigDialog::setup_ui(void)
{
    // Список конфигурирующих виджетов
    contentsWidget = new QListWidget;
    contentsWidget->setViewMode(QListView::ListMode);
    contentsWidget->setMovement(QListView::Static);
    contentsWidget->setMinimumWidth(128);
    contentsWidget->setMaximumWidth(150);
    contentsWidget->setCurrentRow(0);
    contentsWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    // Область переключения конфигурирующих виджетов
    pagesWidget = new QStackedWidget;

    // Кнопки закрытия диалога
    confirmButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
}


void ConfigDialog::setup_signals(void)
{
    connect(confirmButtons, SIGNAL(accepted()), this, SLOT(apply_changes()));

    connect(confirmButtons, SIGNAL(rejected()), this, SLOT(reject()));

    connect(contentsWidget,
            SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
            this, SLOT(change_page(QListWidgetItem *, QListWidgetItem*)));

}


void ConfigDialog::assembly(void)
{
    QHBoxLayout *horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(contentsWidget);
    horizontalLayout->addWidget(pagesWidget, 1);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(confirmButtons);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(horizontalLayout);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}


void ConfigDialog::set_window_title(QString title)
{
 // Строка в заголовке окна
 setWindowTitle(title);
}


void ConfigDialog::add_widget(QWidget *inswidget, QString name)
{
 pagesWidget->addWidget(inswidget);

 create_items(name);
}


// Создаются пункты для вызова нужных конфигурирующих виджетов
void ConfigDialog::create_items(QString name)
{
 QListWidgetItem *configButton = new QListWidgetItem(contentsWidget);
 configButton->setText(name);
 configButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}


void ConfigDialog::change_page(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

    pagesWidget->setCurrentIndex(contentsWidget->row(current));
}


void ConfigDialog::updateListWidth(void)
{
 contentsWidget->updateGeometry();
 contentsWidget->update();
}


void ConfigDialog::apply_changes(void)
{
 int difficult_flag=0;

 // Перебираются виджеты настройки
 for(int i=0;i<pagesWidget->count();i++)
  {
   // Выясняется указатель на виджет
   ConfigPage *cnpg=qobject_cast<ConfigPage *>(pagesWidget->widget(i));
    
   // Вызывается метод apply_changes() для текущего перебираемого виджета
   if( cnpg->apply_changes()==1 )
    difficult_flag=1;
  }
 
 // Если требуется перезапустить программу для принятия изменений
 if(difficult_flag==1)
  {
   QMessageBox::warning(this, tr("Warning"),
                              tr("The program will have to be restarted for changes to take effect."),
                              QMessageBox::Ok); 
   exit(0);
  }

 // Диалог настройки закрывается
 close();
}

