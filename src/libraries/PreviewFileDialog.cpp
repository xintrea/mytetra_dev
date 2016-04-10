#include <QLabel>
#include <QGridLayout>

#include "PreviewFileDialog.h"


PreviewFileDialog::PreviewFileDialog(QWidget *parent,
                                     const QString &caption,
                                     const QString &directory,
                                     const QString &filter) : /* Список инициализации */
                   QFileDialog(parent, caption, directory, filter)
                   {
                     setObjectName("PreviewFileDialog");
                     QVBoxLayout *box=new QVBoxLayout(this);

                     previewLabel=new QLabel(tr("Preview"), this);
                     previewLabel->setAlignment(Qt::AlignCenter);
                     previewLabel->setObjectName("labelPreview");
		     
                     box->addWidget(previewLabel);
                     box->addStretch();

                     // Добавление на основной слой QFileDialog
                     {
                       QGridLayout *layout=(QGridLayout*)this->layout();
                       layout->addLayout(box, 1, 3, 3, 1);
                     }

                     connect(this, SIGNAL(currentChanged(const QString&)), this, SLOT(OnCurrentChanged(const QString&)));
                   }

                   
void PreviewFileDialog::OnCurrentChanged(const QString &path)
{
  QPixmap pixmap = QPixmap(path);
  
  if(pixmap.isNull()) 
  {
    previewLabel->setText("Not an image");
  } 
  else 
  {
    previewLabel->setPixmap(pixmap.scaled(previewLabel->width(), previewLabel->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
  }
}
