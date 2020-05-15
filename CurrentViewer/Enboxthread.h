#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QEvent>

class OfficeEvent : public QEvent
{
public:
    OfficeEvent(const QString& strServer, const QString& strDlName, int nCBId) : QEvent((QEvent::Type)((int)QEvent::User + 1)) 
	{
        m_strServer = strServer;
        m_strDlName = strDlName;
		m_nCBId = nCBId;
    }
private:  
	QString m_strServer;
	QString m_strDlName;
	int m_nCBId;	

public:
    QString getServerCode() const { return m_strServer; }
    QString getDlName() const { return m_strDlName; }
	int getCBId() const { return m_nCBId; }
};

class EnboxMonitoring : public QThread
{
    Q_OBJECT
public:
    explicit EnboxMonitoring(QVector<int> enboxData, QObject *parent = 0);
    ~EnboxMonitoring();

signals:
    void mySignal();

public slots:

private:
    int m_count;
    bool m_loop;
	QVector<int> m_enboxData;

protected:
    void run();

public:
    void quit() { m_loop = false; }
};

#endif // MYTHREAD_H
