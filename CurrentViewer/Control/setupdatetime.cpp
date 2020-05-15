#include "StdAfx.h"
#include "setupdatetime.h"

// 날짜설정 팝업 화면
SetupDatetime::SetupDatetime(QWidget *parent) : QWidget(parent)
{
	m_btnCalendar = 0;
	setDateType(HOUR);

	m_calendar = new QCalendarWidget();
	m_calendar->setWindowFlags(Qt::Popup);
	//m_calendar = new QDateEdit();
	//m_calendar->setWindowModality(Qt::ApplicationModal);	
	connect(m_calendar, SIGNAL(clicked(QDate)), SLOT(onCalendarClicked(QDate)));

	QLayout* layout = createSearch();	
	QVBoxLayout* title = new QVBoxLayout(this);
	
	QLabel* setTime = new QLabel(QString::fromLocal8Bit("| 시간설정"), this);
	setTime->setProperty("class", "setTime QLabel");
	QWidget* setDate = new QWidget(this);
	setDate->setObjectName("setTimeWidget");
	setDate->setLayout(layout);	
	title->addWidget(setTime);	
	title->addWidget(setDate);
	setLayout(title);
}

SetupDatetime::~SetupDatetime()
{
}

void SetupDatetime::onCalendarClicked(QDate date)
{
	if(m_btnCalendar) 
	{
		m_btnCalendar->setText(date.toString("yyyy-MM-dd"));
		m_calendar->hide();
		m_btnCalendar = 0;	
	}
}

void SetupDatetime::onDateButtonClicked()
{
	m_btnCalendar = qobject_cast<QPushButton*>(sender());
	if(m_btnCalendar)
	{
		QPoint p = m_btnCalendar->mapToGlobal(QPoint(0, m_btnCalendar->height())) + QPoint(0, 2);
		m_calendar->move(p);
		m_calendar->show();		
	}
}

QLayout* SetupDatetime::createSearch()
{
	QStringList items;
	QString str;
	for(int i = 0; i < 24; i++) 
	{
		str = QString::number(i);
		if(i < 10)
			str = "0" + str;
		items.append(str);
	}
	str = QDate::currentDate().toString("yyyy-MM-dd");

	m_btnFromDate = new QPushButton(this);
	m_btnToDate = new QPushButton(this);		
	m_btnFromDate->setText(str);
	m_btnToDate->setText(str);
	connect(m_btnFromDate, SIGNAL(clicked()), SLOT(onDateButtonClicked()));
	connect(m_btnToDate, SIGNAL(clicked()), SLOT(onDateButtonClicked()));

	m_cmbFromHour = new QComboBox(this);
	m_cmbToHour = new QComboBox(this);
	m_cmbFromHour->addItems(items);
	m_cmbToHour->addItems(items);
	m_cmbFromHour->setCurrentIndex(0); //00:from
	m_cmbToHour->setCurrentIndex(items.count() - 1); //23:to
	
	// Month or Day가 입력되면 시간 부분 비활성화 
	m_btnMonth = new QRadioButton(QString::fromLocal8Bit("월간"), this);
	connect(m_btnMonth, SIGNAL(clicked(bool)), SLOT(SendDateTypeMonth()));
	m_btnDay = new QRadioButton(QString::fromLocal8Bit("일간"), this);
	connect(m_btnDay, SIGNAL(clicked(bool)), SLOT(SendDateTypeDay()));
	m_btnHour = new QRadioButton(QString::fromLocal8Bit("시간"), this);	
	connect(m_btnHour, SIGNAL(clicked(bool)), SLOT(SendDateTypeHour()));
	m_btnHour->setChecked(true);

	m_DateGroup = new QButtonGroup(this);
	m_DateGroup->addButton(m_btnMonth);
	m_DateGroup->addButton(m_btnDay);
	m_DateGroup->addButton(m_btnHour);

	QHBoxLayout* result = new QHBoxLayout(this);
	result->addSpacing(5);
	result->addWidget(m_btnMonth);    
	result->addSpacing(5);
	result->addWidget(m_btnDay);   
	result->addSpacing(5);
	result->addWidget(m_btnHour);    
	result->addSpacing(5);
	result->addWidget(m_btnFromDate);
	result->addSpacing(5);
	result->addWidget(m_cmbFromHour);
	result->addWidget(new QLabel(QString::fromLocal8Bit(" ~ "), this));
	result->addWidget(m_btnToDate);
	result->addSpacing(5);
	result->addWidget(m_cmbToHour);
	result->addSpacing(5);

	QPushButton* btnSpecChar = new QPushButton(QString::fromLocal8Bit("시간차트 출력"), this);
	btnSpecChar->setCheckable(true);
	result->addWidget(btnSpecChar);
	connect(btnSpecChar, SIGNAL(clicked()), SIGNAL(dateChartClicked()));
	result->addSpacing(20);

	return result;
}


QString SetupDatetime::searchFrom() 
{
	//return m_btnFromDate->text() + m_cmbFromHour->currentText();
	QString result = m_btnFromDate->text() + m_cmbFromHour->currentText();
	if(m_btnMonth->isChecked())
		result = result.left(8);
	else if(m_btnDay->isChecked())
		result = result.left(10);
	return result;

}
QString SetupDatetime::searchTo() 
{
	//return m_btnToDate->text() + m_cmbToHour->currentText();
	QString result = m_btnToDate->text() + m_cmbToHour->currentText();
	if(m_btnMonth->isChecked())
		result = result.left(8);
	else if(m_btnDay->isChecked())
		result = result.left(10);
	return result;
}

void SetupDatetime::SendDateTypeMonth()
{
	setDateType(MONTH);
	m_cmbToHour->setEnabled(false);
	m_cmbFromHour->setEnabled(false);
}

void SetupDatetime::SendDateTypeDay()
{
	setDateType(DAY);
	m_cmbToHour->setEnabled(false);
	m_cmbFromHour->setEnabled(false);
}

void SetupDatetime::SendDateTypeHour()
{
	setDateType(HOUR);
	m_cmbToHour->setEnabled(true);
	m_cmbFromHour->setEnabled(true);
}

void SetupDatetime::setDateType(DateType dDateType)
{
	m_nDateType = dDateType;
}

DateType SetupDatetime::getDateType()
{
	return m_nDateType;
}

