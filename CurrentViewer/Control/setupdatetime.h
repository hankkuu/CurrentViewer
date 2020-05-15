#ifndef SETUPDATETIME_H
#define SETUPDATETIME_H

#include <QWidget>
#include "DBHandler/agDBFunction.h"

class SetupDatetime : public QWidget
{
	Q_OBJECT

public:
	SetupDatetime(QWidget *parent = 0);
	~SetupDatetime();

signals:
	void sectionChartClicked();
	void sectionChartChecked();
	void dateChartClicked();
	void deleteAllLine();


private slots:
	void onDateButtonClicked();
	void onCalendarClicked(QDate _date);
	void SendDateTypeMonth();
	void SendDateTypeDay();
	void SendDateTypeHour();

private:
	QCalendarWidget* m_calendar;
	//QDateEdit* m_calendar;
	QPushButton* m_btnFromDate;
	QPushButton* m_btnToDate;
	QPushButton* m_btnCalendar;
	QComboBox* m_cmbFromHour;
	QComboBox* m_cmbToHour;

	QRadioButton* m_btnMonth; 
	QRadioButton* m_btnDay; 
	QRadioButton* m_btnHour; 
	QButtonGroup* m_DateGroup; 

	DateType		m_nDateType;

private:
	QLayout* createSearch();

public:
	QString searchFrom(); //yyyyMMddhh
	QString searchTo(); //yyyyMMddhh
	void setDateType(DateType nDateType);
	DateType getDateType();
};

#endif // SETUPDATETIME_H
