#ifndef EDITORSHOWTEXT_H
#define EDITORSHOWTEXT_H

#include <QDialog>
#include <QTextEdit>
#include <QEvent>
#include <QCloseEvent>


// Класс отдельного (открепляемого) окна для просмотра текста записи без возможности редактирования
class EditorShowText : public QDialog
{
    Q_OBJECT
public:
    explicit EditorShowText(QWidget *parent = nullptr);
    virtual ~EditorShowText();

    void setNoteId(const QString &noteId);
    void setHtml(QString text);
    void setDocument(QSharedPointer<QTextDocument> pDocument);

    // Действия при закрытии диалога
    void closeEvent(QCloseEvent *event);

signals:

    void editorShowTextClose(const QString &noteId);

public slots:

private:

    QString mNoteId;
    QSharedPointer<QTextEdit> mTextArea;
    QSharedPointer<QTextDocument> mTextDocument;

    int mGeomX;
    int mGeomY;
    int mGeomW;
    int mGeomH;

    void setupUi(void);
    void setupSignals(void);
    void assembly(void);

    void hideEvent(QHideEvent *event);
    void showEvent(QShowEvent *event);
};

#endif // EDITORSHOWTEXT_H
