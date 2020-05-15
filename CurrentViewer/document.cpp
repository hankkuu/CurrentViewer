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
	bool bSuccess = DBFunction->CreateOLD(ServerCode, nCBId);	// �� �並 ���� �� �� XML data�� �����´� // �����ڿ��� �����͸� �������°��� �̻��ϴٸ� ����
	if( bSuccess )
	{
		strXMLString = DBFunction->GetXMLDLInfo();
		setXMLData(strXMLString); 
	}
	else
	{
		m_strXMLString = "";
		// ȭ�� ���� ���� �α� ����
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
	//�ܼ��� ȭ�� ����		
	m_pCanvasWidget = new agCanvasWidget(this , NULL , 0);	
	m_pCanvasWidget->setProperty("style", true);
	//�˻���ư ����
	m_pControlWidget = new SearchWidget(this);		
	//��Ʈ�� ����
	m_pChartTab = new QTabWidget(this);
	m_pChartTab->setTabsClosable(true);	

	// ���̾ƿ� ����
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

	connect(m_pCanvasWidget, SIGNAL(printChart()), SLOT(onPrintChart()));						// ���� ���� ��Ʈ ���
	connect(m_pCanvasWidget, SIGNAL(printCurrentDayChart()), SLOT(onPrintCurrentDayChart()));	// ��ü ���� ��Ʈ ���
	connect(m_pCanvasWidget, SIGNAL(printMainLineChart()),  SLOT(onPrintMainLineChart()));		// ���� ���� ��Ʈ ���
	connect(m_pCanvasWidget, SIGNAL(reTimeSettings()),  SLOT(onReTimeSettings()));				// �˻� �ð� ���� ���� Ȱ��ȭ

	connect( m_pChartTab , SIGNAL(tabCloseRequested(int)) , this , SLOT(closeTab(int)));
	connect( m_pChartTab , SIGNAL(currentChanged(int)) , this , SLOT(CurrentChartTab(int)));

	connect(m_pControlWidget, SIGNAL(newChartClicked()), SLOT(onNewChartClicked()));			// ���� ���

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
	//plot->setSelectedList();									// �ø��� ���ÿ� ���� ���� ���� ǥ�� �ٲٸ鼭 �� ��ȯ���� �ϴ� ����
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
		m_pCanvasWidget->makeMainLine( strServer, nCBId );							// ������ ���ο� �� ����... �̵������� ����� ����������.. �Ӹ��� �ȱ�������..

		agCanvasScene* agScene = (agCanvasScene*)m_pCanvasWidget->scene();
		agScene->setTabView(this);													// ���� ����� ���� ������ ����
		agScene->mainLastLine();													// ������ ���õ� ������ ����
	}	
	
	// �������� ��Ʈ���� ����   Context Menu�� �ϳ��� ����	
	// �ش� DL�� 24�ð� data�� �����´� -- �����Լ��� ��ü������ ���� �Ű� ����Ʈ ��Ʈ�� CB �����͸������´�.
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
	// ��ü�� ���� Ŭ���ϸ� �������� �ִ� �����ؾ� �� ���� �ִ�  // �ϴ� ��� ��Ⱑ ������ ��Ȱ��ȭ
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
		QList<agLineItem*> MainLineItem = agScene->getMainLineItem();		// ������ ���� ������ LineItem�� get
		//��Ʈ ������ ���
		m_pCanvasWidget->makeChartXData(MainLineItem);		
		QMap<int, QString> XValue = m_pCanvasWidget->getChartXData();		// ���� ���� ��Ʈ ����Ʈ ������ �Ѱ� �ش�

		if( XValue.isEmpty() )
			XValue.insert(nMaxPValue, strDLName);							// CB�� �ִ� ��� DL �̸��� ��Ʈ ����Ʈ ������ �Ѱ� �ش�

		QList<agLineItem*> selAllLineItem = ((agCanvasScene*)m_pCanvasWidget->scene())->lineItems();   // ��ü�� �������� ���� ��� ���� ���� ��

		// 		if( selAllLineItem.size() == 0 )
		// 			return;	

		DBFunction->makeCBTimeChart(strServer, nCBId, StartTime, EndTime, HOUR);				// 24�ð� ���� ��Ʈ ���
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
		//��Ʈ ������ ���
		m_pCanvasWidget->makeChartXData(MainLineItem);
		QMap<int, QString> XValue = m_pCanvasWidget->getChartXData();						// ���� ���� ��Ʈ ���̺� ������ �Ѱ� �ش�

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
		m_pChartTab->addTab(m_pChartList , m_strDLName + QString::fromLocal8Bit("(��ü����)") );		
		m_pChartTab->setCurrentWidget(m_pChartList);
	}	
}

void TabView::onPrintMainLineChart()
{
	//���� ���� ȹ��
	agCanvasScene* agScene = (agCanvasScene*)m_pCanvasWidget->scene();	
	QList<agLineItem*> MainLineItem = agScene->getMainLineItem();
	//��Ʈ ������ ���
	m_pCanvasWidget->makeChartXData(MainLineItem);
	m_pCanvasWidget->makeChartYData(MainLineItem);								// ���� �ϴ� ��� ����.... 

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

	QList<agLineItem*> selAllLineItem = agScene->lineItems();   // ��ü�� �������� ���� ��� ���� ���� ��

	// ��Ʈ + �ø��� ���� ǥ Qwidget ����	
	//��Ʈ ����
	int nIndex = 1;
	Plot* plot = new Plot(nIndex, nMaxPValue, XValue, YData, this);
	ChartView* ChatView = new ChartView( this, plot, XValue, selAllLineItem, true);	// ����� �� �ʿ� ����//  �����͸� �Ѱ� �شٴ� ���� ���⿡ �����Ѵٴ� ��	
	m_pChartTab->addTab(ChatView , m_strDLName + QString::fromLocal8Bit("(��������)"));	
	m_pChartTab->setCurrentWidget(ChatView);
}

void TabView::sumSectionValue()
{
	if( m_pCanvasWidget != NULL )
	{
		m_selItem = ((agCanvasScene*)m_pCanvasWidget->scene())->selectedLineItems();	  // m_pCanvasWidget ������ �Ķ� new�� ���ص� �����Ͱ� ����

		////  ���õ� ���ΰ� ������ ���� �� �� �ִ� ���Ͼ��ϴ� ������ 0���� �� ü����
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

		//// ������Ʈ�� �ʼ��� �ƴ϶� ����� ����	
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
		SerchTime += QString::fromLocal8Bit("(�����˻�)");
		StartTime = sStartYear + '-' + sStartMonth + '-' + "01" + ' ' + "00:00:00";
		// �Ŵ� ������ �ٸ��� ������ ������ ������ �޶�����.... �ù�... �ϴ� ������� �ϴ� ���� ����� ���߿�
		if( sEndMonth == "01" || sEndMonth == "03" || sEndMonth == "05" || sEndMonth == "07" || sEndMonth == "08" || sEndMonth == "10" || sEndMonth == "12" )
			EndTime = sEndYear + '-' + sEndMonth + '-' + "31" + ' ' + "23:59:59";
		else if( sEndMonth == "04" || sEndMonth == "06" || sEndMonth == "09" || sEndMonth == "11" )
			EndTime = sEndYear + '-' + sEndMonth + '-' + "30" + ' ' + "23:59:59";
		else if( sEndMonth == "02" )
			EndTime = sEndYear + '-' + sEndMonth + '-' + "28" + ' ' + "23:59:59";
	}
	else if( dType == DAY  )
	{
		SerchTime += QString::fromLocal8Bit("(�ϰ��˻�)");
		StartTime = sStartYear + '-' + sStartMonth + '-' + sStartDay + ' ' + "00:00:00";
		EndTime = sStartYear + '-' + sEndMonth + '-' + sEndDay + ' ' + "23:59:59";
	}
	else if( dType == HOUR  )
	{
		SerchTime += QString::fromLocal8Bit("(�ð��˻�)");
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

// ���� ����
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

// ������ ���� ��Ʈ ���
void TabView::onPrintChart()						
{
	m_selItem.clear();	
	m_pChartList = dynamic_cast<ChartView*>(m_pChartTab->currentWidget());
	
	if( m_pChartList->mainLineChart() == true )
	{
		QMessageBox msg;
		msg.setText( QString::fromLocal8Bit("���� �������� ��Ʈ�� X���� �ð������� �ƴմϴ�. ��Ʈ�� �ٲ� �ּ���."));
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
		return;						// ���� ���� ��Ʈ�� X���� ���� �ø��� �߰��� �Ѵ� 
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

	//QMap<int, QString> XData = DBFunction->GetChartTimeXaxis();						// �̹� ������ �ð��� �־ ������ �ʴ´�
	QMap<int, QString> captions = ((agCanvasScene*)m_pCanvasWidget->scene())->selectedCaptions(selItem);
	QMap<int, ChartData> YData = DBFunction->GetChartSortLineData();	
	QMap<int, qreal> resultYdata = makePValueY(YData, nMaxPValue);	

	if( resultYdata.isEmpty() )
	{
		return;								// ���� 0�� ��� ���� ����� ���� �� �ִ�... �޽����� �����.. 
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
	// �Ʒ� �������� �ϸ� ����� ȭ���� �����.... ������ �𸣰ڴ�.....
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