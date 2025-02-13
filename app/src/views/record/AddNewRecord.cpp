#include <QWidget>
#include <QTextEdit>
#include <QtDebug>
#include <QSizePolicy>
#include <QIcon>
#include <QMessageBox>
#include <QTextDocumentFragment>
#include <QDir>
#include <QKeyEvent>

#include "AddNewRecord.h"
#include "main.h"
#include "InfoFieldEnter.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/GlobalParameters.h"
#include "libraries/wyedit/Editor.h"
#include "libraries/helpers/DiskHelper.h"
#include "libraries/ShortcutManager.h"
#include "libraries/helpers/DebugHelper.h"


extern GlobalParameters globalParameters;
extern AppConfig mytetraConfig;
extern ShortcutManager shortcutManager;


// Окно добавления новой записи

AddNewRecord::AddNewRecord( QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f)
{
  setupUI();
  setupShortcuts();
  setupSignals();
  assembly();

  setupEventFilter();
}


AddNewRecord::~AddNewRecord()
{

}


void AddNewRecord::setupUI(void)
{
  this->setWindowTitle(tr("Enter a new note"));

  // Виджет ввода инфополей
  infoField.setDisplayOnlyEditableFields(true);

  // Редактор текста записи
  recordTextEditor.initEnableAssembly(true);
  recordTextEditor.initConfigFileName(globalParameters.getWorkDirectory()+"/editorconf.ini");
  recordTextEditor.initEnableRandomSeed(false);
  recordTextEditor.initDisableToolList( mytetraConfig.getHideEditorTools() + (QStringList() << "save" << "showText" << "toAttach" << "settings" << "expandEditArea") );
  recordTextEditor.init(Editor::WYEDIT_DESKTOP_MODE); // Так как это окно, в мобильном режиме его инициализировать не нужно, так как есть кнопка Отмена

  // В редакторе текста записи устанавливается пустой текст, что активирует правильное
  // начальное положение различных кнопок редактора, например кнопок цвета текста
  recordTextEditor.setTextarea("");

  // Кнопки OK и Cancel
  buttonBox.setOrientation(Qt::Horizontal);
  buttonBox.setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::NoButton|QDialogButtonBox::Cancel);

  // На кнопку OK назначается комбинация клавиш Ctrl+Enter и она устанавливается как кнопка по умолчанию
  QPushButton *OkButton=buttonBox.button(QDialogButtonBox::Ok); // Выясняется указатель на кнопку OK
  OkButton->setAutoDefault(true);
  OkButton->setDefault(true);

  // С кнопки Cancel снимается флаг срабатывания по умолчанию,
  // это нужно для тулкитов основанных на GTK и прочих подобных библиотеках
  QPushButton *CancelButton=buttonBox.button(QDialogButtonBox::Cancel); // Выясняется указатель на кнопку Cancel
  CancelButton->setAutoDefault(false);
  CancelButton->setDefault(false);
}


void AddNewRecord::setupShortcuts(void)
{
    qDebug() << "Setup shortcut for" << this->metaObject()->className();

    // На кнопку OK назначается комбинация клавиш Ctrl+Enter
    QPushButton *okButton=buttonBox.button(QDialogButtonBox::Ok); // Выясняется указатель на кнопку OK
    okButton->setShortcut( shortcutManager.getKeySequence("misc-editConfirm") ); // Устанавливается шорткат
    okButton->setToolTip( shortcutManager.getKeySequenceAsText("misc-editConfirm") ); // ToolTip зависит от шортката
}


void AddNewRecord::setupSignals(void)
{
  connect(&buttonBox, &QDialogButtonBox::accepted, this, &AddNewRecord::okClick);
  connect(&buttonBox, &QDialogButtonBox::rejected, this, &AddNewRecord::reject);

  // Обновление горячих клавиш, если они были изменены
  connect(&shortcutManager, &ShortcutManager::updateWidgetShortcut, this, &AddNewRecord::setupShortcuts);
}


void AddNewRecord::assembly(void)
{
  // Размещалка элементов
  QVBoxLayout *layout=new QVBoxLayout();
  layout->setMargin(8);
  layout->setSpacing(10);

  // Добавление элементов в размещалку
  layout->addWidget(&infoField);
  layout->addWidget(&recordTextEditor);
  layout->addWidget(&buttonBox,0,Qt::AlignRight);

  setLayout(layout);

  // Фокус устанавливается на поле ввода названия записи
  infoField.setFocusToStart();

  // QWidget *wdgt=new QWidget;
  // wdgt->setLayout(vbl);
  // setCentralWidget(wdgt);
}


void AddNewRecord::setupEventFilter(void)
{
  // Для области редактирования задается eventFilter (используется для отлова нажатия на ESC)
  recordTextEditor.installEventFilter(this);
}


bool AddNewRecord::eventFilter(QObject *object, QEvent *event)
{
  // qDebug() << "Editor::eventFilter()";

  // Отслеживание нажатия ESC в области редактирования текста
  if (object == &recordTextEditor)
  {
    if(event->type() == QEvent::KeyPress)
    {
      QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
      if (keyEvent->key() == Qt::Key_Escape)
      {
        // qDebug() << "Press ESC key";
        close();
        return true;
      }
    }
  }

  return QDialog::eventFilter(object, event); // todo: тут подумать, возможно здесь надо возвращать false
}


void AddNewRecord::okClick(void)
{
  QString message="";

  // Проверка наличия названия записи
  if(infoField.getField("name").length()==0)
    message=message+tr("Please enter the note's <b>title</b>. ");

  // Проверка наличия текста записи
  if( !mytetraConfig.getEnableCreateEmptyRecord() ) // Если не разрешено создавать запись с пустым текстом
  {
    QTextDocumentFragment i;
    QString j;
    i=QTextDocumentFragment::fromHtml(getField("text"));
    j=i.toPlainText();
    if(j.length()==0)
      message=message+tr("Please enter the note's <b>text</b>. ");
  }

  // Если что-то не заполнено, выдается предупреждение
  if(message.length()>0)
  {
    QMessageBox::warning(this,tr("A new note cannot be added"),message,
                         QMessageBox::Close);
    return;
  }

  // Картинки сохраняются
  imagesDirName=DiskHelper::createTempDirectory();
  recordTextEditor.setWorkDirectory(imagesDirName);
  recordTextEditor.saveTextareaImages(Editor::SAVE_IMAGES_SIMPLE);

  emit(accept());
}


QString AddNewRecord::getImagesDirectory(void)
{
  if(imagesDirName.length()==0)
  {
    criticalError("In add new record function can not generate temp directory with saved images.");
    return "";
  }

  return imagesDirName;
}


// Получение полей, заполненных в окне добавления записи
QString AddNewRecord::getField(QString name)
{
  if(name=="name" ||
     name=="author" ||
     name=="url" ||
     name=="tags")
    return infoField.getField(name);

  if(name=="text")
    return recordTextEditor.getTextarea();

  // Если запрашиваемого поля нет, возвращается пустая строка
  return QString();
}
