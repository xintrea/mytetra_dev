#include <QWidget>
#include <QTextEdit>
#include <QtDebug>
#include <QSizePolicy>
#include <QIcon>
#include <QMessageBox>
#include <QTextDocumentFragment>
#include <QDir>

#include "AddNewRecord.h"
#include "main.h"
#include "InfoFieldEnter.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/GlobalParameters.h"
#include "libraries/wyedit/Editor.h"

extern GlobalParameters globalParameters;
extern AppConfig mytetraConfig;

// Окно добавления новой записи

#if QT_VERSION < 0x050000
AddNewRecord::AddNewRecord( QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
#else
AddNewRecord::AddNewRecord( QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f)
#endif
{
 setupUI();
 setupSignals();
 assembly();
}

AddNewRecord::~AddNewRecord()
{

}


void AddNewRecord::setupUI(void)
{
 this->setWindowTitle(tr("Enter a new note"));

 // Ввод инфополей записи
 infoField=new InfoFieldEnter();

 // Редактор текста записи
 recordTextEditor=new Editor();
 recordTextEditor->setDisableToolList( mytetraConfig.getHideEditorTools() + (QStringList() << "save" << "show_text" << "attach") );
 recordTextEditor->initEnableAssembly(true);
 recordTextEditor->initConfigFileName(globalParameters.getWorkDirectory()+"/editorconf.ini");
 recordTextEditor->initEnableRandomSeed(false);
 recordTextEditor->init(Editor::WYEDIT_DESKTOP_MODE); // Так как это окно, в мобильном режие его инициализировать ненужно, так как есть кнопка Отмена

 // Кнопки OK и Cancel
 buttonBox=new QDialogButtonBox();
 buttonBox->setOrientation(Qt::Horizontal);
 buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::NoButton|QDialogButtonBox::Cancel);
 
 // На кнопку OK назначается комбинация клавиш Ctrl+Enter
 QPushButton *OkButton=buttonBox->button(QDialogButtonBox::Ok); // Выясняется указатель на кнопку OK
 OkButton->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_Return) ); // Устанавливается шорткат
 OkButton->setToolTip(tr("Ctrl+Enter"));
}


void AddNewRecord::setupSignals(void)
{
 connect(buttonBox, SIGNAL(accepted()), this, SLOT(okClick(void)));
 connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}


void AddNewRecord::assembly(void)
{
 // Размещалка элементов
 QVBoxLayout *layout=new QVBoxLayout();
 layout->setMargin(8);
 layout->setSpacing(10);

 // Добавление элементов в размещалку
 layout->addWidget(infoField);
 layout->addWidget(recordTextEditor);
 layout->addWidget(buttonBox,0,Qt::AlignRight);

 setLayout(layout);

 // Фокус устанавливается на поле ввода названия записи
 infoField->setFocusToStart();

 // QWidget *wdgt=new QWidget;
 // wdgt->setLayout(vbl);
 // setCentralWidget(wdgt);
}


void AddNewRecord::okClick(void)
{
 QString message="";

 // Проверка наличия названия записи
 if(infoField->getField("name").length()==0)
  message=message+tr("Please enter the note's <b>title</b>. ");

 // Проверка наличия текста записи
 QTextDocumentFragment i;
 QString j;
 i=QTextDocumentFragment::fromHtml(getField("text"));
 j=i.toPlainText();
 qDebug() << "AddNewRecord::okClick() : recordtext " << j;
 if(j.length()==0)
  message=message+tr("Please enter the note's <b>text</b>. ");

 // Если что-то не заполнено, выдается предупреждение
 if(message.length()>0)
  {
   QMessageBox::warning(this,tr("A new note cannot be added"),message,
                        QMessageBox::Close);
   return;
  }

 // Картинки сохраняются
 imagesDirName=create_temp_directory();
 recordTextEditor->set_work_directory(imagesDirName);
 recordTextEditor->save_textarea_images(Editor::SAVE_IMAGES_SIMPLE);

 emit(accept());
}


QString AddNewRecord::getImagesDirectory(void)
{
 if(imagesDirName.length()==0)
  {
   critical_error("In add new record function can not generate temp directory with saved images.");
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
  return infoField->getField(name);

 if(name=="text")
  return recordTextEditor->get_textarea();

 // Если запрашиваемого поля нет, возвращается пустая строка
 return QString();
}
