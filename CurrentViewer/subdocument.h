#ifndef SubDOCUMENT_H
#define SubDOCUMENT_H

#include <qwt_plot.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_scale_div.h>
#include <qwt_point_data.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_text.h>
#include <qwt_symbol.h>
#include <qwt_math.h>
#include <qwt_scale_draw.h>
#include <iostream>
#include <string>
#include "./agOLD/agdlnetworkinfo.h"
#include "./agOLD/agCanvasScene.h"
#include "./agOLD/agcanvaswidget.h"
#include "./agOLD/agnetworkparser.h"
#include "./agOLD/agDmsDeviceClass/agswinfoobject.h"
#include "./agOLD/agDmsDeviceClass/agobjectbase.h"
#include "DBHandler/agDBFunction.h"

class XScaleDraw: public QwtScaleDraw
{
public:
	XScaleDraw(const QStringList& Xdata ) : QwtScaleDraw()
	  {
		  m_value = Xdata;

		  setTickLength( QwtScaleDiv::MinorTick, 0 );
		  setTickLength( QwtScaleDiv::MediumTick, 0 );
		  setTickLength( QwtScaleDiv::MajorTick, 1 );	
	  }
	  virtual QwtText label( double v ) const
	  {
		  QwtText lbl;
				  
		  int index = qRound( v );
		 
		  //qDebug("%d/%d ====== %f", index, m_value.count(), v);
		  if( index >= 0 && index < m_value.count() )
		  {	
			  lbl = m_value[index];
		  }
		  return lbl;		 	  		
	  }
	  //int getValueCount() { return m_value.count()/2; }				// 테스트 용

private:
	QStringList m_value;
};

class FunctionData: public QwtSyntheticPointData
{
public:
	FunctionData( size_t tt, QMap<int,double> data ): QwtSyntheticPointData(tt  )
	  {
	     m_size = tt;		
		 m_data = data;
	  }

 	  double y( double x ) const
 	  {
		 int a = (int)x;
		 return m_data[a];		  
 	  }

 	  double(*Test)(double);

	  size_t	m_size;
	  QMap<int,double> m_data;

private:
	//double( *d_y )( double );
};

class ArrowSymbol: public QwtSymbol
{
public:
	ArrowSymbol()
	{
		QPen pen( Qt::black, 0 );
		pen.setJoinStyle( Qt::MiterJoin );

		setPen( pen );
		setBrush( Qt::red );

		QPainterPath path;
		path.moveTo( 0, 8 );
		path.lineTo( 0, 5 );
		path.lineTo( -3, 5 );
		path.lineTo( 0, 0 );
		path.lineTo( 3, 5 );
		path.lineTo( 0, 5 );

		QTransform transform;
		transform.rotate( -30.0 );
		path = transform.map( path );

		setPath( path );
		setPinPoint( QPointF( 0, 0 ) );

		setSize( 10, 14 );
	}
};

class QwtPlotMarker;
class QwtTextLabel;
class Plot : public QwtPlot
{
public:
	// 새탭 추가
	Plot(int nIndex, QString FromTime, QString ToTime, DateType dDateType, QWidget *parent = NULL );				
	// 간선 차트 추가
	Plot(int nIndex, int nMaxPValue, QMap<int, QString> XValue, QMap<int, qreal> YData, QWidget *parent = NULL ); 
	// 시간 차트 추가
	Plot(int nIndex,  int nMaxPValue, QList<agLineItem*> lines, QMap<int, QString> XValue, QMap<int, qreal> YData, QString FromTime, QString ToTime, DateType DateType, QWidget *parent = NULL );
	~Plot();

	void populate(int nIndex, QMap<int, qreal> YData, bool bData = false );
	QString makeDateString(QString FromTime, QString ToTime, DateType dDateType );
	QStringList makeXDateString(QString FromTime, QMap<int, qreal>& YData );
	bool isContains(agLineItem* line );
	void setSelectedList();

	int getIndex() { return m_nIndex; }								// 현재의 차트 시리즈 Index를 리턴
	DateType getDateType() { return m_DateType; }

	QMap<int, QwtPlotCurve*> getPlotMap() {return m_PlotMap; }
	void setPlotMap(int nIdx ,QwtPlotCurve *PlotChart);

	QList<agLineItem*> getLineItem() { return m_lineItem; }
	QList<QString> getSearchTime() { return m_searchTime; }
	QMap<int, QString> getXCount() { return m_XValueMap; }
	int getMaxPValue() { return m_nMaxPValue; }
	void setMaxPValue(int nMaxPValue );
	void delChatList(int delIdx);

	void detachMarker();
	void deleteMarker();

protected:
	bool event(QEvent* _e);
	virtual void resizeEvent( QResizeEvent * );
	void mouseMoveEvent(QMouseEvent *ev);
	void mousePressEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);

	void enterEvent( QEvent * ev );
	void leaveEvent( QEvent * ev );

private slots:
	void repositionMarker(const QPoint& _p);
private:
	QWidget* m_parent;
	QList<agLineItem*> m_lineItem;
	DateType m_DateType;
	int m_nIndex;
	QMap<int, QString> m_XValueMap;
	int m_nMaxPValue;
	QList<QString> m_searchTime;

	QwtPlotMarker* m_toolTip;
	QwtTextLabel* textLabel;
	QStringList m_XData;
	QMap<int, QwtPlotCurve*> m_PlotMap;

	bool m_bData;
};

#endif // DOCUMENT_H
