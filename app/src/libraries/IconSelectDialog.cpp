#include <QLabel>
#include <QComboBox>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDir>
#include <QDebug>
#include <QSize>
#include <QTimer>

#include "main.h"
#include "IconSelectDialog.h"
#include "models/appConfig/AppConfig.h"
#include "views/mainWindow/MainWindow.h"

extern AppConfig mytetraConfig;


IconSelectDialog::IconSelectDialog()
{
  enableIconUpdate=false;

  setupUI();
  setupSignals();
  assembly();
}


IconSelectDialog::~IconSelectDialog()
{

}


void IconSelectDialog::setupUI()
{
  // Установка ширины и высоты окна
  int dialogWidth=int( 0.5 * (float)(find_object<MainWindow>("mainwindow")->width()) );
  int dialogHeight=int( 0.5 * (float)(find_object<MainWindow>("mainwindow")->height()) );
  setMinimumWidth( dialogWidth );
  setMinimumHeight( dialogHeight );
  resize( size() );

  // Заголовок окна
  setWindowTitle(tr("Select icon"));

  // Надпись "Раздел"
  sectionLabel.setText(tr("Section"));

  // В списке иконок устанавливается размер отображаемых иконок
  int iconSize=mytetraConfig.getPreviewIconSize();
  iconList.setIconSize(QSize(iconSize, iconSize));

  // Линейка наполяемости скрывается. Она должна быть видна только в процессе загрузки иконок
  progressBar.hide();

  // Кнопки
  buttonRemoveIcon.setText(tr("Unset icon"));

  buttonOk.setText(tr("Ok"));
  buttonOk.setAutoDefault(true);
  buttonOk.setDefault(true);

  buttonCancel.setText(tr("Cancel"));
}


void IconSelectDialog::setupSignals()
{
  // Выбор раздела
  connect( &sectionComboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(onSectionCurrentIndexChanged(const QString &)));

  // Выбор иконки
  connect( &iconList, SIGNAL(itemSelectionChanged()), this, SLOT(onIconItemSelectionChanged()));

  // Двойной клик
  connect( &iconList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(accept()));

  connect( &buttonRemoveIcon, SIGNAL(clicked()), this, SLOT(onRemoveIconClick()) );
  connect( &buttonOk, SIGNAL(clicked()), this, SLOT(onOkClick(void)) );
  connect( &buttonCancel, SIGNAL(clicked()), this, SLOT(onCancelClick()) );
}


void IconSelectDialog::assembly()
{
  QHBoxLayout *sectionLayout=new QHBoxLayout(); // Указывать this не нужно, так как он назначится в момент вставки в основной слой
  sectionLayout->addWidget( &sectionLabel );
  sectionLayout->addWidget( &sectionComboBox );

  QHBoxLayout *buttonLayout=new QHBoxLayout(); // Указывать this не нужно, так как он назначится в момент вставки в основной слой
  buttonLayout->addWidget( &buttonRemoveIcon );
  buttonLayout->addStretch();
  buttonLayout->addSpacing( int( 0.05 * (float)(find_object<MainWindow>("mainwindow")->width()) ) );
  buttonLayout->addWidget( &buttonOk );
  buttonLayout->addWidget( &buttonCancel );

  QVBoxLayout *mainLayout=new QVBoxLayout(); // Указывать this не нужно, так как он назначится когда этот слой станет основным слоем
  mainLayout->addLayout( sectionLayout );
  mainLayout->addWidget( &iconList );
  mainLayout->addWidget( &progressBar );
  mainLayout->addLayout( buttonLayout );

  setLayout(mainLayout);
}


int IconSelectDialog::exec()
{
  // Обновление списка иконок разрешается непосредственно перед основным циклом, чтобы процесс загрузки иконок
  // был виден на линейке заполняемости
  enableIconUpdate=true;

  // Загрузка иконок запустится через 1 сек после старта основного цикла диалога
  QTimer::singleShot(0, this, SLOT(updateIcons()));

  return QDialog::exec();
}


void IconSelectDialog::setPath(QString iPath)
{
  // Если переданный путь не является директорией
  if( !QFileInfo(iPath).isDir() )
  {
    criticalError("Cant set icon directory path for IconSelectDialog. Path is not directory: "+iPath);
    return;
  }


  // Если директорию по переданному пути невозможно прочитать
  if( !QFileInfo(iPath).isReadable() )
  {
    showMessageBox(tr("The icons directory %1 is not readable.").arg(iPath));
    return;
  }

  // Запоминается заданный путь
  path=iPath;

  // В момент установки пути к директории, устанавливается перечень разделов согласно поддиректориям
  QDir dir(path);
  dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
  QFileInfoList subdirList=dir.entryInfoList();

  // Если нет никаких поддиректорий, значит нет и секций
  if(subdirList.count()==0)
  {
    showMessageBox(tr("The directory %1 has not any icon section.").arg(iPath));
    this->close();
    return;
  }


  // Заполняется перечень секций
  for(int i=0; i<subdirList.size(); ++i)
  {
    QFileInfo subdirInfo=subdirList.at(i);

    qDebug() << "Find icons section: " << subdirInfo.fileName();

    sectionComboBox.addItem(subdirInfo.fileName());
  }


  // Если была задана секция по-умолчанию
  if(defaultSectionName.length()>0)
  {
    bool find=false;
    for(int i=0; i<sectionComboBox.count(); ++i)
      if(sectionComboBox.itemText(i)==defaultSectionName)
      {
        sectionComboBox.setCurrentIndex(i);
        find=true;
        break;
      }

    if(!find)
    {
      showMessageBox(tr("Unable to set a default section %1.").arg(defaultSectionName));
      return;
    }
  }
}


void IconSelectDialog::setDefaultSection(QString iSectionName)
{
  if(path.length()>0)
  {
    criticalError("Cant set icon default section. Set default section before set path.");
    return;
  }

  defaultSectionName=iSectionName;
}


QString IconSelectDialog::getCurrentSection()
{
  return currentSectionName;
}


// Обновление экранного списка иконок
void IconSelectDialog::updateIcons()
{
  if(defaultSectionName.length()>0)
    onSectionCurrentIndexChanged(defaultSectionName);
  else
    onSectionCurrentIndexChanged( sectionComboBox.itemText(0) );
}


// Слот при изменении строки раздела в sectionComboBox
void IconSelectDialog::onSectionCurrentIndexChanged(const QString &iText)
{
  // Если еще не разрешено обновлять список иконок
  if(!enableIconUpdate)
    return;

  currentSectionName=iText;

  // Очищается экранный список иконок
  iconList.clear(); // todo: Здесь сегфолт... Разобраться.

  QString iconDirName=path+"/"+iText;

  QDir dir(iconDirName);
  dir.setFilter(QDir::Files | QDir::Readable);
  dir.setNameFilters( (QStringList() << "*.svg" << "*.png") );
  QFileInfoList iconFileList=dir.entryInfoList();


  // Если в выбранной секции нет никаких иконок
  if(iconFileList.count()==0)
  {
    showMessageBox(tr("The section \"%1\" has not any icons").arg(iText));
    this->close();
    return;
  }

  // Отрисовывается линейка наполняемости, так как считывание иконок может быть долгим
  progressBar.setMinimum(0);
  progressBar.setMaximum(iconFileList.size());
  progressBar.show();

  // Заполняется экранный список иконок
  for(int i=0; i<iconFileList.size(); ++i)
  {
    progressBar.setValue(i);

    QFileInfo iconInfo=iconFileList.at(i);

    // qDebug() << "Find icon: " << iconInfo.fileName();

    // Создается элемент списка, который вставляется в iconList (поэтому он уничтожится при уничтожении саписка)
    QListWidgetItem *item=new QListWidgetItem( iconInfo.fileName(), &iconList);
    item->setIcon(QIcon(iconInfo.filePath()));
  }

  progressBar.hide();
}


// Когда выбрана иконка
void IconSelectDialog::onIconItemSelectionChanged()
{
  // QString shortSelectFileName=iconList.selectedItems().at(0)->text(); // Неясно, но похоже что после этой конструкции идет сегфолт в методе clean()
  QString shortSelectFileName=iconList.currentItem()->text();

  currentFileName=path+"/"+currentSectionName+"/"+shortSelectFileName;
}


void IconSelectDialog::onRemoveIconClick()
{
  done(RemoveIconCode);
}


void IconSelectDialog::onOkClick()
{
  accept();
}


void IconSelectDialog::onCancelClick()
{
  reject();
}


QString IconSelectDialog::getSelectFileName()
{
  return currentFileName;
}

