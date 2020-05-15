#include "StdAfx.h"
#include "widgetitem.h"
#include "listitem.h"
#include "agOLD/agCanvasScene.h"


WidgetItem::WidgetItem( ListItem* listItem, const QList<agLineItem*>& selLine, QListWidget* parent) : QListWidgetItem(parent)
{
	m_itemTarget = listItem;
	m_lineItem = selLine;
}

WidgetItem::~WidgetItem()
{

}

void WidgetItem::setSelectedList()
{
	if(m_lineItem.count() > 0) {
		
		foreach(agLineItem* line, ((agCanvasScene*)m_lineItem.first()->scene())->lineItems())
		{
			line->setSelected(false);															// 기존에 선택된 것은 모두 펄스한다.. 
		}

		foreach(QListWidgetItem* selItem, listWidget()->selectedItems())						// 다시 체크한다.. 시팔 효율적으로 할 수 있는 부분인데 그냥 한다.. 그냥한다. 그냥 한다.
		{
			WidgetItem* item = static_cast<WidgetItem*>(selItem);
			for(int i = 0; i < item->lineItem().count() ; ++i)
			{
				item->lineItem().at(i)->setSelected(true);		
			}
		}
	}
}
