#ifndef CURRENTVIEWER_H
#define CURRENTVIEWER_H

#include <QtGui/QMainWindow>
#include <QMutex>

#include "ui_currentviewer.h"
#include "ItemMetaData.h"
#include "DBHandler/agDBFunction.h"

class TabView;
class QTabWidget;
class QTreeWidget;
class EnboxMonitoring;
class StyleSheetEditor;
class CurrentViewer : public QMainWindow
{
	Q_OBJECT

public:
	CurrentViewer(QWidget *parent = 0, Qt::WFlags flags = 0);
	~CurrentViewer();

public:
	void bulidLayout();											// ·¹ÀÌ¾Æ¿ô »ý¼º	
	void startEnboxThread();										// Enbox Å½»ö ½ÃÀÛ
	void ServerRefresh();

	void InitTreeServer();
	QTreeWidgetItem* OfficeReleam(QString strSubName, QVector<DLInfo>* vtDLName, QString strServer);
	void ChildItem(QVector<DLInfo>* vtDLName, QTreeWidgetItem* pParentItem, QString strServer);	

	void newTabView(QString ServerCode, QString strDLName, int nCBId);
	void addTabView(QString ServerCode, QString strDLName, int nCBId);
	void setCurrentTabView(QString ServerCode, int nCBId, TabView *doc);
	void setTab(QString ServerCode, int nCBId);
	int connectMid(QString strServer );

public slots:
	void	onSignal();
	void    clickTreeItem(QTreeWidgetItem*, int);
	void    changeTreeItem(QTreeWidgetItem *item , QTreeWidgetItem* previousItem);
	void    ClickTreeServerItem(QTreeWidgetItem*, int);	
	void	closeTab(int nIdx);
	void	currentTab(int nIdx);

protected:
	bool	event(QEvent* _e);

private:
	Ui::CurrentViewerClass ui;
	StyleSheetEditor *styleSheetEditor;

	EnboxMonitoring*	m_thread;
	QMutex		m_mutex;

	QTreeWidget* m_treeWidget;
	QTreeWidgetItem* m_Rootitem;
	
	QTabWidget* m_DLTabWidget;	
	QStringList m_DLbNameList;

	TabView *m_pTabView;	
	QString m_CurrentTapServerCode;
	int m_CurrentCBId;
};

#define ITEM_OFFICE 0
#define ITEM_STATION 1
#define ITEM_DL 2

#define ITEM_SERVER 4
#define ITEM_KIND 5

Q_DECLARE_METATYPE(ItemMetaData)

#endif // CURRENTVIEWER_H
