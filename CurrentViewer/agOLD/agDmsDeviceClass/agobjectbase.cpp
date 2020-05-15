#include "StdAfx.h"
#include "agOLD/agDmsDeviceClass/agobjectbase.h"

agObjectBase::agObjectBase(QObject *parent)
	: QObject(parent)
{

}

agObjectBase::~agObjectBase()
{
	DeleteItems();
}	


void agObjectBase::DeleteItems()
{
	/*DeletePolygonItems();
	DeleteTextItems();
	DeleteCircleItems();
	DeleteRectItems();
	DeleteLineItems();
	DeletePolyLineItems();*/
}

void agObjectBase::DeletePolygonItems()
{
	m_ListPolygonItem.clear();
}

void agObjectBase::DeleteRectItems()
{
	m_ListRectItem.clear();
}

void agObjectBase::DeleteCircleItems()
{
	m_ListCircleItem.clear();
}

void agObjectBase::DeleteTextItems()
{
	m_ListTextItem.clear();
}

void agObjectBase::DeleteLineItems()
{
	m_ListLineItem.clear();
}

void agObjectBase::DeletePolyLineItems()
{
	m_ListPolyLineItem.clear();
}


void agObjectBase::SetCircleItem(const sp_CircleItem& pc)
{
	m_ListCircleItem.push_back(pc);
}

void agObjectBase::SetPolygonItem(const sp_PolygonItem& pp)
{
	m_ListPolygonItem.push_back(pp);
}

void agObjectBase::SetRectItem(const sp_RectItem& pr)
{
	m_ListRectItem.push_back(pr);
}

void agObjectBase::SetTextItem(const sp_TextItem& pt)
{
	m_ListTextItem.push_back(pt);
}

void agObjectBase::SetLineItem(const sp_LineItem &pl)
{
	m_ListLineItem.push_back(pl);
}

void agObjectBase::SetPolyLineItem(const sp_PolyLineItem &pp)
{
	m_ListPolyLineItem.push_back(pp);
}

QList<sp_CircleItem>& agObjectBase::getCircleItem()
{
	return m_ListCircleItem;
}

QList<sp_PolygonItem>& agObjectBase::getPolygonItem()
{
	return m_ListPolygonItem;
}

QList<sp_RectItem>& agObjectBase::getRectItem()
{
	return m_ListRectItem;
}

QList<sp_TextItem>& agObjectBase::getTextItem()
{
	return m_ListTextItem;
}

QList<sp_LineItem>& agObjectBase::getLineItem()
{
	return m_ListLineItem;
}

QList<sp_PolyLineItem>& agObjectBase::getPolyLineItem()
{
	return m_ListPolyLineItem;
}
