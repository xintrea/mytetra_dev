#ifndef _EDITORIMAGEPROPERTIES_H_
#define	_EDITORIMAGEPROPERTIES_H_

#include <QtGlobal>
#include <QWidget>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>


class EditorImageProperties : public QDialog
{
 Q_OBJECT

public:
 EditorImageProperties(QWidget *parent=0);
 virtual ~EditorImageProperties();

 void set_info(QString infoText);
 void set_real_width(int width);
 void set_real_height(int height);
 void set_width(int width);
 void set_height(int height);

 int get_width(void);
 int get_height(void);

 void update_percent(void);

private slots:

 void on_changed_percent(int n);
 void on_changed_width(int n);
 void on_changed_height(int n);

 void on_click_reset_size();

private:

 QLabel *infoLabel;

 QLabel *percentSizeLabel;
 QSpinBox *percentSizeSpin;


 QLabel *widthLabel;
 QSpinBox *widthSpin;

 QLabel *heightLabel;
 QSpinBox *heightSpin;

 QPushButton *resetSizeButton;

 QDialogButtonBox *buttonBox;

 void setup_ui(void);
 void setup_signals(void);
 void assembly(void);

 int imageRealWidth;
 int imageRealHeight;

 bool isRelateSizeSetted;
};

#endif	/* _EDITORIMAGEPROPERTIES_H_ */

