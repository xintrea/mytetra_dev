#include <QtGlobal>
#include <QApplication>
#include <QScreen>
#include <QFile>
#include <QDebug>
#include <QStyle>
#include <QLabel>
#include <QTimer>

#include "CssHelper.h"

#include "libraries/GlobalParameters.h"
#include "libraries/FixedParameters.h"
#include "models/appConfig/AppConfig.h"
#include "models/appConfig/AppFiles.h"
#include "libraries/helpers/DiskHelper.h"
#include "libraries/helpers/PaletteHelper.h"

extern GlobalParameters globalParameters;
extern FixedParameters fixedParameters;
extern AppConfig mytetraConfig;
extern AppFiles mytetraFiles;


CssHelper::CssHelper()
{

}


qreal CssHelper::getCalculateIconSizePx(void)
{
#if QT_VERSION >= 0x040000 && QT_VERSION < 0x050000
    qreal dpiX=qApp->desktop()->physicalDpiX();
    qreal dpiY=qApp->desktop()->physicalDpiY();
    qreal dpi=(dpiX+dpiY)/2;
#endif

#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000
    qreal dpi=QApplication::screens().at(0)->physicalDotsPerInch();
#endif

    qreal iconSizeMm=6; // Размер иконки в миллиметрах (рекомендованный)
    qreal iconSizeInch=iconSizeMm/25.4; // Размер иконки в дюймах
    qreal iconSizePx=iconSizeInch*dpi;

    return iconSizePx;
}


// Замена в CSS-стиле все вхождения подстроки META_ICON_SIZE на вычисленный размер иконки в пикселях
QString CssHelper::replaceCssMetaIconSize(const QString &styleText)
{
    QMap<QString, qreal> table;
    table["META_ICON_FOUR_SIZE"]       = getCalculateIconSizePx() * 4.0;
    table["META_ICON_TRIPLE_SIZE"]     = getCalculateIconSizePx() * 3.0;
    table["META_ICON_DOUBLE_SIZE"]     = getCalculateIconSizePx() * 2.0;
    table["META_ICON_ONEANDHALF_SIZE"] = getCalculateIconSizePx() * 1.5;

    table["META_ICON_SIZE"] = getCalculateIconSizePx();

    table["META_ICON_SEVFIVEPERC_SIZE"]  = getCalculateIconSizePx() * 0.75;
    table["META_ICON_HALF_SIZE"]         = getCalculateIconSizePx() / 2.0;
    table["META_ICON_THIRD_PART_SIZE"]   = getCalculateIconSizePx() / 3.0;
    table["META_ICON_QUARTER_PART_SIZE"] = getCalculateIconSizePx() / 4.0;

    QString resultText = styleText;

    for (auto name : table.keys())
    {
      qreal value=table.value(name);
      resultText.replace( name, QString::number( (int) value )+"px" );
    }

    return resultText;
}


QString CssHelper::removeCssComments(const QString& input)
{
    QString result;
    result.reserve(input.length());

    int pos = 0;
    int length = input.length();
    bool inComment = false;

    while (pos < length) {
        if (!inComment) {
            // Начало комментария
            int commentStart = input.indexOf("/*", pos);

            if (commentStart == -1) {
                // Комментариев больше нет - в результат добавляется оставшийся текст
                result.append(input.mid(pos));
                break;
            } else {
                // Добавляется текст до начала комментария
                result.append(input.mid(pos, commentStart - pos));
                pos = commentStart + 2; // Пропускаем "/*"
                inComment = true;
            }
        } else {
            // Поиск конца комментария
            int commentEnd = input.indexOf("*/", pos);

            if (commentEnd == -1) {
                // Комментарий не закрыт - просто выход
                break;
            } else {
                pos = commentEnd + 2; // Пропускается "*/"
                inComment = false;
            }
        }
    }

    return result;
}


void CssHelper::extractCssStyles()
{
    // Удаление устаревших стилей, которые размещались по устаревшим соглашениям
    removeVeryOldCssStyles();


    QString dirName=globalParameters.getWorkDirectory();

    // Если в рабочей директории нет каталога с темами
    QDir themesDir(dirName+"/themes");
    if ( !themesDir.exists() )
    {
        // Каталог с темами распаковывается из ресурсов
        mytetraFiles.createThemesFiles( dirName+"/themes" );
    }
}


// Удаление устаревших стилей, размещаемых по устаревшим соглашениям
void CssHelper::removeVeryOldCssStyles()
{
    QString dirName=globalParameters.getWorkDirectory();

    // Устаревший файл стиля, который ранее находился рядом с конфиг-файлами
    // а не в отдельной директории, удаляется
    QFile fromFile(dirName+"/stylesheet.css");
    if ( fromFile.exists() )
    {
        fromFile.remove();
    }

    // Устаревший каталог стиля, который делал пользователь gee12, удаляется
    QDir veryOldstyleDir(dirName+"/style");
    if ( veryOldstyleDir.exists() )
    {
        DiskHelper::removeDirectory( veryOldstyleDir.absolutePath() );
    }
}


void CssHelper::loadCurrentTheme()
{
    applyTheme( mytetraConfig.getInterfaceTheme() );
}


bool CssHelper::applyTheme(const QString &themeName)
{
    if ( !fixedParameters.themesAvailableList.contains(themeName) )
    {
        qWarning() << "Incorrect interface theme name: " << themeName;
        return false;
    }

    QString dirName = globalParameters.getWorkDirectory();
    QString fileName = dirName+"/themes/"+themeName+"/stylesheet.css";

    QFile cssFile( fileName );

    bool openResult=cssFile.open(QIODevice::ReadOnly | QIODevice::Text);

    // Если файла не существует
    if(!openResult)
    {
        qWarning() << "Can't find CSS theme file: " << fileName;
        return false;
    }

    // Берется содержимое CSS-файла
    QString styleText = QTextStream(&cssFile).readAll();

    // Удаляются CSS комментарии
    styleText=CssHelper::removeCssComments(styleText);

    // Преобразовывается размер иконок из подстановочных названий
    // в настоящие пиксели
    styleText=CssHelper::replaceCssMetaIconSize(styleText);

    // Загружается и применяется палитра, хранящаяся в CSS файле в правиле CustomPalette
    CssHelper::loadPalette(styleText);

    // Загруженный стиль оформления применяется
    qApp->setStyleSheet(styleText);

    CssHelper::updateInterface();

    return true;
}


void CssHelper::loadPalette(const QString &cssText)
{
    // Применяются цвета из CSS-правила CustomPalette
    PaletteHelper::applyCustomPalette( PaletteHelper::parseCustomPalette(cssText) );
}


void CssHelper::updateInterface()
{
    // Принудительное обновление всех виджетов чтобы применились новые цвета
    foreach (QWidget *widget, QApplication::allWidgets()) {

        // Для QLabel со ссылками - особый подход
        if (QLabel *label = qobject_cast<QLabel*>(widget)) {
            if (label->text().contains("<a ")) {
                // Принудительное обновление через очистку/установку текста
                QString text = label->text();
                label->setText("");
                // QApplication::processEvents();
                label->setText(text);
            }
        }

        QEvent updateEvent(QEvent::UpdateRequest);
        QApplication::sendEvent(widget, &updateEvent);

        widget->style()->unpolish(widget);
        widget->style()->polish(widget);

        widget->update();
        widget->updateGeometry();

        widget->repaint();

        // qDebug() << "Сброс виджета: " << widget->objectName();
    }

    QApplication::processEvents();


    // Уровень 2: Отложенная перерисовка через 10мс
    QTimer::singleShot(10, []() {
        foreach (QWidget *widget, QApplication::allWidgets()) {
            widget->repaint();
        }
        QApplication::processEvents();
    });


    // Уровень 3: Финальное обновление через 50мс
    QTimer::singleShot(50, []() {
        foreach (QWidget *widget, QApplication::allWidgets()) {
            widget->update();
        }
        QApplication::sendPostedEvents(nullptr, QEvent::UpdateRequest);
    });


    // После сброса интерфейса слетают курсоры на таблицах и в деревьях,
    // их надо восстановить. Однако установка CSS возможна до того,
    // как появится главное окно, поэтому восстановление можно запускать
    // только если есть главное осно, например при переключении тем
    // Однако курсоры даже после restoreAllWindowState() не восстанавливается
    // Надо разобраться почему даже этот вызов не восстанавливает курсоры
    /*
    MainWindow *mainWindow = find_object<MainWindow>("mainwindow");
    if (mainWindow)
    {
        find_object<MainWindow>("mainwindow")->restoreAllWindowState();
    }
    */
}

