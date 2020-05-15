#ifndef AGPOLYGONITEM_H
#define AGPOLYGONITEM_H

#include "agItemBase.h"

class agSWInfoObject;
class agPolygonItem : public agItemBase
{
	Q_OBJECT
public:
	agPolygonItem(agSWInfoObject* swObj);
	~agPolygonItem();

public:

	void	mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
	void	mouseMoveEvent(QGraphicsSceneMouseEvent *event){event->accept();}
	void	mousePressEvent ( QGraphicsSceneMouseEvent * event);
	void	paint( QPainter *painter, const QStyleOptionGraphicsItem *, QWidget * );
	
	QRectF	boundingRect() const;
public:
	void	setPolygon(const QPolygonF &polygon);
	void	setText(QString& str);
	void	setRect( const QRect& rect);
signals:
	void	sendConnectTabObj();

private:
	QString		m_str;
	QRect		m_rect;	

public:
	QPolygonF	polygon() const;

protected:
	QPolygonF	m_polygon;
	
};

#endif // AGPOLYGONITEM_H
