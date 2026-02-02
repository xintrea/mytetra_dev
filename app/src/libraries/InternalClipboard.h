#ifndef INTERNALCLIPBOARD_H
#define INTERNALCLIPBOARD_H

#include <QObject>
#include <QSharedPointer>
#include <QPixmap>
#include <QImage>
#include <QMimeData>
#include <QClipboard>

// Частичный аналог класса QClipboard для реализации внутреннего буфера обмена

class InternalClipboard : public QObject
{
    Q_OBJECT
public:

    explicit InternalClipboard(QObject *parent = nullptr);
    ~InternalClipboard() override;


    // Очистка содержимого
    void clear(QClipboard::Mode mode = QClipboard::Clipboard);


    // Работа с текстом
    void setText(const QString &text, QClipboard::Mode mode = QClipboard::Clipboard);
    QString text(QClipboard::Mode mode = QClipboard::Clipboard) const;

    // Работа с изображениями
    void setImage(const QImage &image, QClipboard::Mode mode = QClipboard::Clipboard);
    QImage image(QClipboard::Mode mode = QClipboard::Clipboard) const;

    // Работа с готовым растровыми изображениями
    QPixmap pixmap(QClipboard::Mode mode = QClipboard::Clipboard) const;
    void setPixmap(const QPixmap &pixmap, QClipboard::Mode mode = QClipboard::Clipboard);

    // Методы для работы с MIME данными
    void setMimeData(QMimeData *data, QClipboard::Mode mode = QClipboard::Clipboard);
    QMimeData *mimeData(QClipboard::Mode mode = QClipboard::Clipboard) const;


signals:

    // Сигнал когда меняется тип режима источника данных
    void changed(QClipboard::Mode mode);

    // Сигнал об изменении данных в буфере
    void dataChanged();

private:

    // Внутреннее хранилище для каждого типа источника данных
    struct Storage
    {
        QSharedPointer<QMimeData> mime;
    };

    Storage &storageForMode(QClipboard::Mode mode);
    const Storage &storageForModeConst(QClipboard::Mode mode) const;

    // Три независимых хранилища (согласно QClipboard modes)
    Storage m_clipboardStorage;
    Storage m_selectionStorage;
    Storage m_findBufferStorage;
};

#endif // INTERNALCLIPBOARD_H
