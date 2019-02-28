#ifndef EDITORTOOLBARSETTINGSSCREEN_H
#define EDITORTOOLBARSETTINGSSCREEN_H

#include <QDialog>
#include <QWidget>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QTreeView>
#include <QStackedWidget>

/******************************************************************/
/* Диалог выбора команд для панелей инструментов редактора текста */
/******************************************************************/

class EditorToolbarAvailableCommandsController;
class EditorToolbarUsedCommandsController;
class EditorConfig;

class EditorToolbarSettingsScreen : public QDialog
{
    Q_OBJECT

public:
    EditorToolbarSettingsScreen(QWidget *parent = nullptr);
    ~EditorToolbarSettingsScreen();

    // Возвращает признак необходимости перезагрузки MyTetra, в зависимости от уровеня сложности вносимых изменений
    // false - изменения можно делать на лету, перезагрузка MyTetra не нужна
    // true - для принятия изменений нужна перезагрузка MyTetra
    bool isNeedRestart() const;

protected slots:

    // Переключение видимости виджетов со списком кнопок
    void onCheckViewToolbarWidget();

    // Перемещение выбранной команды из модели всех доступных команд в модель команд рабочей панели
    void onMoveAvailableCommandToUsedCommands();
    // Перемещение выбранной команды из модели команд рабочей панели в модель всех доступных команд
    void onMoveUsedCommandToAvailableCommands();
    // Перемещение выбранной команды вверх в моделе команд рабочей панели инструментов
    void onMoveSelectedCommandUp();
    // Перемещение выбранной команды вниз в моделе команд рабочей панели инструментов
    void onMoveSelectedCommandDown();

    // Отображение информации-подсказки
    void onInfoClick();

    // Сохранение результата распределения команд по панелям инструментов
    void applyChanges();

protected:

    // Доступные команды панелей инструментов
    QLabel *availableToolbarsCommandsLabel;
    EditorToolbarAvailableCommandsController *availableCommandsToolbarController;

    // Виджет выбора панели инструментов
    QListWidget *selectToolbarsListWidget;

    // Надпись над кнопками, перемещающие кнопки панелей (невидимая, нужна для вертикального выравнивания)
    QLabel *exchangeButtonsLabel;

    // Кнопки для перемещения выбранных кнопок панелей
    QPushButton *toUsedCommandsPushButton; // Кнопка для переноса команды в модель используемых команд панели инструментов
    QPushButton *toAvailableCommandsPushButton; // Кнопка для переноса команды в модель всех доступных команд
    QPushButton *usedCommandUpPushButton; // Кнопка для перемещения команды из модели используемых команд вверх в списке
    QPushButton *usedCommandDownPushButton; // Кнопка для перемещения команды из модели используемых команд вниз в списке
    QPushButton *infoPushButton; // Кнопка для отображения информации-подсказки

    // Слой, объединяющий команды для перемещения выбранных кнопок панелей
    QVBoxLayout *buttonsToMoveLayout;

    // Слой для столбца доступных кнопок (заголовок и список)
    QVBoxLayout *availableLayout;

    // Слой для столбца перемещающих кнопок (невидимый заголовок для вертикального выравнивания и список)
    QVBoxLayout *exchangeButtonsLayout;

    // Слой для столбца используемых кнопок (заголовок и список)
    QVBoxLayout *usedLayout;

    // Слой из трех столбцов: доступные кнопки, панель с кнопками перемещения, используемые кнопки
    QHBoxLayout *exchangeLayout;


    // Используемые комманды на панели инструментов
    QLabel *usedCommandsToolbarLabel;
    QStackedWidget *usedCommandsToolbarStackedWidget;
    EditorToolbarUsedCommandsController *usedCommandsToolbar1Controller;
    EditorToolbarUsedCommandsController *usedCommandsToolbar2Controller;

    // Слой, объединяющий все слои
    QVBoxLayout *screenLayout;

    // Кнопки Ok и Cancel
    QDialogButtonBox *dialogButtonBox;

    void setupUI(void);
    void setupSignals(void);
    void assembly(void);

private:

    // Контроллер для работы с моделью панели используемых команд, в зависимости от выбранной панели
    EditorToolbarUsedCommandsController *getCurrentEditorToolbarUsedCommandsController();

    // Признак необходимости перезагрузки MyTetra, в зависимости от уровеня сложности вносимых изменений
    bool needRestart;

    // Признак, были ли внесены изменения в настройки панелей инструментов
    bool changedCommands;

    // Изначальные, в момент заргузки диалога списки команд
    QString loadedAvailableCommandsList;
    QString loadedToolBar1CommandsList;
    QString loadedToolBar2CommandsList;

};

#endif // EDITORTOOLBARSETTINGSSCREEN_H
