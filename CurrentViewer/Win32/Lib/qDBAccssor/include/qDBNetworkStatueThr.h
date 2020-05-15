#ifndef QDBNETWORKSTATUETHR_H
#define QDBNETWORKSTATUETHR_H

#include <QThread>
#include "agDBConnectParser.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QtSql>
#include <QSqlDatabase>

#define THR_TIMER		2000;

typedef QMap<int , bool>				mapSlotConnect;
typedef QMap<int , mapSlotConnect>		mapOfficeConnect;

class qDbStatusListener;

class qDBNetworkStatueThr : public QThread
{
	Q_OBJECT

public:
	qDBNetworkStatueThr(QObject *parent = NULL);
	~qDBNetworkStatueThr();

	void		shutdown();
	void		setDBInfo(mapConnectInfo &info);
	void		setSignalToSlot(qDbStatusListener* pStatus);

	void		setDbConnectInfo(int nOffice , int nSlot , bool bIsConnect , bool bIsFirst);
	void		sendMsg(bool bIsFirstMsg , int nOffice , int nSlot , bool bIsConnect);

	void		reConnectDbInfo(agDBConnectInfo info);

signals:
	void		sendDbConnectMsg(QString strMsg);
	void		sendDbDisConnectMsg(QString strMsg);

protected:
	virtual void run();
private:
	bool				m_bSutdown;

	mapConnectInfo		m_info;

	mapOfficeConnect	m_mapConInfo;
};

#endif // QDBNETWORKSTATUETHR_H
