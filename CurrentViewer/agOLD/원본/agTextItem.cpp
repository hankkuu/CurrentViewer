#include "StdAfx.h"
#include "agTextItem.h"

agTextItem::agTextItem()
{
	/*setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges,true);
	setFlag(QGraphicsItem::ItemStacksBehindParent,true);*/

	m_bIsCaption = false;
	m_bIsMark = false;
	m_nRotate = 0;
	setZValue(7);
}

agTextItem::~agTextItem()
{

}




void	agTextItem::setText(QString& str)
{
	m_str = str;
}

QString agTextItem::getText()
{
	return m_str;
}

void	agTextItem::Draw( QPainter* painter, QPointF offset,const QStyleOptionGraphicsItem *option/* =0 */ )
{
	painter->save();
	const qreal lod = QStyleOptionGraphicsItem::levelOfDetailFromTransform(painter->worldTransform());
	painter->setRenderHint(QPainter::Antialiasing);
	QBrush brush = m_brush;
	QColor fontcolor = m_fontColor;
	
	if ( !m_str.isEmpty() )
	{
		if ( lod *m_font.pointSize() > 3 )
		{	
			painter->setPen(QPen(fontcolor)); 
			painter->setFont(m_font);
			painter->setRenderHint(QPainter::TextAntialiasing,true);
			
			if( m_nRotate != 1 && m_nRotate != 2 )
			{
				painter->drawText(m_rt,Qt::AlignCenter,m_str);
			}			
			else			// 세로 출력
			{
				QRectF rt = m_rt;
				for( int  i = 0; i < m_str.length(); i++ )
				{
					QString strText = m_str.mid(i , 1);
					painter->drawText(rt, Qt::AlignHCenter,strText);

					QFontMetrics font(m_font);

					int nAdjusWidth = font.width(strText);
					int nAdjustHeight = font.height();
					
					rt.adjust(0 ,nAdjustHeight - 3, 0 , 0 );
				}
			}
		}
	}
	//painter->restore();
}

void		agTextItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * )
{
	qreal sizeMax = qMin(m_rt.size().width(),m_rt.size().height());

	const qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());

	if ( lod > 0.15)
		Draw(painter, QPointF(0,0));
}

QRectF agTextItem::boundingRect() const
{
	return m_rt;
}
void	agTextItem::setRect( const QRectF& rect)				
{	 
	m_rt = rect;
}

void	agTextItem::setRect( qreal xPos, qreal yPos, qreal width, qreal height )			
{	 
	setRect( QRectF(xPos,yPos,width,height) ); 
}

void	agTextItem::setBrush(QBrush &brush)
{
	m_brush = brush;
}

void	agTextItem::setPen(QPen& pen)
{
	m_pen = pen;
}


void	agTextItem::setFont(QFont& font)
{
	m_font = font;
}

void	agTextItem::setFontColor(QColor& color)
{
	m_fontColor = color;
}

void	agTextItem::setObjName(QString &strObj)
{
	m_strObj = strObj;
}
QString	agTextItem::getObjName()
{
	return m_strObj;
}

void	agTextItem::setRotate(int nRotate)
{
	m_nRotate = nRotate;
}