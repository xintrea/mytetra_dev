#include "InternalClipboard.h"


InternalClipboard::InternalClipboard(QObject *parent)
    : QObject{parent}
{
    // Инициализация пустого QMimeData для каждого режима
    m_clipboardStorage.mime.reset(new QMimeData());
    m_selectionStorage.mime.reset(new QMimeData());
    m_findBufferStorage.mime.reset(new QMimeData());
}


InternalClipboard::~InternalClipboard() = default;


void InternalClipboard::clear(QClipboard::Mode mode)
{
    Storage &storage = storageForMode(mode);

    storage.mime.reset(new QMimeData());

    emit changed(mode);
    emit dataChanged();
}


void InternalClipboard::setText(const QString &text, QClipboard::Mode mode)
{
    Storage &storage = storageForMode(mode);

    // Всегда заменяется хранимый QMimeData новым объектом с текстом
    storage.mime.reset(new QMimeData());
    storage.mime->setText(text);

    emit changed(mode);
    emit dataChanged();
}


QString InternalClipboard::text(QClipboard::Mode mode) const
{
    const Storage &storage = storageForModeConst(mode);

    if (!storage.mime)
    {
        return QString();
    }

    return storage.mime->text();
}


void InternalClipboard::setImage(const QImage &image, QClipboard::Mode mode)
{
    Storage &storage = storageForMode(mode);

    storage.mime.reset(new QMimeData());

    // QMimeData не имеет метода setImageData(const QImage&) устанавливающего картинку напрямую
    storage.mime->setImageData(QVariant::fromValue(image));

    emit changed(mode);
    emit dataChanged();
}


QImage InternalClipboard::image(QClipboard::Mode mode) const
{
    const Storage &storages = storageForModeConst(mode);

    if (!storages.mime)
    {
        return QImage();
    }

    QVariant v = storages.mime->imageData();

    if (v.isNull())
    {
        return QImage();
    }

    // QVariant may contain QImage or QPixmap; try QImage first
    if (v.canConvert<QImage>())
    {
        return qvariant_cast<QImage>(v);
    }

    if (v.canConvert<QPixmap>())
    {
        QPixmap pm = qvariant_cast<QPixmap>(v);
        return pm.toImage();
    }

    return QImage();
}


QPixmap InternalClipboard::pixmap(QClipboard::Mode mode) const
{
    const Storage &storage = storageForModeConst(mode);

    if (!storage.mime)
    {
        return QPixmap();
    }

    QVariant v = storage.mime->imageData();
    if (v.isNull())
    {
        return QPixmap();
    }

    if (v.canConvert<QPixmap>())
    {
        return qvariant_cast<QPixmap>(v);
    }

    if (v.canConvert<QImage>())
    {
        QImage img = qvariant_cast<QImage>(v);
        return QPixmap::fromImage(img);
    }

    return QPixmap();
}


void InternalClipboard::setPixmap(const QPixmap &pixmap, QClipboard::Mode mode)
{
    Storage &storage = storageForMode(mode);

    storage.mime.reset(new QMimeData());
    storage.mime->setImageData(QVariant::fromValue(pixmap));

    emit changed(mode);
    emit dataChanged();
}


void InternalClipboard::setMimeData(QMimeData *data, QClipboard::Mode mode)
{
    Storage &storage = storageForMode(mode);

    if (data)
    {
        // Получение владения через QSharedPointer, но без удаления оригинала,
        // если вызывающий объект сохраняет указатель;
        // Примечание: чтобы избежать двойного удаления, надо создавать клон, если это возможно.
        // В QMimeData нет метода clone, но QMimeData неявно разделяется только если это пользовательский метод.
        // Более безопасный подход: обернуть указатель в QSharedPointer, получая право собственности.
        storage.mime.reset(data);
    }
    else
    {
        storage.mime.reset(new QMimeData());
    }

    emit changed(mode);
    emit dataChanged();
}


QMimeData *InternalClipboard::mimeData(QClipboard::Mode mode) const
{
    const Storage &storage = storageForModeConst(mode);

    if (!storage.mime)
    {
        // Выделяется память, если хранилище отсутствует
        const_cast<InternalClipboard*>(this)->storageForMode(mode).mime.reset(new QMimeData());

        // Возвращается неконстантный указатель на только что созданные пустые данные
        return const_cast<InternalClipboard*>(this)->storageForMode(mode).mime.data();
    }

    return storage.mime.data();
}


InternalClipboard::Storage &InternalClipboard::storageForMode(QClipboard::Mode mode)
{
    switch (mode)
    {
        case QClipboard::Clipboard:
            return m_clipboardStorage;
        case QClipboard::Selection:
            return m_selectionStorage;
        case QClipboard::FindBuffer:
            return m_findBufferStorage;
        default:
            return m_clipboardStorage;
    }
}


const InternalClipboard::Storage &InternalClipboard::storageForModeConst(QClipboard::Mode mode) const
{
    switch (mode)
    {
        case QClipboard::Clipboard:
            return m_clipboardStorage;
        case QClipboard::Selection:
            return m_selectionStorage;
        case QClipboard::FindBuffer:
            return m_findBufferStorage;
        default:
            return m_clipboardStorage;
    }
}
