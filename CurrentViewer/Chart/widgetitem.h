#ifndef WIDGETITEM_H
#define WIDGETITEM_H

#include <QListWidgetItem>

class ListItem;
class agLineItem;
class WidgetItem : public QListWidgetItem
{
public:
	WidgetItem( ListItem* listItem , const QList<agLineItem*>& selLine, QListWidget* parent = 0 );
	~WidgetItem();

	void setSelectedList();

private:
	ListItem* m_itemTarget;
	QList<agLineItem*> m_lineItem;

public:
	ListItem* itemTarget() { return m_itemTarget; }	
	QList<agLineItem*> lineItem() { return m_lineItem; }
};


#endif // WIDGETITEM_H
