#ifndef AGSWINFOOBJECT_H
#define AGSWINFOOBJECT_H

#include <QObject>
#include "agobjectbase.h"

class agCanvasWidget;
class agSWInfoObject : public QObject
{
	Q_OBJECT

public:
	agSWInfoObject(QObject *parent = NULL , agCanvasWidget* canvas = NULL);
	~agSWInfoObject();


//	typedef enum { nLT , nRT , nLB , nRB , nMT , nMB};
	typedef enum { nLeftTop , nMiddleTop , nRightTop , nLeftBottom , nMiddleBottom , nRightBottom};

	void			SetTextItem(const sp_TextItem& pt);
	QString			GetCaptionTextName() { return m_strCaptionName; }
	void			SetRectItem(const sp_RectItem& pr );
	void			SetCircleItem(const sp_CircleItem& pc);
	void			SetPolygonItem(const sp_PolygonItem& pp);
	void			SetLineItem(const sp_LineItem& pl );
	void			SetPolyLineItem(const sp_PolyLineItem& pp );

	void			SetObjName(const QString& strObjName);
	QString			GetObjName();
	void			SetSwId(int swId);

	// 그룹 판별할 정보 추가
	void            SetGroup(bool bGroup) { m_bGroup = bGroup; }
	bool			GetGroup() { return m_bGroup; }
	int 			getGroupCount() { return m_GroupCount; }
	void			setGroupCount(int nGroupCount) { m_GroupCount = nGroupCount; }

	// ID 0인 녀석들을 처리하기 위해 추가된 정보들 에이18 퉤!
	void			SetSecId(int secId) { m_secId = secId; }
	void			setSourceSecId(int nSecId) { m_nSourceSecId = nSecId; }
	void			setTargetSecId(int nSecId) { m_nTargetSecId = nSecId; }
	void			setDownSecId(int nSecId) { m_nDownSecId = nSecId; }
	void			setUpSecId(int nSecId) { m_nUpSecId = nSecId; }

	int				getSecId() { return m_secId; }
	int				getSourceSecId() { return m_nSourceSecId; }
	int				getTargetSecId() { return m_nTargetSecId; }
	int				getUpSecId() { return m_nUpSecId; }
	int				getDownSecId() { return m_nDownSecId; }
	//

//	void			paint(QPainter &painter);	
	int				getSwId();

	void			setFiPosition(int nPos , bool bIsOld = true);/* { m_nPos = nPos; }*/
	int				getFiPosition() { return m_nPos; }

	void			setPadData(sp_SwInfoObj obj);
	void			setCircuitPos();
	void			setTwoCircuitPos();
	void			setThreeCircuitPos();
	void			setFourCircuitPos();
	void			setFiveCircuitPos();
	void			setSixCircuitPos();

	void			startBlinkObj();
	void			stopBlinkObj();
	void			showFocus();
	void			showObj();
	void			hideObj();
	bool			getObjPos(QPointF &pos);
	void			setCircleRect(QRect rt);
	QRect			getCircleRect();
	void			setPadRect(QRect rt);
	bool			checkCircleItem();

	agSWInfoObject*	getPadParentData();
	void			setPadParentData(agSWInfoObject* obj);

	bool			checkSwLoc(QString strSwLoc);

	void			setObjRect(QRectF rt);
	QRectF			getObjRect() { return m_objRect; }

	void	setSourceSwitch(int nSource) { m_nSource = nSource; }
	void	setTargetSwitch(int nTarget) { m_nTarget = nTarget; }

	int		getSourceSwitch() { return m_nSource; }
	int		getTargetSwitch() { return m_nTarget; }

	bool	getObjPos(QRectF &rt);
	bool	getObjRect(sp_RectItem &sp_rt);
	void	setCustomerPenData();

	sp_LineItem	lineObject();
	sp_PolyLineItem	polylineObject();								// 20151210 KHK
	void	setEndPointLine(int nEndLine);							// 20151223 KHK
	int		GetOrderCount() { return m_swPadinfo.size(); }			// 20151223 KHK
	QList<sp_SwInfoObj>*  getPadData() { return &m_swPadinfo; }					

	//void	changeTextColor(QColor color);

signals:
	void			sendSwObj(int swId , QString strObjName);
	void			sendCanvasPos(QRectF rt);

public slots:

	void		connectOtherTabObj();
	void		blinkShow();

private:
	QString								m_strObjName;
	QString								m_strModifyDate;

	int									m_swId;

	int									m_nViewMode;
	QTimer*								m_timer;
	bool								m_bIsBlink;
	int									m_nPos;
	
	QRect								m_cirRt;
	QRect								m_padRt;
		
	std::tr1::shared_ptr<agObjectBase>	m_spObjectBase;

	QList<sp_SwInfoObj>					m_swPadinfo;
	agSWInfoObject*						m_swParentPadInfo;

	QRectF								m_objRect;

	int									m_nSource;
	int									m_nTarget;

	// 퉤퉤 나중에 알고리즘? 만들 수 있음 만들던지 해야 할듯..
	// 하하 이런상황에 알고리즘이라니 개가 웃을 듯 아무튼 나중에 객체 클래스 다이어그램부터 다시 하자
	int									m_secId;
	int									m_nSourceSecId;
	int									m_nTargetSecId;
	int									m_nUpSecId;
	int									m_nDownSecId;
	int									m_GroupCount;
	bool								m_bGroup;

	QList<int>							m_nEndSecList;			// 20151223 KHK 엔트포인트 소스측 선로만 저장리스트
	QString								m_strCaptionName;
};

#endif // AGSWINFOOBJECT_H
