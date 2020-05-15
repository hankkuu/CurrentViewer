#include "StdAfx.h"
#include "agPolyLineItem.h"

agPolyLineItem::agPolyLineItem()
{
	setZValue(2);	
}	

agPolyLineItem::~agPolyLineItem()
{

}

void	agPolyLineItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	//emit sendConnectTabObj();
	QGraphicsItem::mouseDoubleClickEvent(event);
}	

void	agPolyLineItem::mousePressEvent ( QGraphicsSceneMouseEvent * event)
{
	//event->ignore();
	QGraphicsItem::mousePressEvent(event);
}


void	agPolyLineItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *s, QWidget * )
{
	/*qreal sizeMax = qMin(m_rt.size().width(),m_rt.size().height());

	const qreal lod = s->levelOfDetailFromTransform(painter->worldTransform());

	if ( lod < 0.1 && m_strObj.compare("GroundLine") != 0 )
		return;*/

	painter->setRenderHint(QPainter::Antialiasing);
	painter->setBrush(m_brush);

	QPen pen = m_pen;
	pen.setCapStyle(Qt::FlatCap);
	pen.setJoinStyle(Qt::MiterJoin);
	painter->setPen(pen); 

//	painter->setPen(m_pen); 
	painter->drawPolyline(m_polygon);
}

void	agPolyLineItem::setPolygon( const QPolygonF &polygon )
{
	m_polygon = polygon;
	bool anti = false;
	QPointF current = m_polygon.first();
	for ( int i =1 ; i < m_polygon.size() ; i++ )
	{
		QPointF last = m_polygon[i];
		if ( current.x() != last.x() && current.y() != last.y())
		{
			anti = true;
			break;
		}
		current = last;
	}
}

QRectF	agPolyLineItem::boundingRect() const
{
	qreal width = m_pen.widthF()/2;
	return m_polygon.boundingRect().adjusted(-width,-width,width,width);
}

