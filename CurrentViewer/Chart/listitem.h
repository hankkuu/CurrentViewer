#ifndef LISTITEM_H
#define LISTITEM_H

#include <QObject>

class ListItem : public QWidget					// 로우 열학을 하는 녀석
{
	Q_OBJECT
public:
	ListItem( QWidget* parent = 0);
	~ListItem();

signals:
	void checkedChartSerise(bool);

private:
	QWidget* m_parent;
	QCheckBox* m_chkChartSerise;
	QLabel* m_label0;
	QLabel* m_label1;

private:
	void initObject();

public:	
	void deleteThis();								// 필요 없음 안씀
	QString getColum0Text();
	void setColum0Text(const QString& _text);
	void setColum1Text(const QString& _text);
};


#endif // LISTITEM_H
