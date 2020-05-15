#ifndef AGITEMBASE_H
#define AGITEMBASE_H

#include <QGraphicsObject>

class agItemBase :  public QGraphicsObject
{
	Q_OBJECT
public:
	agItemBase(QGraphicsItem* parent = 0);
	~agItemBase();


	void	setRect( const QRectF& rect);
	void	setRect( qreal xPos, qreal yPos, qreal width, qreal height );

	void	setFont(QFont& font);
	void	setFontColor(QColor& color);
	void	setPen(QPen& pen);
	void	setBrush(QBrush &brush);
	void	setObjName(QString &strObj);
	QString getObjName();

	virtual QRectF	boundingRect () const;

	QPen	getPen();

protected:
	QRectF			m_rt;
	QBrush			m_brush;
	QColor			m_fontColor;
	QFont			m_font;
	QPen			m_pen;	
	QString			m_strObj;
};

#endif // AGITEMBASE_H
