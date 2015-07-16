#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QDebug>

#include "views/printPreview/PrintPreview.h"
#include "models/recordTable/RecordTableProxyModel.h"

#include "RecordTablePrint.h"


RecordTablePrint::RecordTablePrint(QWidget *parent) : QDialog(parent)
{
  setup_ui();
  setup_signals();
  assembly();
}


RecordTablePrint::~RecordTablePrint()
{
  delete textArea;
  delete buttonBox;
}


void RecordTablePrint::setup_ui()
{
 setModal(true);

 QSizePolicy sizePolicy;
 sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
 sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);

 // Область отображения распечатываемой таблицы
 textArea=new QTextEdit(this);
 textArea->setAcceptRichText(true);
 textArea->setSizePolicy(sizePolicy);

 // Линейка с кнопками управления
 buttonBox=new QDialogButtonBox(Qt::Horizontal);
 buttonBox->addButton(tr("Print"),QDialogButtonBox::AcceptRole);
 buttonBox->addButton(tr("Cancel"),QDialogButtonBox::RejectRole);

 // Устанавливается размер окна, равный виджету, из которого
 // этот виджет был вызван
 if(this->parent()->isWidgetType())
  {
   QWidget *parentWidget=qobject_cast<QWidget *>(this->parent());
   QRect geom(parentWidget->pos(), parentWidget->size());

   qDebug() << "Parent window geometry " << geom.x() << geom.y() << geom.width() << geom.height();

   setMinimumSize(parentWidget->size());
  }

 // Устанавливается заголовок окна
 this->setWindowTitle(tr("Print notes table"));
}


void RecordTablePrint::setup_signals()
{
 connect(buttonBox, SIGNAL(accepted()), this, SLOT(print()));
 connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}


void RecordTablePrint::assembly()
{
 QVBoxLayout *mainLayout=new QVBoxLayout(this);

 // Добавляется область текста
 mainLayout->addWidget(textArea);

 // Добавляется линейка кнопок Print и Cancel
 mainLayout->addWidget(buttonBox);
}


// Метод установки модели данных, из которой будет формироваться таблица
void RecordTablePrint::setModel(RecordTableProxyModel *model)
{
  QString html="<table border='1' style='border-style:solid; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;' cellspacing='0'' cellpadding='10'>";

  // Заголовок таблицы
  html+="<tr>";
  for(int nColumn=0; nColumn < model->columnCount(); nColumn++)
  {
    QVariant cellTextVariant=model->headerData(nColumn, Qt::Horizontal, Qt::DisplayRole);
    QString cellText=cellTextVariant.toString();

    html+="<td><b>"+cellText+"</b></td>";
  }
  html+="</tr>";


  // Данные из модели сразу преобразуются в HTML
  for(int nRow=0; nRow < model->rowCount(); nRow++)
  {
    html+="<tr>";

    for(int nColumn=0; nColumn < model->columnCount(); nColumn++)
      if (model->index(nRow, nColumn).isValid())
      {
        QVariant cellTextVariant=model->index(nRow, nColumn).data(Qt::DisplayRole);
        QString cellText=cellTextVariant.toString();

        html+="<td>"+cellText+"</td>";
      }

    html+="</tr>";
  }

  html+="</table>";

  qDebug() << html;

  textArea->setHtml(html);
}


// Слот в котором происходит вызов служебного окна предпросмотра тачатаемой таблицы
void RecordTablePrint::print(void)
{
  QTextDocument doc; // Документ, который будет отправлен на печать

  doc.setHtml( textArea->toHtml() );

  PrintPreview *preview = new PrintPreview(&doc, this);

  preview->setModal(true);
  preview->setAttribute(Qt::WA_DeleteOnClose);
  preview->show();
}
