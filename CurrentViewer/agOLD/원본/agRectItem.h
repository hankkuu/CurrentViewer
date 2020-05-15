#ifndef AGRECTITEM_H
#define AGRECTITEM_H

#include "agItemBase.h"

class agSWInfoObject;
class agRectItem : public agItemBase
{
	Q_OBJECT
public:
	agRectItem(agSWInfoObject* swObj);
	~agRectItem();

protected:
	void	mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
	void	mouseMoveEvent(QGraphicsSceneMouseEvent *event){event->accept();}
	void	mousePressEvent ( QGraphicsSceneMouseEvent * event);
	
public:
	void			paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * );
	void	setRotate(int nRotate);

	void	setWidth(int nWidth) { m_nWidth = nWidth; }
	void	setHeight(int nHeight) { m_nHeight = nHeight; }

	int		getWidth() { return m_nWidth; }
	int		getHeight() { return m_nHeight; }

signals:
	void	sendConnectTabObj();
private:
	int				m_nRotate;

	int				m_nWidth;
	int				m_nHeight;
	
};

#endif // AGRECTITEM_H
