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

	bool setEnbox();									// Enbox ����
	void setEnboxData( int nEnboxNum );					// Enbox ����
	QVector<int> getEnboxData();						// Enbox Number ����
	bool LoadDataSubsAndCbName();						// DL ��� ����
	TypeServerMap GetServerMap();						// DL ��� ����
	void cleanServerData();								// DL ��� ����

	void setOLDCreateTime(QString strAlarmTime );		// �ܼ��� ȭ����� �ð� ����
	QString getOLDCreateTime( );						// �ܼ��� ȭ����� �ð� ����
	void SetXMLDLInfo(QString strOffice, int CBId, std::wstring strDLInfo);		// XML Data ����
	QString GetXMLDLInfo();														// XML Data ����
	bool CreateOLD(QString strOffice, int CBId);		// �ܼ��� ȭ�� ����

	void SetChartLineData(QList<int> LineIdList);		// ������ ����ID ����
	QList<int> GetChartLineData();						// ������ ����ID ����
	void SetChartSortLineData(TypeChartDataMap data);	// ���� ��� Data ����
	TypeChartDataMap GetChartSortLineData();			// ���� ��� Data ����
	void setNoQueryLineList(QList<int> LineIdList);		// ���� ����� �ȳ����� ���� ID ����
	QList<int> getNoQueryLineList();					// ���� ����� �ȳ����� ���� ID ����
	
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
