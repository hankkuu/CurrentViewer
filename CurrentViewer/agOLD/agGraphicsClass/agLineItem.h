#ifndef AGLINEITEM_H
#define AGLINEITEM_H

#include "agItemBase.h"

class agSWInfoObject;
typedef std::tr1::shared_ptr<agSWInfoObject>		sp_SwInfoObj;

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
	qreal   getPolygonX() 	
	{
		return m_polygon[0].x();
	}
	QPolygonF   getLinePolygon()
	{
		return m_polygon;
	}


private:
	//구간 부하 수치
	float m_chartDataA;
	float m_chartDataB;
	float m_chartDataC;
	QString m_chartDataDate;
	float m_PValue;

	QPolygonF	m_polygon;
	//QRectF	m_polygon;
	QRectF		m_rect;	

	bool		m_bCBLine;
	QPolygonF	m_CBLinePolygon;
	int			m_lineStyle;

	QString m_lineId;
	QString m_SourceId; 
	QString m_TargetId;

public:
	float chartDataA() const { return m_chartDataA; }
	float chartDataB() const { return m_chartDataB; }
	float chartDataC() const { return m_chartDataC; }
	QString chartDataDate() const { return m_chartDataDate; }   
	float getChartPValue() const { return m_PValue; }
	void setChartData(const float _a, const float _b, const float _c, const QString& _date);

	QString lineId() const { return m_lineId; }									// get을 붙일지 말지.. 좀 애매하다.. QT 함수는 get안쓰면 그쪽에 맞춤??
	void setLineId(const QString& Lineid) { m_lineId = Lineid; }
	QString lineSourceId() const { return m_SourceId; }
	void setLineSourceId(const QString& Lineid) { m_SourceId = Lineid; }
	QString lineTargetId() const { return m_TargetId; }
	void setLineTargetId(const QString& Lineid) { m_TargetId = Lineid; }

	// CB 라인 정보 파서에서 가져옴
	bool CBLine() { return m_bCBLine; }									// 본인 우측이 CB 즉 시작하는 라인
	void setCBLine(bool bCBLine) { m_bCBLine = bCBLine; }
	QPolygonF CBLinePolygon() {return m_CBLinePolygon; }
	void setCBLinePolygon(QPolygonF& polygon) { m_CBLinePolygon = polygon; }

public:
	// 사용안함
	int lineStyle() { return m_lineStyle; }
	void setLineStyle(int LineStyle ) { m_lineStyle = LineStyle; }

protected:
	void	mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
	void	mousePressEvent ( QGraphicsSceneMouseEvent * event);
	void    mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

signals:
	void	sendConnectTabObj();
	void	SearchChangLine();

public:
	void selectShift();
	void setSelectLine(bool _sw) 
	{
		setSelected(_sw);						// 선택 상태를 반전 시킴 헤더에서 선언해서 바로 사용 가능
	}

	bool isConnectedWithEndPoint();
	bool isRepresent();
	QList<int> dummyLines(int _findId);
	void loopAppend(QList<int>* _target, int _findId);
	QList<int> entireGroupIds();
};

#endif // AGLINEITEM_H
