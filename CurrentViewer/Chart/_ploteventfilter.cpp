#include "StdAfx.h"
#include "ploteventfilter.h"

#include <QEvent>
#include <QMouseEvent>

PlotEventFilter::PlotEventFilter(QObject *parent) : QObject(parent)
{

}

PlotEventFilter::~PlotEventFilter()
{

}

bool PlotEventFilter::eventFilter(QObject* _o, QEvent* _e)
{
	if(_e->type() == QEvent::MouseMove) 
	{
		qDebug("A18 %d %d", ((QMouseEvent*)_e)->pos().x(), ((QMouseEvent*)_e)->pos().y());
		emit mouseMoved(((QMouseEvent*)_e)->pos());
	}
	return QObject::eventFilter(_o, _e);
}