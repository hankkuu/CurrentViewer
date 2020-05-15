#include "StdAfx.h"
#include "agLineItem.h"
#include "agOLD/agcanvaswidget.h"

//const int g_DIFF = 3;
agLineItem::agLineItem(agSWInfoObject* swObj) : agItemBase(0)
{
	//setFlag(QGraphicsItem::ItemSendsGeometryChanges,true);
	//setFlag(QGraphicsItem::ItemStacksBehindParent,true);
	//setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemIsFocusable , true);	
	setZValue(3);

	m_chartDataA = 0.0;				
	m_chartDataB = 0.0;				
	m_chartDataC = 0.0;
	m_chartDataDate = "";
	m_PValue = 0.0;
}

agLineItem::~agLineItem()
{
}

void	agLineItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	event->ignore();
	QGraphicsItem::mouseDoubleClickEvent(event);
}

void	agLineItem::mousePressEvent ( QGraphicsSceneMouseEvent * event)
{
	//event->ignore();
	//QGraphicsItem::mousePressEvent(event);
}

void agLineItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	if(event->modifiers() & Qt::ShiftModifier) 
	{
		selectShift();
		setSelectLine(true);
	} 
	else if ( event->button() != Qt::RightButton )
	{
		setSelectLine(!isSelected());
	}	
	update();
}

void agLineItem::selectShift()
{
	int i;
	int idx = -1;

	QList<agLineItem*> lines = ((agCanvasScene*)scene())->lineItems();

	//qDebug() << lines.count(); 
	if(m_polygon.first().y() == m_polygon.last().y()) //수평선 
	{
		for(i = 0; i < lines.count(); i++) 
		{
			if(lines.at(i) == this)
			{
				idx = i;
				break;
			}
		}

		if(idx != -1) 
		{
			int leftIdx = -1;
			int rightIdx = -1;
			// left   // Right 로 바뀜 
			for(i = idx+1; i < lines.count(); i++)  // 자기 자신 다음 부터 
			{
				if(lines[i]->isSelected() && boundingRect().y() == lines[i]->boundingRect().y())
					leftIdx = i;
			}
			// right  // left 로 바뀜
			for(i = idx-1; i >= 0; i--) 
			{
				if(lines[i]->isSelected() && boundingRect().y() == lines[i]->boundingRect().y())
					rightIdx = i;
			}
			// 왼쪽 오른쪽 둘다 선택된 것이 있을 경우??, 왼쪽이 오른쪽에 우선해야 하나?? 협의 필요.
			// 1. 아무것도 선택이 안되었다.
			//qDebug("L %d,  R %d", lIdx, rIdx);
			if(leftIdx == -1 && rightIdx == -1) 
			{
				////
			} 
			else if(leftIdx != -1)							
			{ //2. 왼쪽 선택된 것이 있다.
				//qDebug() << "leftSearch";
				for(i = idx + 1; i <= leftIdx; i++)
				{
					if(boundingRect().y() == lines[i]->boundingRect().y())
					{
						lines[i]->setSelectLine(true);
						//qDebug("%d", i);
					}
				}
			} 
			else if(rightIdx != -1) 
			{ //3. 오른쪽 선택된 것이 있다.
				//qDebug() << "RightSearch";
				for(i = rightIdx; i < idx; i++)
				{
					if(boundingRect().y() == lines[i]->boundingRect().y())
					{
						lines[i]->setSelectLine(true);
						//qDebug("%d", i);
					}
				}
			}
		}
	} 
	else if( m_polygon.first().x() == m_polygon.last().x() )//수직선 
	{
		/////
		for(i = 0; i < lines.count(); i++) 
		{
			if(lines.at(i) == this)
			{
				idx = i;
				break;
			}
		}

		if(idx != -1) 
		{
			int UpIdx = -1;
			int DownIdx = -1;
			// Up    // Down
			for(i = idx+1; i < lines.count(); i++)  // 자기 자신 다음 부터 
			{
				if(lines[i]->isSelected() && boundingRect().x() == lines[i]->boundingRect().x())
					DownIdx = i;  // Up
			}
			// Down    // Up
			for(i = idx-1; i >= 0; i--) 
			{
				if(lines[i]->isSelected() && boundingRect().x() == lines[i]->boundingRect().x())
					UpIdx = i;
			}
			// 위아래 둘다 선택된 것이 있을 경우??
			// 1. 아무것도 선택이 안되었다.
			//qDebug("down %d,  up %d", DownIdx, UpIdx);
			if(DownIdx == -1 && UpIdx == -1) 
			{
				////
			}		
			else if(DownIdx != -1)						// Up		
			{ //2. 위쪽 선택된 것이 있다.
				//qDebug() << "Upsearch";
				for(i = idx + 1; i <= DownIdx; i++)					//	up
				{
					if(boundingRect().x() == lines[i]->boundingRect().x())
					{
						lines[i]->setSelectLine(true);
						//qDebug("%d", i);
					}
				}
			} 
			else if(UpIdx != -1) 
			{ //3. 아래쪽 선택된 것이 있다.
				//qDebug() << "downsearch";
				for(i = UpIdx; i < idx; i++)		
				{
					if(boundingRect().x() == lines[i]->boundingRect().x())
					{
						lines[i]->setSelectLine(true);
						//qDebug("%d", i);
					}
				}
			}
		}
	}
}

void agLineItem::setChartData(const float _a, const float _b, const float _c, const QString& _date)
{
	m_chartDataA = _a * 13.2;				// 공칭 전압이라고 하는 22900을 루트3 약 1.7로 나누면 13200이 된다 이것을 킬로볼트로
	m_chartDataB = _b * 13.2;				// 역률 코사인세타는 1이라고 생각한다 일반적인 가정용 기준
	m_chartDataC = _c * 13.2;
	m_chartDataDate = _date;

	m_PValue = m_chartDataA + m_chartDataB + m_chartDataC;
}

QRectF	agLineItem::boundingRect() const
{
	const int plusArea = 10;
	return m_polygon.boundingRect().adjusted(-plusArea, -plusArea, plusArea, +plusArea);				// 수정한 부분
}

void	agLineItem::setPolygon(const QPolygonF& poly)
{
	// 	if(poly[0].y() == poly[1].y())						// boundingRect 이 함수 하나면 된다
	// 	{													// rect를 그리는 방식
	// 		if(poly[0].x() >= poly[1].x())
	// 		{			
	// 			m_rect.setLeft(poly[1].x());
	// 			m_rect.setRight(poly[0].x());			
	// 		}
	// 		else
	// 		{
	// 			m_rect.setLeft(poly[0].x());
	// 			m_rect.setRight(poly[1].x());
	// 		}
	// 		m_rect.setTop(poly[0].y() - g_DIFF);
	// 		m_rect.setBottom(poly[0].y() + g_DIFF);
	// 	}
	// 	else if(poly[0].x() == poly[1].x())
	// 	{
	// 		if(poly[0].y() >= poly[1].y())
	// 		{
	// 			m_rect.setTop(poly[1].y());
	// 			m_rect.setBottom(poly[0].y());
	// 		}
	// 		else
	// 		{
	// 			m_rect.setTop(poly[0].y());
	// 			m_rect.setBottom(poly[1].y());
	// 		}
	// 		m_rect.setLeft(poly[0].x()-g_DIFF);
	// 		m_rect.setRight(poly[0].x()+g_DIFF);
	// 	}
	// 	setRect(m_rect);

	m_polygon = poly;
	if ( m_polygon.isEmpty() )
		return;
	prepareGeometryChange();
	QPointF current = m_polygon.first();
	bool anti = false;
	for ( int i =1 ; i < m_polygon.size() ; i++ )
	{
		QPointF last = m_polygon[i];
		if ( current.x() != last.x() && current.y() != last.y() )
		{
			anti = true;
			break;
		}
		current = last;
	}
	update();
}

void	agLineItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *s, QWidget * )
{
	/*qreal sizeMax = qMin(m_rt.size().width(),m_rt.size().height());
	const qreal lod = s->levelOfDetailFromTransform(painter->worldTransform());
	if ( lod < 0.1 && m_strObj.compare("GroundLine") != 0 )
		return;*/

	// 선 그리기
	QPen pen = m_pen;
	pen.setWidth(2);
	pen.setColor(isSelected() ? Qt::yellow : Qt::cyan);
	painter->setPen(pen); 
	painter->setRenderHint(QPainter::Antialiasing, true);
	//m_polygon[1].setY(m_polygon[1].y() + 10);	
	//painter->drawPolyline(m_polygon);
	//QBrush brush(Qt::transparent);			khk
	//painter->fillRect(m_rect, brush);			khk	
	painter->drawPolygon(m_polygon);
	//painter->drawRect(m_rect);
	//painter->restore();
	//painter->drawEllipse(m_rt);

	// 부하값 표시

	QString txt;
	QRectF destR;
	//painter->pen().setColor(Qt::gray); -> 동작안됨
	pen.setColor(isSelected() ? Qt::yellow : Qt::green);
	painter->setPen(pen);
	QRectF srcR;

    if(isRepresent()) 
	{
		QList<int> group = entireGroupIds();
		qreal total = 0;
 		sp_DlInfoObj mobj = ((agCanvasWidget*)scene()->views().first())->managerObj();
 		for(int i = 0; i < group.count(); i++)
		{
			sp_SwInfoObj line = mobj->GetObj(group.at(i));

			//if( line)							// SDS가 그룹 라인정보 잘못만들면 망함.... 개시팔 로우데이터 병신이면 프로그램 이상해짐

			if( !line )			// 내부루프 제외
				continue;

 			total += line->lineObject()->getChartPValue();
 		}
		txt = QString::number(total) + QString("(%1)").arg(getChartPValue());
		srcR = painter->fontMetrics().boundingRect(txt);
		if(m_polygon.first().y() == m_polygon.last().y())
		{ // 수평선
			destR = QRectF(boundingRect().x(), boundingRect().y() + boundingRect().height() / 2 + 5, boundingRect().width(), srcR.height());
		} 
		else 
		{ //수직선
 			if(isConnectedWithEndPoint()) 
			{
				if(m_polygon.first().y() < m_polygon.last().y())
				{
					destR = QRectF(boundingRect().x() + boundingRect().width() / 2 + 5, m_polygon.last().y() - 30, srcR.width(), boundingRect().height());
				} 
				else
				{
					destR = QRectF(boundingRect().x() + boundingRect().width() / 2 + 5, m_polygon.last().y() - 16, srcR.width(), boundingRect().height());
				}
			} 
			else
				destR = QRectF(boundingRect().x() + boundingRect().width() / 2 + 5, boundingRect().y(), srcR.width(), boundingRect().height());
		}

		painter->drawText(destR, Qt::AlignCenter, txt);
	} 
	else 
	{
		txt = QString::number(m_PValue);
		srcR = painter->fontMetrics().boundingRect(txt);
		if(m_polygon.first().y() == m_polygon.last().y())
		{ // 수평선
			destR = QRectF(boundingRect().x(), boundingRect().y() + boundingRect().height() / 2 + 5, boundingRect().width(), srcR.height());
		} 
		else 
		{ //수직선
			if(isConnectedWithEndPoint()) 
			{
				if(m_polygon.first().y() < m_polygon.last().y()) 
				{
					destR = QRectF(boundingRect().x() + boundingRect().width() / 2 + 5, m_polygon.last().y() - 30, srcR.width(), boundingRect().height());
				} 
				else
				{
					destR = QRectF(boundingRect().x() + boundingRect().width() / 2 + 5, m_polygon.last().y() - 16, srcR.width(), boundingRect().height());
				}
			} 
			else
				destR = QRectF(boundingRect().x() + boundingRect().width() / 2 + 5, boundingRect().y(), srcR.width(), boundingRect().height());
		}

		painter->drawText(destR, Qt::AlignCenter, txt);
	}
}

bool agLineItem::isConnectedWithEndPoint()
{
	int nId = m_lineId.toInt();
	bool result = true;
	bool find;
	if(m_TargetId.toInt() == 0) 
	{
		sp_DlInfoObj obj = ((agCanvasWidget*)scene()->views().first())->managerObj();
		find = false;
		QList<sp_SwInfoObj>* dummies = obj->getDummyList();
		for(int i = 0; i < dummies->count() && !find; i++)
		{
			sp_SwInfoObj dummy = dummies->at(i);
			find = dummy->getSourceSecId() == nId || dummy->getTargetSecId() == nId || dummy->getUpSecId() == nId || dummy->getDownSecId() == nId;
		}
		if(!find)
			result = false;
	}
	return !result;
}

bool agLineItem::isRepresent()
{
	sp_DlInfoObj obj = ((agCanvasWidget*)scene()->views().first())->managerObj();
	sp_SwInfoObj srcObj = obj->GetObj(m_SourceId.toInt());
	sp_SwInfoObj destObj = obj->GetObj(m_TargetId.toInt());
	return srcObj->GetObjName().toLower() != "dummy" && m_SourceId.toInt() != 0;				//소스 측이 더미가 아니고 ID 0도 아닐때
}

QList<int> agLineItem::dummyLines(int _findId)
{
	QList<int> result;

	sp_DlInfoObj mobj = ((agCanvasWidget*)scene()->views().first())->managerObj();
	QList<sp_SwInfoObj>* dummies = mobj->getDummyList();
	bool isValid = false;
	for(int i = 0; i < dummies->count() && !isValid; i++) 
	{
		isValid = dummies->at(i)->getSourceSecId() == _findId;
		if(isValid) 
		{
			sp_SwInfoObj dummy = dummies->at(i);
			//if(dummy->getSourceSecId() != _findId && dummy->getSourceSecId() != -1) result.append(dummy->getSourceSecId()); //어차피!!!
			if(dummy->getTargetSecId() != _findId && dummy->getTargetSecId() != -1)
				result.append(dummy->getTargetSecId());
			if(dummy->getUpSecId() != _findId && dummy->getUpSecId() != -1)
				result.append(dummy->getUpSecId());
			if(dummy->getDownSecId() != _findId && dummy->getDownSecId() != -1)
				result.append(dummy->getDownSecId());
		}
	}

	return result;
}

QList<int> agLineItem::entireGroupIds() // _tId는 반드시 IsRepresent == true 여야 한다.
{
	QList<int> result;
	loopAppend(&result, m_lineId.toInt());
	return result;
}

void agLineItem::loopAppend(QList<int>* _target, int _findId)
{
	bool isValid;

	_target->append(_findId);
	QList<int> ids = dummyLines(_findId);
	foreach(int id, ids)
		loopAppend(_target, id);
}

