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
#include <QtGlobal>

#include "main.h"
#include "IconSelectDialog.h"
#include "models/appConfig/AppConfig.h"
#include "views/mainWindow/MainWindow.h"
#include "libraries/helpers/ObjectHelper.h"
#include "libraries/helpers/MessageHelper.h"

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


// Проверка наличия коллекции иконок, и создание иконок из ресурсов,
// если иконки на диске отсутствовали
void IconSelectDialog::iconsCollectionCheck()
{
  qDebug() << "In iconsCollectionCheck(). Mytetra XML dir is: " << mytetraConfig.get_tetradir();

  QString mytetraXmlDirName=mytetraConfig.get_tetradir();
  QDir mytetraXmlDir( mytetraXmlDirName );

  QString iconsDirName="icons"; // Путь относительно каталога где лежит mytetra.xml
  QFileInfo iconsDirInfo(mytetraXmlDirName+"/"+iconsDirName);

  // Если директория иконок существует, ничего делать не нужно
  if(iconsDirInfo.exists())
  {
    qDebug() << "Icons directory is exist";
    return;
  }

  // Создаются файлы иконок из ресурсов
  Q_INIT_RESOURCE(icons);

  // Выясняется список директорий (т. е. разделов с иконками)
  QDir sectionListDir(":/resource/icons/");
  sectionListDir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
  QFileInfoList subdirList=sectionListDir.entryInfoList();

  // Перебираются секции
  for(int i=0; i<subdirList.size(); ++i)
  {
    QString sectionName=subdirList.at(i).fileName();

    qDebug() << "Extract icon section: " << sectionName;

    // Создается директория секции
    QString sectionDirName=iconsDirName+"/"+sectionName;
    if( !mytetraXmlDir.mkpath(sectionDirName) )
    {
      QString msg="Can't create icons section directory: "+sectionDirName;
      showMessageBox(msg);
      qDebug() << msg;
      return;
    }

    QFile::setPermissions(sectionDirName, QFile::ReadUser | QFile::WriteUser);

    // Выясняется список иконок в директории раздела
    QDir iconsFileDir(":/resource/icons/"+sectionName);
    iconsFileDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    iconsFileDir.setNameFilters( (QStringList() << "*.svg" << "*.png") );
    QFileInfoList iconsFileList=iconsFileDir.entryInfoList();

    // Перебираются файлы иконок
    for(int i=0; i<iconsFileList.size(); ++i)
    {
      QString iconName=iconsFileList.at(i).fileName();

      QString iconFromName=":/resource/icons/"+sectionName+"/"+iconName;
      QString iconToName=mytetraXmlDirName+"/"+iconsDirName+"/"+sectionName+"/"+iconName;

      // qDebug() << "Copy icon file. From: " << iconFromName << " To: " << iconToName;

      QFile::copy(iconFromName, iconToName);
      QFile::setPermissions(iconFromName, QFile::ReadUser | QFile::WriteUser);
    }
  }

  // Ресурсы иконок из памяти удаляются, так как они теперь развернуты на файловую систему
  Q_CLEANUP_RESOURCE(icons);
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
  connect(&sectionComboBox, qOverload<int>(&QComboBox::currentIndexChanged),
          this,             &IconSelectDialog::onSectionCurrentIndexChanged);

  // Выбор иконки
  connect(&iconList, &QListWidget::itemSelectionChanged,
          this,      &IconSelectDialog::onIconItemSelectionChanged);

  // Двойной клик
  connect(&iconList, &QListWidget::itemDoubleClicked,
          this,      &IconSelectDialog::accept);

  connect(&buttonRemoveIcon, &QPushButton::clicked,
          this,              &IconSelectDialog::onRemoveIconClick);
  connect(&buttonOk, &QPushButton::clicked,
          this,      &IconSelectDialog::onOkClick);
  connect(&buttonCancel, &QPushButton::clicked,
          this,          &IconSelectDialog::onCancelClick);
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
    criticalError("Can't set icon directory path for IconSelectDialog. Path is not directory: "+iPath);
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
  if(defaultSectionIndex != -1)
  {
    if (defaultSectionIndex < sectionComboBox.count())
      sectionComboBox.setCurrentIndex(defaultSectionIndex);
    else
      showMessageBox(tr("Unable to set a default section %1.").arg(sectionComboBox.itemText(defaultSectionIndex)));
  }
}


void IconSelectDialog::setDefaultSection(QString iSectionName)
{
  if(path.length()>0)
  {
    criticalError("Can't set icon default section. Set default section before set path.");
    return;
  }
  for (int i = 0; sectionComboBox.count(); ++i)
    if(sectionComboBox.itemText(i) == iSectionName) {
      defaultSectionIndex = i;
      break;
    }
}


QString IconSelectDialog::getCurrentSection()
{
  return sectionComboBox.itemText(currentSectionIndex);
}


// Обновление экранного списка иконок
void IconSelectDialog::updateIcons()
{
  if(defaultSectionIndex != -1)
    onSectionCurrentIndexChanged(defaultSectionIndex);
  else
    onSectionCurrentIndexChanged(0);
}


// Слот при изменении строки раздела в sectionComboBox
void IconSelectDialog::onSectionCurrentIndexChanged(int idx)
{
  // Если еще не разрешено обновлять список иконок
  if(!enableIconUpdate)
    return;

  currentSectionIndex=idx;

  // Очищается экранный список иконок
  iconList.clear(); // todo: Здесь сегфолт... Разобраться.

  QString iconDirName=path+"/"+sectionComboBox.itemText(idx);

  QDir dir(iconDirName);
  dir.setFilter(QDir::Files | QDir::Readable);
  dir.setNameFilters( (QStringList() << "*.svg" << "*.png") );
  QFileInfoList iconFileList=dir.entryInfoList();


  // Если в выбранной секции нет никаких иконок
  if(iconFileList.count()==0)
  {
    showMessageBox(tr("The section \"%1\" has not any icons").arg(sectionComboBox.itemText(idx)));
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

  currentFileName=path+"/"+sectionComboBox.itemText(currentSectionIndex)+"/"+shortSelectFileName;
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

