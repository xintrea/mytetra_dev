#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QtGlobal>
#include <QtDebug>
#include <QMessageBox>
#include <QString>
#include <QShowEvent>
#include <QString>
#include <QStyle>

#include "main.h"
#include "views/mainWindow/MainWindow.h"
#include "EnterPassword.h"
#include "views/findInBaseScreen/FindScreen.h"


EnterPassword::EnterPassword(int imode, QWidget *parent) : QDialog(parent)
{
    mode=imode;
    password="";
    previousPassword="";
    cancelDelay=0;
    isPasswordTyped=false;

    setupUI();
    setupSignals();
    assembly();
}


void EnterPassword::setupUI(void)
{
    this->setWindowIcon( find_object<MainWindow>("mainwindow")->windowIcon() );

    // Надписи
    label=new QLabel(this);
    label1=new QLabel(this);
    label2=new QLabel(this);
    previousLabel=new QLabel(this);
    if(mode==ENTER_PASSWORD_MODE_SINGLE)
    {
        label->setText(tr("Enter a your password for access to encrypted data"));
        label->setWordWrap(true);

        label1->setText(tr("Password:"));
    }
    if(mode==ENTER_PASSWORD_MODE_DOUBLE)
    {
        label->setText(tr("Enter your password. This password is used to encrypt your notes. Do not forget it, otherwise you can not access to your encrypted data."));
        label->setWordWrap(true);

        label1->setText(tr("Password:"));
        label2->setText(tr("Confirm password:"));
    }
    if(mode==ENTER_PASSWORD_MODE_WITH_PREVIOUS)
    {
        label->setText(tr("Change password.\n\nEnter your old password and new password.\n\n<b>Note!</b> After clicking the OK button starts reencryption of all encrypted data."));
        label->setWordWrap(true);

        previousLabel->setText(tr("Old password:"));
        label1->setText(tr("New password:"));
        label2->setText(tr("Confirm new password:"));
    }

    // Поля ввода пароля
    passwordEdit1=new QLineEdit();
    passwordEdit1->setEchoMode(QLineEdit::Password);

    passwordEdit2=new QLineEdit();
    passwordEdit2->setEchoMode(QLineEdit::Password);

    previousPasswordEdit=new QLineEdit();
    previousPasswordEdit->setEchoMode(QLineEdit::Password);

    // Кнопка OK
    okButton=new QPushButton(tr("OK"), this);
    okButton->setIcon(this->style()->standardIcon(QStyle::SP_DialogOkButton));

    // Кнопка Cancel
    cancelButton=new QPushButton(tr("Cancel"), this);
    cancelButton->setIcon(this->style()->standardIcon(QStyle::SP_DialogCancelButton));

    this->setWindowTitle(tr("MyTetra: Password"));

    this->setMaximumSize(350,450);
}


void EnterPassword::setupSignals(void)
{
    connect(okButton,     &QPushButton::clicked, this, &EnterPassword::okClick);
    connect(cancelButton, &QPushButton::clicked, this, &EnterPassword::reject);

    // Поддержка работы таймера
    connect(&timer, &QTimer::timeout, this, &EnterPassword::cancelCountUpdate);

    // Обработка набора пароля пользователем
    connect(passwordEdit1, &QLineEdit::textChanged,
            this,          &EnterPassword::passwordTyped);
}


void EnterPassword::assembly(void)
{
    // Размещалка элементов
    QVBoxLayout *layout=new QVBoxLayout();
    // layout->setMargin(8);
    // layout->setSpacing(10);

    layout->addWidget(label);

    // Ввод простого пароля
    if(mode==ENTER_PASSWORD_MODE_SINGLE)
    {
        layout->addWidget(label1);
        layout->addWidget(passwordEdit1);
    }

    // Ввод пароля с полем подтверждения пароля
    if(mode==ENTER_PASSWORD_MODE_DOUBLE)
    {
        layout->addWidget(label1);
        layout->addWidget(passwordEdit1);

        layout->addWidget(label2);
        layout->addWidget(passwordEdit2);
    }

    // Ввод пароля с полем подтверждения и с полем ввода старого пароля
    if(mode==ENTER_PASSWORD_MODE_WITH_PREVIOUS)
    {
        layout->addWidget(previousLabel);
        layout->addWidget(previousPasswordEdit);

        layout->addWidget(label1);
        layout->addWidget(passwordEdit1);

        layout->addWidget(label2);
        layout->addWidget(passwordEdit2);
    }

    QHBoxLayout *standartButtonLayout=new QHBoxLayout();
    standartButtonLayout->addStretch();
    standartButtonLayout->addWidget(okButton);
    standartButtonLayout->addWidget(cancelButton);

    layout->addLayout(standartButtonLayout);

    setLayout(layout);

    if(mode==ENTER_PASSWORD_MODE_SINGLE ||
            mode==ENTER_PASSWORD_MODE_DOUBLE)
        passwordEdit1->setFocus();

    if(mode==ENTER_PASSWORD_MODE_WITH_PREVIOUS)
        previousPasswordEdit->setFocus();
}


// Установка времени автозакрытия окна в секундах
void EnterPassword::setCancelDelay(int delay)
{
    cancelDelay=delay;
}


// Действия при открытии окна
void EnterPassword::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

    if(cancelDelay==0) return;

    cancelCount=cancelDelay+1;

    cancelCountUpdate();
}


void EnterPassword::cancelCountUpdate(void)
{
    timer.stop();

    // Если пользователь начал набирать пароль
    if(isPasswordTyped)
    {
        // Счетчик больше не будет обрабатываться
        cancelButton->setText(tr("Cancel"));
        return;
    }

    cancelCount--;

    // qDebug() << "EnterPassword::cancelCountUpdate() : cancelCount" << cancelCount;

    // Если счетчик достиг нуля
    if(cancelCount<=0)
    {
        // Виджет закрывается как будто нажата кнопка Cancel
        emit(reject());
        return;
    }

    cancelButton->setText(tr("Cancel")+" ["+QString::number(cancelCount)+"]");

    timer.start(1000);
}


void EnterPassword::passwordTyped(void)
{
    isPasswordTyped=true;
    cancelCountUpdate();
}

void EnterPassword::okClick(void)
{
    QString previousText=previousPasswordEdit->text();
    QString text1=passwordEdit1->text();
    QString text2=passwordEdit2->text();


    // Простой ввод пароля
    if(mode==ENTER_PASSWORD_MODE_SINGLE)
    {
        if(text1.length()>0)
        {
            password=text1;
            emit(accept());
            return;
        }
        else
        {
            QMessageBox::warning(this,
                                 tr("Error passwords entering"),
                                 tr("The password must not be empty."),
                                 QMessageBox::Close);
            return;
        }
    }


    // Ввод пароля с полем подтверждения пароля
    if(mode==ENTER_PASSWORD_MODE_DOUBLE)
    {
        if(text1.length()==0 || text2.length()==0)
        {
            QMessageBox::warning(this,
                                 tr("Error passwords entering"),
                                 tr("The password must not be empty."),
                                 QMessageBox::Close);
            return;
        }
        else if(text1.length()>0 && text1==text2)
        {
            // Иначе если пароли 1 и 2 совпадают

            password=text1;

            emit(accept());
            return;
        }
        else
        {
            // Иначе пароли 1 и 2 не совпадают
            QMessageBox::warning(this,
                                 tr("Error passwords entering"),
                                 tr("Password and confirm it is not equivalent!"),
                                 QMessageBox::Close);
            return;
        }
    }


    // Ввод пароля с полем подтверждения и с полем ввода старого пароля
    if(mode==ENTER_PASSWORD_MODE_WITH_PREVIOUS)
    {
        // Если хотя бы одно поле ввода пароля пустое
        if(previousText.length()==0 || text1.length()==0 || text2.length()==0)
        {
            QMessageBox::warning(this,
                                 tr("Error passwords entering"),
                                 tr("The password must not be empty."),
                                 QMessageBox::Close);
            return;
        }
        else if(text1.length()>0 && text1==text2)
        {
            // Иначе если пароли 1 и 2 совпадают

            // Проверяется, не совпадает ли новый пароль со старым
            if(previousText==text1)
            {
                QMessageBox::warning(this,
                                     tr("Error passwords entering"),
                                     tr("Old and new passwords is equivalent."),
                                     QMessageBox::Close);
                return;
            }
            else
            {
                previousPassword=previousText;
                password=text1;

                emit(accept());
                return;
            }
        }
        else
        {
            // Иначе пароли 1 и 2 не совпадают
            QMessageBox::warning(this,
                                 tr("Error passwords entering"),
                                 tr("Password and confirm it is not equivalent!"),
                                 QMessageBox::Close);
            return;
        }
    } // Закрылось условие обработки режима ENTER_PASSWORD_MODE_WITH_PREVIOUS

}


QString EnterPassword::getPassword(void)
{
    return password;
}


QString EnterPassword::getPreviousPassword(void)
{
    return previousPassword;
}
