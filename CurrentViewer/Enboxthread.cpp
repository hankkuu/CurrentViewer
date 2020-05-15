#include "stdafx.h"
#include "Enboxthread.h"
//#include "midplusplus.h"	// 미들웨어 없이 구동을 위한 khk 2020-05-15
#include <atlstr.h>

#include <QApplication>

EnboxMonitoring::EnboxMonitoring(QVector<int> enboxData, QObject *parent) : QThread(parent)
{
    m_count = 0;
    m_loop = true;
	m_enboxData = enboxData;	
}

EnboxMonitoring::~EnboxMonitoring()
{

}

void EnboxMonitoring::run()
{
    while(m_loop)
	{
        //qDebug("THREAD %d", m_count++);
        //if(m_count % 10 == 0) emit mySignal();
        //if(m_count % 10 == 0)
		{
            //qApp->postEvent(parent(), new MyEvent(QString("id"), QString("blob data %1").arg(m_count)));
			//msleep(100);
        }        

		char		szEnboxName[MAX_PATH];
		ZeroMemory(szEnboxName,MAX_PATH);

		sprintf_s( szEnboxName, _countof(szEnboxName), "/OLDToCurrentViewer%d", m_enboxData.at(0));		

		unsigned int desc_handle = 0;
		int status = 0; //MidPlusPlus::ConnectPortForRecv(szEnboxName, desc_handle );		
		if( status != 0 )
		{	
			msleep(100);
			continue;
		}	

		// 미들웨어 없이 구동을 위한 khk 2020-05-15
		//MidPlusData midData("DLINFO", 0 );		
		int time_out_val = 2;
		while(1)
		{
			//status  = MidPlusPlus::ReceivePacket(desc_handle, midData, time_out_val );		

			if( 0 == status)
			{	
				int nCBId = 0; //C_POP( unsigned int, midData, cb_id );
				int nOfficeCode = 0; //C_POP( unsigned int, midData, officecode );		// 연계SWID
				wchar_t* strOfficeName = '\0'; //(wchar_t*)midData.GetValue("officename");	//C_POP( wchar_t, midData, officename ); // 연계 사업소 코드
				wchar_t* strDlName = '\0'; //(wchar_t*)midData.GetValue("dl_name");			//C_POP( wchar_t, midData, dl_name );
				
				QString strServer = QString::fromWCharArray(strOfficeName);
				QString strDl = QString::fromWCharArray(strDlName);

				qApp->postEvent(parent(), new OfficeEvent(strServer, strDl, nCBId));
				msleep(100);
				continue;			
			}
			else
			{
				break;
			}
			msleep(100);
 	    }
	}
}
