#ifndef QDBACCSSOR_H
#define QDBACCSSOR_H

#include <QObject>
#include "qdbaccssor_global.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QtSql>
#include <QSqlDatabase>
#include "agDBConnectParser.h"

#define		DAS				0
#define		DAS_LOG			1
#define		DAS_WAVE		2
#define		HEAD			3		// TDM º»»ç DB

class qDbStatusListener;
class qDBNetworkStatueThr;

class QDBACCSSOR_EXPORT qDBAccssor : public QObject
{
	Q_OBJECT

public:
	static	qDBAccssor*		Instance();		
	static	void			ExitInstance();	

public:
	void connectDB();

	bool getCommand(QString strQuery , QSqlQuery& query , QString &strErr/*, QString strServer , int nSlot*/);
	bool getDBInstatnce(int nSlot , QString strServer , QSqlDatabase &db);

	QString getServerName(int nIdx);
	QString getOfficeName(QString strOfficeCode);

	void	setSignalToSlot(qDbStatusListener* pStatus);

	int     GetServerCount();
	int     GetServerOfficeCount();

	
signals:
	void	sendDbConnectMsg(QString strMsg);
	void	sendDbDisConnectMsg(QString strMsg);

protected:
	qDBAccssor();
	~qDBAccssor();

private:
	static qDBAccssor*		m_pInstance;
	mapConnectInfo			m_connectInfo;
	qDBNetworkStatueThr		*m_pDbThr;


	QMutex					m_mutex;
	
};
//#define DBFunction qDBAccssor::Instance()
#define qDbAccssor qDBAccssor::Instance()
#endif // QDBACCSSOR_H
