#ifndef LISTITEM_H
#define LISTITEM_H

#include <QObject>

class ListItem : public QWidget					// �ο� ������ �ϴ� �༮
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
	void deleteThis();								// �ʿ� ���� �Ⱦ�
	QString getColum0Text();
	void setColum0Text(const QString& _text);
	void setColum1Text(const QString& _text);
};


#endif // LISTITEM_H
