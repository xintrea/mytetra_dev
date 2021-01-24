#ifndef EDITORINDENTSLIDERASSISTANT_H
#define EDITORINDENTSLIDERASSISTANT_H

#include <QObject>


class IndentSlider;
class EditorTextArea;

class EditorIndentSliderAssistant : public QObject
{
  Q_OBJECT
public:
  explicit EditorIndentSliderAssistant(QObject *parent, EditorTextArea *iTextArea);
  ~EditorIndentSliderAssistant();

  IndentSlider *getIndentSlider();
  void clear();
  void setVisible(bool flag);

signals:

  // Сигналы установки отступов на линейке с движками согласно текущему форматированию
  void send_set_textindent_pos(int i);
  void send_set_leftindent_pos(int i);
  void send_set_rightindent_pos(int i);

public slots:
  void onUpdateGeometry();
  void updateToActualFormat(void);

  // Слоты обработки перемещения движков настройки отступов
  void onChangeTextindentPos(int i);
  void onChangeLeftindentPos(int i);
  void onChangeRightindentPos(int i);
  void onMouseRelease(void);


protected:

  IndentSlider *indentSlider;
  EditorTextArea *textArea;

  int currentTextIndent;
  int currentLeftIndent;
  int currentRightIndent;

  void setupSignals();
};

#endif // EDITORINDENTSLIDERASSISTANT_H
