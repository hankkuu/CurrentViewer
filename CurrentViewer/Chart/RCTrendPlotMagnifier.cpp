#include "stdafx.h"
#include "RCTrendPlotMagnifier.h"
#include "qwt_plot.h"
#include "qwt_plot_canvas.h"
#include "qwt_scale_div.h"
#include "qwt_plot_magnifier.h"

#include "RCTrendPlotMagnifier.h"
#include <QWheelEvent>
RCTrendPlotMagnifier::RCTrendPlotMagnifier( QwtPlotCanvas * canvas )
	: QwtPlotMagnifier(canvas),m_scrollOption(INCRESSMAX)
{
	setAxisEnabled(QwtPlot::yLeft,true);
	setAxisEnabled(QwtPlot::yRight,false);
	setAxisEnabled(QwtPlot::xBottom,false);
	setAxisEnabled(QwtPlot::xTop,false);
}

RCTrendPlotMagnifier::~RCTrendPlotMagnifier()
{

}

void	RCTrendPlotMagnifier::setBehavior (ScrollBehavior behavior)
{
	m_scrollOption = behavior;
}

void RCTrendPlotMagnifier::widgetWheelEvent( QWheelEvent *wheelEvent )
{
	double scaleFactor = 1.25;
	if ( wheelEvent->modifiers() & Qt::ControlModifier )
		scaleFactor = 1.01;
	if(wheelEvent->delta() > 0) 
	{
		rescale (scaleFactor);
	} 
	else
		rescale(1.0 / scaleFactor);
}

void RCTrendPlotMagnifier::rescale( double factor )
{
	factor = qAbs( factor );
	if ( factor == 1.0 || factor == 0.0 )
		return;
	bool doReplot = false;
	QwtPlot* plt = plot();

	const bool autoReplot = plt->autoReplot();
	//plt->setAutoReplot( false );
	for ( int axisId = 0; axisId < QwtPlot::axisCnt; axisId++ )
	{

		const QwtScaleDiv& scaleDiv = plt->axisScaleDiv( axisId );
		if ( this->isAxisEnabled( axisId ) )
		{
			qreal min = 0; qreal max = 0;
			if ( m_scrollOption == DECRESSMININCRESSMAX)
			{
				const double center =
				scaleDiv.lowerBound() + scaleDiv.range() / 2;
				const double width_2 = scaleDiv.range() / 2 * factor;
				min = center -width_2;
				max = center + width_2;
				//plt->setAxisScale( axisId, center - width_2, center + width_2 );
			}
			else if (m_scrollOption == INCRESSMAX )
			{
				const double center =scaleDiv.lowerBound();
				const double width_2 = scaleDiv.range()  * factor;
				//plt->setAxisScale( axisId, center , center + width_2 );
				min = center ;
				max = center + width_2;

			}
			doReplot = true;
			plt->setAxisScale( axisId, min , max );
			plt->replot();
		}
	}	
}

RCTrendPlotMagnifier::ScrollBehavior RCTrendPlotMagnifier::behavior()
{
	return m_scrollOption;
}