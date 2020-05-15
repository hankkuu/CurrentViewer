#include "StdAfx.h"
#include <QTimer>
#include "agOLD/agCanvasWidget.h"

#define CIRCUIT_2 2
#define CIRCUIT_3 3
#define CIRCUIT_4 4
#define CIRCUIT_5 5
#define CIRCUIT_6 6

agSWInfoObject::agSWInfoObject(QObject *parent , agCanvasWidget* canvas)
	: QObject(parent),
	m_spObjectBase(new agObjectBase())
{
	m_secId = 0;
	m_nSourceSecId = 0;
	m_nTargetSecId = 0;
	m_bGroup = false;

	m_swId = 0;
	m_nPos = 0;

	m_nTarget = 0;
	m_nSource = 0;

	m_bIsBlink = false;
	m_timer = new QTimer(this);
	
	m_swParentPadInfo = NULL;

	connect(m_timer , SIGNAL(timeout()) , this , SLOT(blinkShow()));

	QObject::connect(this , SIGNAL(sendSwObj(int , QString)) , canvas , SLOT(receiveSwObj(int , QString)));
	QObject::connect(this , SIGNAL(sendCanvasPos(QRectF)) , canvas , SLOT(receiveCanvasPos(QRectF)));
}

agSWInfoObject::~agSWInfoObject()
{
	m_spObjectBase.reset();
	m_swParentPadInfo = NULL;
}


void agSWInfoObject::SetCircleItem(const sp_CircleItem& pc)
{
	m_spObjectBase->SetCircleItem(pc);
}

void agSWInfoObject::SetTextItem(const sp_TextItem& pt)
{
	if( pt->IsCaption() )
		m_strCaptionName =  pt->getText();
	
	m_spObjectBase->SetTextItem(pt);
}

void agSWInfoObject::SetPolygonItem(const sp_PolygonItem& pp)
{
	m_spObjectBase->SetPolygonItem(pp);
}

void agSWInfoObject::SetRectItem(const sp_RectItem& pr )
{
	m_spObjectBase->SetRectItem(pr);
}

void agSWInfoObject::setCustomerPenData()
{
	if( m_spObjectBase->getRectItem().size() > 1 )
	{	
		int nSize = m_spObjectBase->getRectItem().size() - 1;
		QList<sp_RectItem> lstpr = m_spObjectBase->getRectItem();

		lstpr[nSize]->setPen(lstpr[0]->getPen());
	}
}

void agSWInfoObject::SetLineItem(const sp_LineItem& pl )
{
	m_spObjectBase->SetLineItem(pl);
}

void agSWInfoObject::SetPolyLineItem(const sp_PolyLineItem& pp )
{
	m_spObjectBase->SetPolyLineItem(pp);
}

void agSWInfoObject::SetSwId(int swId)
{
	m_swId = swId;
}

int agSWInfoObject::getSwId()
{
	return m_swId;
}

void agSWInfoObject::SetObjName(const QString& strObjName)
{
	m_strObjName = strObjName;
}

QString agSWInfoObject::GetObjName()
{
	return m_strObjName;
}

void agSWInfoObject::connectOtherTabObj()
{
	emit sendSwObj(getSwId() , GetObjName());
}

void agSWInfoObject::startBlinkObj()
{
	// focus 이동 후
	showFocus();
	m_timer->start(1000);
}

void agSWInfoObject::stopBlinkObj()
{
	m_timer->stop();
	m_bIsBlink = false;
	blinkShow();
}

void agSWInfoObject::blinkShow()
{
	if( m_bIsBlink )
	{
		// hide
		hideObj();
		m_bIsBlink = false;	
	}
	else
	{
		// show
		showObj();
		m_bIsBlink = true;
	}
}

void agSWInfoObject::showObj()
{
	QList<sp_CircleItem> lstCir = m_spObjectBase->getCircleItem();

	if( lstCir.size() != 0 )
	{
		// focus
		QListIterator<sp_CircleItem>	itr(lstCir);
		while( itr.hasNext() )
		{
			sp_CircleItem sp = itr.next();

			if( sp._Get() != NULL )
				sp->show();
		}
	}

	QList<sp_LineItem> lstLine = m_spObjectBase->getLineItem();

	if( lstLine.size() != 0 )
	{
		// focus
		QListIterator<sp_LineItem>	itr(lstLine);
		while( itr.hasNext() )
		{
			sp_LineItem sp = itr.next();

			if( sp._Get() != NULL )
				sp->show();
		}
	}

	QList<sp_RectItem> lstRet = m_spObjectBase->getRectItem();

	if( lstRet.size() != 0 )
	{
		// focus
		QListIterator<sp_RectItem>	itr(lstRet);
		while( itr.hasNext() )
		{
			sp_RectItem sp = itr.next();

			if( sp._Get() != NULL )
				sp->show();
		}
	}

	QList<sp_PolygonItem> lstPoly = m_spObjectBase->getPolygonItem();

	if(lstPoly.size() != 0 )
	{
		// focus 
		QListIterator<sp_PolygonItem>	itr(lstPoly);
		while( itr.hasNext() )
		{
			sp_PolygonItem sp = itr.next();

			if( sp._Get() != NULL )
				sp->show();
		}
	}

	QList<sp_TextItem> lsttext = m_spObjectBase->getTextItem();

	if(lsttext.size() != 0 )
	{
		// focus 
		QListIterator<sp_TextItem>	itr(lsttext);
		while( itr.hasNext() )
		{
			sp_TextItem sp = itr.next();

			if( sp._Get() != NULL )
			{
				if( sp->IsMark() == true )
					sp->show();
			}
		}
	}
}

void agSWInfoObject::hideObj()
{
	QList<sp_CircleItem> lstCir = m_spObjectBase->getCircleItem();

	if( lstCir.size() != 0 )
	{
		// focus
		QListIterator<sp_CircleItem>	itr(lstCir);
		while( itr.hasNext() )
		{
			sp_CircleItem sp = itr.next();

			if( sp._Get() != NULL )
				sp->hide();
		}
	}

	QList<sp_LineItem> lstLine = m_spObjectBase->getLineItem();

	if( lstLine.size() != 0 )
	{
		// focus
		QListIterator<sp_LineItem>	itr(lstLine);
		while( itr.hasNext() )
		{
			sp_LineItem sp = itr.next();

			if( sp._Get() != NULL )
				sp->hide();
		}
	}

	QList<sp_RectItem> lstRet = m_spObjectBase->getRectItem();

	if( lstRet.size() != 0 )
	{
		// focus
		QListIterator<sp_RectItem>	itr(lstRet);
		while( itr.hasNext() )
		{
			sp_RectItem sp = itr.next();

			if( sp._Get() != NULL )
				sp->hide();
		}
	}

	QList<sp_PolygonItem> lstPoly = m_spObjectBase->getPolygonItem();

	if(lstPoly.size() != 0 )
	{
		// focus 
		QListIterator<sp_PolygonItem>	itr(lstPoly);
		while( itr.hasNext() )
		{
			sp_PolygonItem sp = itr.next();

			if( sp._Get() != NULL )
				sp->hide();
		}
	}

	QList<sp_TextItem> lsttext = m_spObjectBase->getTextItem();

	if(lsttext.size() != 0 )
	{
		// focus 
		QListIterator<sp_TextItem>	itr(lsttext);
		while( itr.hasNext() )
		{
			sp_TextItem sp = itr.next();

			if( sp._Get() != NULL )
			{
				if( sp->IsMark() == true )
					sp->hide();
			}
		}
	}
}

void agSWInfoObject::showFocus()
{
// 	QList<sp_CircleItem> lstCir = m_spObjectBase->getCircleItem();
// 
// 	if( lstCir.size() != 0 )
// 	{
// 		// focus
// 		QListIterator<sp_CircleItem>	itr(lstCir);
// 		if( itr.hasNext() )
// 		{
// 			sp_CircleItem sp = itr.next();
// 			
// 			QRectF rt = sp->boundingRect();
// 
// 			emit sendCanvasPos(rt);
// 
// 		}
// 		return;
// 	}

// 	QList<sp_LineItem> lstLine = m_spObjectBase->getLineItem();
// 
// 	if( lstLine.size() != 0 )
// 	{
// 		// focus
// 		QListIterator<sp_LineItem>	itr(lstLine);
// 		if( itr.hasNext() )
// 		{
// 			sp_LineItem sp = itr.next();
// 
// 			QRectF rt = sp->boundingRect();
// 
// 			emit sendCanvasPos(rt);
// 
// 		}
// 		return;
// 	}
	
	QList<sp_RectItem> lstRet = m_spObjectBase->getRectItem();

	if( lstRet.size() != 0 )
	{
		// focus
		QListIterator<sp_RectItem>	itr(lstRet);
		if( itr.hasNext() )
		{
			sp_RectItem sp = itr.next();

			QRectF rt = sp->boundingRect();

			emit sendCanvasPos(rt);

		}
		return;
	}

	QList<sp_PolygonItem> lstPoly = m_spObjectBase->getPolygonItem();

	if(lstPoly.size() != 0 )
	{
		// focus 
		QListIterator<sp_PolygonItem>	itr(lstPoly);
		if( itr.hasNext() )
		{
			sp_PolygonItem sp = itr.next();

			QRectF rt = sp->boundingRect();

			emit sendCanvasPos(rt);
		}
	}
}

bool agSWInfoObject::checkCircleItem()
{
	QList<sp_CircleItem> lstCir = m_spObjectBase->getCircleItem();
	if( lstCir.size() != 0 )
		return true;
	else 
		return false;
}

bool agSWInfoObject::getObjPos(QPointF &pos)
{
	QList<sp_CircleItem> lstCir = m_spObjectBase->getCircleItem();

	if( lstCir.size() != 0 )
	{
		// focus
		QListIterator<sp_CircleItem>	itr(lstCir);
		while( itr.hasNext() )
		{
			sp_CircleItem sp = itr.next();

			if( sp._Get() != NULL )
			{
				sp->show();
				pos = sp->boundingRect().center();
				return true;
			}
		}
	}

	QList<sp_RectItem> lstRet = m_spObjectBase->getRectItem();

	if( lstRet.size() != 0 )
	{
		// focus
		QListIterator<sp_RectItem>	itr(lstRet);
		while( itr.hasNext() )
		{
			sp_RectItem sp = itr.next();

			if( sp._Get() != NULL )
			{
				sp->show();
				pos = sp->boundingRect().center();
				return true;
			}
		}
	}

	QList<sp_PolygonItem> lstPoly = m_spObjectBase->getPolygonItem();

	if(lstPoly.size() != 0 )
	{
		// focus 
		QListIterator<sp_PolygonItem>	itr(lstPoly);
		while( itr.hasNext() )
		{
			sp_PolygonItem sp = itr.next();

			if( sp._Get() != NULL )
			{
				sp->show();
				pos = sp->boundingRect().center();
				return true;
			}
		}
	}

	return false;
}

void agSWInfoObject::setPadData(sp_SwInfoObj obj)
{
	m_swPadinfo.push_back(obj);
}

void agSWInfoObject::setCircuitPos()
{
	// 2회로 구분(TR , ALTS , HSSB)
	if( m_swPadinfo.size() == 2 )
	{
		setGroupCount(2);
		setTwoCircuitPos();
	}
	else if( m_swPadinfo.size() == 3 )	// 3회로는 아직 없지만,...개발 예정이 있어서 미리 만들어둠.
	{	
		setGroupCount(3);
		setThreeCircuitPos();
	}
	// 4회로 구분
	else if( m_swPadinfo.size() == 4 )
	{	
		setGroupCount(4);
		setFourCircuitPos();
	}
	// 5회로 구분
	else if( m_swPadinfo.size() == 5 )
	{	
		setGroupCount(5);
		setFiveCircuitPos();
	}
	// 6회로 구분
	else if( m_swPadinfo.size() == 6 )
	{	
		setGroupCount(6);
		setSixCircuitPos();
	}

}
void agSWInfoObject::setTwoCircuitPos()
{
	QRect	rect[CIRCUIT_2];
	int		nPos[CIRCUIT_2] = {0,};

	QListIterator<sp_SwInfoObj>		obj_itr(m_swPadinfo);

	int nCnt = 0;
	while(obj_itr.hasNext())
	{
		sp_SwInfoObj obj = obj_itr.next();

		if( obj )
		{
			rect[nCnt++] = obj->getCircleRect();
			obj->setPadParentData(this);
		}
	}

	for( int i = 0; i < CIRCUIT_2; i++ )
	{
		if( m_padRt.center().x() < rect[i].center().x() && m_padRt.center().y() < rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nRightBottom;
		}
		else if( m_padRt.center().x() > rect[i].center().x() && m_padRt.center().y() < rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nLeftBottom;
		}
		else if( m_padRt.center().x() > rect[i].center().x() && m_padRt.center().y() > rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nLeftTop;
		}
		else if( m_padRt.center().x() < rect[i].center().x() && m_padRt.center().y() > rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nRightTop;
		}
		else if( m_padRt.center().x() == rect[i].center().x() && m_padRt.center().y() > rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nLeftTop;
		}
		else if( m_padRt.center().x() == rect[i].center().x() && m_padRt.center().y() < rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nRightBottom;
		}
		else if( m_padRt.center().x() > rect[i].center().x() && m_padRt.center().y() == rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nLeftBottom;
		}
		else if( m_padRt.center().x() < rect[i].center().x() && m_padRt.center().y() == rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nRightTop;
		}
	}

	while(obj_itr.hasPrevious())
	{
		sp_SwInfoObj obj = obj_itr.previous();

		if( obj )
		{
			obj->setFiPosition(nPos[--nCnt]);
		}
	}
}

void agSWInfoObject::setThreeCircuitPos()
{
	QRect	rect[CIRCUIT_3];
	int		nPos[CIRCUIT_3] = {0,};

	QListIterator<sp_SwInfoObj>		obj_itr(m_swPadinfo);

	int nCnt = 0;
	while(obj_itr.hasNext())
	{
		sp_SwInfoObj obj = obj_itr.next();

		if( obj )
		{
			rect[nCnt++] = obj->getCircleRect();
			obj->setPadParentData(this);
		}
	}

	for( int i = 0; i < CIRCUIT_3; i++ )
	{
		if( m_padRt.center().x() < rect[i].center().x() && m_padRt.center().y() < rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nRightBottom;
		}
		else if( m_padRt.center().x() > rect[i].center().x() && m_padRt.center().y() < rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nLeftBottom;
		}
		else if( m_padRt.center().x() > rect[i].center().x() && m_padRt.center().y() > rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nLeftTop;
		}
		else if( m_padRt.center().x() < rect[i].center().x() && m_padRt.center().y() > rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nRightTop;
		}
		else if( m_padRt.center().x() == rect[i].center().x() && m_padRt.center().y() > rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nLeftTop;
		}
		else if( m_padRt.center().x() == rect[i].center().x() && m_padRt.center().y() < rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nRightBottom;
		}
		else if( m_padRt.center().x() > rect[i].center().x() && m_padRt.center().y() == rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nLeftBottom;
		}
		else if( m_padRt.center().x() < rect[i].center().x() && m_padRt.center().y() == rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nRightTop;
		}
	}

	while(obj_itr.hasPrevious())
	{
		sp_SwInfoObj obj = obj_itr.previous();

		if( obj )
		{
			obj->setFiPosition(nPos[--nCnt]);
		}
	}
}


void agSWInfoObject::setFourCircuitPos()
{
	QRect	rect[CIRCUIT_4];
	int		nPos[CIRCUIT_4] = {0,};

	QListIterator<sp_SwInfoObj>		obj_itr(m_swPadinfo);

	int nCnt = 0;
	while(obj_itr.hasNext())
	{
		sp_SwInfoObj obj = obj_itr.next();

		if( obj )
		{
			rect[nCnt++] = obj->getCircleRect();
			obj->setPadParentData(this);
		}
	}

	for( int i = 0; i < CIRCUIT_4; i++ )
	{
		if( m_padRt.center().x() < rect[i].center().x() && m_padRt.center().y() < rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nRightBottom;
		}
		else if( m_padRt.center().x() > rect[i].center().x() && m_padRt.center().y() < rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nLeftBottom;
		}
		else if( m_padRt.center().x() > rect[i].center().x() && m_padRt.center().y() > rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nLeftTop;
		}
		else if( m_padRt.center().x() < rect[i].center().x() && m_padRt.center().y() > rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nRightTop;
		}
		else if( m_padRt.center().x() == rect[i].center().x() && m_padRt.center().y() > rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nLeftTop;
		}
		else if( m_padRt.center().x() == rect[i].center().x() && m_padRt.center().y() < rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nRightBottom;
		}
		else if( m_padRt.center().x() > rect[i].center().x() && m_padRt.center().y() == rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nLeftBottom;
		}
		else if( m_padRt.center().x() < rect[i].center().x() && m_padRt.center().y() == rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nRightTop;
		}
	}

	while(obj_itr.hasPrevious())
	{
		sp_SwInfoObj obj = obj_itr.previous();

		if( obj )
		{
			obj->setFiPosition(nPos[--nCnt]);
		}
	}
}

void agSWInfoObject::setFiveCircuitPos()
{
	QRect	rect[CIRCUIT_5];
	int		nPos[CIRCUIT_5] = {0,};

	QListIterator<sp_SwInfoObj>		obj_itr(m_swPadinfo);

	int nCnt = 0;
	while(obj_itr.hasNext())
	{
		sp_SwInfoObj obj = obj_itr.next();

		if( obj )
		{
			rect[nCnt++] = obj->getCircleRect();
			obj->setPadParentData(this);
		}
	}

	for( int i = 0; i < CIRCUIT_5; i++ )
	{
		if( m_padRt.center().x() < rect[i].center().x() && m_padRt.center().y() < rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nRightBottom;
		}
		else if( m_padRt.center().x() > rect[i].center().x() && m_padRt.center().y() < rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nLeftBottom;
		}
		else if( m_padRt.center().x() > rect[i].center().x() && m_padRt.center().y() > rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nLeftTop;
		}
		else if( m_padRt.center().x() < rect[i].center().x() && m_padRt.center().y() > rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nRightTop;
		}
		else if( m_padRt.center().x() == rect[i].center().x() && m_padRt.center().y() > rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nLeftTop;
		}
		else if( m_padRt.center().x() == rect[i].center().x() && m_padRt.center().y() < rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nRightBottom;
		}
		else if( m_padRt.center().x() > rect[i].center().x() && m_padRt.center().y() == rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nLeftBottom;
		}
		else if( m_padRt.center().x() < rect[i].center().x() && m_padRt.center().y() == rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nRightTop;
		}
	}

	while(obj_itr.hasPrevious())
	{
		sp_SwInfoObj obj = obj_itr.previous();

		if( obj )
		{
			obj->setFiPosition(nPos[--nCnt]);
		}
	}

}

void agSWInfoObject::setSixCircuitPos()
{
	QRect	rect[CIRCUIT_6];
	int		nPos[CIRCUIT_6] = {0,};

	QListIterator<sp_SwInfoObj>		obj_itr(m_swPadinfo);

	int nCnt = 0;
	while(obj_itr.hasNext())
	{
		sp_SwInfoObj obj = obj_itr.next();

		if( obj )
		{
			rect[nCnt++] = obj->getCircleRect();
			obj->setPadParentData(this);
		}
	}

	for( int i = 0; i < CIRCUIT_6; i++ )
	{
		if( m_padRt.center().x() > rect[i].center().x() && m_padRt.center().y() < rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nLeftBottom;
		}
		else if( m_padRt.center().x() > rect[i].center().x() && m_padRt.center().y() > rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nLeftTop;
		}
		else if( m_padRt.center().x() < rect[i].center().x() && m_padRt.center().y() > rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nRightTop;
		}
		else if( m_padRt.center().x() == rect[i].center().x() && m_padRt.center().y() > rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nMiddleTop;
		}
		else if( m_padRt.center().x() == rect[i].center().x() && m_padRt.center().y() < rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nMiddleBottom;
		}
		else if( m_padRt.center().x() > rect[i].center().x() && m_padRt.center().y() == rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nLeftBottom;
		}
		else if( m_padRt.center().x() < rect[i].center().x() && m_padRt.center().y() == rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nRightTop;
		}
		else if( m_padRt.center().x() < rect[i].center().x() && m_padRt.center().y() < rect[i].center().y() )
		{
			nPos[i] = agSWInfoObject::nRightBottom;
		}
	}

	while(obj_itr.hasPrevious())
	{
		sp_SwInfoObj obj = obj_itr.previous();

		if( obj )
		{
			obj->setFiPosition(nPos[--nCnt]);
		}
	}
}

void agSWInfoObject::setCircleRect(QRect rt)
{
	m_cirRt = rt;
}

QRect agSWInfoObject::getCircleRect()
{
	return m_cirRt;
}

void agSWInfoObject::setPadRect(QRect rt)
{
	m_padRt = rt;
}


void agSWInfoObject::setFiPosition(int nPos , bool bIsOld) 
{ 
	m_nPos = nPos;
}

void agSWInfoObject::setPadParentData(agSWInfoObject* obj)
{
	m_swParentPadInfo = obj;
}
agSWInfoObject* agSWInfoObject::getPadParentData()
{
	return m_swParentPadInfo;
}

bool agSWInfoObject::checkSwLoc(QString strSwLoc)
{
	QList<sp_TextItem>	text = m_spObjectBase->getTextItem();

	QListIterator<sp_TextItem>	itr(text);
	while( itr.hasNext() )
	{
		sp_TextItem sp = itr.next();

		if( sp._Get() != NULL )
		{
			if( sp->getText().compare(QString::fromLocal8Bit(strSwLoc.toLocal8Bit()))  == 0 )
				return true;
		}
	}

	return false;
}

void agSWInfoObject::setObjRect(QRectF rt)
{
	if( m_objRect.isEmpty() )
	{
		m_objRect = rt;
	}
	else
	{
		if( rt.center().x() > m_objRect.width() )
		{
			m_objRect.setWidth(rt.center().x());
		}

		if( rt.center().y() > m_objRect.height() )
		{
			m_objRect.setHeight(rt.center().y());
		}
	}
}


bool agSWInfoObject::getObjPos(QRectF &rt)
{
	QList<sp_CircleItem> lstCir = m_spObjectBase->getCircleItem();

	if( lstCir.size() != 0 )
	{
		// focus
		QListIterator<sp_CircleItem>	itr(lstCir);
		while( itr.hasNext() )
		{
			sp_CircleItem sp = itr.next();

			if( sp._Get() != NULL )
			{
				rt = sp->boundingRect();
				return true;
			}
		}
	}

	QList<sp_RectItem> lstRet = m_spObjectBase->getRectItem();

	if( lstRet.size() != 0 )
	{
		// focus
		QListIterator<sp_RectItem>	itr(lstRet);
		while( itr.hasNext() )
		{
			sp_RectItem sp = itr.next();

			if( sp._Get() != NULL )
			{
				rt = sp->boundingRect();
				return true;
			}
		}
	}

	QList<sp_PolygonItem> lstPoly = m_spObjectBase->getPolygonItem();

	if(lstPoly.size() != 0 )
	{
		// focus 
		QListIterator<sp_PolygonItem>	itr(lstPoly);
		while( itr.hasNext() )
		{
			sp_PolygonItem sp = itr.next();

			if( sp._Get() != NULL )
			{
				rt = sp->boundingRect();
				return true;
			}
		}
	}

	return false;
}

bool agSWInfoObject::getObjRect(sp_RectItem &sp_rt)
{
	QList<sp_RectItem> lstRet = m_spObjectBase->getRectItem();

	if( lstRet.size() != 0 )
	{
		// focus
		QListIterator<sp_RectItem>	itr(lstRet);
		while( itr.hasNext() )
		{
			sp_RectItem sp = itr.next();

			if( sp._Get() != NULL )
			{
				sp_rt =  sp;
				return true;
			}
		}
	}

	return false;
}

/*
void agSWInfoObject::changeTextColor(QColor color)
{
	QList<sp_TextItem> lstText = m_spObjectBase->getTextItem();

	if( lstText.size() != 0 )
	{
		// focus
		QListIterator<sp_TextItem>	itr(lstText);
		while( itr.hasNext() )
		{
			sp_TextItem sp = itr.next();

			if( sp._Get() != NULL )
			{
				if( sp->IsCaption() == true )
				{
					sp->setFontColor(color);
				}
				
			}
		}
	}
}*/
sp_LineItem	agSWInfoObject::lineObject()					// 20151210 KHK
{
	QList<sp_LineItem> lstline = m_spObjectBase->getLineItem();

	if( lstline.size() > 0 )
	{
		return lstline[0];
	}
	
	// khk 이런식 처리가 맞는가?
	sp_LineItem empty(new agLineItem(new agSWInfoObject()));
	return empty;
}

sp_PolyLineItem	agSWInfoObject::polylineObject()					// 20151210 KHK
{
	QList<sp_PolyLineItem> lstline = m_spObjectBase->getPolyLineItem();

	if( lstline.size() > 0 )
	{
		return lstline[0];
	}

	// khk 이런식 처리가 맞는가?
	sp_PolyLineItem empty(new agPolyLineItem());
	return empty;
}

void agSWInfoObject::setEndPointLine(int nEndLine)
{
	m_nEndSecList.push_back(nEndLine);
}