#ifndef AGCANVASSCENE_H
#define AGCANVASSCENE_H

#include <QGraphicsScene>
#include "agDmsDeviceClass/agobjectbase.h"

class agLineItem;
class TabView;
class agCanvasScene : public QGraphicsScene
{
	Q_OBJECT

public:
	agCanvasScene(QObject *parent);
	~agCanvasScene();

	void setTabView(TabView* tabView) { m_pTabView = tabView; }
	TabView* getTabView() { return m_pTabView; }
	/*void setFocus(const sp_CircleItem sp);*/
/*
protected:
	void	dragMoveEvent ( QGraphicsSceneDragDropEvent * event );*/

/*
protected:
	void focusInEvent( QFocusEvent * focusEvent );*/

public:
	agLineItem* mainCBTargetLine();										// CB의 타켓 라인만 리턴한다

	QList<agLineItem*> lineItems();										// DL내의 모든 라인 아이템을 리턴한다
	void	   collectLineID(int nLineID);								// DL내의 선로ID만 추출 	
	QList<int> getLineIDList() { return m_lstTotalLineID; }				// DL내의 선로ID만 리턴한다

	agLineItem* mainLastLine();											// 간선의 마지막 선로를 리턴한다
	void collectMainLine(agLineItem* line);								// 간선 선로만 추출
	QList<agLineItem*> getMainLineItem() { return m_lstMainLines; }		// 간선 LineItem만 리턴
	QList<int> getMainLineIDList() { return m_lstMainLineID; }			// 간선 선로ID만 리턴

	QList<QPair<int, int> > getMainSourceLine() { return m_MainSourceLine; }  // 거지 같은 시팍 리턴

public:
	QList<int>& internalLooopLines()	
	{ 		
		return m_internalLoopLines; 	
	}
	void appendInternalLoopLine(int _internalLineId) 
	{
		m_internalLoopLines.append(_internalLineId);
	}
	
	QList<agLineItem*> selectedLineItems();

	QMap<int, QString> selectedCaptions(const QList<agLineItem*>& _lines);
	QString captionOfSwitch(int _swId) const;
	
	QMap<int, QList<int>> getTestData() { return m_sectionLineMap; }
	QMap<int, QList<int>> m_sectionLineMap;

public:													// 탐색부분
	int mainOfMain(int mainLine);						// 인자는 간선이어야 한다  // 리턴은 노드여야 한다	// 
	bool isMainLine(int lineId);						// 	선택된 라인이 간선인지 확인한다
	int seletionIsWhat(QList<agLineItem*> selLines);	// 선택을 결정하는 함수  // 위의 세함수를 조합해 간선/지선/간선지선 선택인지 확인한다 
	int below_BelowNode(QList<agLineItem*> selLine);
	int below_UpperNode(QList<agLineItem*> selLines);
	int upper_BelowNode(QList<agLineItem*> selLines);
	int upper_UpperNode(QList<agLineItem*> selLines);

	QList<QPair<int, qreal> > represntTotal();

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* _e);
	void mouseMoveEvent(QGraphicsSceneMouseEvent* _e);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* _e);
	void drawForeground(QPainter* _p, const QRectF& _r);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _e);

public slots:
	void deleteAllLine();

private:
	QList<int> m_lstTotalLineID;								// 전체 라인ID
	QList<agLineItem*> m_lstMainLines;							// 간선 라인Item
	QList<int> m_lstMainLineID;									// 간선 라인ID

	QList<QPair<int, int> > m_MainSourceLine;					// 개시발색기1
	QList<int> m_internalLoopLines;								// 개시발색기2

	QPointF m_pt1;												// 마우스 드래그를 위해 만든 녀석 위젯에서 자동지원 함수가 있으니 안쓴다
	QPointF m_pt2;
	QRectF m_selRect;

	TabView* m_pTabView;										// 현재 구간 부하 합산 수치를 넘기기 위해서 추가

private:
	QList<int> mainRepresent();            // 씬에서 하는 것으로 변경.. 

	int targetSwId(int _keyIdx);
	qreal totalOfRepresent(int _represent, int _blockNodeId, QList<int>& sectionLine);
	QList<sp_SwInfoObj> targetNodes(int _lineId, int _blockNodeId); //간선/지선에 연결된 노드들의 리스트(일반적으로 하나이지만, 패드처럼 그룹으로 되어있을경우)
	QList<int> nodeTargetLines(sp_SwInfoObj _node, int _exceptLineId); //노드에 연결된 선들
	void loopAppendLine(QList<int>* _totalTargetLines, int _findLineId, int _blockNodeId, bool _addSelf);
};

#endif // AGCANVASSCENE_H
