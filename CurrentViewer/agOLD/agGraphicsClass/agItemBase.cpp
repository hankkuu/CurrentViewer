#include "StdAfx.h"
#include "agItemBase.h"

agItemBase::agItemBase(QGraphicsItem* parent) : QGraphicsObject(parent)
{
	m_rt.setRect(0,0,100,50);
	// ��ü ���� ���콺�� �ø��� ȭ�� �̵��� �̵����� �ʾҴ� ������ �Ʒ� �ɼ�ó���� �ذ���.
	//setAcceptedMouseButtons(false);
}

agItemBase::~agItemBase()
{

}

QRectF agItemBase::boundingRect() const
{
	return m_rt;
}
void	agItemBase::setRect( const QRectF& rect)				
{	 
	m_rt = rect;
}

void	agItemBase::setRect( qreal xPos, qreal yPos, qreal width, qreal height )			
{	 
	setRect( QRectF(xPos,yPos,width,height) ); 
}

void	agItemBase::setBrush(QBrush &brush)
{
	m_brush = brush;
}

void	agItemBase::setPen(QPen& pen)
{
	m_pen = pen;
}

QPen	agItemBase::getPen()
{
	return m_pen;
}

void	agItemBase::setFont(QFont& font)
{
	m_font = font;
}

void	agItemBase::setFontColor(QColor& color)
{
	m_fontColor = color;
}

void	agItemBase::setObjName(QString &strObj)
{
	m_strObj = strObj;
}
QString	agItemBase::getObjName()
{
	return m_strObj;
}