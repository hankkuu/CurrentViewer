#ifndef AGNETWORKPARSER_H
#define AGNETWORKPARSER_H


/**
	@class		agNetworkParser
	@date		2015/10/06
	@author		이지수
	@brief		계통 정보가 저장되어있는 xml을 파싱하여 저장하는 클래스
	@warning
*/


#include <QObject>
#include <QtCore/QCoreApplication>
#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <QXmlStreamReader>
#include <iostream>
#include <QTextCodec>
#include <string>
#include "agdlnetworkinfo.h"

class agCanvasScene;

class agNetworkParser : public QObject
{
	Q_OBJECT

	typedef enum { nRight , nUp , nDown , nLeft };
	typedef enum { brush , pen , font , fontColor};


public:
	agNetworkParser(QObject *parent = NULL , agCanvasWidget* canvas = 0);
	~agNetworkParser();

	//const sp_CircleItem&			ReadXmlNetwork(const QString &strXml , sp_DlInfoObj Obj , agCanvasScene* scene);
	bool							ReadXmlNetwork(const QString &strXml , sp_DlInfoObj Obj , agCanvasScene* scene);
	
	void							setViewType(int nType);
	
	sp_CircleItem	m_pc;
	bool			m_bIs1st;
	sp_SwInfoObj	m_swPad;

	bool			m_bIsPad;
	int				m_nCircuitCnt;

private:
	// 계통 읽기
	void 			ReadDiagram(QXmlStreamReader &reader, sp_DlInfoObj Obj, agCanvasScene* scene);
	void 			ReadDevice(QXmlStreamReader &reader, sp_DlInfoObj Obj, agCanvasScene* scene, QString& nCBCount);


	void 			ReadText(QXmlStreamReader &reader, sp_SwInfoObj Obj , agCanvasScene* scene, bool bIsCaption = false);
	void 			ReadCircle(QXmlStreamReader &reader, sp_SwInfoObj Obj , agCanvasScene* scene);
	void 			ReadRect(QXmlStreamReader &reader, sp_SwInfoObj Obj , agCanvasScene* scene);
	void 			ReadPolygon(QXmlStreamReader &reader, sp_SwInfoObj Obj , agCanvasScene* scene);
	void 			ReadLine(QXmlStreamReader &reader, sp_SwInfoObj Obj , agCanvasScene* scene);
	void			ReadPolyLine(QXmlStreamReader &reader, sp_SwInfoObj Obj , agCanvasScene* scene );
	void			ReadTextBoxRect(QXmlStreamReader &reader, sp_SwInfoObj Obj , agCanvasScene* scene);
	void			ReadConnectDL(QXmlStreamReader &reader, sp_SwInfoObj Obj , agCanvasScene* scene);	

	//void			ColculateColor(int nColor , QColor& color);
	void			ColculateColor(int nColor , QColor& color );
	QColor			ColculateColor(int nColor);
	void			ColculateColor(int nColor , agObjectBase& pObj , int nColorStyle);

	void			SetQPointData(QStringList &strPoint , QVector<QPoint>& vt);
	void			ParserPointsString(QString& strPoint , sp_PolygonItem pp);
	//void			ParserPointsString(QString& strPoint , sp_PolyLineItem pp);
	void			ParserPointsString(QString& strPoint , /*sp_PolyLineItem pp*/QVector<QPoint>	&vt);
	bool			ReadRectPoint(QRect& rect , QXmlStreamReader &reader);
	void			ReadOrderCount(QXmlStreamReader &reader , sp_SwInfoObj Obj);
	void			ReadPadRect(QXmlStreamReader &reader , sp_SwInfoObj Obj);

	bool			checkTargetCurtomer(int nTargetSw , sp_SwInfoObj& sp);
	void			makePosition(sp_SwInfoObj sp , QPoint& p1 , QPoint& p2);

	agCanvasWidget* m_pCanvas;

	int				m_nViewType;

	//QList<sp_SwInfoObj>			m_custmer;
	QMap<int , sp_SwInfoObj>		m_mapCustomer;

	int								CBId() { return m_nCBId; }					// QT 서는 기본 겟함수에서 get을 쓰지 않는다...는..
	void							setCBId(int nCBId) { m_nCBId = nCBId; }
	int								m_nCBId;									// 20151208 KHK
	
};

#endif // AGNETWORKPARSER_H
