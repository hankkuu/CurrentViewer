#ifndef AGDBFUNCTION_H
#define AGDBFUNCTION_H

#include <QObject>

typedef struct 
{
	int nCBId; 
	QString strDLName;
} DLInfo; 
typedef QVector<DLInfo> TypeDLName;
typedef QMap<QString, TypeDLName*>	TypeSubStationMap;	
typedef QMap<QString, TypeSubStationMap>	TypeServerMap;

typedef QMap<int, QString> TypeDLXmlData;
typedef QMap<QString, TypeDLXmlData> TypeServerDLInfoMap; 

typedef struct
{
	int nSecId; 
	float fAvg_Current_a;
	float fAvg_Current_b;
	float fAvg_Current_c;
	QString sDatetime;
} ChartData; 
typedef QMap<int, ChartData>	TypeChartDataMap;	

typedef enum{ HOUR = 0 , DAY , MONTH} DateType;

class agDBFunction : public QObject
{
	Q_OBJECT	

protected:
	agDBFunction(QObject *parent=0);
	~agDBFunction();

public:		
	static	agDBFunction*	Instance();		
	static	void			ExitInstance();	

	bool setEnbox();									// Enbox 설정
	void setEnboxData( int nEnboxNum );					// Enbox 저장
	QVector<int> getEnboxData();						// Enbox Number 추출
	bool LoadDataSubsAndCbName();						// DL 목록 생성
	TypeServerMap GetServerMap();						// DL 목록 추출
	void cleanServerData();								// DL 목록 삭제

	void setOLDCreateTime(QString strAlarmTime );		// 단선도 화면생성 시간 설정
	QString getOLDCreateTime( );						// 단선도 화면생성 시간 추출
	void SetXMLDLInfo(QString strOffice, int CBId, std::wstring strDLInfo);		// XML Data 저장
	QString GetXMLDLInfo();														// XML Data 추출
	bool CreateOLD(QString strOffice, int CBId);		// 단선도 화면 생성

	void SetChartLineData(QList<int> LineIdList);		// 쿼리할 라인ID 설정
	QList<int> GetChartLineData();						// 쿼리할 라인ID 추출
	void SetChartSortLineData(TypeChartDataMap data);	// 쿼리 결과 Data 설정
	TypeChartDataMap GetChartSortLineData();			// 쿼리 결과 Data 추출
	void setNoQueryLineList(QList<int> LineIdList);		// 쿼리 결과에 안나오는 선로 ID 저장
	QList<int> getNoQueryLineList();					// 쿼리 결과에 안나오는 선로 ID 추출
	
	bool CreateTotalCurrent(QString strOffice, int nCBId);
	
	bool makeCBTimeChart(QString strOffice, int nCBId, QString fromDate, QString toDate, DateType dType);
	QString makeCBDBTableQuery(QString fromDate, QString toDate, DateType dType);
	
	void SetChartTimeXaxis(QMap<int, QString> sChartTime);
	QMap<int, QString>  GetChartTimeXaxis();

	bool makeTimeChart(QString strOffice, QString fromDate, QString toDate, DateType dType);

	QString makeSecQuery();
	QString makeDBTableQuery(QString fromDate, QString toDate, DateType Type);
	QString getCurrentTime();
	void setXCount(QMap<int, QString> nXCount);
	QMap<int, QString> getXCount(); // { return m_nXCount; }

private:
	static agDBFunction*	m_pInstance;

	QVector<int>				m_enboxData;
	TypeServerMap				m_ServerMap;
	QString						m_strOLDCreateTime;
	QString						m_XMLDLInfo;
	//std::wstring				m_XMLDLInfo;
	QList<int>					m_LineIdList;
	TypeChartDataMap			m_ChartMap;
	QList<int>					m_lstNoQueryLindID;	
	QMap<int, QString>			m_ChartTime;
	QMap<int, QString>			m_nXCount;

};

#define DBFunction agDBFunction::Instance()

#endif // AGDBFUNCTION_H
