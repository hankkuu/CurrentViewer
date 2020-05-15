#ifndef PLOTEVENTFILTER_H
#define PLOTEVENTFILTER_H

#include <QObject>

class PlotEventFilter : public QObject
{
	Q_OBJECT

public:
	PlotEventFilter(QObject *parent);
	~PlotEventFilter();

signals:
	void mouseMoved(const QPoint& _p);

protected:
	bool eventFilter(QObject* _o, QEvent* _e);
private:
	
};

#endif // PLOTEVENTFILTER_H
