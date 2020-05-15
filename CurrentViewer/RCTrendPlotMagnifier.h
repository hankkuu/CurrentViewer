#ifndef RCTRENDPLOTMAGNIFIER_H
#define RCTRENDPLOTMAGNIFIER_H

#include "qwt_plot_magnifier.h"
class QwtPlotCanvas;
class QwtPlot;


class RCTrendPlotMagnifier : public QwtPlotMagnifier
{
	Q_OBJECT
public:
	enum ScrollBehavior {
						  INCRESSMAX  ,
						  DECRESSMININCRESSMAX 
						}; 
public:
	RCTrendPlotMagnifier( QwtPlotCanvas * );
	~RCTrendPlotMagnifier();

public:
	void	setBehavior (ScrollBehavior behavior);
	ScrollBehavior	behavior();
signals:
	void	scaleXChanged(qreal min,qreal max);

protected:
	virtual void	rescale( double factor );
	virtual void	widgetWheelEvent( QWheelEvent *wheelEvent );
	virtual void	widgetMouseMoveEvent( QMouseEvent *mouseEvent ){}
private:
	ScrollBehavior	m_scrollOption;
};

#endif // RCTRENDPLOTMAGNIFIER_H
