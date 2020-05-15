#include "StdAfx.h"
#include "agCanvasScene.h"
#include "document.h"

//#include <QGraphicsSceneMouseEvent>

agCanvasScene::agCanvasScene(QObject *parent) : QGraphicsScene(parent)
{
	m_selRect = QRectF();
}

agCanvasScene::~agCanvasScene()
{

}

/*
void agCanvasScene::setFocus(const sp_CircleItem sp)
{
	sp->setFocus();
}
*/

/*
void agCanvasScene::dragMoveEvent( QGraphicsSceneDragDropEvent * event )
{
	agCanvasScene::dragMoveEvent(event);
}*/
/*
void agCanvasScene::focusInEvent( QFocusEvent * focusEvent )
{
	if( focusEvent->reason() == Qt::MouseFocusReason )
	{
		int aa = 0;
	}

	QGraphicsScene::focusInEvent(focusEvent);
}*/

QList<agLineItem*> agCanvasScene::lineItems() 
{
	QList<QGraphicsItem*> LineItems = items();
	QList<agLineItem*> ResultLine;
	//bool isAnySelected = false;
	foreach(QGraphicsItem* item, LineItems) 
	{
		agLineItem* line = dynamic_cast<agLineItem*>(item);
		if(line) 
		{
			ResultLine.append(line);
			collectLineID( line->lineId().toInt() );

			//if(line->isSelected() && !isAnySelected)
			//	isAnySelected = true;
		}
	}
	return ResultLine;
}

agLineItem* agCanvasScene::mainCBTargetLine()
{	
	QList<QGraphicsItem*> LineItems = items();
	agLineItem* ResultCBLine;
	ResultCBLine = NULL;
	foreach(QGraphicsItem* item, LineItems) 
	{
		agLineItem* line = dynamic_cast<agLineItem*>(item);
		if(line) 
		{		
			QPolygonF CBLine  = line->CBLinePolygon();
			if( !CBLine.isEmpty() )
			{
				ResultCBLine = line;
			} 		
		}
	}
	return ResultCBLine;
}

agLineItem* agCanvasScene::mainLastLine()									// �� �Լ��� ������ ������ SDS���� ���� -> �������� ������ ������ �����ϱ� ����
{																			// �� ������ ������ �ƹ�¦�� �������.. ���� ����/Ŭ���̾�Ʈ.. �ִϰ���Ʈ��....���� �� ���� ������ ������....
	QList<QGraphicsItem*> allItems = items();
	agLineItem* LastLine = mainCBTargetLine();	
	int nSize = allItems.count();											 // �� / ��Ŭ/ �ؽ�Ʈ ��� ��� �������� ����			

	sp_DlInfoObj obj = ((agCanvasWidget*)views().first())->managerObj();     // first ��� ���� �ϳ��� ���� �������� �並 ������ ���ΰ�??
	int nLastSourceId;
	for(int i = nSize - 1 ; i >= 0 ; --i)									 // �ڿ� ���� �����Ѵ� ���� ����? ���� ���� �� ���� ���� ��	
	//foreach(QGraphicsItem* item, allItems) 
	{
		//agLineItem* line = dynamic_cast<agLineItem*>(item);
		agLineItem* line = dynamic_cast<agLineItem*>(allItems.at(i));		 // ���� �����۸� ���� CB�� ������ �� ����
		if(line) 
		{		
			if( LastLine->getLinePolygon()[0].y() == line->getLinePolygon()[0].y() )	// ������ ���ϱ����� Y�� ���� �༮ ����	
			{
				if( line->lineSourceId().toInt() == 0 )									// ������ �ҽ� �� ID�� 0�̶�� ����? ID = 0 �� ����... �Ͼ�.. ������ ����..����...����....����....
				{
					m_MainSourceLine.append(qMakePair(nLastSourceId, line->lineId().toInt()));		
				}
				else
				{
					sp_SwInfoObj Sw = obj->GetObj( line->lineSourceId().toInt() );			// ��ǥ���� �����ϱ� ���� �κ�
					if( Sw->GetObjName() != "Dummy" )
					{
						m_MainSourceLine.append(qMakePair(line->lineSourceId().toInt(), line->lineId().toInt())); // ���̸� �ȸ����� ��� ������ �ҽ��� ���Ǳ⸦ ����
						nLastSourceId = line->lineSourceId().toInt();
					}
					else
					{
						m_MainSourceLine.append(qMakePair(nLastSourceId, line->lineId().toInt()));	// Key: ������ ���� ����� Value: ���� ������ ����� ����
					}											
				}
				// ���� Item�� �����Ͽ� ����
				collectMainLine(line);
				
				if( line->getLinePolygon()[0].x() > LastLine->getLinePolygon()[0].x() )		// X ��ǥ�� �� ũ�ٴ� ���� CB ������ �����ϴ� �����̶�� ��
					LastLine = line;														// ���� ������ Ž���ϸ鼭 ����								
			} 				
		}
	}
	return LastLine;
}

void agCanvasScene::collectMainLine(agLineItem* line)
{
	m_lstMainLineID.push_back(line->lineId().toInt());
	m_lstMainLines.append(line);
}

void agCanvasScene::collectLineID(int nLineID)
{
	m_lstTotalLineID.push_back(nLineID);
}

QList<agLineItem*> agCanvasScene::selectedLineItems() 
{
	QList<QGraphicsItem*> LineItems = items();
	QList<agLineItem*> ResultLine;
	
	//bool isAnySelected = false;
	foreach(QGraphicsItem* item, LineItems) 
	{
		agLineItem* line = dynamic_cast<agLineItem*>(item);
		if(line && line->isSelected())
		{
			ResultLine.append(line);
			//if(line->isSelected() && !isAnySelected)
			//	isAnySelected = true;
		}
	}
	return ResultLine;
}

QString agCanvasScene::captionOfSwitch(int _swId) const
{
	if(_swId > 0) 
	{
		sp_DlInfoObj mobj = ((agCanvasWidget*)views().first())->managerObj();
		sp_SwInfoObj swObj = mobj->GetObj(_swId);
		agSWInfoObject* pPadData = swObj->getPadParentData();
		if(pPadData) 
		{
			return pPadData->GetCaptionTextName();
		}
		return swObj->GetCaptionTextName();
	}
	return QString();
}

QMap<int, QString> agCanvasScene::selectedCaptions(const QList<agLineItem*>& _lines)
{
	QMap<int, QString> result;
	QList<QString> names;
	sp_DlInfoObj mobj = ((agCanvasWidget*)views().first())->managerObj();
	sp_SwInfoObj lineObj;
	int srcSwId, destSwId;
	QString name;
	int nIdx = 0;
	foreach(agLineItem* lineItem, _lines) 
	{
		lineObj = mobj->GetObj(lineItem->lineId().toInt());
		srcSwId = lineObj->getSourceSwitch();
		destSwId = lineObj->getTargetSwitch();
		name = captionOfSwitch(srcSwId);
		if(!name.isEmpty())
			names.append(name);
		name = captionOfSwitch(destSwId);
		if(!name.isEmpty())
			names.append(name);
	}
	//int reverse = names.size();
	for(int i = 0 ; i < names.count() ; i++)
	{
		if(names.indexOf(names.at(i)) == names.lastIndexOf(names.at(i)))
		{
			//result.append(names.at(i));
			result.insert(nIdx,names.at(i));
			nIdx++;
		}
	}
	return result;
}

void agCanvasScene::deleteAllLine()
{
	QList<QGraphicsItem*> myItem = items();
	QList<agLineItem*> ResultLine;
	foreach(QGraphicsItem* item, myItem)
	{
		agLineItem* line = dynamic_cast<agLineItem*>(item);
		
		if( line && line->isSelected() )
		{
			line->setSelectLine(false);
		}	
	}
}

void agCanvasScene::mousePressEvent(QGraphicsSceneMouseEvent* _e)
{
	QGraphicsScene::mousePressEvent(_e);
	// ��Ʈ��Ű�� ���콺 ���� ��ư�� �������� �������� �Ǵ� ����̳� �׷��Ƚ����� �ɼ����� ���������� �� �� �ִ� �����ص� �Ǵ� ���
	if(_e->modifiers() == Qt::ControlModifier && _e->button() == Qt::LeftButton)	
	{
		m_pt1 = _e->scenePos();
	}
}

void agCanvasScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _e)
{
	QGraphicsScene::mouseDoubleClickEvent(_e);
}

void agCanvasScene::mouseMoveEvent(QGraphicsSceneMouseEvent* _e)
{
	QGraphicsScene::mouseMoveEvent(_e);
	// ��Ʈ��Ű�� ���콺 ���� ��ư�� �������� �������� �Ǵ� ����̳� �׷��Ƚ����� �ɼ����� ���������� �� �� �ִ� �����ص� �Ǵ� ���
	if(_e->modifiers() == Qt::ControlModifier && _e->buttons() & Qt::LeftButton) 
	{
		qreal x, y, w, h;
		m_pt2 = _e->scenePos();
		x = m_pt1.x() > m_pt2.x() ? m_pt2.x() : m_pt1.x();
		y = m_pt1.y() > m_pt2.y() ? m_pt2.y() : m_pt1.y();
		w = qAbs(m_pt1.x() - m_pt2.x());
		h = qAbs(m_pt1.y() - m_pt2.y());
		m_selRect = QRectF(x, y, w, h);
		update();
	}
}
void agCanvasScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* _e)
{
	QGraphicsScene::mouseReleaseEvent(_e);
	// ��Ʈ��Ű�� ���콺 ���� ��ư�� �������� �������� �Ǵ� ����̳� �׷��Ƚ����� �ɼ����� ���������� �� �� �ִ�..���� �����ص� �Ǵ� ���
	if( _e->modifiers() == Qt::ControlModifier && _e->button() == Qt::LeftButton) 
	{
		//qDebug("MOVE %f %f %f %f", m_pt1.x(), m_pt1.y(), m_pt2.x(), m_pt2.y());
		agLineItem* line;
		foreach(QGraphicsItem* item, items(m_selRect))
		{
			line = dynamic_cast<agLineItem*>(item);
			if(line)
				line->setSelectLine(true);
		}
		// first ���ÿ����� agLineItem�� �����ϴ� ���
		m_selRect = QRectF();
		update();
	}

	// �� �ȿ� �Ǻ並 �־� �־� ����... 
	TabView* tabView = getTabView();
	tabView->sumSectionValue();
}

void agCanvasScene::drawForeground(QPainter* _p, const QRectF& _r)
{
	Q_UNUSED(_r);
	if(m_selRect.isValid()) 
	{
		_p->setPen(Qt::white);
		_p->drawRect(m_selRect);
	}	
}

QList<int> agCanvasScene::mainRepresent()
{
	sp_DlInfoObj obj = ((agCanvasWidget*)views().first())->managerObj();
	QList<int> mainLines;
	int mainLine;

	QList<QPair<int, int> > mainSource = getMainSourceLine();
	for(int i = 0; i < m_MainSourceLine.count(); i++)
	//foreach(int nSourceId, mainSource.keys())
	{
		sp_SwInfoObj mainSw = obj->GetObj(m_MainSourceLine.at(i).first);
		
		if( mainSw->GetObjName().toLower() == "cb" )
		{
			mainLine = mainSw->getSourceSecId();
			mainLines.push_back(mainLine);
		}
		else
		{
			agSWInfoObject* pPadData = mainSw->getPadParentData();
			if( pPadData )
				mainLine = mainSw->getSecId();
			else
				mainLine = mainSw->getTargetSecId();

			if( mainLine != -1 && mainLine != 0 && !mainLines.contains(mainLine) )
				mainLines.push_back(mainLine);
		}
	}
	
	return mainLines; 
}

int agCanvasScene::targetSwId(int keyIdx)
{
	sp_DlInfoObj mobj = ((agCanvasWidget*)views().first())->managerObj();
	QList<int> lines;
	for(int i = 0; i < m_MainSourceLine.count(); i++) 
	{
		if(keyIdx == m_MainSourceLine.at(i).first)
			lines.append(m_MainSourceLine.at(i).second);
	}
	if(lines.count() > 0) 
	{
		return mobj->GetObj(lines.last())->getTargetSwitch();
	}
	return -1;
}

QList<QPair<int, qreal> > agCanvasScene::represntTotal()
{
	QList<QPair<int, qreal> > result;
	int keyIdx = -1;
	int targetSw;
	qreal total;
	// ������ ������ ���� ū ������� �������� �߰� 
	QList<int> sectionLine;
	//QMap<int, QList<int>> m_sectionLineMap;
	int nSectionIdx = 0;
	QList<qreal> sectionData;

	for(int i = 0; i < m_MainSourceLine.count(); i++)
	{
		if(keyIdx != -1 && keyIdx == m_MainSourceLine.at(i).first)
			continue;
		keyIdx = m_MainSourceLine.at(i).first;
		targetSw = targetSwId(keyIdx);
		total = totalOfRepresent(m_MainSourceLine.at(i).second, targetSw, sectionLine);
		result.append(qMakePair(keyIdx, total));
		sectionData.push_back(total);

		m_sectionLineMap.insert(total, sectionLine);
	}
	return result;
}
qreal agCanvasScene::totalOfRepresent(int represent, int blockNodeId, QList<int>& sectionLine)
{
	sectionLine.clear();
	qreal result = 0;
	QList<int> ids;
	sp_DlInfoObj mobj = ((agCanvasWidget*)views().first())->managerObj();
	sp_SwInfoObj lineObj = mobj->GetObj(represent);
	sp_SwInfoObj srcObj = mobj->GetObj(lineObj->getSourceSwitch());

#define SEARCH_ON
#ifdef SEARCH_ON
	agSWInfoObject* pPadData = srcObj->getPadParentData();
	if(pPadData)
	{
		QList<sp_SwInfoObj>* pChildList = pPadData->getPadData();
		int cc = 0;
		for( int i = 0 ; i < pChildList->count() ; i++ ) 
		{
			if(!m_lstMainLineID.contains(pChildList->at(i)->getSecId())
				&& !m_internalLoopLines.contains(pChildList->at(i)->getSecId())
				&& pChildList->at(i)->getSecId() != -1
				&& pChildList->at(i)->getSecId() != represent)
			{
				loopAppendLine(&ids, pChildList->at(i)->getSecId(), blockNodeId, true);
			}
		}
	}
	loopAppendLine(&ids, represent, blockNodeId, true);

	//�ߺ��Ǵ°��� ���´�. ��������.
	QList<int> resultIds;
	for(int i = 0; i < ids.count(); i++) 
	{
		if(ids.indexOf(ids.at(i), i) == ids.lastIndexOf(ids.at(i)) && !resultIds.contains(ids.at(i)))
		{
			resultIds.append(ids.at(i));
			
		} 
		else 
			qDebug("dupplicated ID = %d", ids.at(i));
	}

	//qDebug("GROUP COUNT %d", resultIds.count());
	QList<agLineItem*> lines = lineItems();
	foreach(int id, resultIds)
	{
		//qDebug("   => id %d", id);
		for(int i = 0; i < lines.count(); i++) 
		{
			if(lines.at(i)->lineId().toInt() == id)
			{
				result += lines.at(i)->getChartPValue();
				sectionLine.append(id);
				break;
			}
		}
	}
#endif
	return result;
}

QList<sp_SwInfoObj> agCanvasScene::targetNodes(int lineId, int blockNodeId)
{
	QList<sp_SwInfoObj> result;

	if(lineId > 0 && !m_internalLoopLines.contains(lineId))
	{
		sp_DlInfoObj mobj = ((agCanvasWidget*)views().first())->managerObj();
		sp_SwInfoObj lineObj = mobj->GetObj(lineId);
		if(lineObj->getTargetSwitch() == 0) 
		{
			//���� �� �����ִ�.
			QList<sp_SwInfoObj>* dummies = ((agCanvasWidget*)views().first())->managerObj()->getDummyList();
			foreach(sp_SwInfoObj dummy, *dummies) 
			{
				if( dummy->getSourceSecId() == lineId)
				{
					/*|| dummy->getTargetSecId() == lineId
					|| dummy->getUpSecId() == lineId
					|| dummy->getDownSecId() == lineId ) {*/ //�߸��ƴ�. �����ϰ�� �ҽ�(����)�κ��� ã�ư��� ���̱� ������ �ҽ��� ���ϸ� �Ŵ� ���̾���.
					result.append(dummy);
					break;
				}
			}
			return result;
		}

		sp_SwInfoObj targetSw = mobj->GetObj(lineObj->getTargetSwitch());
		if(targetSw->getSwId() != blockNodeId)
		{ //Ž������ ��嵵 �ƴϰ�, ��������Ʈ�� �ƴϴ�.
			//���� ��༮�� �׷쿡 ���� ������.
			agSWInfoObject* pPadData = targetSw->getPadParentData();
			if(pPadData) 
			{
				//pad �׷�����!. �Ҽӱ׷��� ������ ��������.
				QList<sp_SwInfoObj>* pChildList = pPadData->getPadData();
				for( int i = 0 ; i < pChildList->count() ; i++ )
				{
					if(pChildList->at(i)->getSecId() != lineId && pChildList->at(i)->getSecId() != -1)
						result.append(pChildList->at(i));
				}
				return result;
			}
			// �ƴ϶��, �ܼ��� �Ϲ� ����̴�.
			result.append(targetSw);
		}
	}
	return result;
}
QList<int> agCanvasScene::nodeTargetLines(sp_SwInfoObj node, int exceptLineId)
{
	QList<int> result;
	if(((agCanvasWidget*)views().first())->managerObj()->getDummyList()->contains(node)) 
	{
		//���̸�, ��-��-��-�� �ִ� 4���� ���ἱ�� ���´�.
		if(node->getSourceSecId() != -1 && node->getSourceSecId() != exceptLineId && !m_internalLoopLines.contains(node->getSourceSecId()))
			result.append(node->getSourceSecId());
		if(node->getTargetSecId() != -1 && node->getTargetSecId() != exceptLineId && !m_internalLoopLines.contains(node->getTargetSecId()))
			result.append(node->getTargetSecId());
		if(node->getUpSecId() != -1 && node->getUpSecId() != exceptLineId && !m_internalLoopLines.contains(node->getUpSecId()))
			result.append(node->getUpSecId());
		if(node->getDownSecId() != -1 && node->getDownSecId() != exceptLineId && !m_internalLoopLines.contains(node->getDownSecId()))
			result.append(node->getDownSecId());
		return result;
	}
	agSWInfoObject* pPadData = node->getPadParentData();
	if(pPadData) 
	{
		//pad �׷�����! ����� ���ἱ���� �ʿ��ϴ�.
		QList<sp_SwInfoObj>* pChildList = pPadData->getPadData();
		for( int i = 0 ; i < pChildList->count() ; i++ )
		{
			sp_SwInfoObj pChildren = pChildList->at(i);
			bool cond = pChildren->getSecId() == -1 || pChildren->getSecId() == exceptLineId;
			cond = cond || m_internalLoopLines.contains(pChildren->getSecId());
			//cond = cond || (pChildren == node);
			//if(pChildren->getSecId() == -1  || !m_internalLoopLines.contains(pChildren->getSecId()) || (pChildren == node))
			if(cond)
				continue;
			result.append(pChildren->getSecId());
		}
		return result;
	}
	if(node->getSwId() != 0) 
	{
		//���̵� 0�� �ƴ� �Ϲݳ���̴�
		if(node->getTargetSecId() > 0 && !m_internalLoopLines.contains(node->getTargetSecId()))
			result.append(node->getTargetSecId());
		return result;
	}

	//�׿ܿ��� ��������Ʈ�� ���̴�. �ƹ��͵� ���� �� ����Ʈ�� ��������.
	return result;
}

void agCanvasScene::loopAppendLine(QList<int>* totalTargetLines, int findLineId, int blockNodeId, bool addSelf)
{
	int i;
	QList<sp_SwInfoObj> nodeObjs = targetNodes(findLineId, blockNodeId);
	
	if(addSelf)
		totalTargetLines->append(findLineId);

	//qDebug("##=> Search route::Current[%d]", findLineId);
	QList<int> lineIds;
	foreach(sp_SwInfoObj nodeObj, nodeObjs)
	{
		lineIds = nodeTargetLines(nodeObj, findLineId);
		foreach(int lineId, lineIds)
		{
			loopAppendLine(totalTargetLines, lineId, blockNodeId, true);
		}
	}
}
