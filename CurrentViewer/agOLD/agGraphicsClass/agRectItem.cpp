#include "StdAfx.h"
#include "agRectItem.h"
#include "agOLD/agDmsDeviceClass/agswinfoobject.h"

agRectItem::agRectItem(agSWInfoObject* swObj)
{
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	//setFlag(QGraphicsItem::ItemSendsGeometryChanges,true);
	//setFlag(QGraphicsItem::ItemStacksBehindParent,true);
	setFlag(QGraphicsItem::ItemIsFocusable , true);
	m_nRotate = 0;

	setZValue(5);

	QObject::connect(this , SIGNAL(sendConnectTabObj()) , swObj , SLOT(connectOtherTabObj()));
}

agRectItem::~agRectItem()
{

}




void	agRectItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	emit sendConnectTabObj();
	QGraphicsItem::mouseDoubleClickEvent(event);
}	

void	agRectItem::mousePressEvent ( QGraphicsSceneMouseEvent * event)
{
	event->ignore();
	QGraphicsItem::mousePressEvent(event);
}


void agRectItem::paint( QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * )
{
	const qreal lod = QStyleOptionGraphicsItem::levelOfDetailFromTransform(painter->worldTransform());
	qreal sizeMax = qMin(m_rt.size().width(),m_rt.size().height());

	if ( lod < 0.1 && m_strObj.compare("Substation") != 0 )
		return;

	painter->setRenderHint(QPainter::Antialiasing,1);

	if( m_strObj.compare("Customer") != 0 )
		painter->setBrush(m_brush);
	painter->setPen(m_pen); 

	painter->drawRect(m_rt);
}

void	agRectItem::setRotate(int nRotate)
{
	m_nRotate = nRotate;
}