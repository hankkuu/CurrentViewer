#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>

class Plot;
class ChartViewTable;
class agLineItem;
class ChartView : public QWidget
{
	Q_OBJECT

public:
	ChartView(QWidget *parent, Plot *plot );
	ChartView(QWidget *parent, Plot *plot, QMap<int, QString> XValue,  const QList<agLineItem*>& selLine, bool bMainLineChart = false );
	~ChartView();

	Plot* getChartItem();
	ChartViewTable* getChartTable();
	QLayout* makeChartViewLayout();

	bool mainLineChart() { return m_bMain; }

private:
	QWidget* m_parent;
	Plot* m_plot;
	int m_nIdx;
	ChartViewTable* m_chartSerise;
	QPushButton* m_btnDelChart;

	bool m_bMain;							// 간선 / 시간 차트를 나누는 놈

public slots:	
	void onReceiveIdx(QString idx, bool state);
	void onChartDelList(QList<QString>);
};

#endif // CHARTVIEW_H
