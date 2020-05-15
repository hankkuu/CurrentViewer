#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QWidget>
#include <QGridLayout>

#include "subdocument.h"

class agCanvasWidget;				// agOLD
class SearchWidget;					// Control	
class Plot;							// PlotChart -> qwt
class ChartView;					// Chart Control
class SetupDatetime;				// Control
class TabView : public QWidget
{
    Q_OBJECT

public:
	TabView(QWidget *parent = 0, QString ServerCode = NULL, QString strDLName = NULL, int nCBId = 0);
	~TabView();

	void bulidLayout();
	QString getServerCode() { return m_ServerCode; }
	QString getDlName() { return m_strDLName; }
	int getCBId() { return m_nCBId; }
	void setXMLData(QString strXMLData) { m_strXMLString = strXMLData; }
	QString getXMLData() { return m_strXMLString; }
	void SetNetwork(agCanvasWidget* data);
	agCanvasWidget* GetNetwork();

	void sumSectionValue();															// ���� �����ؼ� ���� PValue �� ��� 
	void makeTimeString(QString, QString, QString&, QString&, QString&, DateType ); // ��Ʈ�� �ð� �� ����
	QList<int> makeLineIdList(QList<agLineItem*> lines);							// ���õǾ� �ִ� ����Item���� ������ ���� ID ����
	QMap<int, qreal> makePValueY( QMap<int, ChartData> YData, int& nMaxPValue );	// �ִ� ���� ���� ���� ���� ��Ʈ�� Y�࿡ ���

	void createTapView(QString strServer, int nCBId, QString strDLName);			// ���� �� ����
	void printCBDayChart(QString strServer, int nCBId, QString strDLName);			// CB 24�ð� ���� ��Ʈ ���

public slots:
	void StopObject();						// ��ü ������ ���� ���� �Լ�			

	void closeTab(int idx);					// ��Ʈ �� ����
	void CurrentChartTab(int idx);			// ���� ���� ��Ʈ �� ���� ���ϸ� ���õ� ���θ� ��� ���� ��Ų��

	void onNewChartClicked();				// ���� �����ϴ� ���� �Լ�		
	void onDateClicked();					// �ð� �����ؼ� ��Ʈ ��� ���� �Լ�
	
	void onPrintChart();					// ���ؽ�Ʈ �޴����� ���� ������ ���� �͸� ��Ʈ ���
	void onPrintCurrentDayChart();			// ��ü ���� ��Ʈ ��� ���� - ��ü ���� ���� 
	void onPrintMainLineChart();			// ���� ���� ��Ʈ ���
	void onReTimeSettings();				// �ð� �缳�� �˾� ȭ�� ����

signals:

protected:
	
private:
	QString m_ServerCode;
	QString m_strDLName;
	int m_nCBId;

	QList<agLineItem*> m_selItem;			// ���� ���õǾ� �ִ� ����
	QString m_strXMLString;
	
	agCanvasWidget* m_pCanvasWidget;
	agCanvasWidget* m_Canvas;				// ��������� �ϴ� ���д� ���� ��� ����.... ���߿�.. �������� ĵ���� �̷� ���� �ұ�??? 
	SearchWidget* m_pControlWidget;
	SetupDatetime* m_timeWidget;
	QTabWidget* m_pChartTab;	
	ChartView* m_pChartList;
	Plot *m_plot;
	QTableWidget *m_chartSeriesTable;

	// ���� ��� ����
	sp_DlInfoObj m_Dlinobj;
};

#endif // DOCUMENT_H
