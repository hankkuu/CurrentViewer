#include "StdAfx.h"
#include "chartviewtable.h"
#include "listitem.h"
#include "widgetitem.h"
#include "agOLD/agGraphicsClass/agLineItem.h"

ChartViewTable::ChartViewTable(int nSeriseIndex, QWidget *parent) : QListWidget(parent)
{
	m_parent = parent;
	m_nSeriesIndex = nSeriseIndex;
	setSelectionMode(QAbstractItemView::MultiSelection);
}

ChartViewTable::ChartViewTable(int nSeriseIndex, QMap<int, QString> XValue,  const QList<agLineItem*>& selLine, QWidget *parent) : QListWidget(parent)
{	
	m_parent = parent;
	m_nSeriesIndex = nSeriseIndex - 1;
	setSelectionMode(QAbstractItemView::MultiSelection);

	makeSeriesRow( XValue,  selLine );
}

ChartViewTable::~ChartViewTable()
{
}

void ChartViewTable::makeSeriesRow( QMap<int, QString> XValue, const QList<agLineItem*>& selLine )
{
	m_nSeriesIndex++;
	QString strSeriesName = makeSeriesName(XValue);

	m_itemWidget = new ListItem(this);
	m_itemWidget->setColum0Text(QString::number(m_nSeriesIndex));
	m_itemWidget->setColum1Text(strSeriesName);	

	m_listItem = new WidgetItem( m_itemWidget, selLine , this);
	m_listItem->setSizeHint(QSize(100, 40));
	setItemWidget(m_listItem, m_itemWidget);

	connect(m_itemWidget, SIGNAL(checkedChartSerise(bool)), SLOT(onCheckedChartSerise(bool)));
	connect(this, SIGNAL(sendDelIdx(QString, bool)), m_parent, SLOT(onReceiveIdx(QString, bool)));
	connect(this, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(onListItemSelected(QListWidgetItem*)));

	connect(this, SIGNAL(sendDelChartIdx(QList<QString>)), m_parent, SLOT(onChartDelList(QList<QString>)));

}

void ChartViewTable::onListItemSelected(QListWidgetItem* _item)
{
	WidgetItem* item = static_cast<WidgetItem*>(_item);
	item->setSelectedList();
}

QString ChartViewTable::makeSeriesName( QMap<int, QString> sectionList)
{
	int nSize = sectionList.count();
	QString strStart = sectionList.value(0);

	if( nSize == 1)
		return strStart;
	
	QString strEnd = sectionList.value(--nSize);
	QString sectionName = strStart + " - " + strEnd;

	return sectionName;
}

void ChartViewTable::onReceiveDelItem()
{
	QList<QListWidgetItem*> listItems = selectedItems();
	QList<QString> idxList;

	foreach(QListWidgetItem* item, listItems)
	{
		WidgetItem* listItem = dynamic_cast<WidgetItem*>(item);

		QList<agLineItem*> lineItem = listItem->lineItem();

		foreach(agLineItem* line, lineItem)
		{
			line->setSelected(false);
		}

		ListItem* delitem = listItem->itemTarget();
		idxList.append(delitem->getColum0Text());

		delete takeItem(row(listItem));
		delitem->deleteLater();
	}

	emit sendDelChartIdx(idxList);
}

void ChartViewTable::onCheckedChartSerise(bool state)
{
	ListItem* item = qobject_cast<ListItem*>(sender());

	if(item)
	{
		QString idx = item->getColum0Text();
		emit sendDelIdx(idx, state);
		//qDebug("COUNT %d", count());
	}
}
