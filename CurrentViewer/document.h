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

	void sumSectionValue();															// 구간 선택해서 더한 PValue 값 출력 
	void makeTimeString(QString, QString, QString&, QString&, QString&, DateType ); // 차트의 시간 값 생성
	QList<int> makeLineIdList(QList<agLineItem*> lines);							// 선택되어 있는 선로Item에서 쿼리할 선로 ID 추출
	QMap<int, qreal> makePValueY( QMap<int, ChartData> YData, int& nMaxPValue );	// 최대 구간 부하 값을 추출 차트의 Y축에 사용

	void createTapView(QString strServer, int nCBId, QString strDLName);			// 우측 탭 생성
	void printCBDayChart(QString strServer, int nCBId, QString strDLName);			// CB 24시간 기준 차트 출력

public slots:
	void StopObject();						// 개체 깜박임 없앨 슬롯 함수			

	void closeTab(int idx);					// 차트 탭 삭제
	void CurrentChartTab(int idx);			// 현재 선태 차트 탭 탭이 변하면 선택된 선로를 모두 해제 시킨다

	void onNewChartClicked();				// 새탭 생성하는 슬롯 함수		
	void onDateClicked();					// 시간 변경해서 차트 출력 슬롯 함수
	
	void onPrintChart();					// 컨텍스트 메뉴에서 선택 구간에 대한 것만 차트 출력
	void onPrintCurrentDayChart();			// 전체 구간 차트 출력 기준 - 전체 선로 선택 
	void onPrintMainLineChart();			// 간선 구간 차트 출력
	void onReTimeSettings();				// 시간 재설정 팝업 화면 생성

signals:

protected:
	
private:
	QString m_ServerCode;
	QString m_strDLName;
	int m_nCBId;

	QList<agLineItem*> m_selItem;			// 현재 선택되어 있는 선로
	QString m_strXMLString;
	
	agCanvasWidget* m_pCanvasWidget;
	agCanvasWidget* m_Canvas;				// 백업용으로 일단 놔둔다 현재 사용 안함.... 나중에.. 여러개의 캔버스 이력 저장 할까??? 
	SearchWidget* m_pControlWidget;
	SetupDatetime* m_timeWidget;
	QTabWidget* m_pChartTab;	
	ChartView* m_pChartList;
	Plot *m_plot;
	QTableWidget *m_chartSeriesTable;

	// 현재 사용 안함
	sp_DlInfoObj m_Dlinobj;
};

#endif // DOCUMENT_H
