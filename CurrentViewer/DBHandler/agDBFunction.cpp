#include "StdAfx.h"

#include "agDBFunction.h"

#include <qhostinfo.h>
#include <QNetworkInterface.h> 
#include <QhostInfo>
#include "Cgzip.h"
#include "DBHandler/qDBAccssor.h"

agDBFunction* agDBFunction::m_pInstance = 0;

agDBFunction*	agDBFunction::Instance()
{
	if ( m_pInstance == 0 )
	{
		m_pInstance = new agDBFunction;
		std::atexit(&ExitInstance);
	}
	return m_pInstance;
}

void	agDBFunction::ExitInstance()
{
	if ( m_pInstance )
	{
		delete m_pInstance;
		m_pInstance = 0;
	}
}

agDBFunction::agDBFunction(QObject *parent) : QObject(parent)
{
	qDbAccssor->connectDB();
}

agDBFunction::~agDBFunction()
{

}

bool agDBFunction::setEnbox()
{
	bool bRet = false;
	QString strMyComName = QHostInfo::localHostName();

	if( strMyComName.isNull() )
		return bRet;

	int nCount = 0;
	nCount = qDbAccssor->GetServerCount();	
	
	QString officeCode = qDbAccssor->getServerName(0);
	QString officeName = qDbAccssor->getOfficeName(officeCode);

	QString strQuery;
	strQuery.sprintf("SELECT enbox_number FROM enbox_info where computer_name = '%s'", strMyComName.toStdString().c_str() );
	int nEnboxNum = 0;

	QSqlDatabase db; 
	if( qDbAccssor->getDBInstatnce(DAS, officeCode, db) == true )	// QSqlDatabase�ǰ�ü�����������Լ�( ������ڵ��DB Ÿ�����ΰ�����)
	{
		if( db.isOpen() )
		{
			QSqlQuery query(db);
			QString strErr;
			if( qDbAccssor->getCommand(strQuery , query , strErr) != true )  // ���� ������ �ش����� QSqlQuery ��ü����ƿ�.
			{
				// strErr ǥ��
				//GlobalLog->writelog(strErr.toLocal8Bit().data());
				return bRet;
			}

			while(query.next())                // ���Ȯ��
			{	
				nEnboxNum = query.value(0).toInt();			
			}
			setEnboxData( nEnboxNum );	
			bRet = true;				
		}
	}


	return bRet;
}

void agDBFunction::setEnboxData( int nEnboxNum )
{
	m_enboxData.push_back(nEnboxNum);
}

QVector<int> agDBFunction::getEnboxData()
{
	return m_enboxData;
}

bool agDBFunction::LoadDataSubsAndCbName()
{
	bool bRet = false;
	int nCount = 0;
	nCount = qDbAccssor->GetServerCount();	

	for(int i = 0 ; i < nCount ; i++)
	{
		QString officeCode = qDbAccssor->getServerName(i);
		QString officeName = qDbAccssor->getOfficeName(officeCode);
		
		QString strQuery;
		strQuery.sprintf("select subs_name, dl_name, cb_id from mmi_subs_info order by subs_name, dl_name asc");
		
		QSqlDatabase db;
		if( qDbAccssor->getDBInstatnce(DAS, officeCode, db) == true )	// QSqlDatabase�ǰ�ü�����������Լ�( ������ڵ��DB Ÿ�����ΰ�����) 
		{
			if( db.isOpen() )
			{
				QSqlQuery query(db);
				QString strErr;
				if( qDbAccssor->getCommand(strQuery , query , strErr) != true )  // ���� ������ �ش����� QSqlQuery ��ü����ƿ�.
				{
					// strErr ǥ��
					//GlobalLog->writelog(strErr.toLocal8Bit().data());
					return bRet;
				}

				DLInfo Info;
				QVector<DLInfo>* vtDlName;
				TypeSubStationMap SubStationMap; 

				while(query.next())
				{
					QString strSubStationName = query.value(0).toString();
					QString strDLName = query.value(1).toString();
					int nCbId = query.value(2).toInt();				

					TypeSubStationMap::iterator iter = SubStationMap.find(strSubStationName);
					if( iter != SubStationMap.end() )
					{
						Info.nCBId = nCbId;
						Info.strDLName = strDLName;	
						vtDlName->push_back(Info);
					}
					else
					{
						Info.nCBId = nCbId;
						Info.strDLName = strDLName;
						vtDlName = new QVector<DLInfo>;			   // ���α׷� ����� �޸� ���� ����� ��		
						vtDlName->push_back(Info);
						SubStationMap.insert(strSubStationName, vtDlName);
					}
				}
				m_ServerMap.insertMulti( officeCode, SubStationMap );
				bool bRet = true;
			}
		}
	}

 	return bRet;
}

TypeServerMap agDBFunction::GetServerMap()
{
	return m_ServerMap;
}

void agDBFunction::cleanServerData()
{
	TypeServerMap cleanServer = GetServerMap();

	QMapIterator<QString, TypeSubStationMap> serverItr(cleanServer);
	while( serverItr.hasNext() )
	{
		serverItr.next();
		TypeSubStationMap cleanSubstation = serverItr.value();
		QMapIterator<QString, TypeDLName*> subItr(cleanSubstation);
		while( subItr.hasNext() )
		{
			subItr.next();
			TypeDLName* cleanDL = subItr.value();
			if( !cleanDL->isEmpty() )
			{
				delete cleanDL;
				cleanDL = NULL;
			}			
		}
	}
	if( !cleanServer.isEmpty() )
		m_ServerMap.clear();
}

bool agDBFunction::CreateOLD(QString strOffice, int nCBId)
{
	bool bRet = false;

	QString officeName = qDbAccssor->getOfficeName(strOffice);

	QString strQuery;
	strQuery.sprintf("select Top (1) XMLTable.AlarmTime, XMLTable.xml, XmlOutSize, XmlInSize, CBData.subs_name, CBData.dl_name, CBData.cb_id \
					 from Enterprise_OLDXML XMLTable, \
					 ( \
					 select Tree.subs_name, Tree.dl_name, Tree.dl_id, Tree.cb_id \
					 from mmi_subs_info Tree \
					 inner join dl DL \
					 on Tree.dl_id = DL.dl_id \
					 and Tree.cb_id = DL.cb_id \
					 ) CBData \
					 where CBData.cb_id = %d  \
					 and old_type = 0 \
					 and XMLTable.dl_id = CBData.dl_id  \
					 order by alarmTime desc", nCBId);

	QSqlDatabase db;
	if( qDbAccssor->getDBInstatnce(DAS, strOffice, db) == true )	// QSqlDatabase�ǰ�ü�����������Լ�( ������ڵ��DB Ÿ�����ΰ�����) 
	{
		if( db.isOpen() )
		{
			QSqlQuery query(db);
			QString strErr;
			if( qDbAccssor->getCommand(strQuery , query , strErr) != true )  // ���� ������ �ش����� QSqlQuery ��ü����ƿ�.
			{
				// strErr ǥ��
				//GlobalLog->writelog(strErr.toLocal8Bit().data());
				return bRet;
			}

			while(query.next())
			{
				QString strAlarmTime = query.value(0).toString();
				QByteArray strXml = query.value(1).toByteArray();
				int nXMLOutSize = query.value(2).toInt();
				int nXMLInSize = query.value(3).toInt();
				QString strSubST = query.value(4).toString();
				QString strDLName = query.value(5).toString();
				int nCBId = query.value(6).toInt();

				PBYTE pData = reinterpret_cast<PBYTE>(strXml.data());
				std::string strData(pData , pData + nXMLOutSize);

				Cgzip zip;
				std::wstring strXmlData = zip.DCompress(nXMLInSize , nXMLOutSize , strData).c_str();
				setOLDCreateTime( strAlarmTime );
				SetXMLDLInfo(strOffice, nCBId, strXmlData);
			}
			bRet = true;
		}
	}
	return bRet;
}

void agDBFunction::setOLDCreateTime(QString strAlarmTime )	// �ܼ��� ȭ�鿡 ���� ���� �ð��� ���η��� �ߴµ�... �̱����� ��
{
	m_strOLDCreateTime = strAlarmTime;
}

QString agDBFunction::getOLDCreateTime( )
{
	return m_strOLDCreateTime;
}

QString agDBFunction::GetXMLDLInfo()
{
	return m_XMLDLInfo;
}

void agDBFunction::SetXMLDLInfo(QString strOffice, int nCBId, std::wstring strDLInfo)
{
	QString strDLXmlData;
	strDLXmlData = QString::fromStdWString(strDLInfo);

	TypeDLXmlData DLInfoMap;							// �Ŀ� XML �̷� ������ �ؾ��� ���� �����Ƿ� �ϴ� ���д�(2�� ����)
	TypeServerDLInfoMap ServerDLInfoMap;

	TypeServerDLInfoMap::iterator ServerDLIter = ServerDLInfoMap.find(strOffice);
	if( ServerDLIter != ServerDLInfoMap.end() )								// ����� �����̶��� ���� ����
	{
		TypeDLXmlData::iterator XmlIter = DLInfoMap.find(nCBId);
		if( XmlIter != DLInfoMap.end() )
		{
			m_XMLDLInfo = (*XmlIter);
		}
		else
		{
			DLInfoMap.insert(nCBId, strDLXmlData);
			ServerDLInfoMap.insertMulti(strOffice, DLInfoMap);
		}
	}
	else
	{
		DLInfoMap.insert(nCBId, strDLXmlData);
		ServerDLInfoMap.insertMulti(strOffice, DLInfoMap);
		m_XMLDLInfo = strDLXmlData;
	}
}

bool agDBFunction::CreateTotalCurrent(QString strOffice, int nCBId)
{
	bool bIsEmptyLine = false;
	QString LineDataQuery = makeSecQuery();
	if( LineDataQuery.isEmpty() )
		bIsEmptyLine = true;	

	QString (LineDataQuery.toLocal8Bit());
	QList<int> lineDataList = GetChartLineData();
	int nMainLineSize = lineDataList.count();		
	QString strCurrentData = getCurrentTime();
	QString sTable = makeDBTableQuery(strCurrentData, strCurrentData, HOUR);		

	bool bRet = false;
	QString officeName = qDbAccssor->getOfficeName(strOffice);

	QString strQuery;
	strQuery.sprintf("SELECT top(%d) [sec_id] \
					 ,[avg_current_a] \
					 ,[avg_current_b] \
					 ,[avg_current_c]    \
					 ,[datetime] \
					 ,[treacherousData] \
					 ,[msg] \
					 FROM [%s] \
					 where sec_id in (%s) \
					 order by [datetime] desc", nMainLineSize, sTable.toStdString().c_str(), LineDataQuery.toStdString().c_str() );
			
	QSqlDatabase db;
	if( qDbAccssor->getDBInstatnce(DAS_LOG, strOffice, db) == true )	// QSqlDatabase�ǰ�ü�����������Լ�( ������ڵ��DB Ÿ�����ΰ�����) 
	{
		if( db.isOpen() )
		{
			QSqlQuery query(db);
			QString strErr;
			if( qDbAccssor->getCommand(strQuery , query , strErr) != true )  // ���� ������ �ش����� QSqlQuery ��ü����ƿ�.
			{
				// strErr ǥ��
				//GlobalLog->writelog(strErr.toLocal8Bit().data());
				return bRet;
			}

			if( nMainLineSize == 0 )
			{
				QMessageBox msg;
				msg.setText( QString::fromLocal8Bit("CB�� �����ϰų� ���������� �������� ���߽��ϴ�."));
				msg.exec();
				return bRet;
			}

			ChartData stChart;	
			TypeChartDataMap	ChartMap;
			QList<int> noQueryLineId;

			while(query.next())
			{
				int nSec_id = query.value(0).toInt();
				float	fAvg_Current_a = query.value(1).toFloat();
				float	fAvg_Current_b = query.value(2).toFloat();
				float	fAvg_Current_c = query.value(3).toFloat();
				QString sDateTime = query.value(4).toString();
				bool bUnreliable = query.value(5).toBool();
				QString sMsg = query.value(6).toString();

				QListIterator<int> checkLineList(lineDataList);
				while( checkLineList.hasNext() )
				{
					if( checkLineList.findNext(nSec_id) )
					{
						stChart.nSecId = nSec_id;
						stChart.fAvg_Current_a = fAvg_Current_a;
						stChart.fAvg_Current_b = fAvg_Current_b;
						stChart.fAvg_Current_c = fAvg_Current_c;

						ChartMap.insert(stChart.nSecId, stChart);	
					}						
				}
			} 	
			QListIterator<int> checkQueryLine(lineDataList);			// ���� data�� ���� ���
			while ( checkQueryLine.hasNext() )
			{
				int nCheckLine = checkQueryLine.next();
				QMap<int, ChartData>::iterator checkRemoveLine = ChartMap.find(nCheckLine);
				while (checkRemoveLine == ChartMap.end())
				{
					stChart.nSecId = nCheckLine;
					stChart.fAvg_Current_a = 0.0;
					stChart.fAvg_Current_b = 0.0;
					stChart.fAvg_Current_c = 0.0;
					ChartMap.insert(stChart.nSecId, stChart );
					noQueryLineId.append(nCheckLine);					
					checkRemoveLine++;
				}
			}		
			setNoQueryLineList(noQueryLineId);
			SetChartSortLineData(ChartMap);
			bRet = true;
		}
	}
	return bRet;
}

void agDBFunction::SetChartLineData(QList<int> LineIdList)
{
	m_LineIdList = LineIdList;
}

QList<int> agDBFunction::GetChartLineData()
{
	return m_LineIdList;
}

void agDBFunction::setNoQueryLineList(QList<int> LineIdList)
{
	m_lstNoQueryLindID = LineIdList;
}

QList<int> agDBFunction::getNoQueryLineList()
{
	return m_lstNoQueryLindID;
}

bool agDBFunction::makeTimeChart(QString strOffice, QString fromDate, QString toDate, DateType dType)
{
 	m_ChartMap.clear();	
 	
 	bool bIsEmptyLine = false;
 	QString LineDataQuery = makeSecQuery();
 	if( LineDataQuery.isEmpty() )
 		 bIsEmptyLine = true;	
 
 	QString (LineDataQuery.toLocal8Bit());
 	QString sTable = makeDBTableQuery(fromDate, toDate, dType);		
 
	bool bRet = false;
	QString officeName = qDbAccssor->getOfficeName(strOffice);
	QString strQuery;

	if( dType == MONTH || dType == DAY )
	{
		strQuery = QString("SELECT sum([avg_current_a]), sum([avg_current_b]), sum([avg_current_c]), sumDate "	
			"FROM ("
			"SELECT  [avg_current_a], [avg_current_b], [avg_current_c], convert(char(10), [datetime], 23) as \"sumDate\" "    
			"FROM [%1] "
			"WHERE [datetime] > '%2' "
			"AND [datetime] < '%3' "
			"AND sec_id in ( %4 )"
			" ) a "
			"GROUP BY  sumDate "
			"ORDER BY sumDate ")
			.arg(sTable) //���̺�� 
			.arg(fromDate) // �˻����۽ð�
			.arg(toDate)   // ����ð�
			.arg(LineDataQuery);  // ����ID
	}			
	else if( dType == HOUR )
	{
		strQuery = QString("SELECT  SUM([avg_current_a]), SUM([avg_current_b]), SUM([avg_current_c]), [datetime] "					
			"FROM [%1] "
			"WHERE [datetime] > '%2' "
			"AND [datetime] < '%3' "
			"AND sec_id in ( %4 )"
			"GROUP BY  datetime "
			"ORDER BY [datetime] ")
			.arg(sTable) //���̺��
			.arg(fromDate)   // �˻����۽ð�
			.arg(toDate)     // ����ð�
			.arg(LineDataQuery);    // ����ID 
	}
	// 			QString sSql = QString("SELECT [sec_id] "
	// 				", [avg_current_a] "
	// 				", [datetime] "
	// 				", [avg_current_b] "
	// 				", [avg_current_c] "
	// 				", [treacherousData] "
	// 				", [msg] "
	// 				"FROM [daslogv2].[dbo].[%1] "
	// 				"where convert(varchar, [datetime], 112) > '%2' "
	// 				"  and convert(varchar, [datetime], 112) > '%3' "
	// 				"  and sec_id = %4 "
	// 				"order by [datetime] ")
	// 				.arg(sTable) //���̺��
	// 				.arg(fromDate + "00")
	// 				.arg(toDate + "99")
	// 				.arg(LineDataQuery);

	QSqlDatabase db; 
	if( qDbAccssor->getDBInstatnce(DAS_LOG, strOffice, db) == true )	// QSqlDatabase�ǰ�ü�����������Լ�( ������ڵ��DB Ÿ�����ΰ�����) 
	{
		if( db.isOpen() )
		{
			QSqlQuery query(db);
			QString strErr;
			if( qDbAccssor->getCommand(strQuery , query , strErr) != true )  // ���� ������ �ش����� QSqlQuery ��ü����ƿ�.
			{
				// strErr ǥ��
				//GlobalLog->writelog(strErr.toLocal8Bit().data());
				return bRet;
			}   
 
 			if( bIsEmptyLine == true )
 			{
 				QMessageBox msg;
 				msg.setText( QString::fromLocal8Bit("���μ����� ���� �ʾҽ��ϴ�."));
 				msg.exec();
 				return bRet;
 			}
 
// 			if(!query.isActive()  )
// 			{
//				QMessageBox::warning(NULL , tr("Database Error") , query.lastError().text(), QMessageBox::NoButton);
// 			}
 
 			ChartData stChart;	
 			TypeChartDataMap chartCurrentY;
 			int nMaxCurrent_a = 0;
 			int nMaxCurrent_b = 0;
 			int nMaxCurrent_c = 0;
 			int nTimeCount = 0;
 
 			while(query.next())
 			{
 				// Month �϶� // Day �϶�
 				if( dType == MONTH || dType == DAY)
 				{
 					//int		nSecId = query.value(0).toInt();
 					float	fAvg_Current_a = query.value(0).toFloat();
 					float	fAvg_Current_b = query.value(1).toFloat();
 					float	fAvg_Current_c = query.value(2).toFloat();
 					QString sDateTime = query.value(3).toString();
 
 					stChart.nSecId = nTimeCount;
 					stChart.fAvg_Current_a = fAvg_Current_a;
 					stChart.fAvg_Current_b = fAvg_Current_b;
 					stChart.fAvg_Current_c = fAvg_Current_c;
 					stChart.sDatetime = sDateTime;
 
 					chartCurrentY.insert(nTimeCount, stChart);
 					nTimeCount++;
 				}							 
 				else if( dType == HOUR)			// Hour �϶�
 				{
 					float	fAvg_Current_a = query.value(0).toFloat();
 					float	fAvg_Current_b = query.value(1).toFloat();
 					float	fAvg_Current_c = query.value(2).toFloat();
 					QString sDateTime = query.value(3).toString();
 
 					stChart.nSecId = nTimeCount;
 					stChart.fAvg_Current_a = fAvg_Current_a;
 					stChart.fAvg_Current_b = fAvg_Current_b;
 					stChart.fAvg_Current_c = fAvg_Current_c;
 					stChart.sDatetime = sDateTime;				
 
 					chartCurrentY.insert(nTimeCount, stChart);
 					nTimeCount++;
 				}				
 			}
 
 			if( query.size() == 0  )
 			{
 				QMap<int, QString> nXCount = getXCount();
 				for(int i = 0 ; i < nXCount.count() ; i++)
 				{
 					stChart.nSecId = nTimeCount;
 					stChart.fAvg_Current_a = 0;
 					stChart.fAvg_Current_b = 0;
 					stChart.fAvg_Current_c = 0;
 					stChart.sDatetime = nXCount.value(i);	
 
 					chartCurrentY.insert(nTimeCount,  stChart);
 					nTimeCount++;
 				}				
 			}
 
 			SetChartSortLineData( chartCurrentY );
 
 			// X ���� �Ǵ� �ð� ������ ���� �����Ѵ�
 			nTimeCount = 0;
 			QMap<int, QString>	chartTimeX;
 			int nDateCount = chartCurrentY.count(); 			
 			if( nDateCount > 0 )
 			{	
 				QMapIterator<int, ChartData> timeItr(chartCurrentY);
 				while (timeItr.hasNext())
 				{
 					timeItr.next();		
 					chartTimeX.insert(nTimeCount, timeItr.value().sDatetime); 
 					nTimeCount++;
 				}								
 			}
 			SetChartTimeXaxis( chartTimeX );
			bRet = true;
 		}
 	}	
	return bRet;
}

bool agDBFunction::makeCBTimeChart(QString strOffice, int nCBId, QString fromDate, QString toDate, DateType dType)
{
	m_ChartMap.clear();	

	QString sTable = makeCBDBTableQuery(fromDate, toDate, dType);	

	bool bRet = false;
	QString officeName = qDbAccssor->getOfficeName(strOffice);
	QString strQuery;

	// CB�� ���� �߰��� ������ �ð� ���� �� ���� or CB�� �����ؼ� �� �� �ְ� �ؾ� �Ѵ� ����/�ϰ� �̱���
	if( dType == MONTH  )   // ������ �ϰ�31���� ����� ���� UNION �̳� �ؼ� �ٿ��� �� �����Ѱ��� ���̺��� �߰�
	{
		// 				strQuery = QString("SELECT sum([avg_current_a]), sum([avg_current_b]), sum([avg_current_c]), sumDate "	
		// 					"FROM ("
		// 					"SELECT  [avg_current_a], [avg_current_b], [avg_current_c], convert(char(10), [datetime], 23) as \"sumDate\" "    
		// 					"FROM [%1] "
		// 					"WHERE [datetime] > '%2' "
		// 					"AND [datetime] < '%3' "
		// 					"AND sec_id in ( %4 )"
		// 					" ) a "
		// 					"GROUP BY  sumDate "
		// 					"ORDER BY sumDate ")
		// 					.arg(sTable) //���̺�� 
		// 					.arg(fromDate) // �˻����۽ð�
		// 					.arg(toDate)   // ����ð�
		// 					.arg(LineDataQuery);  // ����ID
	}			
	else if( dType == HOUR || dType == DAY)
	{
		strQuery = QString("SELECT  [cb_id], [current_a], [current_b], [current_c], [update_time] 	 "					
			"FROM [%1] "
			"WHERE [update_time] > '%2' "
			"AND [update_time] < '%3' "
			"AND cb_id in ( %4 )"					
			"ORDER BY [update_time] ")
			.arg(sTable) //���̺��
			.arg(fromDate)   // �˻����۽ð�
			.arg(toDate)     // ����ð�
			.arg(QString::number(nCBId));    // ����ID 
	}
	// 			QString sSql = QString("SELECT [sec_id] "
	// 				", [avg_current_a] "
	// 				", [datetime] "
	// 				", [avg_current_b] "
	// 				", [avg_current_c] "
	// 				", [treacherousData] "
	// 				", [msg] "
	// 				"FROM [daslogv2].[dbo].[%1] "
	// 				"where convert(varchar, [datetime], 112) > '%2' "
	// 				"  and convert(varchar, [datetime], 112) > '%3' "
	// 				"  and sec_id = %4 "
	// 				"order by [datetime] ")
	// 				.arg(sTable) //���̺��
	// 				.arg(fromDate + "00")
	// 				.arg(toDate + "99")
	// 				.arg(LineDataQuery);


	QSqlDatabase db; 
	if( qDbAccssor->getDBInstatnce(DAS_LOG, strOffice, db) == true )	// QSqlDatabase�ǰ�ü�����������Լ�( ������ڵ��DB Ÿ�����ΰ�����) 
	{
		if( db.isOpen() )
		{
			QSqlQuery query(db);
			QString strErr;
			if( qDbAccssor->getCommand(strQuery , query , strErr) != true )  // ���� ������ �ش����� QSqlQuery ��ü����ƿ�.
			{
				// strErr ǥ��
				//GlobalLog->writelog(strErr.toLocal8Bit().data());
				return bRet;
			}   

			//if(!query.isActive()  )
			//{
			//	QMessageBox::warning(NULL , tr("Database Error") , query.lastError().text(),QMessageBox::NoButton);
			//}

			ChartData stChart;	
			TypeChartDataMap chartCurrentY;
			int nTimeCount = 0;

			while(query.next())
			{
				// Month �϶� // Day �϶�			// ������ �ϴ� ����
				if( dType == MONTH )
				{
					// 					int		nCBId = query.value(0).toInt();
					// 					float	fCurrent_a = query.value(1).toFloat();
					// 					float	fCurrent_b = query.value(2).toFloat();
					// 					float	fCurrent_c = query.value(3).toFloat();
					// 					QString sDateTime = query.value(4).toString();
					// 
					// 					stChart.nSecId = nCBId;
					// 					stChart.fAvg_Current_a = fCurrent_a;
					// 					stChart.fAvg_Current_b = fCurrent_b;
					// 					stChart.fAvg_Current_c = fCurrent_c;
					// 					stChart.sDatetime = sDateTime;
					// 
					// 					chartCurrentY.insertMulti(nTimeCount, stChart);
					// 					nTimeCount++;
				}							 
				else if( dType == HOUR || dType == DAY )			// Hour �϶�
				{
					int		nCBId = query.value(0).toInt();
					float	fCurrent_a = query.value(1).toFloat();
					float	fCurrent_b = query.value(2).toFloat();
					float	fCurrent_c = query.value(3).toFloat();
					QString sDateTime = query.value(4).toString();

					stChart.nSecId = nCBId;
					stChart.fAvg_Current_a = fCurrent_a;
					stChart.fAvg_Current_b = fCurrent_b;
					stChart.fAvg_Current_c = fCurrent_c;
					stChart.sDatetime = sDateTime;				

					chartCurrentY.insert(nTimeCount, stChart);
					nTimeCount++;
				}				
			}

			if( query.size() == 0  )							// ���� ����� ������ �׳� 0���� ǥ������.... 
			{
				int nXCount = 0;
				stChart.nSecId = 0;
				stChart.fAvg_Current_a = 0;
				stChart.fAvg_Current_b = 0;
				stChart.fAvg_Current_c = 0;
				stChart.sDatetime = QString::fromLocal8Bit("");
				chartCurrentY.insert(nXCount,  stChart);
			}

			SetChartSortLineData( chartCurrentY );						//   .. �ù� �̷��� ó���ϱ� ������... �ϴ� ȭ�鳪����...

			// X ���� �Ǵ� �ð� ������ ���� �����Ѵ�					// ���� ��� ���̶� �������߷���... �̷��� �� �׳� ��.. �� ��
			nTimeCount = 0;
			QMap<int, QString>	chartTimeX;
			int nDateCount = chartCurrentY.count(); 			
			if( nDateCount > 0 )
			{	
				QMapIterator<int, ChartData> timeItr(chartCurrentY);
				while (timeItr.hasNext())
				{
					timeItr.next();		
					chartTimeX.insert(nTimeCount, timeItr.value().sDatetime); 
					nTimeCount++;
				}								
			}

			SetChartTimeXaxis( chartTimeX );
			bRet = true;
		}
	}
	return bRet;
}

void agDBFunction::setXCount(QMap<int, QString> nXCount)
{
	m_nXCount = nXCount;
}
QMap<int, QString> agDBFunction::getXCount()
{ 
	return m_nXCount; 
}

void agDBFunction::SetChartSortLineData(TypeChartDataMap chartData)
{
	m_ChartMap.clear();
	m_ChartMap = chartData;
}

TypeChartDataMap agDBFunction::GetChartSortLineData()
{
	return m_ChartMap;
}

void agDBFunction::SetChartTimeXaxis(QMap<int, QString> chartXMap )
{
	m_ChartTime.clear();
	m_ChartTime = chartXMap;
}

QMap<int, QString>  agDBFunction::GetChartTimeXaxis()
{
	return m_ChartTime;
}

QString agDBFunction::makeSecQuery()
{
	QString LineDataQuery;
	
	QList<int> lineDataList = GetChartLineData();
	QList<int>::iterator lineItr = lineDataList.begin();
	while( lineItr != lineDataList.end() )
	{	
		QString LineData = QString::number((*lineItr));
		LineDataQuery.append(LineData);
		if( (*lineItr) != lineDataList.back() )
		{
			LineDataQuery.append(',');
		}
		lineItr++;
	}

	return LineDataQuery;
}

QString agDBFunction::makeDBTableQuery(QString fromDate, QString toDate, DateType dType)
{
	QString sTable;
	QString sYear = fromDate.mid(0, 4);
	QString sMonth = fromDate.mid(5,2);

	if( dType == MONTH )
	{
		sTable = "MonthLoadData";
	}
	else if( dType == DAY )
	{		
		sTable = "DayLoadData" + sYear;
	}
	else if( dType == HOUR )
	{
		QString sDayType;
		if( sMonth == "01" || sMonth == "02" || sMonth == "03" )
			sDayType = "0103";
		else if( sMonth == "04" || sMonth == "05" || sMonth == "06" )
			sDayType = "0406";
		else if( sMonth == "07" || sMonth == "08" || sMonth == "09" )
			sDayType = "0709";
		else if( sMonth == "10" || sMonth == "11" || sMonth == "12" )
			sDayType = "1012";
		else
			return NULL;

		sTable = "HourLoadData" + sYear + sDayType;
	}

	return sTable;
}

QString agDBFunction::makeCBDBTableQuery(QString fromDate, QString toDate, DateType dType)
{
	QString sTable;
	QString sYear = fromDate.mid(0, 4);
	QString sMonth = fromDate.mid(5,2);

	if( dType == MONTH || dType == DAY )
	{
		sTable = "CbDayLoadData" + sYear;
	}
	else if( dType == HOUR )
	{
		sTable = "CbHourLoadData" + sYear;
	}

	return sTable;
}

QString agDBFunction::getCurrentTime()
{
	//�⺻ �ð� ��Ʈ ���� - ���� �ð��� �������� 24�ð� ��Ʈ ���
	QString dDate = QDate::currentDate().toString("yyyy-MM-dd");
	QString dHour = QTime::currentTime().toString("hh-mm-ss");
	// 2020-05-15 �ӽ÷� �����ֱ� ���� ������ ����� KHK
	QString temp = "2016-01-03";
	QString dCurrentTime = temp + "" + dHour; //dDate + "" +dHour;

	return dCurrentTime;
}
