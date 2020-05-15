#ifndef QDBSTATUSLISTENER_H
#define QDBSTATUSLISTENER_H

#include <QObject>
#include "qdbaccssor_global.h"

class QDBACCSSOR_EXPORT qDbStatusListener : public QObject
{
	Q_OBJECT

public:
	qDbStatusListener(QObject *parent = NULL);
	virtual ~qDbStatusListener();

	public slots:
		virtual void	receiveDbConnect(QString strMsg) = 0;
		virtual void	receiveDbDisConnect(QString strMsg) = 0;

private:
	
};

#endif // QDBSTATUSLISTENER_H
