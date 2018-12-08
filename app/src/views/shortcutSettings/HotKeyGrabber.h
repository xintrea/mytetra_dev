#ifndef HOTKEYGRABBER_H
#define HOTKEYGRABBER_H

#include <QKeySequenceEdit>

// Модификация QKeySequenceEdit чтобы он захватывал одну комбинацию
// Не используется, так как у него не срабатывает сигнал завершения ввода через 1 сек

class HotKeyGrabber: public QKeySequenceEdit
{
    Q_OBJECT

public:
    explicit HotKeyGrabber( QWidget* parent = nullptr);
    ~HotKeyGrabber();

protected:
    void keyPressEvent ( QKeyEvent * event );
};


#endif // HOTKEYGRABBER_H
