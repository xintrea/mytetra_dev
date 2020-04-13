#include <QWidget>
#include <QBoxLayout>
#include <QDir>
#include <QLineEdit>
#include <QDebug>

#include "ConfigPage.h"


ConfigPage::ConfigPage(QWidget *parent) : QWidget(parent)
{

}


ConfigPage::~ConfigPage()
{
    qDebug() << Q_FUNC_INFO;
}


int ConfigPage::applyChanges(void)
{
 return 0;
}

