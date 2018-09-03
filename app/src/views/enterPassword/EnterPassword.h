#ifndef _ENTERPASSWORD_H_
#define	_ENTERPASSWORD_H_

#include <QWidget>
#include <QDialog>
#include <QTimer>

class QLabel;
class QLineEdit;
class QPushButton;

#define ENTER_PASSWORD_MODE_SINGLE 0
#define ENTER_PASSWORD_MODE_DOUBLE 1
#define ENTER_PASSWORD_MODE_WITH_PREVIOUS 2


class EnterPassword : public QDialog
{
 Q_OBJECT

public:
 EnterPassword(int imode, QWidget *parent=0);

 QString getPassword(void);

 QString getPreviousPassword(void);

 void setCancelDelay(int delay);

private slots:

 void okClick(void);
 void cancelCountUpdate(void);
 void passwordTyped(void);

private:
 QLabel    *label;
 QLabel    *label1;
 QLabel    *label2;
 QLineEdit *passwordEdit1;
 QLineEdit *passwordEdit2;

 QLabel    *previousLabel;
 QLineEdit *previousPasswordEdit;

 // QDialogButtonBox *buttonBox;
 QPushButton *okButton;
 QPushButton *cancelButton;

 QString password;
 QString previousPassword;
 int mode;

 int cancelDelay; // Сколько времени ждать автонажатия Cancel (сек)
 int cancelCount; // Сколько времени просшло с начала открытия окна (сек)

 QTimer timer;

 bool isPasswordTyped; // Начал ли пользователь набирать пароль

 void setupUI(void);
 void setupSignals(void);
 void assembly(void);

 void showEvent(QShowEvent *event);
};

#endif	/* _ENTERPASSWORD_H_ */

