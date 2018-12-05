#ifndef SHORTCUTSETTINGSVIEW_H
#define SHORTCUTSETTINGSVIEW_H

#include <QTreeView>


class ShortcutSettingsView : public QTreeView
{
    Q_OBJECT
public:
    ShortcutSettingsView(QWidget *parent=nullptr);
    ~ShortcutSettingsView();

//    void init();
//    void setController(ShortcutSettingsController *pController);


};

#endif // SHORTCUTSETTINGSVIEW_H
