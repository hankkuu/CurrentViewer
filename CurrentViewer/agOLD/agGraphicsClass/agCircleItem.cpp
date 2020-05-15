#include "StdAfx.h"
#include "agCircleItem.h"
#include "agOLD/agDmsDeviceClass/agswinfoobject.h"

agCircleItem::agCircleItem(agSWInfoObject* swObj)
{
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges,true);
	//setFlag(QGraphicsItem::ItemStacksBehindParent,true);
	setFlag(QGraphicsItem::ItemIsFocusable , true);
	setZValue(6);
	
	QObject::connect(this , SIGNAL(sendConnectTabObj()) , swObj , SLOT(connectOtherTabObj()));
}

agCircleItem::~agCircleItem()
{

}

void	agCircleItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	emit sendConnectTabObj();
	QGraphicsItem::mouseDoubleClickEvent(event);
}
void	agCircleItem::mousePressEvent ( QGraphicsSceneMouseEvent * event)
{
	event->ignore();
	QGraphicsItem::mousePressEvent(event);
}

void agCircleItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * )
{
	qreal sizeMax = qMin(m_rt.size().width(),m_rt.size().height());

	const qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());

	if ( lod < 0.1)
		return;

	painter->setRenderHint(QPainter::Antialiasing,true);
	painter->setBrush(m_brush);
	painter->setPen(m_pen); 
	painter->drawEllipse(m_rt);
}



void	agCircleItem::setRect( const QRectF& rect)
{
	m_rt = rect;
}

void agCircleItem::setRect( qreal xPos, qreal yPos, qreal width, qreal height )
{
	setRect(QRectF(xPos,yPos,width,height)) ;
}

QRectF agCircleItem::boundingRect() const
{
	return m_rt;
}