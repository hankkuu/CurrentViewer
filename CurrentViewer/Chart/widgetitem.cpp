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
			line->setSelected(false);															// ������ ���õ� ���� ��� �޽��Ѵ�.. 
		}

		foreach(QListWidgetItem* selItem, listWidget()->selectedItems())						// �ٽ� üũ�Ѵ�.. ���� ȿ�������� �� �� �ִ� �κ��ε� �׳� �Ѵ�.. �׳��Ѵ�. �׳� �Ѵ�.
		{
			WidgetItem* item = static_cast<WidgetItem*>(selItem);
			for(int i = 0; i < item->lineItem().count() ; ++i)
			{
				item->lineItem().at(i)->setSelected(true);		
			}
		}
	}
}
