#ifndef AGCANVASWIDGET_H
#define AGCANVASWIDGET_H

#include "agdlnetworkinfo.h"
#include "agCanvasScene.h"
#include "currentviewer.h"

class agCanvasWidget : public QGraphicsView
{
	Q_OBJECT

public:
	agCanvasWidget(QWidget *parent =0 , QGridLayout* layout = 0 ,  int nMode = 0);
	~agCanvasWidget();

	typedef enum{NetWorkMode , OLDMode};
	typedef enum{firstClick , SecondClick, NonClick};

	void			setXmlData(QString &strXml);
	bool			SaveOldNetwork(int nIdx);
	bool			SaveNzedNetwork();
	void			showEvent(QShowEvent * event);
	QRectF			setSceneRectSizeUp();

	void			showChangeScreen();
	void			setOfficeCode(int nOfficeCode) { m_nOfficeCode = nOfficeCode; }
	int				getOfficeCode() { return m_nOfficeCode; }
	void			setDlCode(int nDl) { m_nDlCode = nDl; }
	int				getDlCode() { return m_nDlCode; }

	void			setViewMode();
	void			setViewMode(int nIdx);
	void			deleteNzedScene();
	void			deleteOldScene(int nIdx);

	bool			checkFocusSwObj(int nSwId, QString strObjName);
	bool			checkFocusSwObjToCaption(int nSwId, QString strCaption);
	bool			checkFocusSwObjToCaption(int nSwId, QString strCaption , int nIdx);
	bool			checkFocusSwObj(int nSwId);
	bool			checkFocusSwObj(int nSwId, QString strObjName , int nCurView);
	void			showWidgetView();
	//void			setDlQueueData(agCommThrQueue::DLNetwork &network);
	//agCommThrQueue::DLNetwork&	getDlQueueData();
	void			stopBlinkObj();

	int				getViewMode() { return m_nViewMode; }
	int				getCurrentViewMode() { return m_nCurView; }


	// 이부분은 아직 사용 안하는 함수..
	// 나중에 쓰래드 쪽에서 계통 저장하는 부분 수정할때 사용되는 함수	
	agCanvasScene*	getCanvasScene();
	agCanvasScene*  getOldCanvasScene();
	sp_DlInfoObj&	getInfoObj();
	sp_DlInfoObj&	getInfoObj(int nIdx);
	void			deleteObj();
	void			deleteObj(int nIdx);
	void			SaveNzedNetwork(agCanvasScene* scene);

	bool			getSwObjPos(int nSwId, QPointF &pos, int &dir);
	bool			getOld1SwObjPos(int nSwId, QPointF &pos, int &dir);
	bool			getOld2SwObjPos(int nSwId, QPointF &pos, int &dir);
	bool			getOld3SwObjPos(int nSwId, QPointF &pos, int &dir);
	void			getOldCanvasSenes(agCanvasScene*	oldscenes[3]);
////////////////////////////////////////////////////////////////////////////
	/*
	void	addFI(QString ID, int FiType, QPointF pos, int dir);
		void	revertFI(QString ID, int FiType);
		void	setFtCurrent(QString ID, QMap<QString,QVariant> &current);
	
		void	addOLD1FI(QString ID, int FiType, QPointF pos, int dir);
		void	revertOLD1FI(QString ID, int FiType);
		void	setOLD1FtCurrent(QString ID, QMap<QString,QVariant> &current);
	
		void	addOLD2FI(QString ID, int FiType, QPointF pos, int dir);
		void	revertOLD2FI(QString ID, int FiType);
		void	setOLD2FtCurrent(QString ID, QMap<QString,QVariant> &current);
	
		void	addOLD3FI(QString ID, int FiType, QPointF pos, int dir);
		void	revertOLD3FI(QString ID, int FiType);
		void	setOLD3FtCurrent(QString ID, QMap<QString,QVariant> &current);*/
	
	
	void	reverseBlackColor();
	void	reverseWhiteColor();
	void    reverseTextColor();
	void	setTextColor();
	
	void	setViewSize();										// 화면 조정 a18
	void	makeMainLine(QString strServer , int nCBId);		// 부하 데이터 추출
	void    setMaxPVlaue(QList<agLineItem*> lineItems);			// 맥스값 셋팅
	int     getMaxPVlaue() { return m_nMaxPValue; }				// 맥스값 겟
	
	void	makeChartXData(QList<agLineItem*> LineItem);
	void	makeChartYData(QList<agLineItem*> LineItem);
	void    makeSectionXData(int nIx, QString strSwCaption);
	QMap<int, QString> getChartXData() { return m_sectionXData; }

private:
	void			resizeEvent(QResizeEvent *event);
	void			wheelEvent(QWheelEvent *event);
	void			scaleView(qreal scaleFactor );
	virtual void	dragMoveEvent ( QDragMoveEvent * event );
// 	void			mousePressEvent(QMouseEvent * e);
// 	void			mouseReleaseEvent(QMouseEvent * e);


protected:
	void			keyPressEvent(QKeyEvent *event);


signals:
	void	 printChart();
	void	 printCurrentDayChart();
	void	 printMainLineChart();
	void	 reTimeSettings();

public slots:
	void		receiveSwObj(int nSwId, QString strObjName);
	void		receiveCanvasPos(QRectF rt);

	void		ShowContextMenu(const QPoint &pos); 
	void		onDragMouse();
	void		onGrabMaouse();

private:
	//Ui::agCanvasWidget ui;

	QString			m_strXmlData;
	//agMainWindowBase	*m_parent;
	QWidget*		m_parent;

	sp_DlInfoObj	m_obj;
	sp_DlInfoObj	m_oldobj[3];
	QRect			m_rt;

	agCanvasScene*	m_scene;
	agCanvasScene*	m_oldscene[3];

	//sp_CircleItem		m_pc;
    //agControlWidget*	m_widget;

	bool				m_bFirstShow;
	bool				m_bIsFullScreen;

	int					m_nViewMode;
	int					m_nCurView;
	int					m_nOfficeCode;
	int					m_nDlCode;

	//agCommThrQueue::DLNetwork m_oldNetwork;

	sp_SwInfoObj		m_objSp;
	sp_SwInfoObj		m_oldObjSp[3];
	
	//FiDrawer			m_fiDrawer;
	//FiDrawer			m_fiOldDrawer[3];

	// KHK 추가
	QMap<int, QString>  m_sectionXData;					// 간선 구간 
	int					m_nMaxPValue;
	bool				m_ShiftDown;
	bool				m_CtrDown;

public:
	sp_DlInfoObj	managerObj() { return m_obj; }
};

#endif // AGCANVASWIDGET_H
