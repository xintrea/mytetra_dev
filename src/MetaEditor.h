#ifndef _METAEDITOR_H_
#define _METAEDITOR_H_

#include <QBoxLayout>
#include <QGridLayout>
#include <QWidget>
#include <QLabel>
#include <QTextCodec>

#include "libraries/wyedit/Editor.h"


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

 void setName  (QString name);
 void setAuthor(QString author);
 void setUrl   (QString url);
 void setTags  (QString tags);

private:
 void setupLabels(void);
 void metaAssembly(void);
 void setupSignals(void);

 QLabel *recordName; // Надпись Title
 QLabel *recordAuthor; // Надпись Author(s)

 QLabel *labelUrl; // Надпись "Url"
 QLabel *recordUrl;

 QLabel *labelTags; // Надпись "Tags"
 QHBoxLayout *recordTagsLayout;
 QString recordTagsText;
 QStringList recordTagsTextList;
 QList<QLabel*> recordTagsLabels;

 // Группировалка всех инфополей и редактора
 QGridLayout *metaEditorAssemblyLayout;

};

#endif /* _METAEDITOR_H_ */
