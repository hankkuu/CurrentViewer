#include "StdAfx.h"
#include "agPolygonItem.h"
#include "agOLD/agDmsDeviceClass/agswinfoobject.h"

agPolygonItem::agPolygonItem(agSWInfoObject* swObj)
{
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges,true);
	//setFlag(QGraphicsItem::ItemStacksBehindParent,true);
	setFlag(QGraphicsItem::ItemIsFocusable , true);

	setZValue(4);
	QObject::connect(this , SIGNAL(sendConnectTabObj()) , swObj , SLOT(connectOtherTabObj()));
}

agPolygonItem::~agPolygonItem()
{

}

void	agPolygonItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	emit sendConnectTabObj();
	QGraphicsItem::mouseDoubleClickEvent(event);
}	

void	agPolygonItem::mousePressEvent ( QGraphicsSceneMouseEvent * event)
{
	event->ignore();
	QGraphicsItem::mousePressEvent(event);
}



void	agPolygonItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *, QWidget * )
{
	const qreal lod = QStyleOptionGraphicsItem::levelOfDetailFromTransform(painter->worldTransform());
	qreal sizeMax = qMin(m_rect.size().width(),m_rect.size().height());

	if ( lod < 0.1)
		return;

	painter->setRenderHint(QPainter::Antialiasing);
	painter->setBrush(m_brush);

	QPen pen = m_pen;
	pen.setCapStyle(Qt::FlatCap);
	pen.setJoinStyle(Qt::MiterJoin);
	painter->setPen(pen); 

//	painter->setPen(m_pen); 
	painter->drawPolygon(m_polygon);

	if ( !m_str.isEmpty() )
	{
		if ( lod *m_font.pointSize() > 3 )
		{
			QColor fontcolor = m_fontColor;
			
			painter->setPen(QPen(fontcolor)); 
			painter->setFont(m_font);
			painter->setRenderHint(QPainter::TextAntialiasing,true);
			painter->drawText(m_rect,Qt::AlignCenter,m_str);

			//painter->restore();
		}
	}
	
}

void	agPolygonItem::setPolygon( const QPolygonF &polygon )
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

QRectF	agPolygonItem::boundingRect() const
{
	qreal width = m_pen.widthF()/2;
	return m_polygon.boundingRect().adjusted(-width,-width,width,width);
}


void agPolygonItem::setText(QString& str)
{
	m_str = str;
}

void	agPolygonItem::setRect( const QRect& rect)				
{	 
	m_rect = rect;
}