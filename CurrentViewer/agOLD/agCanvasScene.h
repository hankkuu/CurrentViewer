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
	agLineItem* mainCBTargetLine();										// CB�� Ÿ�� ���θ� �����Ѵ�

	QList<agLineItem*> lineItems();										// DL���� ��� ���� �������� �����Ѵ�
	void	   collectLineID(int nLineID);								// DL���� ����ID�� ���� 	
	QList<int> getLineIDList() { return m_lstTotalLineID; }				// DL���� ����ID�� �����Ѵ�

	agLineItem* mainLastLine();											// ������ ������ ���θ� �����Ѵ�
	void collectMainLine(agLineItem* line);								// ���� ���θ� ����
	QList<agLineItem*> getMainLineItem() { return m_lstMainLines; }		// ���� LineItem�� ����
	QList<int> getMainLineIDList() { return m_lstMainLineID; }			// ���� ����ID�� ����

	QList<QPair<int, int> > getMainSourceLine() { return m_MainSourceLine; }  // ���� ���� ���� ����

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

public:													// Ž���κ�
	int mainOfMain(int mainLine);						// ���ڴ� �����̾�� �Ѵ�  // ������ ��忩�� �Ѵ�	// 
	bool isMainLine(int lineId);						// 	���õ� ������ �������� Ȯ���Ѵ�
	int seletionIsWhat(QList<agLineItem*> selLines);	// ������ �����ϴ� �Լ�  // ���� ���Լ��� ������ ����/����/�������� �������� Ȯ���Ѵ� 
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
	QList<int> m_lstTotalLineID;								// ��ü ����ID
	QList<agLineItem*> m_lstMainLines;							// ���� ����Item
	QList<int> m_lstMainLineID;									// ���� ����ID

	QList<QPair<int, int> > m_MainSourceLine;					// ���ù߻���1
	QList<int> m_internalLoopLines;								// ���ù߻���2

	QPointF m_pt1;												// ���콺 �巡�׸� ���� ���� �༮ �������� �ڵ����� �Լ��� ������ �Ⱦ���
	QPointF m_pt2;
	QRectF m_selRect;

	TabView* m_pTabView;										// ���� ���� ���� �ջ� ��ġ�� �ѱ�� ���ؼ� �߰�

private:
	QList<int> mainRepresent();            // ������ �ϴ� ������ ����.. 

	int targetSwId(int _keyIdx);
	qreal totalOfRepresent(int _represent, int _blockNodeId, QList<int>& sectionLine);
	QList<sp_SwInfoObj> targetNodes(int _lineId, int _blockNodeId); //����/������ ����� ������ ����Ʈ(�Ϲ������� �ϳ�������, �е�ó�� �׷����� �Ǿ��������)
	QList<int> nodeTargetLines(sp_SwInfoObj _node, int _exceptLineId); //��忡 ����� ����
	void loopAppendLine(QList<int>* _totalTargetLines, int _findLineId, int _blockNodeId, bool _addSelf);
};

#endif // AGCANVASSCENE_H
