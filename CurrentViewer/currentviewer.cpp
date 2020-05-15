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
	setWindowTitle("Current Viewer[GD.Wide_Cent.V5.R1(160601)]");					// 메인 윈도우 타이틀 명	
	styleSheetEditor = new StyleSheetEditor(this);									// 스타일 시트 파일 로딩
	//ui.setupUi(this);
		
	bulidLayout();																	// 레이아웃 생성	
	ServerRefresh();																// Tree Item 생성
	startEnboxThread();																// Enbox 탐색 시작
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
	splitter->setSizes(QList<int>() << 100 << 1000);			// 스플리터가 싸이즈를 관리
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
		// 미들웨어 없이 구동을 위한 khk 2020-05-15 이 데이터를 받으면 안된다
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
			m_treeWidget->setItemWidget(m_Rootitem, 0, NULL);	// Tree에 버튼이나 위젯을 넣을 수 있음
		}
		else
			continue;											// 사업소 코드가 안맞으면 다음거 연결

		nConnectMidCheck = connectMid( szServer ); // 미들웨어 접속 함수;

		if( nConnectMidCheck != 0 )
		{
			continue;   // 미들웨어 상태 이상시 다음것과 연결 해준다
		}
		m_treeWidget->expandItem(m_Rootitem);
	}
}

QTreeWidgetItem* CurrentViewer::OfficeReleam(QString strSubName, QVector<DLInfo>* vtDLName, QString strServer)
{	
	QTreeWidgetItem* pOfficeItem = new QTreeWidgetItem();	
	pOfficeItem->setText(0, strSubName+ QString::fromLocal8Bit("변전소"));
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

void CurrentViewer::newTabView(QString ServerCode, QString strDLName, int nCBId)				// OLD를 통해 실행 될 함수
{	
	addTabView(ServerCode, strDLName, nCBId);	
}

void CurrentViewer::addTabView(QString ServerCode, QString strDLName, int nCBId)
{
	for(int i = 0 ; i < m_DLbNameList.size(); i ++)									// 이미 떠 있는 탭은 제외 한다
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

void CurrentViewer::currentTab(int nIdx)												// 인덱스를 관리 할 거냐? 말거냐 관리할 거면 확실히 // <인덱스/오브젝트> 상으로 관리만 해도 할 수 있음
{
	if( nIdx == -1 )  //|| nIdx == 0 )   // 0번일 때는 제외 처음 시작시 2번 호출되기는 하나 탭이 변경되면 항상 최신xml로 위치하려고 createOLD를 아래서 재호출 
		return;							 // 리프레시 기능은 탭을 재성성에만 가능하지만 위에서 저장된 xml을 가져오면 탭이 변경될 때마다 리프레시가 가능(현재 미구현)
	
	m_pTabView = static_cast<TabView*>(m_DLTabWidget->currentWidget());		// 현재 탭으로 위치 
	m_CurrentTapServerCode = m_pTabView->getServerCode();
	m_CurrentCBId = m_pTabView->getCBId();
	bool bSuccess = DBFunction->CreateOLD(m_CurrentTapServerCode, m_CurrentCBId);// 리프레시할때나 제대로 써먹자 지금은 막해라 젠장
	if( bSuccess )
	{
		m_DLTabWidget->setCurrentIndex(nIdx);		// 기존에 떠있는 화면이고 트리를 선택하면 시그널이 발생하는 것이 아니라 이리로 들어옴
	}
	else
	{
		// 실패 로그 생성
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
		// // 미들웨어 없이 구동을 위한 khk 2020-05-15
		nConnectMid = 0; //MidPlusPlus::ConnectMid(strServer);
		if( nConnectMid != 0)
		{
			//네트워크 연결이 안되어있을때 3번 시도후 계속해서 연결 안될시 미들웨어 읽기 중단하고 메시지 박스로 사용자에게 알림
			nConnectMidError++;
		}
		else
			return nConnectMid;

	} while ( nConnectMidError <= 3 );

	if( nConnectMidError >= 3 )
	{
		//AfxMessageBox(L"미들웨어 연결이 불안정 합니다.\n 미들웨어 점검 후 프로그램을 재시작 해주시기 바랍니다.");
		//CW32Log::instance()->ag_log("미들웨어 연결이 불안정 합니다.\n 미들웨어 점검 후 프로그램을 재시작 해주시기 바랍니다.");
		return nConnectMid;
	}

	return nConnectMid;
}