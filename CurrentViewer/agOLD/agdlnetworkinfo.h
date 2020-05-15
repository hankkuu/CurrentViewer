#ifndef AGDLNETWORKINFO_H
#define AGDLNETWORKINFO_H

/**
	@class		agDLNetworkInfo
	@date		2015/10/06
	@author		이지수
	@brief		DL 또는 Network 계통의 전체 객체를 저장하는 클래스
	@warning
*/

#include <QObject>
#include "agDmsDeviceClass/agswinfoobject.h"

typedef std::tr1::shared_ptr<agSWInfoObject>		sp_SwInfoObj;

class agDLNetworkInfo : public QObject
{
	Q_OBJECT

public:
	agDLNetworkInfo(QObject *parent = NULL);
	~agDLNetworkInfo();

	void			SetModifyDate(const QString &strModifyDate);

	void			SetNetworkPosition(int nHeight , int nWidth);

	void			SetSwInfoList(const sp_SwInfoObj& sw_info);
	
	int				getNeworkHeight();
	int				getNetworkWidth();
	bool			checkSwObj(int nSwid , sp_SwInfoObj& obj, QString strObjName);
	bool			checkSwObj(int nSwid , sp_SwInfoObj& obj);

	bool			getSwObjPos(int nSwid, QPointF &pos, int &dir);

	bool			checkSwObjToCaption(int nSwid , sp_SwInfoObj& obj, QString strCaption);

	void			setCBRect(QRectF rt);
	QRectF			getCBRect();

	QRectF			getOldSceneRect() { return m_oldSceneRect; }

	void			setOldXPoint(QPointF center);

	sp_SwInfoObj			GetObj(int nSwId);						// 20151224 KHK
	void					setDummyList(sp_SwInfoObj obj);			// 20151224 KHK
	QList<sp_SwInfoObj>*	getDummyList();							// 20151224 KHK
	
private:
	int									m_nHeight;
	int									m_nWidth;
	QString								m_strModifyDate;

	QMultiMap<int, sp_SwInfoObj>		m_mapSwInfoObj;

	QRectF								m_CBRt;
	QRectF								m_oldSceneRect;

	QList<sp_SwInfoObj>					m_DummyList;				// 계통에 있는 더미 리스트
};

typedef std::tr1::shared_ptr<agDLNetworkInfo>		sp_DlInfoObj;

#endif // AGDLNETWORKINFO_H
