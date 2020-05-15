#ifndef AGDBCONNECTPARSER_H
#define AGDBCONNECTPARSER_H

#define		DAS				0
#define		DAS_LOG			1
#define		DAS_WAVE		2
#define		HEAD			3		// TDM 본사 DB

#include <QObject>

class agDBConnectInfo 
{
public :
	agDBConnectInfo();
	~agDBConnectInfo();

	QString GetServerName() { return m_strServer; }
	QString GetOfficeName() { return m_strOfficeName; }
	QString GetUserName() { return m_strUserName; }
	QString GetUserPassWord() { return m_strPassword; }
	QString GetDBName() { return m_strDBName; }
	QString GetProvider() { return m_strProvider; }
	QString GetDBSource() { return m_strDBSource; }
	QString GetDBType() { return m_strDbType; }
	QString GetFailOver() { return m_strFailOver; }

public:
	QString			m_strServer;
	QString			m_strOfficeName;
	QString			m_strUserName;
	QString			m_strPassword;
	QString			m_strDBName;
	QString			m_strProvider;
	QString			m_strDBSource;
	QString			m_strDbType;
	QString			m_strFailOver;
};

typedef QMap<int , agDBConnectInfo>		mapSlotType;			// 슬롯 타입, 
typedef QMap<QString , mapSlotType>			mapConnectInfo;

class agDBConnectParser : public QObject
{
	Q_OBJECT

public:
	agDBConnectParser(QObject *parent=0);
	~agDBConnectParser();

	void	ReadDbInfo(mapConnectInfo &info);

private:
	void	ReadDBConfgifure(QXmlStreamReader &reader , mapConnectInfo &info);
	void	ReadArea(QXmlStreamReader &reader , mapConnectInfo &info);
	void	ReadSlot(QXmlStreamReader &reader , QString strServer , QString strOfficeName , mapSlotType& slot);
	void 	ReadProvider(QXmlStreamReader &reader , agDBConnectInfo& connect);
	void 	ReadDataSource(QXmlStreamReader &reader , agDBConnectInfo& connect);
	void 	ReadDbName(QXmlStreamReader &reader , agDBConnectInfo& connect);
	void 	ReadUserId(QXmlStreamReader &reader , agDBConnectInfo& connect);
	void 	ReadUserPassword(QXmlStreamReader &reader , agDBConnectInfo& connect);
	void 	ReaderFailOver(QXmlStreamReader &reader , agDBConnectInfo& connect);

	
};

#endif // AGDBCONNECTPARSER_H
