#include "StdAfx.h"
#include "agLineItem.h"
#include "../agDmsDeviceClass/agswinfoobject.h"
#include <QGraphicsScene>
#include "./agOLD/agcanvaswidget.h"

const int g_DIFF = 3;
agLineItem::agLineItem(agSWInfoObject* swObj) : agItemBase(0)
{
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	//setFlag(QGraphicsItem::ItemSendsGeometryChanges,true);
	//setFlag(QGraphicsItem::ItemStacksBehindParent,true);
	setFlag(QGraphicsItem::ItemIsFocusable , true);	
	setZValue(3);
	//QObject::connect(this , SIGNAL(sendConnectTabObj()) , swObj , SLOT(connectSelectedLineObj()));
	//QObject::connect(this , SIGNAL(SearchChangLine()) , swObj , SLOT(connectSelectedLineObj()));
	//m_LineCurrent = new ChartData;
	//ChartData m_LineCurrent;
}

agLineItem::~agLineItem()
{
	//delete m_LineCurrent;

}

void	agLineItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	event->ignore();
	QGraphicsItem::mouseDoubleClickEvent(event);
}


void	agLineItem::mousePressEvent ( QGraphicsSceneMouseEvent * event)
{
	//emit sendConnectTabObj();
	//event->ignore();
	//if( event->button() == Qt::LeftButton)
	//{
	//	qDebug() << "testtesttesttesttesttesttesttest"; 
	//}
	//agCanvasWidget* view = ((agCanvasWidget*)scene()->views().first());
	//view->ChangeLineColor();
	//update();
	//QGraphicsItem::mousePressEvent(event);
}

void agLineItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	if(event->modifiers() & Qt::ShiftModifier) 
	{
		selectShift();
		setSelected(true);
	} 
	else
	{
		setSelected(!isSelected());
	}
	agCanvasWidget* view = ((agCanvasWidget*)scene()->views().first());
	view->ChangeLineColor();
	qDebug("HAHAHA");
	update();
}

void	agLineItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *s, QWidget * )
{
	/*qreal sizeMax = qMin(m_rt.size().width(),m_rt.size().height());

	const qreal lod = s->levelOfDetailFromTransform(painter->worldTransform());

	if ( lod < 0.1 && m_strObj.compare("GroundLine") != 0 )
		return;*/

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
}


QRectF	agLineItem::boundingRect() const
{
	const int plusArea = 10;
	//return m_rect;
	return m_polygon.boundingRect().adjusted(-plusArea, -plusArea, plusArea, +plusArea);				// 수정한 부분
}

void	agLineItem::setPolygon(const QPolygonF& poly)
{
// 	if(poly[0].y() == poly[1].y())
// 	{
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

qreal agLineItem::getPolygonX()
{
	return m_polygon[0].x();
}

QPolygonF agLineItem::getLinePolygon()
{
	return m_polygon;
}

// 
// QList<agLineItem*> agLineItem::lineItems() 
// {
// 	QList<QGraphicsItem*> myItems = (agCanvasScene)scene()->items();   // 이런 방식으로 사용 가능
// 	QList<agLineItem*> result;
// 	bool isAnySelected = false;
// 	foreach(QGraphicsItem* item, myItems) 
// 	{
// 		agLineItem* line = dynamic_cast<agLineItem*>(item);
// 		if(line) {
// 			result.append(line);
// 			if(line->isSelected() && !isAnySelected)
// 				isAnySelected = true;
// 		}
// 	}
// 	return result;
// }
// 
// QList<agLineItem*> agLineItem::SelectedlineItems() 
// {
// 	QList<QGraphicsItem*> myItems = scene()->items();
// 	QList<agLineItem*> result;
// 	bool isAnySelected = false;
// 	foreach(QGraphicsItem* item, myItems) 
// 	{
// 		agLineItem* line = dynamic_cast<agLineItem*>(item);
// 		if(line && line->isSelected())
// 		{
// 			result.append(line);
// 			if(line->isSelected() && !isAnySelected)
// 				isAnySelected = true;
// 		}
// 	}
// 	return result;
// }

// void agLineItem::getMainlinesId()
// {
// 	//QList<agLineItem*> lines = ((agCanvasScene*)scene())->lineItems();
// }

void agLineItem::selectShift()
{
	int i;
	int idx = -1;
	//agCanvasScene* agScene;
	QList<agLineItem*> lines = ((agCanvasScene*)scene())->lineItems();

	qDebug() << lines.count(); 

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
						lines[i]->setSelected(true);
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
						lines[i]->setSelected(true);
					    //qDebug("%d", i);
					}
				}
			}
		}

	} 
	else //수직선 
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
			// Up    
			for(i = idx+1; i < lines.count(); i++)  // 자기 자신 다음 부터 
			{
				if(lines[i]->isSelected() && boundingRect().x() == lines[i]->boundingRect().x())
					UpIdx = i;
			}
			// Down 
			for(i = idx-1; i >= 0; i--) 
			{
				if(lines[i]->isSelected() && boundingRect().x() == lines[i]->boundingRect().x())
					DownIdx = i;
			}
			// 위아래 둘다 선택된 것이 있을 경우??
			// 1. 아무것도 선택이 안되었다.
			//qDebug("L %d,  R %d", lIdx, rIdx);
			if(UpIdx == -1 && DownIdx == -1) 
			{
				////
			} 
			else if(UpIdx != -1)							
			{ //2. 위쪽 선택된 것이 있다.
				//qDebug() << "leftSearch";
				for(i = idx + 1; i <= UpIdx; i++)
				{
					if(boundingRect().x() == lines[i]->boundingRect().x())
					{
						lines[i]->setSelected(true);
						//qDebug("%d", i);
					}
				}
			} 
			else if(DownIdx != -1) 
			{ //3. 아래쪽 선택된 것이 있다.
				//qDebug() << "RightSearch";
				for(i = DownIdx; i < idx; i++)
				{
					if(boundingRect().x() == lines[i]->boundingRect().x())
					{
						lines[i]->setSelected(true);
						//qDebug("%d", i);
					}
				}
			}
		}
	}
}

// ChartData* agLineItem::lineCurrent() const
// {
// 
// 	return m_LineCurrent; 
// 
// }
// void agLineItem::setLineCurrent( ChartData* chartData )
// {
// 	*m_LineCurrent = *chartData;
// }