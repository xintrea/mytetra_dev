#ifndef _METAEDITOR_H_
#define _METAEDITOR_H_

#include <QWidget>
#include <QLabel>
#include <QTextCodec>

#include "libraries/wyedit/Editor.h"

class QHBoxLayout;
class QGridLayout;
class QScrollArea;
class QSplitter;
class AttachTableScreen;

class MetaEditor: public Editor
{
 Q_OBJECT

public slots:

 void setField(QString n, QString v);
 void clearAll(void);
 void onClickToTag(const QString &text);


signals:
 // Сигнал вырабатывается при клике на текстовой метке
 void setFindTextSignal(QString text);

public:
 MetaEditor(void);
 ~MetaEditor(void);

 void setTreePath(QString path);
 void setName    (QString name);
 void setAuthor  (QString author);
 void setUrl     (QString url);
 void setTags    (QString tags);

 static void toAttachCallback(void);

 void switchToEditorLayout(void);
 void switchToAttachLayout(void);

 void setReadOnly(bool state);

private:
 void setupLabels(void);
 void setupUI(void);
 void metaAssembly(void);
 void setupSignals(void);

 QLabel *treePath; // Надпись Path (только для мобильного интерфейса)

 QLabel *recordName; // Надпись Title
 QLabel *recordAuthor; // Надпись Author(s)

 QLabel *labelUrl; // Надпись "Url"
 QLabel *recordUrl;

 QLabel *labelTags; // Надпись "Tags"
 QWidget *recordTagsContainer;
 QHBoxLayout *recordTagsLayout;
 QScrollArea *recordTagsScrollArea;
 QString recordTagsText;
 QStringList recordTagsTextList;
 QList<QLabel*> recordTagsLabels;

 QSplitter *editorAndFileTableSplitter;

 // Виджет слоя прикрепляемых файлов
 AttachTableScreen *attachTableScreen;

 // Виджет слоя редактирования текста
 QGridLayout *editorMainLayer;
 QWidget *editorMainScreen;

 // Группировалка виджетов всех слоев (слоя редактирования и слоя прикрепляемых файлов)
 QVBoxLayout *metaEditorJoinLayer;

};

#endif /* _METAEDITOR_H_ */
