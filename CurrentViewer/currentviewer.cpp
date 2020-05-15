#include "stdafx.h"
#include "currentviewer.h"
#include "document.h"
#include "EnboxThread.h"
#include "StyleSheet\stylesheeteditor.h"

#include <QTreeWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include <QEvent>
#include "DBHandler/qDBAccssor.h"

CurrentViewer::CurrentViewer(QWidget *parent, Qt::WFlags flags) : QMainWindow(parent, flags)
{
	//Titel
	setWindowTitle("Current Viewer[GD.Wide_Cent.V5.R1(160601)]");					// ���� ������ Ÿ��Ʋ ��	
	styleSheetEditor = new StyleSheetEditor(this);									// ��Ÿ�� ��Ʈ ���� �ε�
	//ui.setupUi(this);
		
	bulidLayout();																	// ���̾ƿ� ����	
	ServerRefresh();																// Tree Item ����
	startEnboxThread();																// Enbox Ž�� ����
}

CurrentViewer::~CurrentViewer()
{
	m_thread->quit();
	m_thread->wait();
	DBFunction->cleanServerData();
}

void CurrentViewer::bulidLayout()
{
	m_treeWidget = new QTreeWidget(this);	
	m_treeWidget->setProperty("style", true);
	m_treeWidget->setMinimumWidth(100);
	m_treeWidget->setMaximumWidth(200);

	QObject::connect(m_treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(ClickTreeServerItem(QTreeWidgetItem*, int)));
	QObject::connect(m_treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(clickTreeItem(QTreeWidgetItem*, int)));
	QObject::connect(m_treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(changeTreeItem(QTreeWidgetItem*, QTreeWidgetItem*)));

	m_DLTabWidget = new QTabWidget(this);
	m_DLTabWidget->setTabsClosable(true);

	connect( m_DLTabWidget , SIGNAL(tabCloseRequested(int)) , this , SLOT(closeTab(int)));
	connect( m_DLTabWidget , SIGNAL(currentChanged(int)) , this , SLOT(currentTab(int)));

	QWidget* central = new QWidget(this);
	QSplitter* splitter = new QSplitter(this);	
	QVBoxLayout* vLayout = new QVBoxLayout(this);

	splitter->addWidget(m_treeWidget);
	splitter->addWidget(m_DLTabWidget);
	vLayout->addWidget(splitter);
	central->setLayout(vLayout);
	setCentralWidget(central);
	splitter->setSizes(QList<int>() << 100 << 1000);			// ���ø��Ͱ� ����� ����
}

void CurrentViewer::startEnboxThread()
{
	m_thread = new EnboxMonitoring(DBFunction->getEnboxData(), this);
	connect(m_thread, SIGNAL(mySignal()), SLOT(onSignal()));
	m_thread->start();
}

void CurrentViewer::onSignal()
{
	update();
}

bool CurrentViewer::event(QEvent* _e)
{
	if( _e->type() == QEvent::User + 1)
	{
		OfficeEvent* e = static_cast<OfficeEvent*>(_e);
		// �̵���� ���� ������ ���� khk 2020-05-15 �� �����͸� ������ �ȵȴ�
		//newTabView(e->getServerCode(), e->getDlName(), e->getCBId());
	}
	return QWidget::event(_e);
}

void CurrentViewer::ServerRefresh()
{
	//m_treeWidget
	m_treeWidget->clear();
	m_treeWidget->setColumnCount(1);
	m_treeWidget->header()->setVisible(false);
	
	InitTreeServer();	
}

void CurrentViewer::InitTreeServer()
{
	TypeServerMap ServerMap = DBFunction->GetServerMap();
	if( ServerMap.empty() )
		return; 

	int nConnectMidCheck = 0;

	for ( int i = 0 ; i < ServerMap.size() ; i++ )
	{
		QString szServer = qDbAccssor->getServerName(i);	
		QString strOfficeName = qDbAccssor->getOfficeName(szServer);
		TypeServerMap::iterator Serveriter = ServerMap.find(szServer);
		if( Serveriter != ServerMap.end() )
		{			
			m_Rootitem = new QTreeWidgetItem();
			QTreeWidgetItem* childitem = NULL;
			ItemMetaData OfficeRoot;
			OfficeRoot.nTreeLevel = ITEM_OFFICE;
			m_Rootitem->setData(0, Qt::UserRole, QVariant::fromValue(OfficeRoot));
			m_Rootitem->setData(0, Qt::DisplayRole, strOfficeName);
			m_Rootitem->setData(0, Qt::TextColorRole, Qt::darkRed);
			m_Rootitem->setIcon(0, QIcon(":/CurrentViewer/Resources/icons/TreeIcon/icon_office.png"));

			QMap<QString,TypeDLName*>	SubStation = (*Serveriter);
			QMapIterator<QString, TypeDLName*>	SubItr(SubStation);

			while( SubItr.hasNext() )
			{
				SubItr.next();
				QString strSubName = SubItr.key();
				QVector<DLInfo>* vtDLName = SubItr.value();

				if( vtDLName->size() > 0 )
				{
					childitem = OfficeReleam(strSubName, vtDLName, szServer);
					ItemMetaData SubStationRoot;								
					SubStationRoot.nTreeLevel = ITEM_STATION;
					SubStationRoot.ServerCode = szServer;
					childitem->setData(0, Qt::UserRole, QVariant::fromValue(SubStationRoot));
					m_Rootitem->addChild(childitem);
				}
			}	

			m_treeWidget->addTopLevelItem(m_Rootitem);
			m_treeWidget->setExpandsOnDoubleClick(true);
			m_treeWidget->setItemWidget(m_Rootitem, 0, NULL);	// Tree�� ��ư�̳� ������ ���� �� ����
		}
		else
			continue;											// ����� �ڵ尡 �ȸ����� ������ ����

		nConnectMidCheck = connectMid( szServer ); // �̵���� ���� �Լ�;

		if( nConnectMidCheck != 0 )
		{
			continue;   // �̵���� ���� �̻�� �����Ͱ� ���� ���ش�
		}
		m_treeWidget->expandItem(m_Rootitem);
	}
}

QTreeWidgetItem* CurrentViewer::OfficeReleam(QString strSubName, QVector<DLInfo>* vtDLName, QString strServer)
{	
	QTreeWidgetItem* pOfficeItem = new QTreeWidgetItem();	
	pOfficeItem->setText(0, strSubName+ QString::fromLocal8Bit("������"));
	pOfficeItem->setIcon(0, QIcon(":/CurrentViewer/Resources/icons/TreeIcon/icon_substation.png"));
	ChildItem(vtDLName, pOfficeItem, strServer);

	return pOfficeItem;
}

void CurrentViewer::ChildItem(QVector<DLInfo>* vtDLName, QTreeWidgetItem* pParentItem, QString strServer)
{
	ItemMetaData DLRoot;
	DLRoot.nTreeLevel = ITEM_DL;

	for( int i = 0 ; i < vtDLName->size() ; i++ )
	{
		QString sDLName = vtDLName->at(i).strDLName;
		DLRoot.nCBId = vtDLName->at(i).nCBId;
		DLRoot.ServerCode = strServer;
		QTreeWidgetItem* pTypeItem = new QTreeWidgetItem;
		pTypeItem->setData(0, Qt::UserRole, QVariant::fromValue(DLRoot));
		pTypeItem->setText(0, sDLName);
		pTypeItem->setIcon(0, QIcon(":/CurrentViewer/Resources/icons/TreeIcon/icon_DL.png"));
		pParentItem->addChild(pTypeItem);
	}
}

void CurrentViewer::ClickTreeServerItem(QTreeWidgetItem*, int)
{
	QTreeWidgetItem* pCurItem = m_treeWidget->currentItem();	
	QString strDLName;
	ItemMetaData sServerData;
	sServerData = pCurItem->data(0, Qt::UserRole).value<ItemMetaData>();
	
	if(sServerData.nTreeLevel != 2)
		return;

	int nCBId = sServerData.nCBId;
	strDLName = pCurItem->text(0);
	
	newTabView(sServerData.ServerCode, strDLName, nCBId);
}
void CurrentViewer::clickTreeItem(QTreeWidgetItem *item , int nColumn)
{
	ItemMetaData curItem;
	curItem = item->data(0, Qt::UserRole).value<ItemMetaData>();

	if( curItem.nTreeLevel == 0 )
	{
		item->setIcon(0, QIcon(":/CurrentViewer/Resources/icons/TreeIcon/icon_office.png"));
	}
	else if( curItem.nTreeLevel == 1 )
	{
		item->setIcon(0, QIcon(":/CurrentViewer/Resources/icons/TreeIcon/icon_substation_select.png"));
	}
	else if( curItem.nTreeLevel == 2 )
	{
		item->setIcon(0, QIcon(":/CurrentViewer/Resources/icons/TreeIcon/icon_DL_select.png"));
		QTreeWidgetItem* parentItem = item->parent();
		parentItem->setIcon(0, QIcon(":/CurrentViewer/Resources/icons/TreeIcon/icon_substation_select.png"));
	}	
}

void CurrentViewer::changeTreeItem(QTreeWidgetItem *item , QTreeWidgetItem *previousItem)
{
	if(previousItem == NULL)
		return;

	ItemMetaData curItem, preItem;
	curItem = item->data(0, Qt::UserRole).value<ItemMetaData>();
	preItem = previousItem->data(0, Qt::UserRole).value<ItemMetaData>();
	
	if( preItem.nTreeLevel == 0 )
	{
		previousItem->setIcon(0, QIcon(":/CurrentViewer/Resources/icons/TreeIcon/icon_office.png"));
	}
 	else if( preItem.nTreeLevel == 1 )
 	{
		previousItem->setIcon(0, QIcon(":/CurrentViewer/Resources/icons/TreeIcon/icon_substation.png"));
 	}
 	else if( preItem.nTreeLevel == 2 )
 	{
		previousItem->setIcon(0, QIcon(":/CurrentViewer/Resources/icons/TreeIcon/icon_DL.png"));
		QTreeWidgetItem* parentItem = previousItem->parent();
		parentItem->setIcon(0, QIcon(":/CurrentViewer/Resources/icons/TreeIcon/icon_substation.png"));
	}	
}

void CurrentViewer::newTabView(QString ServerCode, QString strDLName, int nCBId)				// OLD�� ���� ���� �� �Լ�
{	
	addTabView(ServerCode, strDLName, nCBId);	
}

void CurrentViewer::addTabView(QString ServerCode, QString strDLName, int nCBId)
{
	for(int i = 0 ; i < m_DLbNameList.size(); i ++)									// �̹� �� �ִ� ���� ���� �Ѵ�
	{
		if( strDLName == m_DLbNameList.at(i))
		{
			currentTab(i);
			return;
		}
	}
	m_DLbNameList.push_back(strDLName);

	m_pTabView = new TabView(this, ServerCode, strDLName, nCBId );	
	m_pTabView->setProperty("a", true);	
	m_pTabView->createTapView(ServerCode, nCBId, strDLName);

	m_DLTabWidget->addTab(m_pTabView, strDLName);
	setCurrentTabView(ServerCode, nCBId, m_pTabView);
}

void CurrentViewer::closeTab(int nIdx)
{
	m_DLTabWidget->removeTab(nIdx);
	m_DLbNameList.removeAt(nIdx);	
}

void CurrentViewer::currentTab(int nIdx)												// �ε����� ���� �� �ų�? ���ų� ������ �Ÿ� Ȯ���� // <�ε���/������Ʈ> ������ ������ �ص� �� �� ����
{
	if( nIdx == -1 )  //|| nIdx == 0 )   // 0���� ���� ���� ó�� ���۽� 2�� ȣ��Ǳ�� �ϳ� ���� ����Ǹ� �׻� �ֽ�xml�� ��ġ�Ϸ��� createOLD�� �Ʒ��� ��ȣ�� 
		return;							 // �������� ����� ���� �缺������ ���������� ������ ����� xml�� �������� ���� ����� ������ �������ð� ����(���� �̱���)
	
	m_pTabView = static_cast<TabView*>(m_DLTabWidget->currentWidget());		// ���� ������ ��ġ 
	m_CurrentTapServerCode = m_pTabView->getServerCode();
	m_CurrentCBId = m_pTabView->getCBId();
	bool bSuccess = DBFunction->CreateOLD(m_CurrentTapServerCode, m_CurrentCBId);// ���������Ҷ��� ����� ����� ������ ���ض� ����
	if( bSuccess )
	{
		m_DLTabWidget->setCurrentIndex(nIdx);		// ������ ���ִ� ȭ���̰� Ʈ���� �����ϸ� �ñ׳��� �߻��ϴ� ���� �ƴ϶� �̸��� ����
	}
	else
	{
		// ���� �α� ����
	}
	
}

void CurrentViewer::setCurrentTabView(QString ServerCode,int nCBId, TabView *CurrentTab)
{
	setTab(ServerCode, nCBId);	
	m_DLTabWidget->setCurrentWidget(CurrentTab);
}

void CurrentViewer::setTab(QString ServerCode, int nCBId)
{
	m_CurrentTapServerCode = ServerCode;
	m_CurrentCBId = nCBId;		
}

int CurrentViewer::connectMid(QString strServer) 
{
	int nConnectMidError = 0;
	int nConnectMid = 0;

	do 
	{
		// // �̵���� ���� ������ ���� khk 2020-05-15
		nConnectMid = 0; //MidPlusPlus::ConnectMid(strServer);
		if( nConnectMid != 0)
		{
			//��Ʈ��ũ ������ �ȵǾ������� 3�� �õ��� ����ؼ� ���� �ȵɽ� �̵���� �б� �ߴ��ϰ� �޽��� �ڽ��� ����ڿ��� �˸�
			nConnectMidError++;
		}
		else
			return nConnectMid;

	} while ( nConnectMidError <= 3 );

	if( nConnectMidError >= 3 )
	{
		//AfxMessageBox(L"�̵���� ������ �Ҿ��� �մϴ�.\n �̵���� ���� �� ���α׷��� ����� ���ֽñ� �ٶ��ϴ�.");
		//CW32Log::instance()->ag_log("�̵���� ������ �Ҿ��� �մϴ�.\n �̵���� ���� �� ���α׷��� ����� ���ֽñ� �ٶ��ϴ�.");
		return nConnectMid;
	}

	return nConnectMid;
}