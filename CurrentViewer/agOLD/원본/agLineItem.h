#ifndef AGLINEITEM_H
#define AGLINEITEM_H
#include "agItemBase.h"

class agSWInfoObject;
class agLineItem : public agItemBase
{
	Q_OBJECT
public:
	agLineItem(agSWInfoObject* swObj);
	~agLineItem();

public:
	
	void	paint( QPainter *painter, const QStyleOptionGraphicsItem *s, QWidget * );
	
	QRectF	boundingRect() const;
	//void	setPolygon(const QRectF &polygon);
	void	setPolygon(const QPolygonF &polygon);
	qreal   getPolygonX();
	QPolygonF   getLinePolygon();


private:
	QString m_lineId;
	QString m_lineName;
 	QString m_SourceId; 
 	QString m_TargetId;
	ChartData m_LineCurrent;

public:
	QString lineId() const { return m_lineId; }
	void setLineId(const QString& Lineid) { m_lineId = Lineid; }

	QString lineName() const { return m_lineName; }
	void setLineName(const QString& Lineid) { m_lineName = Lineid; }

	QString lineSourceId() const { return m_SourceId; }
	void setLineSourceId(const QString& Lineid) { m_SourceId = Lineid; }

	QString lineTargetId() const { return m_TargetId; }
	void setLineTargetId(const QString& Lineid) { m_TargetId = Lineid; }

	int lineStyle() { return m_lineStyle; }
	void setLineStyle(int LineStyle ) { m_lineStyle = LineStyle; }

	bool CBLine() { return m_bCBLine; }
	void setCBLine(bool bCBLine) { m_bCBLine = bCBLine; }

	QPolygonF CBLinePolygon() {return m_CBLinePolygon; }
	void setCBLinePolygon(QPolygonF& polygon) { m_CBLinePolygon = polygon; }

	//ChartData* lineCurrent() const;
	//void setLineCurrent( ChartData* chartData );

	void selectShift();


	


protected:
	void	mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
	void	mousePressEvent ( QGraphicsSceneMouseEvent * event);
	void    mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

signals:
	void	sendConnectTabObj();
	void	SearchChangLine();

public:
	QPolygonF	m_polygon;
	//QRectF	m_polygon;
	QRectF		m_rect;	
	int			m_lineStyle;
	bool		m_bCBLine;
	QPolygonF	m_CBLinePolygon;
};

#endif // AGLINEITEM_H
