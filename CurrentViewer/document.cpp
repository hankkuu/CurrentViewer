#include "stdafx.h"
#include "document.h"

#include "agOLD/agcanvaswidget.h"
#include "agOLD/agCanvasScene.h"
#include "Control/searchwidget.h"
#include "Control/setupdatetime.h"

#include "subdocument.h"
#include "Chart/chartview.h"
#include "Chart/chartviewtable.h"

#include <QSplitter>
#include <QTime>
#include <QtGui/QListWidget>

TabView::TabView(QWidget *parent, QString ServerCode, QString strDLName, int nCBId) : QWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QString strXMLString;
	bool bSuccess = DBFunction->CreateOLD(ServerCode, nCBId);	// 탭 뷰를 생성 할 때 XML data를 가져온다 // 생성자에서 데이터를 가져오는것이 이상하다면 변경
	if( bSuccess )
	{
		strXMLString = DBFunction->GetXMLDLInfo();
		setXMLData(strXMLString); 
	}
	else
	{
		m_strXMLString = "";
		// 화면 생성 실패 로그 생성
	}
	
	m_ServerCode = ServerCode;
	m_strDLName = strDLName;
	m_nCBId = nCBId;

	bulidLayout();
}

TabView::~TabView()
{
}

void TabView::bulidLayout()
{
	//단선도 화면 삽입		
	m_pCanvasWidget = new agCanvasWidget(this , NULL , 0);	
	m_pCanvasWidget->setProperty("style", true);
	//검색버튼 삽입
	m_pControlWidget = new SearchWidget(this);		
	//차트탭 삽입
	m_pChartTab = new QTabWidget(this);
	m_pChartTab->setTabsClosable(true);	

	// 레이아웃 구성
	QWidget* tabContainer = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout(this);
	QVBoxLayout* subLayout = new QVBoxLayout(this);	
	QSplitter* splitter = new QSplitter(Qt::Vertical, this);

	splitter->addWidget(m_pCanvasWidget);	
	splitter->addWidget(tabContainer);
	layout->addWidget(splitter);
	setLayout(layout);	
	subLayout->addWidget(m_pControlWidget);
	subLayout->addWidget(m_pChartTab);		
	tabContainer->setLayout(subLayout);

	connect(m_pCanvasWidget, SIGNAL(printChart()), SLOT(onPrintChart()));						// 선택 구간 차트 출력
	connect(m_pCanvasWidget, SIGNAL(printCurrentDayChart()), SLOT(onPrintCurrentDayChart()));	// 전체 구간 차트 출력
	connect(m_pCanvasWidget, SIGNAL(printMainLineChart()),  SLOT(onPrintMainLineChart()));		// 간선 구간 차트 출력
	connect(m_pCanvasWidget, SIGNAL(reTimeSettings()),  SLOT(onReTimeSettings()));				// 검색 시간 변경 위젯 활성화

	connect( m_pChartTab , SIGNAL(tabCloseRequested(int)) , this , SLOT(closeTab(int)));
	connect( m_pChartTab , SIGNAL(currentChanged(int)) , this , SLOT(CurrentChartTab(int)));

	connect(m_pControlWidget, SIGNAL(newChartClicked()), SLOT(onNewChartClicked()));			// 새탭 출력

}

void TabView::closeTab(int nIdx)
{
	m_pChartTab->removeTab(nIdx);		
}

void TabView::CurrentChartTab(int nIdx)
{
	if( nIdx == -1 ) 
		return;

	m_pChartTab->setCurrentIndex(nIdx);
	agCanvasScene* agScene = (agCanvasScene*)m_pCanvasWidget->scene();
	QList<agLineItem*> selLines = agScene->selectedLineItems();
	foreach(agLineItem* line, selLines)
	{
		line->setSelected(false);
	}
	
	m_pChartList = dynamic_cast<ChartView*>(m_pChartTab->currentWidget());
	Plot* plot = m_pChartList->getChartItem();
	//plot->setSelectedList();									// 시리즈 선택에 따라 선택 선로 표시 바꾸면서 탭 전환에는 일단 없앰
}

void TabView::createTapView(QString strServer, int nCBId, QString strDLName)
{
	if( m_pCanvasWidget != NULL )
	{
		QString strXMLData = getXMLData();
		m_pCanvasWidget->setXmlData(strXMLData);
		if( m_pCanvasWidget->SaveNzedNetwork() == true )
		{
			SetNetwork(m_pCanvasWidget);
		}
		m_pCanvasWidget->makeMainLine( strServer, nCBId );							// 각각의 라인에 값 대입... 이딴식으로 만들고 싶지않은데.. 머리가 안굴러간다..

		agCanvasScene* agScene = (agCanvasScene*)m_pCanvasWidget->scene();
		agScene->setTabView(this);													// 씬과 연결된 우측 위젯을 연결
		agScene->mainLastLine();													// 간선과 관련된 데이터 추출
	}	
	
	// 간선구간 차트탭을 생성   Context Menu의 하나로 변경	
	// 해당 DL의 24시간 data를 가져온다 -- 슬롯함수는 전체라인을 더한 거고 디폴트 차트는 CB 데이터를가져온다.
	printCBDayChart(strServer, nCBId, strDLName);
}

void TabView::SetNetwork(agCanvasWidget* CanvasData)
{
	m_Canvas = CanvasData;
}

agCanvasWidget* TabView::GetNetwork()
{
	return m_Canvas;
}

void TabView::StopObject()
{
	// 개체를 더블 클릭하면 깜박임이 있다 해제해야 할 수도 있다  // 일단 기능 얘기가 없으니 비활성화
	//m_Canvas->stopBlinkObj();
}

void TabView::printCBDayChart(QString strServer, int nCBId, QString strDLName)
{
	QString dDate = QDate::currentDate().addDays(-1).toString("yyyy-MM-dd");
	QString dHour = QTime::currentTime().toString("hh-mm-ss");
	QString FromTime = dDate + dHour;
	QString ToTime = DBFunction->getCurrentTime();
	QString StartTime; 
	QString EndTime; 
	QString SerchTime;
	int nMaxPValue = 0;
	makeTimeString( FromTime,  ToTime,  StartTime,  EndTime,  SerchTime, HOUR);

	if( m_pCanvasWidget != NULL )
	{
		agCanvasScene* agScene = (agCanvasScene*)m_pCanvasWidget->scene();
		QList<agLineItem*> MainLineItem = agScene->getMainLineItem();		// 씬에서 만든 간선의 LineItem만 get
		//차트 데이터 취득
		m_pCanvasWidget->makeChartXData(MainLineItem);		
		QMap<int, QString> XValue = m_pCanvasWidget->getChartXData();		// 구간 값을 차트 리스트 쪽으로 넘겨 준다

		if( XValue.isEmpty() )
			XValue.insert(nMaxPValue, strDLName);							// CB만 있는 경우 DL 이름을 차트 리스트 쪽으로 넘겨 준다

		QList<agLineItem*> selAllLineItem = ((agCanvasScene*)m_pCanvasWidget->scene())->lineItems();   // 전체를 가져오는 것은 모든 선로 가져 옴

		// 		if( selAllLineItem.size() == 0 )
		// 			return;	

		DBFunction->makeCBTimeChart(strServer, nCBId, StartTime, EndTime, HOUR);				// 24시간 기준 차트 출력
		QMap<int, QString> XData = DBFunction->GetChartTimeXaxis();		
		QMap<int, ChartData> YData = DBFunction->GetChartSortLineData();
		QMap<int, qreal> resultYdata = makePValueY(YData, nMaxPValue);		

		int nIndex = 1;
		QList<agLineItem*> nullList;
		Plot *plot = new Plot(nIndex, nMaxPValue, nullList , XData, resultYdata, StartTime, EndTime, HOUR, this);
		m_pChartList = new ChartView(this, plot, XValue, selAllLineItem);	
		m_pChartTab->addTab(m_pChartList , m_strDLName + QString::fromLocal8Bit("(CB)") );		
		m_pChartTab->setCurrentWidget(m_pChartList);
	}	
}


void TabView::onPrintCurrentDayChart()
{
	QString dDate = QDate::currentDate().addDays(-1).toString("yyyy-MM-dd");
	QString dHour = QTime::currentTime().toString("hh-mm-ss");
	QString FromTime = dDate + dHour;
	QString ToTime = DBFunction->getCurrentTime();
	QString StartTime; 
	QString EndTime; 
	QString SerchTime;
	int nMaxPValue = 0;

	makeTimeString( FromTime,  ToTime,  StartTime,  EndTime,  SerchTime, HOUR);

	if( m_pCanvasWidget != NULL )
	{
		agCanvasScene* agScene = (agCanvasScene*)m_pCanvasWidget->scene();
		QList<agLineItem*> MainLineItem = agScene->getMainLineItem();
		//차트 데이터 취득
		m_pCanvasWidget->makeChartXData(MainLineItem);
		QMap<int, QString> XValue = m_pCanvasWidget->getChartXData();						// 구간 값을 차트 테이블 쪽으로 넘겨 준다

		QList<agLineItem*> selAllLineItem = ((agCanvasScene*)m_pCanvasWidget->scene())->lineItems();		
		if( selAllLineItem.size() == 0 )
			return;	

		QList<int> selAllLineId = makeLineIdList(selAllLineItem);
		QString strServer = getServerCode();

		DBFunction->SetChartLineData(selAllLineId);	
		DBFunction->makeTimeChart(strServer, StartTime, EndTime, HOUR);
		QMap<int, QString> XData = DBFunction->GetChartTimeXaxis();		
		QMap<int, ChartData> YData = DBFunction->GetChartSortLineData();
		QMap<int, qreal> resultYdata = makePValueY(YData, nMaxPValue);		

		int nIndex = 1;
		Plot *plot = new Plot(nIndex, nMaxPValue, selAllLineItem, XData, resultYdata, StartTime, EndTime, HOUR, this);
		m_pChartList = new ChartView(this, plot, XValue, selAllLineItem);	
		m_pChartTab->addTab(m_pChartList , m_strDLName + QString::fromLocal8Bit("(전체선로)") );		
		m_pChartTab->setCurrentWidget(m_pChartList);
	}	
}

void TabView::onPrintMainLineChart()
{
	//간선 구간 획득
	agCanvasScene* agScene = (agCanvasScene*)m_pCanvasWidget->scene();	
	QList<agLineItem*> MainLineItem = agScene->getMainLineItem();
	//차트 데이터 취득
	m_pCanvasWidget->makeChartXData(MainLineItem);
	m_pCanvasWidget->makeChartYData(MainLineItem);								// 현재 하는 기능 없다.... 

	QMap<int, QString> XValue = m_pCanvasWidget->getChartXData();
	int nMaxPValue = 0;
	QMap<int, qreal> YData;
	int nIdx = 0;
	QList<QPair<int, qreal> > sectionTotals = agScene->represntTotal();
	for(int i = 0; i < sectionTotals.count(); i++) 
	{
		if( sectionTotals.at(i).second > nMaxPValue  )
		{
			nMaxPValue = sectionTotals.at(i).second;
		}
		//qDebug("SECTION ID[%d]  DATA = %f", sectionTotals.at(i).first, sectionTotals.at(i).second);
		YData.insert(nIdx, sectionTotals.at(i).second);
		nIdx++;
	}
	//qDebug("#################################  Main Line Searching Completed.. ###################################");

	QList<agLineItem*> selAllLineItem = agScene->lineItems();   // 전체를 가져오는 것은 모든 선로 가져 옴

	// 차트 + 시리즈 관련 표 Qwidget 삽입	
	//차트 생성
	int nIndex = 1;
	Plot* plot = new Plot(nIndex, nMaxPValue, XValue, YData, this);
	ChartView* ChatView = new ChartView( this, plot, XValue, selAllLineItem, true);	// 멤버로 할 필요 없음//  포인터를 넘겨 준다는 것은 저기에 접근한다는 것	
	m_pChartTab->addTab(ChatView , m_strDLName + QString::fromLocal8Bit("(간선구간)"));	
	m_pChartTab->setCurrentWidget(ChatView);
}

void TabView::sumSectionValue()
{
	if( m_pCanvasWidget != NULL )
	{
		m_selItem = ((agCanvasScene*)m_pCanvasWidget->scene())->selectedLineItems();	  // m_pCanvasWidget 생성한 후라서 new를 안해도 데이터가 들어옴

		////  선택된 선로가 없으면 리턴 할 수 있다 리턴안하는 이유는 0으로 값 체인지
		// 		if( m_selItem.size() == 0 )
		// 		{
		// 			return;	
		// 		}

		int nIdx = 0;
		QMap<int, QString> XData;
		QMap<int, qreal> YData;
		qreal sumPvalue = 0.0;		

		if( m_selItem.size() != 0 )
		{
			foreach(agLineItem* lineItems, m_selItem)
			{
				sumPvalue += lineItems->getChartPValue();
			}
		}	

		m_pControlWidget->setTotalPValue(sumPvalue);

		//// 구간차트는 필수가 아니라 출력을 막음	
		// 		QList<QString> captions = ((agCanvasScene*)m_pCanvasWidget->scene())->selectedCaptions(selItem);
		// 		foreach(QString caption, captions)
		// 		{
		// 			qDebug("   ### CAPTION: %s", caption.toUtf8().data());
		// 			XData.insert(nIdx, caption);	
		// 			YData.insert(nIdx, sumPvalue);
		// 			nIdx++;
		// 		}
		// 		//YData.erase(--nIdx);
		// 		m_pCanvasWidget->setMaxPVlaue(selItem);
		// 		//int nMaxPValue = m_pCanvasWidget->getMaxPVlaue();
		// 		int nMaxPValue = sumPvalue;
		// 		Plot *plot = new Plot(nMaxPValue, selItem, XData, YData, this);				
		// 		m_pChartTab->addTab(plot , XData.value(0) );	
		// 		//m_SelLines.clear();
		// 		m_pChartTab->setCurrentWidget(plot);
	}
}

void TabView::makeTimeString(QString FromTime, QString ToTime, QString& StartTime, QString& EndTime, QString& SerchTime, DateType dType)
{
	QString sStartYear = FromTime.mid(0,4);
	QString sStartMonth = FromTime.mid(5,2);
	QString sStartDay = FromTime.mid(8,2);
	QString sStartHour = FromTime.mid(10,2);
	QString sEndYear = ToTime.mid(0,4);
	QString sEndMonth = ToTime.mid(5,2);
	QString sEndDay = ToTime.mid(8,2);
	QString sEndHour = ToTime.mid(10,2);

	if( dType == MONTH )
	{
		SerchTime += QString::fromLocal8Bit("(월간검색)");
		StartTime = sStartYear + '-' + sStartMonth + '-' + "01" + ' ' + "00:00:00";
		// 매달 말일이 다르기 때문에 월간은 말일이 달라진다.... 시발... 일단 보여줘야 하니 윤달 계산은 나중에
		if( sEndMonth == "01" || sEndMonth == "03" || sEndMonth == "05" || sEndMonth == "07" || sEndMonth == "08" || sEndMonth == "10" || sEndMonth == "12" )
			EndTime = sEndYear + '-' + sEndMonth + '-' + "31" + ' ' + "23:59:59";
		else if( sEndMonth == "04" || sEndMonth == "06" || sEndMonth == "09" || sEndMonth == "11" )
			EndTime = sEndYear + '-' + sEndMonth + '-' + "30" + ' ' + "23:59:59";
		else if( sEndMonth == "02" )
			EndTime = sEndYear + '-' + sEndMonth + '-' + "28" + ' ' + "23:59:59";
	}
	else if( dType == DAY  )
	{
		SerchTime += QString::fromLocal8Bit("(일간검색)");
		StartTime = sStartYear + '-' + sStartMonth + '-' + sStartDay + ' ' + "00:00:00";
		EndTime = sStartYear + '-' + sEndMonth + '-' + sEndDay + ' ' + "23:59:59";
	}
	else if( dType == HOUR  )
	{
		SerchTime += QString::fromLocal8Bit("(시간검색)");
		StartTime = sStartYear + '-' + sStartMonth + '-' + sStartDay + ' ' + sStartHour + ":00:00";
		EndTime = sEndYear + '-' + sEndMonth + '-' + sEndDay + ' ' + sEndHour + ":59:59";
	}
}

QList<int> TabView::makeLineIdList(QList<agLineItem*> lines)
{
	QList<int> lineList;
	QListIterator<agLineItem*> agLine(lines);
	while (agLine.hasNext())
	{
		int nLine;
		nLine = agLine.next()->lineId().toInt();
		lineList.push_back(nLine);
	}

	return lineList;
}

QMap<int, qreal> TabView::makePValueY( QMap<int, ChartData> YData, int& nMaxPValue )
{
	float chartDataA, chartDataB, chartDataC;	
	float PValue;
	int nIdx = 0;	
	QMap<int, qreal> resultYdata;

	QMapIterator<int, ChartData> data(YData);
	while( data.hasNext() )
	{
		data.next();
		chartDataA = data.value().fAvg_Current_a * 13.2; 
		chartDataB = data.value().fAvg_Current_b * 13.2;
		chartDataC = data.value().fAvg_Current_c * 13.2;
		PValue = chartDataA + chartDataB + chartDataC;

		if( PValue > nMaxPValue )
			nMaxPValue = PValue;

		resultYdata.insert(nIdx, PValue);
		nIdx++;
	}

	return resultYdata;
}

// 새탭 생성
void TabView::onNewChartClicked()
{
	m_selItem.clear();	

	QString dDate = QDate::currentDate().addDays(-1).toString("yyyy-MM-dd");
	QString dHour = QTime::currentTime().toString("hh-mm-ss");
	QString FromTime = dDate + dHour;
	QString ToTime = DBFunction->getCurrentTime();
	QString StartTime; 
	QString EndTime; 
	QString SerchTime = getDlName();

	makeTimeString( FromTime,  ToTime,  StartTime,  EndTime,  SerchTime, HOUR);
	
	if( m_pCanvasWidget != NULL )
	{
		int nIndex = 0;
		Plot* plot = new Plot(nIndex, StartTime, EndTime, HOUR, this);
		m_pChartList = new ChartView(this, plot);	
		m_pChartTab->addTab(m_pChartList , SerchTime );
		m_pChartTab->setCurrentWidget(m_pChartList);
	}	
}

// 선택한 구간 차트 출력
void TabView::onPrintChart()						
{
	m_selItem.clear();	
	m_pChartList = dynamic_cast<ChartView*>(m_pChartTab->currentWidget());
	
	if( m_pChartList->mainLineChart() == true )
	{
		QMessageBox msg;
		msg.setText( QString::fromLocal8Bit("현재 간선구간 차트는 X축이 시간단위가 아닙니다. 차트를 바꿔 주세요."));
		msg.exec();
		return;
	}

	Plot* plot = m_pChartList->getChartItem();
	QList<QString> searchTime = plot->getSearchTime();
	QMap<int, QString> nXCount = plot->getXCount();
	int nMaxPValue = plot->getMaxPValue();
	DateType currentDateType = plot->getDateType();
	
	if( searchTime.isEmpty() )
	{
		return;						// 선택 구간 차트는 X축은 같고 시리즈 추가만 한다 
	}

	QString StartTime = searchTime.at(0); 
	QString EndTime = searchTime.at(1); 

	QList<agLineItem*> selItem = ((agCanvasScene*)m_pCanvasWidget->scene())->selectedLineItems();	

	if( selItem.size() == 0 )
		return;	

	QList<int> selLines = makeLineIdList(selItem);
	QString strServer = getServerCode();

	DBFunction->setXCount(nXCount);
	DBFunction->SetChartLineData(selLines);	
	DBFunction->makeTimeChart(strServer, StartTime, EndTime, currentDateType);

	//QMap<int, QString> XData = DBFunction->GetChartTimeXaxis();						// 이미 생성된 시간이 있어서 사용되지 않는다
	QMap<int, QString> captions = ((agCanvasScene*)m_pCanvasWidget->scene())->selectedCaptions(selItem);
	QMap<int, ChartData> YData = DBFunction->GetChartSortLineData();	
	QMap<int, qreal> resultYdata = makePValueY(YData, nMaxPValue);	

	if( resultYdata.isEmpty() )
	{
		return;								// 값이 0일 경우 쿼리 결과가 없을 수 있다... 메시지를 띄우자.. 
	}
	
	ChartViewTable* chartSeriesTable = m_pChartList->getChartTable();
	chartSeriesTable->makeSeriesRow( captions, selItem );
	
	int idx = plot->getIndex();
	plot->setMaxPValue(nMaxPValue);	
	plot->populate(++idx, resultYdata);		
	plot->replot();
	plot->show();

	foreach(agLineItem* line, selItem)
	{
		line->setSelected(false);
	}

}

void TabView::onReTimeSettings()
{
	m_timeWidget = new SetupDatetime(this);	
	m_timeWidget->setWindowTitle("Edit Time");
	m_timeWidget->setWindowFlags(Qt::Tool);
	m_timeWidget->setWindowModality(Qt::WindowModal);

	QPoint point = QCursor::pos();
	QPoint codiPoint = mapFromGlobal(point);
	// 아래 공식으로 하면 모니터 화면을 벗어난다.... 이유는 모르겠다.....
	//m_timeWidget->move(codiPoint.x() -  (m_timeWidget->width()), codiPoint.y() - (m_timeWidget->height()));	
	m_timeWidget->move(QCursor::pos().x() -  (m_timeWidget->width()/2.5), QCursor::pos().y() - (m_timeWidget->height()/4));
	//qDebug("%d/%d ====== %d/%d", QCursor::pos().x() - (m_timeWidget->width()) , m_timeWidget->width() , QCursor::pos().y() - (m_timeWidget->height()) , m_timeWidget->height() );
	//qDebug("%d/%d ====== %d/%d", QCursor::pos().x() , (m_timeWidget->width()/2.5) , QCursor::pos().y() , (m_timeWidget->height()/4) );
	m_timeWidget->show();

	connect(m_timeWidget, SIGNAL(dateChartClicked()), SLOT(onDateClicked()));
}

void TabView::onDateClicked()
{
	m_selItem.clear();	

	DateType dType = m_timeWidget->getDateType();
	QString FromTime = m_timeWidget->searchFrom();
	QString ToTime = m_timeWidget->searchTo();
	QString StartTime; 
	QString EndTime; 
	QString SerchTime = getDlName();

	makeTimeString( FromTime,  ToTime,  StartTime,  EndTime,  SerchTime, dType);

	if( m_pCanvasWidget != NULL )
	{
		QList<agLineItem*> selItem = ((agCanvasScene*)m_pCanvasWidget->scene())->selectedLineItems();	

		if( selItem.size() == 0 )
			return;		

		QList<int> selLines = makeLineIdList(selItem);
		QString strServer = getServerCode();

		DBFunction->SetChartLineData(selLines);	
		DBFunction->makeTimeChart(strServer, StartTime, EndTime, dType);
		QMap<int, QString> XData = DBFunction->GetChartTimeXaxis();	
		QMap<int, QString> captions = ((agCanvasScene*)m_pCanvasWidget->scene())->selectedCaptions(selItem);	
		QMap<int, ChartData> YData = DBFunction->GetChartSortLineData();
		
		int nMaxPValue = 0;
		QMap<int, qreal> resultYdata = makePValueY(YData, nMaxPValue);	

		int nIndex = 1;
		Plot* plot = new Plot(nIndex, nMaxPValue, selItem, XData, resultYdata, StartTime, EndTime, dType, this);
		m_pChartList = new ChartView(this, plot, captions, selItem);
		m_pChartTab->addTab(m_pChartList , SerchTime );
		m_pChartTab->setCurrentWidget(m_pChartList);
	}	

	m_timeWidget->hide();
}