#include "stdafx.h"
#include "searchwidget.h"

#include <QPushButton>
#include <QLabel>

SearchWidget::SearchWidget(QWidget *parent) : QWidget(parent)
{  
	QLayout* layout = createSearch();
    setLayout(layout);
}

SearchWidget::~SearchWidget()
{
}

QLayout* SearchWidget::createSearch()
{
    QHBoxLayout* result = new QHBoxLayout(this);

	QPushButton* btnNewTab = new QPushButton(QString::fromLocal8Bit("새 탭"), this);
	btnNewTab->setObjectName("btnNewTab");
	btnNewTab->setCheckable(true);
	btnNewTab->setMinimumWidth(100);
	btnNewTab->setMinimumHeight(30);

	connect(btnNewTab, SIGNAL(clicked()), SIGNAL(newChartClicked()));

	result->addWidget(btnNewTab);	
	result->addSpacing(25);
	result->addStretch();

	//QFont font = btnNewTab->property("font").value<QFont>();
	//qDebug() << font.family() << font.pointSize();

	QLabel* label1 = new QLabel(QString::fromLocal8Bit("현재 구간부하"), this);
	result->addWidget(label1);
	result->addSpacing(5);	

	m_lbPValue = new QLabel(this);
	m_lbPValue->setProperty("class", "mandatory QLabel");
	m_lbPValue->setMaximumWidth(100);
	result->addWidget(m_lbPValue);
	result->addSpacing(5);

	QLabel* label2 = new QLabel(QString::fromLocal8Bit("KVA"), this);
	result->addWidget(label2);

    return result;
}

void SearchWidget::setTotalPValue(qreal sumPvalue)
{
	m_lbPValue->setText(QString::number(sumPvalue));
}




