#include "StdAfx.h"
#include "agdlnetworkinfo.h"

agDLNetworkInfo::agDLNetworkInfo(QObject *parent)
	: QObject(parent)
{
	m_mapSwInfoObj.clear();
	m_nHeight = 0;
	m_nWidth = 0;
}

agDLNetworkInfo::~agDLNetworkInfo()
{
	m_mapSwInfoObj.clear();
}	

/**
	@brief		그림정보를 저장하고 있는 객체를 저장하는 함수
	@param		sp_SwInfoObj&		하나의 객체에 그림정보를 전부 저장하고 있는 클래스
*/
void agDLNetworkInfo::SetSwInfoList(const sp_SwInfoObj& sw_info)
{
	m_mapSwInfoObj.insert(sw_info->getSwId() , sw_info);
}


/**
	@brief		현 계통의 가로 세로 길이 정보를 저장하는 함수
	@param		int nHeight			세로 길이
	@param		int nWidth			가로 길이
*/
void agDLNetworkInfo::SetNetworkPosition(int nHeight , int nWidth)
{
	m_nHeight = nHeight;
	m_nWidth = nWidth;
}


/**
	@brief		현 계통의 세로 길이 정보를 리턴하는 함수
*/

int agDLNetworkInfo::getNeworkHeight()
{
	return m_nHeight;
}

/**
	@brief		현 계통의 가로 길이 정보를 리턴하는 함수
*/
int agDLNetworkInfo::getNetworkWidth()
{
	return m_nWidth;
}

/**
	@brief		현 계통의 계통 수정 날짜를 저장하는 함수
	@param		*strModifyDate			계통 수정 날짜
*/

void agDLNetworkInfo::SetModifyDate(const QString &strModifyDate)
{
	m_strModifyDate = strModifyDate;
}


/**
	@brief		현 계통의 찾고자 하는 개폐기가 있는지 확인하는 함수
	@param		nSwid		개폐기 ID
	@param		obj			개폐기 그림정보를 저장하고 있는 객체
	@param		strObjName	개폐기 오브젝트 이름
*/
bool agDLNetworkInfo::checkSwObj(int nSwid , sp_SwInfoObj& obj, QString strObjName)
{
	QMultiMap<int, sp_SwInfoObj>::ConstIterator const_itr = m_mapSwInfoObj.find(nSwid);

	while( const_itr != m_mapSwInfoObj.end() && const_itr.key() == nSwid )
	{
		//qDebug() << const_itr.value();
		if( const_itr.value()->GetObjName().compare(strObjName) == 0 )
		{
			obj = const_itr.value();
			return true;
		}
		const_itr++;
	}

	return false;
}

sp_SwInfoObj agDLNetworkInfo::GetObj(int nSwid)
{
	QMultiMap<int, sp_SwInfoObj>::Iterator const_itr = m_mapSwInfoObj.find(nSwid);

	while( const_itr != m_mapSwInfoObj.end() && const_itr.key() == nSwid )
	{
		//qDebug() << const_itr.value();
		return const_itr.value();
	}

	// khk 이런식 처리가 맞는가?
	sp_SwInfoObj empty(new agSWInfoObject());
	return empty;
}

bool agDLNetworkInfo::checkSwObj(int nSwid , sp_SwInfoObj& obj)
{
	QMultiMap<int, sp_SwInfoObj>::ConstIterator const_itr = m_mapSwInfoObj.find(nSwid);

	while( const_itr != m_mapSwInfoObj.end() && const_itr.key() == nSwid )
	{
		//qDebug() << const_itr.value();
		if( const_itr.value()->GetObjName().compare(QString::fromLocal8Bit("Dummy")) != 0 &&
			//const_itr.value()->GetObjName().compare(QString::fromLocal8Bit("UnderLine")) != 0 &&
			//const_itr.value()->GetObjName().compare(QString::fromLocal8Bit("GroundLine")) != 0 &&
			const_itr.value()->GetObjName().compare(QString::fromLocal8Bit("Customer")) != 0 )
		{
			obj = const_itr.value();
			return true;
		}
		const_itr++;
	}

	return false;
}

bool agDLNetworkInfo::checkSwObjToCaption(int nSwid , sp_SwInfoObj& obj, QString strCaption)
{
	QMultiMap<int, sp_SwInfoObj>::ConstIterator const_itr = m_mapSwInfoObj.find(nSwid);

	while( const_itr != m_mapSwInfoObj.end() && const_itr.key() == nSwid )
	{
		//qDebug() << const_itr.value();

		sp_SwInfoObj search = const_itr.value();

		if( search )
		{
			// PAD인지 확인
			agSWInfoObject* pad = search->getPadParentData();
			if( pad != NULL)
			{
				QStringList strTmp = strCaption.split("(");

				if( pad->checkSwLoc(strTmp.at(0)) == true )
				{
					obj = const_itr.value();
					return true;
				}
			}
			else
			{
				if( search->checkSwLoc(strCaption) == true )
				{
					obj = const_itr.value();
					return true;
				}
			}
			

		}

		const_itr++;
	}

	return false;
}


bool agDLNetworkInfo::getSwObjPos(int nSwid, QPointF &pos, int &dir)
{
	QMultiMap<int, sp_SwInfoObj>::ConstIterator const_itr = m_mapSwInfoObj.find(nSwid);

	while( const_itr != m_mapSwInfoObj.end() && const_itr.key() == nSwid )
	{
		//qDebug() << const_itr.value();

		if( const_itr.value()->checkCircleItem() == true )
		{
			if( const_itr.value()->getObjPos(pos) )
			{
				dir = const_itr.value()->getFiPosition();
				return true;
			}
		}
		else
			const_itr++;
	}

	return false;
}

void agDLNetworkInfo::setOldXPoint(QPointF center)
{
	if( m_oldSceneRect.isEmpty() )
	{
		m_oldSceneRect.setWidth(center.x());
		m_oldSceneRect.setHeight(center.y());
	}
	else
	{
		if( center.x() > m_oldSceneRect.width() )
		{
			m_oldSceneRect.setWidth(center.x());
		}

		if( center.y() > m_oldSceneRect.height() )
		{
			m_oldSceneRect.setHeight(center.y());
		}
	}
}

void agDLNetworkInfo::setCBRect(QRectF rt)
{
	m_CBRt = rt;
}

QRectF agDLNetworkInfo::getCBRect()
{
	return m_CBRt;
}

void agDLNetworkInfo::setDummyList(sp_SwInfoObj obj)
{
	m_DummyList.push_back(obj);
}

QList<sp_SwInfoObj>* agDLNetworkInfo::getDummyList()
{
	return &m_DummyList;
}