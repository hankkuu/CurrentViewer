#ifndef AGCIRCLEITEM_H
#define AGCIRCLEITEM_H

#include "agItemBase.h"

class agSWInfoObject;
class agCircleItem : public agItemBase
{
	Q_OBJECT
public:
	agCircleItem(agSWInfoObject* swObj);
	~agCircleItem();
protected:
	void	mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
	void	mousePressEvent ( QGraphicsSceneMouseEvent * );

public:
	void	paint( QPainter *painter, const QStyleOptionGraphicsItem *, QWidget * );
	void	setRect( const QRectF& rect);
	void	setRect( qreal xPos, qreal yPos, qreal width, qreal height );
	QRectF	boundingRect() const;

signals:
	void	sendConnectTabObj();

private:
	
};

#endif // AGCIRCLEITEM_H
