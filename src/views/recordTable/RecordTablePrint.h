#ifndef _RECORD_TABLE_PRINT_H_
#define	_RECORD_TABLE_PRINT_H_

#include <QtGlobal>
#include <QWidget>
#include <QDialog>
#include <QDialogButtonBox>
#include <QTextEdit>


class RecordTableProxyModel;


class RecordTablePrint : public QDialog
{
 Q_OBJECT

public:
 RecordTablePrint(QWidget *parent=0);
 virtual ~RecordTablePrint();

 void setModel(RecordTableProxyModel *model);

public slots:

 void print(void);

private:

 QTextEdit *textArea; // Отображение текста с таблицей, выводимой на печать
 QDialogButtonBox *buttonBox; // Линейка с кнопками

 void setup_ui(void);
 void setup_signals(void);
 void assembly(void);
};

#endif	/* _RECORD_TABLE_PRINT_H_ */

