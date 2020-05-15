#include "stdafx.h"

#include "subdocument.h"
//#include <QMouseEvent>
//#include "RCTrendPlotMagnifier.h"				// 파워씬에서 가져온 건데 쓰지 지금은 안씀
//#include "ploteventfilter.h"					// 자동 마우스 툴팁 구현하고자 했음
#include "qwt_text_label.h"

Plot::Plot(int nIndex, QString FromTime, QString ToTime, DateType dType, QWidget *parent ) : QwtPlot( parent )
{
	setProperty("style", true);
	m_parent = parent;
	m_DateType = dType;
	m_nMaxPValue = 1;
	m_nIndex = nIndex;

	m_searchTime.push_back(FromTime);
	m_searchTime.push_back(ToTime);

	m_toolTip = NULL;
	//grabMouse();
// 	setMouseTracking(true);
// 	canvas()->setMouseTracking(true);
// 	PlotEventFilter* filter = new PlotEventFilter(this);
// 	connect(filter, SIGNAL(mouseMoved(QPoint&)), SLOT(repositionMarker(Point&)));
// 	//canvas()->installEventFilter(filter);
// 	installEventFilter(filter);

	setPalette( QPalette( QColor( 165, 193, 228 ) ) );

	QMap<int, qreal> YData;							// 값이 없으므로 널을 보낸다
	QString dDateFormat = makeDateString( FromTime,  ToTime,  dType );
	QStringList dXdate = makeXDateString( FromTime, YData);
	m_XData = dXdate;

	QwtText titleName = QString::fromLocal8Bit("시간") + dDateFormat;
	titleName.setColor(QColor(255,255,255 ));
	titleName.setFont(QFont("Malgun Gothic", 14));

	textLabel = titleLabel();
	textLabel->setProperty("style", true);
	textLabel->setText(titleName);

	QwtLegend *le = new QwtLegend();
	le->setStyleSheet("QWidget{"
		"background-color: rgb(240,240,240);     "
		"}"
		);
	insertLegend( le, QwtPlot::RightLegend );	
	QwtAbstractLegend *rightLegend = legend();
		
	// axes	
	setAxisTitle( xBottom, QString::fromLocal8Bit("시간") );

	setAxisScaleDraw( QwtPlot::xBottom,  new XScaleDraw( dXdate ) );

	setAxisTitle( yLeft, QString::fromLocal8Bit("전력") );
	setAxisScale( yLeft, 0.0, m_nMaxPValue );	

	QwtPlot::Axis axisX = QwtPlot::xBottom;
	QwtPlot::Axis axisY = QwtPlot::yLeft;

	// canvas
	QwtPlotCanvas *canvas = new QwtPlotCanvas();
	canvas->setLineWidth( 5 );
	canvas->setFrameStyle( QFrame::Panel | QFrame::Raised );
	canvas->setBorderRadius( 15 );

	QPalette canvasPalette( Qt::white );
	canvasPalette.setColor( QPalette::Foreground, QColor( 133, 190, 232 ) );
	canvas->setPalette( canvasPalette );
	setCanvas( canvas );
	
	populate(nIndex, YData );
}

Plot::Plot(int nIndex, int nMaxPValue, QMap<int, QString> XValue, QMap<int, qreal> YData, QWidget *parent ) : QwtPlot( parent )
{
	setProperty("style", true);
	m_parent = parent;
	m_XData = XValue.values();
	m_nIndex = nIndex;
	m_nMaxPValue = nMaxPValue;

	m_toolTip = NULL;
	//grabMouse();
// 	setMouseTracking(true);
// 	canvas()->setMouseTracking(true);
// 	PlotEventFilter* filter = new PlotEventFilter(this);
// 	connect(filter, SIGNAL(mouseMoved(QPoint&)), SLOT(repositionMarker(Point&)));
// 	//canvas()->installEventFilter(filter);
// 	installEventFilter(filter);

	setPalette( QPalette( QColor( 165, 193, 228 ) ) );

	int nSize = XValue.count();
	QString strStart = XValue.value(0);
	QString strEnd = XValue.value(--nSize);
	QString sectionName = strStart + " - " + strEnd;

	QwtText titleName = sectionName;
	titleName.setColor(QColor(255,255,255 ));
	titleName.setFont(QFont("Malgun Gothic", 14));
	textLabel = titleLabel();
	textLabel->setProperty("style", true);
	textLabel->setText(titleName);

	QwtLegend *le = new QwtLegend();
	le->setStyleSheet("QWidget{"
		"background-color: rgb(240,240,240);     "
		"}"
		);
	insertLegend( le, QwtPlot::RightLegend );	

	// axes	
	setAxisTitle( xBottom, QString::fromLocal8Bit("구간") );

	setAxisScaleDraw( QwtPlot::xBottom,  new XScaleDraw( XValue.values() ) );

	setAxisTitle( yLeft, QString::fromLocal8Bit("전력") );
	setAxisScale( yLeft, 0.0, nMaxPValue + (nMaxPValue/10) );

	QwtPlot::Axis axisX = QwtPlot::xBottom;
	QwtPlot::Axis axisY = QwtPlot::yLeft;

	// canvas
	QwtPlotCanvas *canvas = new QwtPlotCanvas();
	canvas->setLineWidth( 5 );
	canvas->setFrameStyle( QFrame::Panel | QFrame::Raised );
	canvas->setBorderRadius( 15 );

	QPalette canvasPalette( Qt::white );
	canvasPalette.setColor( QPalette::Foreground, QColor( 133, 190, 232 ) );
	canvas->setPalette( canvasPalette );
	setCanvas( canvas );

	populate(nIndex, YData, true);

}

Plot::Plot(int nIndex, int nMaxPValue, QList<agLineItem*> lines, QMap<int, QString> XValue, QMap<int, qreal> YData, QString FromTime, QString ToTime, DateType dType, QWidget *parent) :QwtPlot( parent )
{
	setProperty("style", true);
	m_parent = parent;
	m_lineItem = lines;
	m_DateType = dType;
	m_nIndex = nIndex;
	m_XValueMap = XValue; 
	m_nMaxPValue = nMaxPValue;

	m_searchTime.push_back(FromTime);
	m_searchTime.push_back(ToTime);

	m_toolTip = NULL;
	//grabMouse();
// 	setMouseTracking(true);
// 	canvas()->setMouseTracking(true);
// 	PlotEventFilter* filter = new PlotEventFilter(this);
// 	connect(filter, SIGNAL(mouseMoved(QPoint&)), SLOT(repositionMarker(Point&)));
// 	//canvas()->installEventFilter(filter);
// 	installEventFilter(filter);

	setPalette( QPalette( QColor( 165, 193, 228 ) ) );

	QString dDateFormat = makeDateString( FromTime,  ToTime,  dType );

	QwtText titleName = QString::fromLocal8Bit("시간") + dDateFormat;
	titleName.setColor(QColor(255,255,255 ));
	titleName.setFont(QFont("Malgun Gothic", 14));
	textLabel = titleLabel();
	textLabel->setProperty("style", true);
	textLabel->setText(titleName);

	QwtLegend *le = new QwtLegend();
	le->setStyleSheet("QWidget{"
		"background-color: rgb(240,240,240);     "
		"}"
		);
	insertLegend( le, QwtPlot::RightLegend );			// QwtLegend 내부 기능 있음

	// axes	
	setAxisTitle( xBottom, QString::fromLocal8Bit("시간") );

	if( dType < 4 )
	{
		int nIdx = 0;
		QList<QString> makeXAxis = XValue.values();
		QListIterator<QString> Xitr(makeXAxis);
		while ( Xitr.hasNext() )
		{
			QString sLabel = Xitr.next();
			QString sDateLabel;
			if( dType == MONTH )
			{
				sDateLabel = sLabel.mid(5,2);
			}
			else if( dType == DAY )
			{
				sDateLabel = sLabel.mid(8,2);
			}
			else if( dType == HOUR )
			{
				sDateLabel = sLabel.mid(11,2);
			}
			m_XData.insert(nIdx, sDateLabel);		
			nIdx++;
		}
	}	
		
	setAxisScaleDraw( QwtPlot::xBottom,  new XScaleDraw( m_XData ) );
	
	setAxisTitle( yLeft, QString::fromLocal8Bit("전력") );
	setAxisScale( yLeft, 0.0, nMaxPValue + (nMaxPValue/10) );	
	
 	QwtPlot::Axis axisX = QwtPlot::xBottom;
 	QwtPlot::Axis axisY = QwtPlot::yLeft;

	// canvas
	QwtPlotCanvas *canvas = new QwtPlotCanvas();
	canvas->setLineWidth( 5 );
	canvas->setFrameStyle( QFrame::Panel | QFrame::Raised );
	canvas->setBorderRadius( 15 );

	QPalette canvasPalette( Qt::white );
	canvasPalette.setColor( QPalette::Foreground, QColor( 133, 190, 232 ) );
	canvas->setPalette( canvasPalette );
	setCanvas( canvas );

	populate(nIndex, YData);
}

Plot::~Plot() 
{ 
	delete m_toolTip; 
	m_toolTip = NULL;
}

void Plot::populate(int nIndex, QMap<int, qreal> YData, bool bData )
{
	m_nIndex = nIndex;
	m_bData = bData;

	if( m_toolTip == NULL )
		m_toolTip = new QwtPlotMarker();

	QwtPlotCurve *PlotChart = new QwtPlotCurve( /*QString::number(nIndex)*/ );
	if( nIndex != 0)
	{
		PlotChart->setRenderHint( QwtPlotItem::RenderAntialiased );
		PlotChart->setLegendAttribute( QwtPlotCurve::LegendShowLine, true );
		PlotChart->setTitle(QString::number(nIndex) );
	}

	QColor randomColor;
	if( nIndex == 1)
		PlotChart->setPen( Qt::red );
	else if( nIndex == 0)
	{
		randomColor.setRgb(qRgb(240, 240, 240));
		PlotChart->setPen( randomColor );
	}
	else
	{
		randomColor.setRgb(qRgb(rand()% 256, rand()%256, rand()%256));
		PlotChart->setPen( randomColor );
	}

	PlotChart->attach( this );

	int nIdx = YData.count(); 

	FunctionData* makeYData = new FunctionData(nIdx/2, YData);

	QwtInterval iterval(0, nIdx);
	makeYData->setInterval(iterval);

	if ( bData == true )
		makeYData->setSize(nIdx * 5);
	else
		makeYData->setSize(nIdx );

	QwtPlot::Axis axisX = QwtPlot::xBottom;
	QwtPlot::Axis axisY = QwtPlot::yLeft;

	XScaleDraw * XscaleDraw = dynamic_cast<XScaleDraw* > (axisScaleDraw(axisX ));
	setAxisScale(axisX, 0, nIdx, 1 );

	PlotChart->setData( makeYData );
	setPlotMap(nIndex, PlotChart);	
}

void Plot::setPlotMap(int nIdx ,QwtPlotCurve *PlotChart)
{
	m_PlotMap.insert(nIdx, PlotChart);
}

void Plot::resizeEvent( QResizeEvent *event )
{
	QwtPlot::resizeEvent( event );
}

bool Plot::isContains(agLineItem* line )
{
	return m_lineItem.contains(line);
}

void Plot::setSelectedList()
{
	for(int i = 0; i < m_lineItem.count() ; ++i)
	{
		m_lineItem.at(i)->setSelected(true);		
	}
}

bool Plot::event(QEvent* _e)
{
	if(_e->type() == QEvent::MouseMove) 
	{
		//repositionMarker(((QMouseEvent*)_e)->pos());
	}
	return QwtPlot::event(_e);
}
void Plot::repositionMarker(const QPoint& _p)						// 중학교때 배운 두점 사이의 거리 공식 마우스 찍은 곳과 가까운 시리즈를 연결
{
	QwtPlotItemList curves = itemList(QwtPlotItem::Rtti_PlotCurve);
	int point_index;
	int i;

	if( curves.isEmpty() )
		return;

	if( m_toolTip == NULL)
		m_toolTip = new QwtPlotMarker();

	m_toolTip->show();

	QPoint canvasP = canvas()->mapFrom(this, _p);
	int nearestIdx = -1;
	qreal mouseDist = (qreal)(canvas()->rect().height() - canvasP.y()) / canvas()->rect().height() * (m_nMaxPValue * 1.1);
	qreal nearestDist = 99999999.99999;
	qreal currentDist;
	QPointF diffP;
	QPointF curveP;
	//qDebug("CURVE COUNT = %d", curves.count());
	for(i = 0; i < curves.count(); i++) 
	{
		point_index = static_cast<QwtPlotCurve*>(curves[i])->closestPoint(canvasP);

		if( !curves[i]->isVisible() )
			continue;
	
		curveP = static_cast<QwtPlotCurve*>(curves[i])->sample(point_index);
		currentDist = qAbs(mouseDist - curveP.y());
		//qDebug("IDX[%d], curveP(%f) - mouseP(%f), dist(%f), {%d = '%s'}", i, curveP.y(),  mouseDist, currentDist, point_index, qPrintable(m_XValueMap.value(point_index)));
		//diffP = curveP - _p;
		//currentDist = qSqrt(diffP.x() * diffP.x() + diffP.y() * diffP.y());
		//qDebug("IDX[%d], curveP(%f %f) - mouseP(%d, %d), dist(%f)", i, curveP.x(), curveP.y(), _p.x(), _p.y(), currentDist);

		if(currentDist < nearestDist) 
		{
			nearestIdx = i;
			nearestDist = currentDist;
		}
	}
	//const QwtPlotItemList& itmList = this->itemList();
	// works only for one curve, otherwise iterate over them and compare
	//for( QwtPlotItemIterator it = itmList.begin() ; it != itmList.end() ; ++it )
	if(nearestIdx != -1)
	{
		point_index = static_cast<QwtPlotCurve*>(curves[nearestIdx])->closestPoint(canvasP);

		if(point_index >= 0)
		{
			QPointF p = static_cast<QwtPlotCurve*>(curves[nearestIdx])->sample(point_index);
			int xx = p.x();
			int yy = p.y();

			m_toolTip->setRenderHint( QwtPlotItem::RenderAntialiased, true );
			//m_toolTip->setItemAttribute( QwtPlotItem::Legend, true );
			m_toolTip->setSymbol( new ArrowSymbol() );
			//m_toolTip->setLabel("X: "+QString::number(p.x())+" Y: "+QString::number(p.y()));
			if( m_bData == true)
				m_toolTip->setLabel( QString::fromLocal8Bit("구간: ") + m_XData[xx] + "\n"   +  " KVA: " + QString::number(p.y()));
			else
				m_toolTip->setLabel( QString::fromLocal8Bit("시간: ") + m_XData[xx] + QString::fromLocal8Bit("시") + "\n" + " KVA: " + QString::number(p.y()));
			m_toolTip->setXValue(p.x());
			m_toolTip->setYValue(p.y());
			if( xx >= 23)
				m_toolTip->setLabelAlignment( /*Qt::AlignRight |*/ Qt::AlignLeft );
			else if( yy < 10 )
				m_toolTip->setLabelAlignment( /*Qt::AlignRight |*/ Qt::AlignVertical_Mask );
			else
				m_toolTip->setLabelAlignment( /*Qt::AlignRight |*/ Qt::AlignRight );
			m_toolTip->attach(this);
			replot();				
		}
	}
}

void Plot::detachMarker()
{
	m_toolTip->hide();
}

void Plot::deleteMarker()
{
	delete m_toolTip;
	m_toolTip = NULL;
}

void Plot::enterEvent( QEvent * ev )
{
	QwtPlot::enterEvent(ev);
	//grabMouse();
}

void Plot::leaveEvent( QEvent * ev )
{
	//releaseMouse();
	QwtPlot::leaveEvent(ev);
}

void Plot::mousePressEvent(QMouseEvent* ev)
{
	if(ev->buttons() & Qt::LeftButton)
	{
// 		QwtPlotItemList curves = itemList(QwtPlotItem::Rtti_PlotCurve);
// 		point_index = static_cast<QwtPlotCurve*>(*it)->closestPoint(QPoint(ev->x(), ev->y()));
		return;
	}
	QwtPlot::mousePressEvent(ev);

}
void Plot::mouseReleaseEvent(QMouseEvent* ev)
{
	if(ev->button() == Qt::LeftButton)
		repositionMarker(ev->pos());
	QwtPlot::mouseReleaseEvent(ev);
}
void Plot::mouseMoveEvent(QMouseEvent *ev)
{
	// get all curves
	QwtPlot::mouseMoveEvent(ev);
	//grabMouse();
	//if(ev->buttons() & Qt::LeftButton)
	{
		//do stuff
		//repositionMarker(ev->pos());
	}
}

QString Plot::makeDateString(QString FromTime, QString ToTime, DateType dType )
{
	QString StartTime; 
	QString EndTime; 
	QString dDateFormat;

	if( dType == MONTH )
	{
		QString sStartHour = FromTime.mid(0,7);
		QString sEndHour = ToTime.mid(0,7);
		dDateFormat = " - " + sStartHour + QString::fromLocal8Bit("월") + " ~ " + sEndHour + QString::fromLocal8Bit("월");
		return dDateFormat;
	}
	else if( dType == DAY )
	{
		QString sStartHour = FromTime.mid(0,10);
		QString sEndHour = ToTime.mid(0,10);
		dDateFormat = " - " + sStartHour + QString::fromLocal8Bit("일") + " ~ " + sEndHour + QString::fromLocal8Bit("일");
		return dDateFormat;
	}
	else if( dType == HOUR )
	{
		QString sStartHour = FromTime.mid(0,13);
		QString sEndHour = ToTime.mid(0,13);
		dDateFormat = " - " + sStartHour + QString::fromLocal8Bit("시") + " ~ " + sEndHour + QString::fromLocal8Bit("시");
		return dDateFormat;
	}

	return dDateFormat;
}

QStringList Plot::makeXDateString(QString FromTime, QMap<int, qreal>& YData )
{
	QStringList defaultDateData;
	QString sStartHour = FromTime.mid(11, 2);
	int nHour = sStartHour.toInt();

	for( int i = 0 ; i < 24 ; i++)
	{
		if( nHour > 23 )
			nHour = 0;
		
		YData.insert(i, 0.0);
		defaultDateData.insert(i, QString::number(nHour));
		nHour++;
	}

	return defaultDateData;
}

void Plot::setMaxPValue(int nMaxPValue )
{
	m_nMaxPValue = nMaxPValue;

	setAxisTitle( yLeft, QString::fromLocal8Bit("전력") );
	setAxisScale( yLeft, 0.0, nMaxPValue + (nMaxPValue/10) );	
}

void Plot::delChatList(int delIdx)
{	
	QwtPlotCurve* delChart = m_PlotMap.value(delIdx);
	m_PlotMap.remove(delIdx);

	if( delChart != 0)
	{		
		delChart->detach();
		delete delChart;
		delChart = NULL;

		if( m_toolTip != NULL )
		{
			delete m_toolTip;
			m_toolTip = NULL;
		}	
	}
}