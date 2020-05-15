#include "StdAfx.h"
#include "listitem.h"


ListItem::ListItem(QWidget* parent) : QWidget(parent)
{
	m_parent = parent;
	initObject();
}

ListItem::~ListItem()
{
}

void ListItem::initObject()
{
	m_chkChartSerise = new QCheckBox();
	m_label0 = new QLabel("first");
	m_label0->setProperty("style", true);
	m_label1 = new QLabel("second");
	m_label1->setProperty("style", true);
	m_label1->setToolTip(m_label1->text());

	QHBoxLayout* hLayout = new QHBoxLayout();
	hLayout->addWidget(m_chkChartSerise);
	hLayout->addSpacing(10);
	hLayout->addWidget(m_label0);
	hLayout->addSpacing(20);
	hLayout->addWidget(m_label1);
	hLayout->addSpacing(20);
	hLayout->addStretch();
	setLayout(hLayout);
	m_chkChartSerise->setChecked(true);

	connect(m_chkChartSerise, SIGNAL(toggled(bool)), SIGNAL(checkedChartSerise(bool)));
}

void ListItem::deleteThis()
{
	//deleteLater();
	delete this;
}

QString ListItem::getColum0Text()
{
	return m_label0->text();
}

void ListItem::setColum0Text(const QString& _text)
{
	m_label0->setText(_text);
}

void ListItem::setColum1Text(const QString& _text)
{
	m_label1->setText(_text);
	m_label1->setToolTip(m_label1->text());
}

