#ifndef AGPOLYLINEITEM_H
#define AGPOLYLINEITEM_H

#include "agItemBase.h"

class agPolyLineItem : public agItemBase
{
	Q_OBJECT
public:
	agPolyLineItem();
	~agPolyLineItem();

public:
	void	mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
	void	mousePressEvent ( QGraphicsSceneMouseEvent * );
	void	paint( QPainter *painter, const QStyleOptionGraphicsItem *, QWidget * );
	QRectF	boundingRect() const;

public:
	void	setPolygon(const QPolygonF &polygon);

private:
	QString		m_str;
	QRect		m_rect;	

public:
	QPolygonF	polygon() const;

protected:
	QPolygonF	m_polygon;
	
	
};

#endif // AGPOLYLINEITEM_H
