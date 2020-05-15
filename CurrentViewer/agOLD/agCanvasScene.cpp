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

agLineItem* agCanvasScene::mainLastLine()									// 이 함수가 가능한 이유는 SDS에서 좌측 -> 우측선로 순으로 라인을 저장하기 때문
{																			// 위 공식이 깨지면 아무짝에 쓸모없음.. 망할 서버/클라이언트.. 애니게이트여....제발 좀 서버 개념좀 가지자....
	QList<QGraphicsItem*> allItems = items();
	agLineItem* LastLine = mainCBTargetLine();	
	int nSize = allItems.count();											 // 선 / 서클/ 텍스트 등등 모든 아이템의 개수			

	sp_DlInfoObj obj = ((agCanvasWidget*)views().first())->managerObj();     // first 라는 것은 하나의 씬은 여러개의 뷰를 가지는 것인가??
	int nLastSourceId;
	for(int i = nSize - 1 ; i >= 0 ; --i)									 // 뒤에 부터 저장한다 스택 구조? 꺼낼 때는 맨 앞의 것이 됨	
	//foreach(QGraphicsItem* item, allItems) 
	{
		//agLineItem* line = dynamic_cast<agLineItem*>(item);
		agLineItem* line = dynamic_cast<agLineItem*>(allItems.at(i));		 // 선로 아이템만 추출 CB만 있으면 널 리턴
		if(line) 
		{		
			if( LastLine->getLinePolygon()[0].y() == line->getLinePolygon()[0].y() )	// 간선만 구하기위해 Y를 같은 녀석 추출	
			{
				if( line->lineSourceId().toInt() == 0 )									// 라인의 소스 측 ID가 0이라는 것은? ID = 0 인 더미... 하아.. 개시팔 시팔..시팔...시팔....시팔....
				{
					m_MainSourceLine.append(qMakePair(nLastSourceId, line->lineId().toInt()));		
				}
				else
				{
					sp_SwInfoObj Sw = obj->GetObj( line->lineSourceId().toInt() );			// 대표선을 추출하기 위한 부분
					if( Sw->GetObjName() != "Dummy" )
					{
						m_MainSourceLine.append(qMakePair(line->lineSourceId().toInt(), line->lineId().toInt())); // 더미를 안만났을 경우 간선의 소스측 개쳬기를 저장
						nLastSourceId = line->lineSourceId().toInt();
					}
					else
					{
						m_MainSourceLine.append(qMakePair(nLastSourceId, line->lineId().toInt()));	// Key: 간선의 시작 개폐기 Value: 시작 개폐기랑 연결된 간선
					}											
				}
				// 간선 Item만 구분하여 저장
				collectMainLine(line);
				
				if( line->getLinePolygon()[0].x() > LastLine->getLinePolygon()[0].x() )		// X 좌표가 더 크다는 것은 CB 다음에 존재하는 간선이라는 것
					LastLine = line;														// 뒤의 라인을 탐색하면서 전진								
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
	// 컨트롤키와 마우스 왼쪽 버튼의 조합으로 영역선택 되는 기능이나 그래픽스뷰의 옵션으로 영역선택을 할 수 있다 사용안해도 되는 기능
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
	// 컨트롤키와 마우스 왼쪽 버튼의 조합으로 영역선택 되는 기능이나 그래픽스뷰의 옵션으로 영역선택을 할 수 있다 사용안해도 되는 기능
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
	// 컨트롤키와 마우스 왼쪽 버튼의 조합으로 영역선택 되는 기능이나 그래픽스뷰의 옵션으로 영역선택을 할 수 있다..현재 사용안해도 되는 기능
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
		// first 선택영영의 agLineItem을 모집하는 기능
		m_selRect = QRectF();
		update();
	}

	// 씬 안에 탭뷰를 넣어 주어 보자... 
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
	// 구간별 라인을 값이 큰 순서대로 담으려고 추가 
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

	//중복되는것이 나온다. 제거하자.
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
			//더미 일 수도있다.
			QList<sp_SwInfoObj>* dummies = ((agCanvasWidget*)views().first())->managerObj()->getDummyList();
			foreach(sp_SwInfoObj dummy, *dummies) 
			{
				if( dummy->getSourceSecId() == lineId)
				{
					/*|| dummy->getTargetSecId() == lineId
					|| dummy->getUpSecId() == lineId
					|| dummy->getDownSecId() == lineId ) {*/ //잘못됐다. 더미일경우 소스(라인)로부터 찾아가는 것이기 때문에 소스만 비교하면 돼는 것이었다.
					result.append(dummy);
					break;
				}
			}
			return result;
		}

		sp_SwInfoObj targetSw = mobj->GetObj(lineObj->getTargetSwitch());
		if(targetSw->getSwId() != blockNodeId)
		{ //탐색제한 노드도 아니고, 엔드포인트도 아니다.
			//먼저 요녀석이 그룹에 속한 것인지.
			agSWInfoObject* pPadData = targetSw->getPadParentData();
			if(pPadData) 
			{
				//pad 그룹멤버다!. 소속그룹의 노드들을 리턴하자.
				QList<sp_SwInfoObj>* pChildList = pPadData->getPadData();
				for( int i = 0 ; i < pChildList->count() ; i++ )
				{
					if(pChildList->at(i)->getSecId() != lineId && pChildList->at(i)->getSecId() != -1)
						result.append(pChildList->at(i));
				}
				return result;
			}
			// 아니라면, 단순한 일반 노드이다.
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
		//더미면, 상-하-좌-우 최대 4개의 연결선을 갖는다.
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
		//pad 그룹멤버다! 멤버별 연결선들이 필요하다.
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
		//아이디가 0이 아닌 일반노드이니
		if(node->getTargetSecId() > 0 && !m_internalLoopLines.contains(node->getTargetSecId()))
			result.append(node->getTargetSecId());
		return result;
	}

	//그외에는 엔드포인트일 뿐이다. 아무것도 없는 빈 리스트를 리턴하자.
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
