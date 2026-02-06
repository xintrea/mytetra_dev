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

#include "IconSelectDialog.h"
#include "models/appConfig/AppConfig.h"
#include "views/mainWindow/MainWindow.h"
#include "libraries/helpers/ObjectHelper.h"
#include "libraries/helpers/MessageHelper.h"

extern AppConfig mytetraConfig;


IconSelectDialog::IconSelectDialog()
{
  m_enableIconUpdate=false;

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
  m_sectionLabel.setText(tr("Section"));

  // В списке иконок устанавливается размер отображаемых иконок
  int iconSize=mytetraConfig.getPreviewIconSize();
  m_iconList.setIconSize(QSize(iconSize, iconSize));

  // Линейка наполяемости скрывается. Она должна быть видна только в процессе загрузки иконок
  m_progressBar.hide();

  // Кнопки
  m_buttonRemoveIcon.setText(tr("Unset icon"));

  m_buttonOk.setText(tr("Ok"));
  m_buttonOk.setAutoDefault(true);
  m_buttonOk.setDefault(true);

  m_buttonCancel.setText(tr("Cancel"));
}


void IconSelectDialog::setupSignals()
{
  // Выбор раздела
  // Так как есть две версии сигнала currentIndexChanged, с разными типами
  // параметров - int и const QString &, то надо прописать какой сигнал
  // выбирается через qOverload. Вариант с const QString & считается устаревшим
  // и объявлен что будет отменен в дальнейших версиях Qt
  connect(&m_sectionComboBox, qOverload<int>(&QComboBox::currentIndexChanged),
          this,             &IconSelectDialog::onSectionCurrentIndexChanged);

  // Выбор иконки
  connect(&m_iconList, &QListWidget::itemSelectionChanged,
          this,      &IconSelectDialog::onIconItemSelectionChanged);

  // Двойной клик
  connect(&m_iconList, &QListWidget::itemDoubleClicked,
          this,      &IconSelectDialog::accept);

  connect(&m_buttonRemoveIcon, &QPushButton::clicked,
          this,              &IconSelectDialog::onRemoveIconClick);
  connect(&m_buttonOk, &QPushButton::clicked,
          this,      &IconSelectDialog::onOkClick);
  connect(&m_buttonCancel, &QPushButton::clicked,
          this,          &IconSelectDialog::onCancelClick);
}


void IconSelectDialog::assembly()
{
  QHBoxLayout *sectionLayout=new QHBoxLayout(); // Указывать this не нужно, так как он назначится в момент вставки в основной слой
  sectionLayout->addWidget( &m_sectionLabel );
  sectionLayout->addWidget( &m_sectionComboBox );

  QHBoxLayout *buttonLayout=new QHBoxLayout(); // Указывать this не нужно, так как он назначится в момент вставки в основной слой
  buttonLayout->addWidget( &m_buttonRemoveIcon );
  buttonLayout->addStretch();
  buttonLayout->addSpacing( int( 0.05 * (float)(find_object<MainWindow>("mainwindow")->width()) ) );
  buttonLayout->addWidget( &m_buttonOk );
  buttonLayout->addWidget( &m_buttonCancel );

  QVBoxLayout *mainLayout=new QVBoxLayout(); // Указывать this не нужно, так как он назначится когда этот слой станет основным слоем
  mainLayout->addLayout( sectionLayout );
  mainLayout->addWidget( &m_iconList );
  mainLayout->addWidget( &m_progressBar );
  mainLayout->addLayout( buttonLayout );

  setLayout(mainLayout);
}


int IconSelectDialog::exec()
{
  // Обновление списка иконок разрешается непосредственно перед основным циклом, чтобы процесс загрузки иконок
  // был виден на линейке заполняемости
  m_enableIconUpdate=true;

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
  m_path=iPath;

  // В момент установки пути к директории, устанавливается перечень разделов согласно поддиректориям
  QDir dir(m_path);
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

    m_sectionComboBox.addItem(subdirInfo.fileName());
  }


  // Если была установлена секция по-умолчанию
  if ( m_defaultSectionIndex != -1 )
  {
      if ( m_defaultSectionIndex < m_sectionComboBox.count() )
        m_sectionComboBox.setCurrentIndex( m_defaultSectionIndex );
      else
        showMessageBox(tr("Unable to set a default section %1.").
                       arg(m_sectionComboBox.itemText( m_defaultSectionIndex )));
  }
}


void IconSelectDialog::setDefaultSection(QString sectionName)
{
  if(m_path.length()>0)
  {
    criticalError("Can't set icon default section. Set default section before set path.");
    return;
  }

  for (int i = 0; m_sectionComboBox.count(); ++i)
  {
    if(m_sectionComboBox.itemText(i) == sectionName) {

      m_defaultSectionIndex = i;

      break;

    }
  }
}


QString IconSelectDialog::getCurrentSection()
{
  return m_sectionComboBox.itemText( m_currentSectionIndex );
}


// Обновление экранного списка иконок
void IconSelectDialog::updateIcons()
{
  if(m_defaultSectionIndex != -1)
    onSectionCurrentIndexChanged( m_defaultSectionIndex );
  else
    onSectionCurrentIndexChanged( 0 );
}


// Слот при изменении строки раздела в sectionComboBox
void IconSelectDialog::onSectionCurrentIndexChanged(int idx)
{
  // Если еще не разрешено обновлять список иконок
  if(!m_enableIconUpdate)
    return;

  m_currentSectionIndex=idx;

  // Очищается экранный список иконок
  m_iconList.clear(); // todo: Здесь сегфолт... Разобраться.

  QString iconDirName=m_path+"/"+m_sectionComboBox.itemText(idx);

  QDir dir(iconDirName);
  dir.setFilter(QDir::Files | QDir::Readable);
  dir.setNameFilters( (QStringList() << "*.svg" << "*.png") );
  QFileInfoList iconFileList=dir.entryInfoList();


  // Если в выбранной секции нет никаких иконок
  if(iconFileList.count()==0)
  {
    showMessageBox(tr("The section \"%1\" has not any icons").arg(m_sectionComboBox.itemText(idx)));
    this->close();
    return;
  }

  // Отрисовывается линейка наполняемости, так как считывание иконок может быть долгим
  m_progressBar.setMinimum(0);
  m_progressBar.setMaximum(iconFileList.size());
  m_progressBar.show();

  // Заполняется экранный список иконок
  for(int i=0; i<iconFileList.size(); ++i)
  {
    m_progressBar.setValue(i);

    QFileInfo iconInfo=iconFileList.at(i);

    // qDebug() << "Find icon: " << iconInfo.fileName();

    // Создается элемент списка, который вставляется в iconList (поэтому он уничтожится при уничтожении саписка)
    QListWidgetItem *item=new QListWidgetItem( iconInfo.fileName(), &m_iconList);
    item->setIcon(QIcon(iconInfo.filePath()));
  }

  m_progressBar.hide();
}


// Когда выбрана иконка
void IconSelectDialog::onIconItemSelectionChanged()
{
  // QString shortSelectFileName=iconList.selectedItems().at(0)->text(); // Неясно, но похоже что после этой конструкции идет сегфолт в методе clean()
  QString shortSelectFileName=m_iconList.currentItem()->text();

  m_currentFileName=m_path+"/"+this->getCurrentSection()+"/"+shortSelectFileName;
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
  return m_currentFileName;
}

