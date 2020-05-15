#ifndef WIDGETSEARCH_H
#define WIDGETSEARCH_H

#include <QWidget>
#include <QDate>

class SearchWidget : public QWidget
{
    Q_OBJECT

public:
    SearchWidget(QWidget *parent = 0);
    ~SearchWidget();

signals:
    void newChartClicked();

private slots:

private:
	QLayout* createSearch();

public:
	void setTotalPValue(qreal sumPValue);

private:
	QLabel*		m_lbPValue;

};

#endif // WIDGETSEARCH_H
