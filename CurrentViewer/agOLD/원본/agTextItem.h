#ifndef AGTEXTITEM_H
#define AGTEXTITEM_H

#include "agItemBase.h"
#include "QGraphicsTextItem"

class agTextItem : public QGraphicsTextItem
{
	Q_OBJECT
public:
	agTextItem();
	~agTextItem();


	virtual void	Draw( QPainter* painter, QPointF offset,const QStyleOptionGraphicsItem *option=0 );
	void			paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * );

	void			setText(QString& str);

	void	setRect( const QRectF& rect);
	void	setRect( qreal xPos, qreal yPos, qreal width, qreal height );

	void	setFont(QFont& font);
	void	setFontColor(QColor& color);
	void	setPen(QPen& pen);
	void	setBrush(QBrush &brush);
	void	setObjName(QString &strObj);
	QString getObjName();

	void	setRotate(int nRotate);

	virtual QRectF	boundingRect () const;

	QString		getText();

	void		setMark(bool bIsMark) { m_bIsMark = bIsMark; }
	bool		IsMark() { return m_bIsMark; }

	void		setCaption(bool bIsCaption) { m_bIsCaption = bIsCaption; }
	bool		IsCaption() { return m_bIsCaption; }

private:
	QRectF			m_rt;
	QBrush			m_brush;
	QColor			m_fontColor;
	QFont			m_font;
	QPen			m_pen;	
	QString			m_strObj;

	int				m_nRotate;
	QString			m_str;
	bool			m_bIsMark;
	bool			m_bIsCaption;
};

#endif // AGTEXTITEM_H
