#ifndef _TREESCREEN_H_
#define	_TREESCREEN_H_

#include <QtGlobal>
#include <QWidget>
#include <QListView>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include <QInputDialog>
#include <QFileInfo>
#include <QDateTime>

class KnowTreeModel;
class KnowTreeView;
class ClipboardBranch;


class TreeScreen : public QWidget
{
 Q_OBJECT

public:
 TreeScreen(QWidget *parent=0);
 virtual ~TreeScreen();

 QMap<QString, QAction *> actionList;

 KnowTreeModel *knowTreeModel;

 void saveKnowTree(void);
 bool reloadKnowTree(void);

 void updateSelectedBranch(void);

 int getFirstSelectedItemIndex(void);
 QModelIndex getCurrentItemIndex(void);
 
 QItemSelectionModel *getSelectionModel(void);

 void exportBranchToDirectory(QString exportDir);
 void importBranchFromDirectory(QString importDir);
 
 // Установка курсора на указанный элемент
 void setCursorToIndex(QModelIndex index);
 void setCursorToId(QString nodeId);
 
 void updateBranchOnScreen(const QModelIndex &index);

private slots:

 void expandAllSubbranch(void);
 void collapseAllSubbranch(void);
 void expandOrCollapseRecurse(QModelIndex index, bool mode);
 void insSubbranch(void);
 void insBranch(void);
 void editBranch(void);
 void setIcon(void);

 void delBranch(QString mode="delete");
 void delOneBranch(QModelIndex index);

 void moveUpBranch(void);
 void moveDnBranch(void);
 void cutBranch(void);
 bool copyBranch(void);
 void pasteBranch(void);
 void pasteSubbranch(void);

 void encryptBranch(void);
 void decryptBranch(void);

 // Действия при клике на ветку дерева
 void onKnowtreeClicked(const QModelIndex &index);

 // Открытие контекстного меню
 void onCustomContextMenuRequested(const QPoint &pos);

private:
 QToolBar *toolsLine;

 KnowTreeView  *knowTreeView;

 QVBoxLayout *treeScreenLayout;

 QDateTime lastKnowTreeModifyDateTime;
 qint64    lastKnowTreeSize;

 void setupUI(void);
 void setupModels(void);
 void setupSignals(void);
 void setupActions(void);
 void assembly(void);
 
 void moveUpDnBranch(int direction);
 bool moveCheckEnable(void);

 void insBranchSmart(bool is_branch);
 void insBranchProcess(QModelIndex index, QString name, bool is_branch);

 void addBranchToClipboard(ClipboardBranch *branch_clipboard_data, QStringList path, bool is_root);

 void pasteBranchSmart(bool is_branch);

 void treeEmptyControl(void);
 void treeCryptControl(void);

 void encryptBranchItem(void);
 void decryptBranchItem(void);

 void updateLastKnowTreeData(QFileInfo fileInfo, bool fileInfoValid);
};


#endif	// _TREESCREEN_H_

