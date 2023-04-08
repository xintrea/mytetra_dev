#include "EditorMathExpressionDialog.h"
#include <QFile>
#include <QScrollBar>

#include "EditorConfig.h"
#include "main.h"
#include "views/mainWindow/MainWindow.h"
#include "libraries/helpers/ObjectHelper.h"
#include "libraries/helpers/UniqueIdHelper.h"


EditorMathExpressionDialog::EditorMathExpressionDialog(MathExpressionFormatter *mathExpressionFormatter, QWidget *parent) : QDialog(parent)
{
    // Период обновления картинки по таймеру (сек.)
    EditorConfig *conf = find_object<EditorConfig>("editorconfig");
    mathExpressionUpdateTime = conf->getMathExpressionUpdateTime();

    // Инициализация диалога
    setupUi();
    setupSignals();
    assembly();

    // Задание размеров диалога
    setMinimumHeight(
        int( 0.25 * static_cast<double>(find_object<MainWindow>("mainwindow")->height()) )
    );
    setMinimumWidth(
        int( 0.25 * static_cast<double>(find_object<MainWindow>("mainwindow")->width()) )
    );

    // todo: переделать на восстановление запомненных размеров и положения диалога
    resize(QGuiApplication::primaryScreen()->availableSize() / 2);

    // Класс для работы с математическими выражениями
    this->mathExpressionFormatter = mathExpressionFormatter;

    // Признак, было ли изменение формулы во время простоя таймера
    formulaModifiedTimePeriod = false;

    // Фокус - на контроле ввода текста формулы
    textArea->setFocus();

    // Запуск таймера с периодичностью, взятой из файла конфигурации
    onStartTimer();
}

EditorMathExpressionDialog::~EditorMathExpressionDialog()
{
    if (textArea!=nullptr)
        delete textArea;
}


void EditorMathExpressionDialog::setMathExpressionText(QString text)
{
    textArea->setPlainText(text);
}

QString EditorMathExpressionDialog::getMathExpressionText()
{
    return textArea->toPlainText();
}

// Был ли изменен текст, показанный в диалоге
bool EditorMathExpressionDialog::isModified()
{
    return textArea->document()->isModified();
}

void EditorMathExpressionDialog::setWordWrapMode(QTextOption::WrapMode mode)
{
    textArea->setWordWrapMode(mode);
}

void EditorMathExpressionDialog::showEvent(QShowEvent *event)
{
    // Первая отрисовка картинки формулы
    updateFormulaPicture();
    QDialog::showEvent(event);
}

// Масштабирование картинки формулы в зависимости от изменения размеров диалога
void EditorMathExpressionDialog::resizeEvent(QResizeEvent *event)
{
    if (imageLabel && !imageLabel->pixmap().isNull()) {
        if (fitToScrollAreaCheckBox->isChecked()) {
            // Масштабирование картинки формулы
            pictureZoom();
        } else {
            // Автопрокрутка до правого края картинки (последний введенный символ в тексте формулы)
            imageScrollArea->horizontalScrollBar()->setValue(imageScrollArea->horizontalScrollBar()->maximum());
        }
    }
    QDialog::resizeEvent(event);
}


void EditorMathExpressionDialog::setupUi()
{
    // Таймер для отрисовки картинки формулы
    timer = new QTimer(this);

    /* Зона всех контролов */
    // Виджет для сборки всех контролов по работе с картинкой формулы
    topWidget = new QWidget(this);
    // Виджет для сборки всех контролов по работе с текстом формулы
    bottomWidget = new QWidget(this);


    // Разделитель области написания формулы от области отображения формулы в графическом виде
    mathSplitter = new QSplitter(Qt::Orientation::Vertical, this);
    mathSplitter->setBackgroundRole(QPalette::Mid);
    //mathSplitter->setHandleWidth(5);


    /* Область работы с картинкой формулы */
    // Метка, обозначающая зону отображения картинки формулы
    pictureFormalaLabel = new QLabel(tr("Picture formula"), this);

    // Масштабирование картинки до размеров ScrollArea
    fitToScrollAreaCheckBox = new QCheckBox(tr("Fit to scroll area for big formula"), this);
    fitToScrollAreaCheckBox->setChecked(true);

    // Переключатели обновления картинки: по таймеру или в реальном времени
    timerRadioButton = new QRadioButton(tr("Timer update (%1) sec.").arg(mathExpressionUpdateTime), this);
    timerRadioButton->setChecked(true);
    realTimeRadioButton = new QRadioButton(tr("Real time update"), this);

    // Метка для отображения картинки формулы
    imageLabel = new QLabel(this);
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    // Прокрутка для картинки формулы
    imageScrollArea = new QScrollArea(this);
    imageScrollArea->setBackgroundRole(QPalette::Light);
    imageScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    imageScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    imageScrollArea->setWidget(imageLabel);


    /* Область работы с текстом формулы */
    // Текстовая область для написания формулы
    textArea = new TexTextEdit();
    textArea->setAcceptRichText(false);
    textArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Формирование зоны контролов масштабирования области написания текста формулы
    textFormalaLabel = new QLabel(tr("Formula text"), this);

    textFormulaZoomInPushButton = new QToolButton(this);
    textFormulaZoomInPushButton->setIcon(QIcon(":/resource/pic/edit_zoom-in.svg"));
    textFormulaZoomInPushButton->setToolTip(tr("Zoom in"));

    textFormulaZoomOutPushButton = new QToolButton(this);
    textFormulaZoomOutPushButton->setIcon(QIcon(":/resource/pic/edit_zoom-out.svg"));
    textFormulaZoomOutPushButton->setToolTip(tr("Zoom out"));


    // Создание набора диалоговых кнопок
    dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
}

void EditorMathExpressionDialog::setupSignals()
{
    // Обработка переключения режима масштабирования картинки формулы
    connect(fitToScrollAreaCheckBox, &QCheckBox::clicked, this, &EditorMathExpressionDialog::onSwitchFitToScrollArea);

    // Увеличение масштаба области текста формулы
    connect(textFormulaZoomInPushButton, &QPushButton::clicked, this, &EditorMathExpressionDialog::onTextZoomIn);

    // Уменьшение масштаба области текста формулы
    connect(textFormulaZoomOutPushButton, &QPushButton::clicked, this, &EditorMathExpressionDialog::onTextZoomOut);

    // Установка флага изменения формулы при наборе символов
    connect(textArea, &QTextEdit::textChanged, this, &EditorMathExpressionDialog::onTextChanged);

    // Обработка undo и redo в редакторе формулы - перерисовка картинки
    connect(textArea, &TexTextEdit::isUndo, this, &EditorMathExpressionDialog::onUndo);
    connect(textArea, &TexTextEdit::isRedo, this, &EditorMathExpressionDialog::onRedo);

    // Переключатели обновления картинки: по таймеру или в реальном времени (включение или отключение таймера)
    connect(timerRadioButton, &QRadioButton::clicked, this, &EditorMathExpressionDialog::onStartTimer);
    connect(realTimeRadioButton, &QRadioButton::clicked, this, &EditorMathExpressionDialog::onStopTimer);

    // Обработка кнопки OK
    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &EditorMathExpressionDialog::accept);

    // Обработка кнопки Cancel
    connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &EditorMathExpressionDialog::reject);

    // Подключение таймера для обновления картинки формулы
    connect(timer, &QTimer::timeout, this, &EditorMathExpressionDialog::onTimerAlarm);
}

void EditorMathExpressionDialog::assembly()
{
    /* Формирование области контролов масштабирования работы с картинкой формулы */
    // Область контролов по работе с картинкой
    pictureFormulaControlLayout = new QHBoxLayout();
    pictureFormulaControlLayout->addWidget(pictureFormalaLabel);
    pictureFormulaControlLayout->addStretch();
    pictureFormulaControlLayout->addWidget(timerRadioButton);
    pictureFormulaControlLayout->addWidget(realTimeRadioButton);
    pictureFormulaControlLayout->addStretch();
    pictureFormulaControlLayout->addWidget(fitToScrollAreaCheckBox);

    // Область работы с картинкой формулы
    pictureFormulaLayout = new QVBoxLayout();
    pictureFormulaLayout->setContentsMargins(5, 5, 5, 5);
    pictureFormulaLayout->addLayout(pictureFormulaControlLayout);
    pictureFormulaLayout->addWidget(imageScrollArea);

    // Сборка на виджет всех контролом по работе с текстом формулы
    topWidget->setLayout(pictureFormulaLayout);


    /* Формирование области контролов масштабирования работы с текстом формулы */
    // Компоновка контролов масштабирования текста формулы
    textFormulaZoomLayout = new QHBoxLayout();
    textFormulaZoomLayout->addWidget(textFormalaLabel);
    textFormulaZoomLayout->addStretch(1);
    textFormulaZoomLayout->addWidget(textFormulaZoomInPushButton);
    textFormulaZoomLayout->addWidget(textFormulaZoomOutPushButton);

    // Область работы с текстом формулы
    textFormulaLayout = new QVBoxLayout();
    textFormulaLayout->setContentsMargins(5, 5, 5, 5);
    textFormulaLayout->addLayout(textFormulaZoomLayout);
    textFormulaLayout->addWidget(textArea);

    // Сборка на виджет всех контролов по работе с текстом формулы
    bottomWidget->setLayout(textFormulaLayout);

    // Сборка всех контролов на разделитель
    mathSplitter->addWidget(topWidget);
    mathSplitter->addWidget(bottomWidget);
    // Блокируется скрытие виджетов при подходе разделителя к краям диалога
    mathSplitter->setCollapsible(0, false);
    mathSplitter->setCollapsible(1, false);

    /* Сборка всех контролов */
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    // Добавление разделителя основных виджетов
    mainLayout->addWidget(mathSplitter);

    // Добавление box кнопок OK и Cancel
    mainLayout->addWidget(dialogButtonBox);

    setLayout(mainLayout);
}

// Обработка переключения режима масштабирования картинки формулы
void EditorMathExpressionDialog::onSwitchFitToScrollArea()
{
    if (!fitToScrollAreaCheckBox->isChecked()) {
        // Обновление картинки формулы
        updateFormulaPicture();
        // Автопрокрутка до правого края картинки (последний введенный символ в тексте формулы)
        imageScrollArea->horizontalScrollBar()->setValue(imageScrollArea->horizontalScrollBar()->maximum());
    } else {
        // Масштабирование картинки формулы
        pictureZoom();
    }

    // Фокус переносится на контрол написания текста формулы
    textArea->setFocus();
}

// Увеличение масштаба области текста формулы
void EditorMathExpressionDialog::onTextZoomIn()
{
     textArea->zoomIn(2);
}

// Уменьшение масштаба области текста формулы
void EditorMathExpressionDialog::onTextZoomOut()
{
    textArea->zoomIn(-2);
}

// Включение таймера обновления картинки формулы
void EditorMathExpressionDialog::onStartTimer()
{
    timer->start(mathExpressionUpdateTime*1000); // отработка таймера в секундах
}

// Отключение таймера обновления картинки формулы
void EditorMathExpressionDialog::onStopTimer()
{
    timer->stop();
}

// Обновление картинки формулы по таймеру
void EditorMathExpressionDialog::onTimerAlarm()
{
    if (textArea->document()->isModified() && formulaModifiedTimePeriod) {
        // Обновление картинки формулы
        updateFormulaPicture();

        // Сброс флага изменения формулы при наборе символов
        formulaModifiedTimePeriod = false;
    }
}

// Изменение картинки при изменении текста (для действий undo/redo редатора формулы)
void EditorMathExpressionDialog::onTextChanged()
{
    if (textArea->document()->isModified()) {
        // Установка флага изменения формулы при наборе символов
        formulaModifiedTimePeriod = true;
    }

    if (realTimeRadioButton->isChecked()) {
        // Обновление картинки формулы в реальном времени
        updateFormulaPicture();
    }
}

void EditorMathExpressionDialog::onUndo()
{
    textArea->undo();
    // Установка флага изменения формулы при наборе символов
    formulaModifiedTimePeriod = true;
    // Обновление картинки формулы
    updateFormulaPicture();
}

void EditorMathExpressionDialog::onRedo()
{
    textArea->redo();
    // Установка флага изменения формулы при наборе символов
    formulaModifiedTimePeriod = true;
    // Обновление картинки формулы
    updateFormulaPicture();
}

// Обновление картинки формулы
void EditorMathExpressionDialog::updateFormulaPicture()
{
    QString tempGifFileName = QDir::tempPath()+"/"+getUniqueId()+".gif";
    mathExpressionFormatter->createGifFromMathExpression( textArea->toPlainText(), tempGifFileName );

    QPixmap currentPixmap(tempGifFileName);

    if (!currentPixmap.isNull()) {
        // Удаление временного Gif файла картинки
        if (QFile::exists(tempGifFileName)) {
            QFile::remove(tempGifFileName);
        }

        // Замена картинки на новую
        if (imageLabel!=nullptr) {
            delete imageLabel;
        }

        imageLabel = new QLabel(this);
        imageLabel->setBackgroundRole(QPalette::Base);
        //imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        imageLabel->setContentsMargins(10, 10, 10, 10);
        imageLabel->setScaledContents(true);
        imageLabel->setPixmap(currentPixmap);

        // Добавление метки-картинки в область прокрутки
        imageScrollArea->setWidget(imageLabel);

        // Масштабирование картинки формулы
        if (fitToScrollAreaCheckBox->isChecked()) {
           pictureZoom();
        }

        // Автопрокрутка до правого края картинки (последний введенный символ в тексте формулы)
        imageScrollArea->horizontalScrollBar()->setValue(imageScrollArea->horizontalScrollBar()->maximum());
    }
}

// Масштабирование картинки формулы
void EditorMathExpressionDialog::pictureZoom()
{
    double pixHeight = imageLabel->pixmap().height();
    double pixWidth = imageLabel->pixmap().width();
    double imageScrollAreaHeight = imageScrollArea->geometry().height()-10;
    double imageScrollAreaWidth = imageScrollArea->geometry().width()-10;
    if (pixHeight > imageScrollAreaHeight || pixWidth > imageScrollAreaWidth) {
        double factor = imageScrollAreaWidth / pixWidth;
        imageLabel->setFixedWidth(int(factor * pixWidth));
        imageLabel->setFixedHeight(int(factor * pixHeight));
    }
}
