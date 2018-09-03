#ifndef PREVIEWFILEDIALOG
#define PREVIEWFILEDIALOG

#include <QFileDialog>

class QLabel;

// Класс, модифицирующий стандартный QFileDialog так, что в правой части появляется
// изображение иконки, размещенной внутри выбранного файла
// Временно не используется

class PreviewFileDialog : public QFileDialog
{
  Q_OBJECT

public:

  explicit PreviewFileDialog(
    QWidget *parent = 0,
    const QString &caption = QString(),
    const QString &directory = QString(),
    const QString &filter = QString()
  );

protected slots:
  void OnCurrentChanged(const QString &path);

protected:

  QLabel* previewLabel;

};

#endif // PREVIEWFILEDIALOG
