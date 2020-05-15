#include "StdAfx.h"
#include "chartview.h"
#include "subdocument.h"
#include "chartviewtable.h"

//#include <QListWidgetItem>
//#include <QListWidget>
//#include <QLabel>
//#include <QPushButton>
//#include <QBoxLayout>
//#include <QVBoxLayout>

ChartView::ChartView(QWidget *parent, Plot *plot ) : QWidget(parent)
{
	m_parent = parent;
	m_plot = plot;
	m_nIdx = m_plot->getIndex();
	ChartViewTable* chartSeriesTable = new ChartViewTable(m_nIdx, this);
	m_chartSerise = chartSeriesTable;

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QLayout* layout = makeChartViewLayout();
	setLayout(layout);

}

ChartView::ChartView(QWidget *parent,  Plot *plot, QMap<int, QString> XValue,  const QList<agLineItem*>& selLine, bool bMainLineChart ) : QWidget(parent)
{
	m_bMain = bMainLineChart;
	m_parent = parent;
	m_plot = plot;
	m_nIdx = m_plot->getIndex();
	ChartViewTable* chartSeriesTable = new ChartViewTable(m_nIdx, XValue,  selLine, this);
	m_chartSerise = chartSeriesTable;

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QLayout* layout = makeChartViewLayout();
	setLayout(layout);
}

ChartView::~ChartView()
{

}

QLayout* ChartView::makeChartViewLayout()
{
	QWidget* chartTable = new QWidget();
	QSplitter* subSplitter = new QSplitter(Qt::Horizontal);	
	subSplitter->addWidget(m_plot);
	subSplitter->addWidget(chartTable);
	m_plot->setMinimumWidth(1000);
	QHBoxLayout* subChartLayout = new QHBoxLayout();
	subChartLayout->addWidget(subSplitter);
	subSplitter->setSizes(QList<int>() << 100 << 200);			// 스플리터가 싸이즈를 관리
	
	m_btnDelChart = new QPushButton( QString::fromLocal8Bit("삭 제"));
	m_btnDelChart->setObjectName("btnDelListItem");
	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->addWidget(m_chartSerise);
	vLayout->addWidget(m_btnDelChart);
	chartTable->setLayout(vLayout);

	connect(m_btnDelChart, SIGNAL(clicked()), m_chartSerise, SLOT(onReceiveDelItem()));

	return subChartLayout;
}

Plot* ChartView::getChartItem()
{
	return m_plot;
}

ChartViewTable* ChartView::getChartTable()
{
	return m_chartSerise;
}

void ChartView::onReceiveIdx(QString idx, bool state)
{
	QString i = idx;
	int nIdx = i.toInt();
	QMap<int, QwtPlotCurve*> delList = m_plot->getPlotMap();

	QwtPlotCurve* plotCurve = delList.value(nIdx);

	if( state == true)
		plotCurve->show();
	else
		plotCurve->hide();

	m_plot->replot();
	m_plot->detachMarker();
	m_plot->show();
}

void ChartView::onChartDelList(QList<QString> delList)
{
	for(int i = delList.count() - 1 ; i >= 0 ; i-- )
	{
		m_plot->delChatList(delList.at(i).toInt());
	}	

	m_plot->replot();
	m_plot->show();
}
