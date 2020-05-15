#ifndef CHARTVIEWTABLE_H
#define CHARTVIEWTABLE_H

#include <QObject>

class agLineItem;
class ListItem;
class ChartViewTable : public QListWidget
{
	Q_OBJECT
public:
	explicit ChartViewTable(int nSeriseIndex, QWidget *parent = 0);
	explicit ChartViewTable(int nSeriseIndex, QMap<int, QString> XValue,  const QList<agLineItem*>& selLine, QWidget *parent = 0);
	~ChartViewTable();

	void	makeSeriesRow( QMap<int, QString> XValue, const QList<agLineItem*>& selLine );
	QString makeSeriesName( QMap<int, QString> sectionList);
	ListItem* getItem() { return m_itemWidget; }

signals:
	void sendDelIdx(QString, bool);
	void checkedChartSerise(bool);
	void sendDelChartIdx(QList<QString>);

private slots:
	void onListItemSelected(QListWidgetItem* _item);

public slots:
	void onReceiveDelItem();

private:
	QWidget* m_parent;
	int m_nSeriesIndex;
	QListWidgetItem* m_listItem;
	ListItem* m_itemWidget; 

private slots:
	void onCheckedChartSerise(bool state);
};


#endif // CHARTVIEWTABLE_H
