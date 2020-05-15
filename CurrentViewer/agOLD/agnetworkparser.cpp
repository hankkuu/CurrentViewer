#include "StdAfx.h"
#include "QPen"
#include "QBrush"
#include "QVector"
#include "agcanvaswidget.h"
#include "agnetworkparser.h"

agNetworkParser::agNetworkParser(QObject *parent, agCanvasWidget* canvas)
	: QObject(parent)
{
	m_bIs1st = true;
	m_pCanvas = canvas;
	m_bIsPad = false;
	m_nCircuitCnt = 0;
	m_mapCustomer.clear();
}

agNetworkParser::~agNetworkParser()
{
	m_mapCustomer.clear();
}

void agNetworkParser::setViewType(int nType)
{
	m_nViewType = nType;
}

/*
	@breif			SDS,NDS에서 전송받은 xml 데이터 형식의 계통정보를 파싱하여 저장하는 함수
	@param			const QString	&strXml		xml 데이터 형식의 계통정보
	@param			sp_DlInfoObj	Obj			DL정보 저장하는 객체
*/
/*const sp_CircleItem& agNetworkParser::ReadXmlNetwork(const QString &strXml, sp_DlInfoObj Obj, agCanvasScene* scene)*/
bool agNetworkParser::ReadXmlNetwork(const QString &strXml, sp_DlInfoObj Obj, agCanvasScene* scene)
{
	QXmlStreamReader reader(strXml);		//		--> 생성자에 xml 데이터 입력하기
	bool bRet = true;

	while( !reader.atEnd() )
	{
		reader.readNext();

		if (reader.error())
		{
			bRet = false;
			// error msg 출력
			QString str = QString::fromLocal8Bit("계통 토폴로지 서비스를 통해 전달받은 데이터 형식이 잘못되었습니다.");
			QMessageBox::warning(NULL , QString::fromLocal8Bit("데이터 형식 오류") , QString::fromLocal8Bit(str.toLocal8Bit()),QMessageBox::NoButton);
			break;
		}

		if( reader.isStartElement() )
		{
			if( reader.name() == "diagram" )
			{
				// diagram 읽는 부분
				ReadDiagram(reader , Obj , scene);
			}
			else
			{
				// error msg 출력
				bRet = false;
				//QString str = reader.errorString();
				QString str = QString::fromLocal8Bit("계통 토폴로지 서비스를 통해 전달받은 데이터 형식이 잘못되었습니다.");
				QMessageBox::warning(NULL , QString::fromLocal8Bit("데이터 형식 오류") , QString::fromLocal8Bit(str.toLocal8Bit()),QMessageBox::NoButton);
				break;
			}
		}
	}

	return bRet;
	//return m_pc;
}

/*
	@breif			diagram 항목 파싱
	@param			const QString &strXml		xml 데이터 형식의 계통정보
	@param			sp_DlInfoObj	Obj			DL정보 저장하는 객체
*/
void agNetworkParser::ReadDiagram(QXmlStreamReader &reader, sp_DlInfoObj Obj , agCanvasScene* scene)
{	
	Q_ASSERT(reader.isStartElement() && reader.name() == "diagram");

	Obj->SetModifyDate(reader.attributes().value("maketime").toString());
	Obj->SetNetworkPosition(reader.attributes().value("height").toString().toInt() , reader.attributes().value("width").toString().toInt());
	
	QString sCB;
	sCB = QString::fromLocal8Bit("one");							// 투개방에 의해 CB가 두개 되는 것을 방지 체크

	while (reader.readNextStartElement()) {
		if (reader.name() == "device")
			ReadDevice(reader , Obj , scene, sCB);
		else
			reader.skipCurrentElement();
	}	
}

/*
	@breif			device 항목 파싱
	@param			const QString &strXml		xml 데이터 형식의 계통정보
	@param			sp_DlInfoObj	Obj			DL정보 저장하는 객체
*/
void agNetworkParser::ReadDevice(QXmlStreamReader &reader, sp_DlInfoObj Obj , agCanvasScene* scene, QString& nCBCount )
{
	Q_ASSERT(reader.isStartElement() && reader.name() == "device");

	sp_SwInfoObj swObj( new agSWInfoObject(NULL, m_pCanvas ) );

	swObj->SetObjName(reader.attributes().value("object_name").toString());
	swObj->SetSwId(reader.attributes().value("dev_id").toString().toInt());

	if( reader.attributes().hasAttribute("SecId") == true )
	{
		swObj->SetSecId(reader.attributes().value("SecId").toString().toInt() );
		swObj->SetGroup(true);
	}
	else
	{
		if( reader.attributes().hasAttribute("SourceSecId") == true )
			swObj->setSourceSecId(reader.attributes().value("SourceSecId").toString().toInt());

		if( reader.attributes().hasAttribute("TargetSecId") == true )
			swObj->setTargetSecId(reader.attributes().value("TargetSecId").toString().toInt());

		if( reader.attributes().hasAttribute("UpId") == true )
			swObj->setUpSecId(reader.attributes().value("UpId").toString().toInt());

		if( reader.attributes().hasAttribute("DownId") == true )
			swObj->setDownSecId(reader.attributes().value("DownId").toString().toInt());
	}

	if( reader.attributes().hasAttribute("Source") == true )
		swObj->setSourceSwitch(reader.attributes().value("Source").toString().toInt());

	if( reader.attributes().hasAttribute("Target") == true )
		swObj->setTargetSwitch(reader.attributes().value("Target").toString().toInt());

	bool bIsMark = false;
	m_bIsPad = true;
	// 파서 시작
	while( reader.readNextStartElement() )
	{
		if( reader.name() == "pad")
		{
			ReadPadRect(reader , swObj);
		}
		else if( reader.name() == "order")
		{
			ReadOrderCount(reader , swObj);
		}
		else if( reader.name() == "circle")
		{
			ReadCircle(reader , swObj , scene);
		}
		else if( reader.name() == "caption" )
		{
			ReadText(reader , swObj , scene , true);
		}
		else if( reader.name() == "mark" || reader.name() == "dlload" )
		{
			ReadText(reader , swObj , scene);
		}
		else if( reader.name() == "polygon" )
		{
			ReadPolygon(reader , swObj , scene);
		}
		else if( reader.name() == "polyline" )
		{
			ReadPolyLine(reader , swObj , scene);
			scene->appendInternalLoopLine(swObj->getSwId());
		}
		else if( reader.name() == "rect")
		{
			ReadRect(reader , swObj , scene);
		}
		else if( reader.name() == "line")
		{
			ReadLine(reader , swObj , scene);
		}
		else if( reader.name() == "textBoxRect" )
		{
			ReadTextBoxRect(reader , swObj , scene);
		}
		else if( reader.name() == "ConnectionDL" )
		{
			ReadConnectDL(reader , swObj, scene);
		}
		else
			reader.skipCurrentElement();		
	}
	
	if( m_nCircuitCnt == 0 && m_bIsPad == false)
	{
		m_swPad->setCircuitPos();
		m_swPad.reset();
	}
		 
	if(( (agCanvasWidget*)scene->parent())->getViewMode() == agCanvasWidget::OLDMode)
	{
		//Obj->setOldXPoint(swObj->getObjRect().center());
		if( swObj->GetObjName().compare("CB") == 0 )
		{
			QRectF rt = swObj->getCircleRect();
			Obj->setCBRect(rt);			
		}
	}

	Obj->SetSwInfoList(swObj);

	int nDevID = swObj->getSwId();
	QString SWName = swObj->GetObjName();

	if( SWName == "Dummy")										// 20151223 KHK dummy가 0일 경우 EndPoint가 0일 때 처리 하기 위해
	{
		Obj->setDummyList(swObj);
	}	

	if( nDevID == 0 && SWName == "EndPoint")
	{
		int nEndSecLine = swObj->getSourceSecId();
		swObj->setEndPointLine(nEndSecLine);
	}	
	
	if( swObj->GetObjName().compare("CB") == 0 && nCBCount == "one" )		// 거지 같네... 루프인지 아닌지 판단
	{
		QString sFindDB;
		sFindDB = QString::fromLocal8Bit("find");

		int nCBId = swObj->getSwId(); 
		setCBId(nCBId); 

		QRectF rt = swObj->getCircleRect();				// CB 좌표 정보 삽입 KHK 엔제드모드에서 되도록 setCircleRect를 바꿈
		Obj->setCBRect(rt);		
		nCBCount = nCBCount + sFindDB;
	}
}

void agNetworkParser::ReadPadRect(QXmlStreamReader &reader , sp_SwInfoObj Obj)
{
	Q_ASSERT(reader.isStartElement() && reader.name() == "pad");
	
	QRect rt;
	ReadRectPoint(rt , reader);

	Obj->setPadRect(rt);

	m_bIsPad = false;
	reader.readNext();
}

void agNetworkParser::ReadOrderCount(QXmlStreamReader &reader , sp_SwInfoObj Obj)
{
	Q_ASSERT(reader.isStartElement() && reader.name() == "order");

	for( int i = 0; i < 6; i++ )
	{
		QString str = QString::fromLocal8Bit("order%1").arg(i + 1);

		if( reader.attributes().hasAttribute(str) == true )
		{
			m_nCircuitCnt++;
		}
		else 
			break;
	}
	if( Obj->GetGroup() ) 
	{
		Obj->setGroupCount(m_nCircuitCnt);   // 그룹
	}
	
	m_swPad = Obj;
	reader.readNext();
}

/*
	@breif			Line 항목 파싱
	@param			QXmlStreamReader &reader		xml 데이터 형식의 계통정보
	@param			sp_SwInfoObj Obj			DL정보 저장하는 객체
	@param			agCanvasScene* scene		QGraphicsScene을 상속받는 클래스	
*/
void agNetworkParser::ReadLine(QXmlStreamReader &reader, sp_SwInfoObj Obj , agCanvasScene* scene)
{
	Q_ASSERT(reader.isStartElement() && reader.name() == "line");

	sp_LineItem pl( new agLineItem(Obj._Get()) );

	if( pl )
	{
		int nColor = reader.attributes().value("stroke").toString().toInt();
		int nWidth = reader.attributes().value("stroke-width").toString().toInt();
		int nStyle = reader.attributes().value("stroke-dasharray").toString().toInt();
		
		int nX1 = reader.attributes().value("x1").toString().toInt();
		int nY1 = reader.attributes().value("y1").toString().toInt();
		int nX2 = reader.attributes().value("x2").toString().toInt();
		int nY2 = reader.attributes().value("y2").toString().toInt();
		
		QPoint p1(nX1 , nY1);
		QPoint p2(nX2 , nY2);

		QColor color;
		ColculateColor(nColor , color);

		if( nStyle == 3 )
		{
			QPen pen(QBrush(color) , nWidth*2 , Qt::DotLine ,/*Qt::RoundCap*/ Qt::FlatCap);
		
			pl->setPen(pen);
		}
		else if( nStyle == 0 )
		{
			QPen pen(QBrush(color) , nWidth*2 , Qt::SolidLine ,/*Qt::RoundCap*/ Qt::FlatCap);
			
			pl->setPen(pen);
		}

		if(( (agCanvasWidget*)scene->parent())->getViewMode() == agCanvasWidget::NetWorkMode)
		{
			// cutomer가 있는지 확인 한다.
			sp_SwInfoObj sp;
			if( checkTargetCurtomer( Obj->getTargetSwitch() , sp ) != true )
			{
				checkTargetCurtomer( Obj->getSourceSwitch() , sp );
			}
			
			if( sp )
				makePosition(sp , p1 , p2);			
		}

		//20151208 KHK CB 인접/평행 선로 저장
		int nSecID = Obj->getSwId();
		QVector<QPoint> vt;
		vt.push_back(p1);
		vt.push_back(p2);			
		pl->setPolygon(QPolygonF(vt));
		
		pl._Get()->setLineId(QString::number(nSecID));						   // KHK 라인ID 멤버로 저장
		pl._Get()->setLineSourceId(QString::number(Obj->getSourceSwitch()));   // KHK 라인ID 멤버로 저장
		pl._Get()->setLineTargetId(QString::number(Obj->getTargetSwitch()));   // KHK 라인ID 멤버로 저장
		pl._Get()->setLineStyle( nStyle );

		int nCBId = CBId();
		if( Obj->getSourceSwitch() == nCBId )									// KHK CB 우측 시작 선로를 알기 위해
		{
			pl->setCBLine(true);
			pl->setCBLinePolygon(QPolygonF(vt));
		}

		Obj->SetLineItem(pl);
		scene->addItem(pl._Get());
	}	

	reader.readNext();
}

void agNetworkParser::makePosition(sp_SwInfoObj sp , QPoint& p1 , QPoint& p2)
{
	if( sp )
	{
		bool bIs = false;
		sp_RectItem sp_rt;
		if( sp->getObjRect(sp_rt) == true )
		{
			QRectF rt = sp_rt->boundingRect();

			int nPixelsWidth = sp_rt->getWidth();
			int nPixelsHeight = sp_rt->getHeight();

			QPoint center = rt.center().toPoint();	

			if( center.y() == p2.y() )
			{
				// 가로 연결임.
				int nXpos = 0;
				bool bIsCompare = true;
				if( center.x() < p2.x() )
				{
					nXpos = center.x() + (nPixelsWidth/2);

					if( p1.x() < p2.x() )
						bIsCompare = true;
					else if( p1.x() > p2.x() ) 
						bIsCompare = false;
				}
				else if( center.x() > p2.x() )
				{
					nXpos = center.x() - (nPixelsWidth/2);

					if( p1.x() < p2.x() )
						bIsCompare = false;
					else if( p1.x() > p2.x() ) 
						bIsCompare = true;
				}
				
				if( bIsCompare == true )
					p1.setX(nXpos);
				else
					p2.setX(nXpos);

				bIs = true;
				
			}
			else if( center.x() == p2.x() )
			{
				// 세로 연결임.
			
				int nYpos = 0;
				bool bIsCompare = true;
				if( center.y() < p2.y() )
				{
					nYpos = center.y() + (nPixelsHeight/2);

					if( p1.y() < p2.y() )
						bIsCompare = true;
					else if( p1.y() > p2.y() ) 
						bIsCompare = false;
				}
				else if( center.y() > p2.y() )
				{
					nYpos = center.y() - (nPixelsHeight/2);

					if( p1.y() < p2.y() )
						bIsCompare = false;
					else if( p1.y() > p2.y() ) 
						bIsCompare = true;
				}
				//p2.setY(nYpos);
				if( bIsCompare == true )
					p1.setY(nYpos);
				else
					p2.setY(nYpos);
				bIs = true;
			}
			else if( center.x() == p1.x() )
			{
				// 세로 연결임.

				int nYpos = 0;
				bool bIsCompare = true;
				if( center.y() < p1.y() )
				{
					nYpos = center.y() + (nPixelsHeight/2);

					if( p1.y() < p2.y() )
						bIsCompare = false;
					else if( p1.y() > p2.y() ) 
						bIsCompare = true;
				}
				else if( center.y() > p1.y() )
				{
					nYpos = center.y() - (nPixelsHeight/2);

					if( p1.y() < p2.y() )
						bIsCompare = true;
					else if( p1.y() > p2.y() ) 
						bIsCompare = false;
				}
				/*p1.setY(nYpos);*/
				if( bIsCompare == true )
					p2.setY(nYpos);
				else
					p1.setY(nYpos);
				bIs = true;
			}
			else if( center.y() == p1.y() )
			{
				// 가로 연결임.
				int nXpos = 0;
				bool bIsCompare = true;
				if( center.x() < p1.x() )
				{
					nXpos = center.x() + (nPixelsWidth/2);
					if( p1.x() < p2.x() )
						bIsCompare = false;
					else if( p1.x() > p2.x() ) 
						bIsCompare = true;
				}
				else if( center.x() > p1.x() )
				{
					nXpos = center.x() - (nPixelsWidth/2);

					if( p1.x() < p2.x() )
						bIsCompare = true;
					else if( p1.x() > p2.x() ) 
						bIsCompare = false;
				}
				/*p1.setX(nXpos);*/

				if( bIsCompare == true )
					p2.setX(nXpos);
				else
					p1.setX(nXpos);
				bIs = true;
			}

			if( bIs == true )
			{
				rt.setX(center.x() - (nPixelsWidth/2));
				rt.setY(center.y() - (nPixelsHeight/2));

				rt.setWidth(nPixelsWidth-5);
				rt.setHeight(nPixelsHeight);

				sp_rt->setRect(rt);
			}				
		}
	}
}


/*
	@breif			polygon 항목 파싱
	@param			QXmlStreamReader &reader		xml 데이터 형식의 계통정보
	@param			sp_SwInfoObj Obj			개폐기 개체 정보 저장하는 객체
	@param			agCanvasScene* scene		QGraphicsScene을 상속받는 클래스	
*/
void agNetworkParser::ReadPolygon(QXmlStreamReader &reader, sp_SwInfoObj Obj , agCanvasScene* scene)
{
	Q_ASSERT(reader.isStartElement() && ( reader.name() == "polygon"  ));
	QString strCaption;
	sp_PolygonItem pp( new agPolygonItem(Obj._Get()) );

	if( pp )
	{
		QColor strokecolor;
		ColculateColor(reader.attributes().value("stroke").toString().toInt() , strokecolor);

		QPen strokePen(strokecolor);
		strokePen.setWidth(reader.attributes().value("stroke-width").toString().toInt());
		pp->setPen(strokePen);

		QColor fillcolor;
		ColculateColor(reader.attributes().value("fill").toString().toInt() , fillcolor);

		// qt에서 setAlpha 함수로 넘기면 인자 1값이 쉬프트 연산으로 인해 다른값으로 변함..
		// 차후 해당 문제 확인후 저장하는걸로..

		/*int nAlpha = reader.attributes().value("fill-opacity").toString().toInt();
		fillcolor.setAlpha(nAlpha);*/

		QBrush brush;
		brush.setColor(fillcolor);
		brush.setStyle(Qt::SolidPattern);
		pp->setBrush(brush);

		ParserPointsString(reader.attributes().value("points").toString() , pp );

		QRect rt;
		if( ReadRectPoint(rt , reader) == true )
		{
			sp_TextItem pt( new agTextItem() );

			if( pt )
			{
				pt->setFontColor(strokecolor);

				pt->setRect(rt);
				/*Obj->setObjRect(rt);*/

				QFont font(QString::fromLocal8Bit("맑은 고딕"), 12);

				font.setBold(true);

				pt->setFont(font);
				pt->setFontColor(strokecolor);

				pt->setText(reader.readElementText());
				Obj->SetTextItem(pt);
				scene->addItem(pt._Get());
			}

		}
		else
			reader.readElementText();
		/*else*/
			//reader.readNext();

		pp->setObjName(Obj->GetObjName());
		Obj->SetPolygonItem(pp);

		if( m_nCircuitCnt > 0  && m_bIsPad == true )
		{
			m_nCircuitCnt--;	// m_nCircuitCnt가 0이 아니면 패드쪽 데이터 파싱중
			Obj->setCircleRect(rt);
			m_swPad->setPadData(Obj);
			m_bIsPad = false;
		}


		scene->addItem(pp._Get());
	}

}

/*
	@breif			polyLine 항목 파싱
	@param			QXmlStreamReader &reader		xml 데이터 형식의 계통정보
	@param			sp_SwInfoObj Obj			개폐기 개체 정보 저장하는 객체
	@param			agCanvasScene* scene		QGraphicsScene을 상속받는 클래스	
*/
void agNetworkParser::ReadPolyLine(QXmlStreamReader &reader, sp_SwInfoObj Obj , agCanvasScene* scene )
{
	Q_ASSERT(reader.isStartElement() && ( reader.name() == "polyline"  ));
	
	sp_PolyLineItem pp( new agPolyLineItem() );

	if( pp )
	{
		QColor strokecolor;
		ColculateColor(reader.attributes().value("stroke").toString().toInt() , strokecolor);

		QPen strokePen(strokecolor);
		strokePen.setWidth(reader.attributes().value("stroke-width").toString().toInt()*2);
		pp->setPen(strokePen);

		QVector<QPoint>	vt;
		ParserPointsString(reader.attributes().value("points").toString() , vt);

		/*if(( (agCanvasWidget*)scene->parent())->getViewMode() == agCanvasWidget::NetWorkMode)
		{
			// cutomer가 있는지 확인 한다.
			sp_SwInfoObj sp;
			if( checkTargetCurtomer( Obj->getTargetSwitch() , sp ) != true )
			{
				checkTargetCurtomer( Obj->getSourceSwitch() , sp );
			}

			if( sp )
			{
				for( int i = 1; i < vt.size(); i++ )
				{
					makePosition(sp , vt[i-1] , vt[i]);
				}
			}

		}*/

		pp->setPolygon(QPolygonF(vt));
		Obj->SetPolyLineItem(pp);
		scene->addItem(pp._Get());
	}

	reader.readNext();

}

/*
	@breif			위치 정보를 파싱하여 저장하는 함수
	@param			QString& strPoint			위치정보가 저장되어있는 문자열 
	@param			sp_PolygonItem pp			위치 정보를 저장할 객체
*/
void agNetworkParser::ParserPointsString(QString& strPoint , sp_PolygonItem pp)
{
	QVector<QPoint>	vt;

	if( strPoint.isEmpty())
	{
		vt.push_back(QPoint(0,0));
		pp->setPolygon(QPolygonF(vt));
		return;
	}

	QStringList  strTmp = strPoint.split(" ");

	for( QStringList::iterator itr = strTmp.begin(); itr != strTmp.end(); itr++ )
	{
		QString strPoint = *itr;

		QStringList strPointList = strPoint.split(",");

		SetQPointData(strPointList , vt);
	}

	pp->setPolygon(QPolygonF(vt));
	
}

/*
	@breif			위치 정보를 파싱하여 저장하는 함수
	@param			QString& strPoint			위치정보가 저장되어있는 문자열 
	@param			sp_PolyLineItem pp			위치 정보를 저장할 객체
*/
void agNetworkParser::ParserPointsString(QString& strPoint , /*sp_PolyLineItem pp*/QVector<QPoint>	&vt)
{
	/*QVector<QPoint>	vt;*/

	if( strPoint.isEmpty())
	{
		vt.push_back(QPoint(0,0));
	//	pp->setPolygon(QPolygonF(vt));
		return;
	}

	QStringList  strTmp = strPoint.split(" ");

	for( QStringList::iterator itr = strTmp.begin(); itr != strTmp.end(); itr++ )
	{
		QString strPoint = *itr;

		QStringList strPointList = strPoint.split(",");

		SetQPointData(strPointList , vt);
	}

	//pp->setPolygon(QPolygonF(vt));
}


/*
	@breif			파싱된 위치 정보를 저장하는 함수
	@param			QStringList &strPoint		위치정보가 저장되어있는 문자열 리스트
	@param			QVector<QPoint>& vt			파싱된 데이터를 저장할 백터 아웃 버퍼
*/
void agNetworkParser::SetQPointData(QStringList &strPoint , QVector<QPoint>& vt)
{
	QPoint p;
	bool bIsFirst = true;
	int nPoint = 0;
	
	for( QStringList::iterator pitr = strPoint.begin(); pitr != strPoint.end(); pitr++ )
	{
		nPoint = (*pitr).toInt();

		if( bIsFirst )
		{
			p.setX(nPoint);
			bIsFirst = false;
		}
		else
		{
			p.setY(nPoint);
			bIsFirst = true;
		}
	}

	if( bIsFirst )
		vt.push_back(p);
}

/*
	@breif			색깔 정보를 계산하여 리턴해주는 함수
	@param			int nColor		계산하기 전 색 값
	@param			QColor&			색 정보를 저장하는 클래스 아웃 버퍼
*/
void agNetworkParser::ColculateColor(int nColor , QColor& color )
{
	int scale = nColor % ( 256 * 256 );

	int b = nColor / ( 256 * 256 );

	int g = scale / ( 256 );

	int r = scale % ( 256 );

	if( ( r == 255 && g == 255 && b == 255 ) || ( r == 200  && g == 200 && b == 255) )
	{
		color = QColor(Qt::white);
	}
	else if ( r == 255 && g == 0 && b == 0 )	// 빨간색
	{
		color = QColor(Qt::red);
	}
	else if ( r == 128 && g == 0 && b == 0 )	// 짙은 빨간색
	{
		color = QColor(Qt::darkRed);
	}
	else if ( r == 0 && g == 255 && b == 0 )	// 초록색
	{
		color = QColor(Qt::green);
	}
	else if ( r == 0 && g == 128 && b == 0 )	// 짙은 초록색
	{
		color = QColor(Qt::darkGreen);
	}
	else if ( r == 0 && g == 0 && b == 255 )	// 파란색
	{
		color = QColor(Qt::blue);
	}
	else if ( r == 255 && g == 255 && b == 0 )	// 노란색
	{
		color = QColor(Qt::yellow);
	}
	else if ( r == 128 && g == 128 && b == 0 )	// 짙은 노란색
	{
		color = QColor(Qt::darkYellow);
	}
	else if ( r == 0 && g == 0 && b == 0 )	// 검정색
	{
		color = QColor(Qt::black);
	}
	else if ( r == 128 && g == 128 && b == 128 )	// 회색
	{
		color = QColor(Qt::gray);
	}
	else if ( r == 192 && g == 192 && b == 192 )	// 회색
	{
		color = QColor(Qt::lightGray);
	}
	else if ( r == 128 && g == 0 && b == 255 )	// 짙은 자주색
	{
		color = QColor(Qt::darkMagenta);
	}
	else if ( r == 0 && g == 255 && b == 255 )
	{
		color = QColor(Qt::cyan);
	}
	else
	{	
		color = QColor(r,g,b);
	}
}


/*
	@breif			색깔 정보를 계산하여 리턴해주는 함수
	@param			int nColor		계산하기 전 색 값
	@return			QColor			색 정보를 저장하는 클래스
*/
QColor agNetworkParser::ColculateColor(int nColor)
{
	int scale = nColor % ( 256 * 256 );

	int b = nColor / ( 256 * 256 );

	int g = scale / ( 256 );

	int r = scale % ( 256 );

	if( r == 255 && g == 255 && b == 255 )
	{
		return QColor(Qt::white);
	}
	else if ( r == 255 && g == 0 && b == 0 )	// 빨간색
	{
		return QColor(Qt::red);
	}
	else if ( r == 128 && g == 0 && b == 0 )	// 짙은 빨간색
	{
		return QColor(Qt::darkRed);
	}
	else if ( r == 0 && g == 255 && b == 0 )	// 초록색
	{
		return QColor(Qt::green);
	}
	else if ( r == 0 && g == 128 && b == 0 )	// 짙은 초록색
	{
		return QColor(Qt::darkGreen);
	}
	else if ( r == 0 && g == 0 && b == 255 )	// 파란색
	{
		return QColor(Qt::blue);
	}
	else if ( r == 255 && g == 255 && b == 0 )	// 노란색
	{
		return QColor(Qt::yellow);
	}
	else if ( r == 128 && g == 128 && b == 0 )	// 짙은 노란색
	{
		return QColor(Qt::darkYellow);
	}
	else if ( r == 0 && g == 0 && b == 0 )	// 검정색
	{
		return QColor(Qt::black);
	}
	else if ( r == 128 && g == 128 && b == 128 )	// 회색
	{
		return QColor(Qt::gray);
	}
	else if ( r == 192 && g == 192 && b == 192 )	// 회색
	{
		return QColor(Qt::lightGray);
	}
	else if ( r == 128 && g == 0 && b == 255 )	// 짙은 자주색
	{
		return QColor(Qt::darkMagenta);
	}
	else
	{	
		return QColor(r,g,b);
	}
}



/*
	@breif			circle 항목 파싱
	@param			QXmlStreamReader &reader		xml 데이터 형식의 계통정보
	@param			sp_SwInfoObj Obj			개폐기 개체 정보 저장하는 객체
	@param			agCanvasScene* scene		QGraphicsScene을 상속받는 클래스	
*/
void agNetworkParser::ReadCircle(QXmlStreamReader &reader, sp_SwInfoObj Obj , agCanvasScene* scene)
{
	Q_ASSERT(reader.isStartElement() && reader.name() == "circle");

	sp_CircleItem pc( new agCircleItem(Obj._Get()) );

	if( pc )
	{
		QColor strokecolor;
		ColculateColor(reader.attributes().value("stroke").toString().toInt() , strokecolor);

		QPen strokePen(strokecolor);
		pc->setPen(strokePen);

		QColor fillcolor;
		ColculateColor(reader.attributes().value("fill").toString().toInt() , fillcolor);

		QBrush brush;
		brush.setColor(fillcolor);
		brush.setStyle(Qt::SolidPattern);
		pc->setBrush(brush);

		QRect rt;
		ReadRectPoint(rt , reader);
		
		QRectF temp = rt;
		if(( (agCanvasWidget*)scene->parent())->getViewMode() == agCanvasWidget::OLDMode)			// KHK 자동보기 화면 정렬를 위해 모드 바꿈
		{
			if( Obj->GetObjName().compare("TRPart1") != 0 && Obj->GetObjName().compare("TRPart2") != 0 )
				temp.adjust(0.5,0.5,-0.5,-0.5);
		}
		else //(scene->parent()->getViewMode() == agCanvasWidget::OLDMode)							// 모드 전환 시킴 
		{
			temp.adjust(0.4,0.4,-0.4,-0.4);

			if( Obj->GetObjName().compare("CB") == 0 )
			{
				Obj->setCircleRect(QRect(temp.x() , temp.y() , temp.width() , temp.height()));
			}
		}

		/*Obj->setObjRect(temp);*/


		pc->setRect(temp);
		pc->setObjName(Obj->GetObjName());

		Obj->SetCircleItem(pc);
		scene->addItem(pc._Get());

		if( m_nCircuitCnt > 0 && m_bIsPad == true)
		{
			m_nCircuitCnt--;	// m_nCircuitCnt가 0이 아니면 패드쪽 데이터 파싱중
			Obj->setCircleRect(rt);
			m_swPad->setPadData(Obj);
			m_bIsPad = false;
		}
	}


	reader.readNext();
}


/*
	@breif			rect 항목 파싱
	@param			QXmlStreamReader &reader		xml 데이터 형식의 계통정보
	@param			sp_SwInfoObj Obj			개폐기 개체 정보 저장하는 객체
	@param			agCanvasScene* scene		QGraphicsScene을 상속받는 클래스	
*/
void agNetworkParser::ReadRect(QXmlStreamReader &reader, sp_SwInfoObj Obj , agCanvasScene* scene)
{
	Q_ASSERT(reader.isStartElement() && reader.name() == "rect");

	sp_RectItem pr( new agRectItem(Obj._Get()) );

	if( pr )
	{
		QRect rt;
		ReadRectPoint(rt , reader);
		
		QRectF temp = rt;
		if( ( (agCanvasWidget*)scene->parent())->getViewMode() == agCanvasWidget::NetWorkMode)
		{
			
		}
		else //(scene->parent()->getViewMode() == agCanvasWidget::OLDMode)						// 모드 전환 시킴
		{
			temp.adjust(0.5,0.5,-0.5,-0.5);
		}

		
		/*Obj->setObjRect(temp);*/
		pr->setRect(temp);

		QColor strokecolor;
		ColculateColor(reader.attributes().value("stroke").toString().toInt() , strokecolor);

		QPen strokePen(strokecolor);
		pr->setPen(strokePen);

		QColor fillcolor;
		ColculateColor(reader.attributes().value("fill").toString().toInt() , fillcolor);

		QBrush brush;
		brush.setColor(fillcolor);
		brush.setStyle(Qt::SolidPattern);
		pr->setBrush(brush);

		pr->setObjName(Obj->GetObjName());
		
		Obj->SetRectItem(pr);
		
		if( ( (agCanvasWidget*)scene->parent())->getViewMode() == agCanvasWidget::OLDMode)
		{
			Obj->setCustomerPenData();
		}

		scene->addItem(pr._Get());

		if( m_nCircuitCnt > 0  && m_bIsPad == true  )
		{
			m_nCircuitCnt--;	// m_nCircuitCnt가 0이 아니면 패드쪽 데이터 파싱중
			Obj->setCircleRect(rt);
			m_swPad->setPadData(Obj);
			m_bIsPad = false;
		}
	}

	reader.readNext();
}

/*
	@breif			rect 항목 파싱
	@param			QXmlStreamReader &reader		xml 데이터 형식의 계통정보
	@param			sp_SwInfoObj Obj			개폐기 개체 정보 저장하는 객체
	@param			agCanvasScene* scene		QGraphicsScene을 상속받는 클래스	
*/
void agNetworkParser::ReadTextBoxRect(QXmlStreamReader &reader, sp_SwInfoObj Obj , agCanvasScene* scene)
{
	Q_ASSERT(reader.isStartElement() && reader.name() == "textBoxRect");

	int nRotate = reader.attributes().value("Rotate").toString().toInt();

	QColor color;
	ColculateColor(reader.attributes().value("color").toString().toInt() , color);

	QColor backcolor;
	ColculateColor(reader.attributes().value("backcolor").toString().toInt() , backcolor);

	int nHeight = reader.attributes().value("height").toString().toInt();
	int nWidth = reader.attributes().value("width").toString().toInt();
	int nCenterY = reader.attributes().value("y").toString().toInt();
	int nCenterX = reader.attributes().value("x").toString().toInt();

	int nX = nCenterX - (nWidth/2);
	int nY = nCenterY - (nHeight/2);

	QRectF rect(nX , nY , nWidth , nHeight);

	if( Obj->GetObjName().compare("CB") == 0 )
	{
		sp_RectItem pr( new agRectItem(Obj._Get()) );

		int nSize = reader.attributes().value("size").toString().toInt();
		QFont font(QString::fromLocal8Bit("맑은 고딕"), nSize*0.7);
		font.setBold(true);
		QString strText ;

		if( pr )
		{
			pr->setRotate(nRotate);
			/*pr->setRect(rect);*/

			QBrush brush;
			brush.setColor(backcolor);
			brush.setStyle(Qt::SolidPattern);
			pr->setBrush(brush);

			pr->setObjName(Obj->GetObjName());

			Obj->SetRectItem(pr);
			scene->addItem(pr._Get());
		}

		sp_TextItem pt( new agTextItem() );

		if( pt )
		{
			pt->setRotate(nRotate);
			pt->setFontColor(color);
			/*pt->setRect(rect);*/
			
			pt->setFont(font);

			strText = reader.readElementText();
			pt->setText(strText);
			Obj->SetTextItem(pt);
			scene->addItem(pt._Get());
		}

		QFontMetrics fm(font);
		int nPixelsWidth; 
		int nPixelsHeight;

		if( nRotate != 1 && nRotate != 2 )
		{	
			nPixelsWidth = fm.width(strText) + 3;
			nPixelsHeight = fm.height();	
		}	
		else
		{
			nPixelsHeight = fm.width(strText) + 3;
			nPixelsWidth = fm.height();
		}

		QPointF point = rect.center();
		QRectF rt((point.x() - (nPixelsWidth/2)) , (point.y() - (nPixelsHeight/2)) , nPixelsWidth , nPixelsHeight);

		pr->setRect(rt);
		pt->setRect(rt);

		/*Obj->setObjRect(rt);*/
	}
	else if( Obj->GetObjName().compare("Customer") == 0 )
	{
		sp_RectItem pr( new agRectItem(Obj._Get()) );

		if( pr )
		{
			QColor penColor(Qt::white);
			QPen pen(penColor);

			pr->setPen(pen);

			pr->setObjName(Obj->GetObjName());
		}

		sp_TextItem pt( new agTextItem() );

		if( pt )
		{
			QString strText = reader.readElementText();
			pt->setFontColor(color);

			int nSize = reader.attributes().value("size").toString().toInt();

			QFont font(QString::fromLocal8Bit("맑은 고딕"), nSize*0.7);
			font.setBold(true);

			pt->setFont(font);

			QFontMetrics fm(font);
			int nPixelsWidth = fm.width(strText);
			int nPixelsHeight = fm.height();
			
			pr->setWidth(nPixelsWidth);
			pr->setHeight(nPixelsHeight);

			pt->setRect(rect);
			pr->setRect(rect);

			pt->setText(strText);

			Obj->SetTextItem(pt);
			scene->addItem(pt._Get());

			Obj->SetRectItem(pr);
			scene->addItem(pr._Get());
		}

		if(( (agCanvasWidget*)scene->parent())->getViewMode() == agCanvasWidget::NetWorkMode)
		{
			if( Obj->GetObjName().compare("Customer") == 0 )
			{
				m_mapCustomer.insert(Obj->getSwId() , Obj);
			}
		}
	}
	else
	{
		sp_TextItem pt( new agTextItem() );

		if( pt )
		{
			pt->setFontColor(color);

			pt->setRect(rect);
			/*Obj->setObjRect(rect);*/

			int nSize = reader.attributes().value("size").toString().toInt();

			QFont font(QString::fromLocal8Bit("맑은 고딕"), nSize*0.7);
			font.setBold(true);

			pt->setFont(font);
			if( Obj->GetObjName().compare("Comment") == 0 )
				pt->setCaption(true);

			pt->setText(reader.readElementText());
			Obj->SetTextItem(pt);
			scene->addItem(pt._Get());
		}
	}
	
}

/*
	@breif			연계 DL 보기 항목 파싱
	@param			QXmlStreamReader &reader		xml 데이터 형식의 계통정보
	@param			sp_SwInfoObj Obj			개폐기 개체 정보 저장하는 객체
	@param			agCanvasScene* scene		QGraphicsScene을 상속받는 클래스	
*/
void agNetworkParser::ReadConnectDL(QXmlStreamReader &reader, sp_SwInfoObj Obj , agCanvasScene* scene)
{
	Q_ASSERT(reader.isStartElement() && ( reader.name() == "ConnectionDL"  ));
	QString strCaption;
	sp_PolygonItem pp( new agPolygonItem(Obj._Get()) );

	if( pp )
	{
		QColor strokecolor;
		ColculateColor(reader.attributes().value("stroke").toString().toInt() , strokecolor);

		QPen strokePen(strokecolor);
		strokePen.setWidth(reader.attributes().value("stroke-width").toString().toInt());
		pp->setPen(strokePen);

		QColor fillcolor;
		ColculateColor(reader.attributes().value("fill").toString().toInt() , fillcolor);

		// qt에서 setAlpha 함수로 넘기면 인자 1값이 쉬프트 연산으로 인해 다른값으로 변함..
		// 차후 해당 문제 확인후 저장하는걸로..

		/*int nAlpha = reader.attributes().value("fill-opacity").toString().toInt();
		fillcolor.setAlpha(nAlpha);*/

		QBrush brush;
		brush.setColor(fillcolor);
		brush.setStyle(Qt::SolidPattern);
		pp->setBrush(brush);

		ParserPointsString(reader.attributes().value("points").toString() , pp );

		QRect rt;
		if( ReadRectPoint(rt , reader) == true )
		{
			sp_TextItem pt( new agTextItem() );

			if( pt )
			{
				pt->setFontColor(strokecolor);

				pt->setRect(rt);
				/*Obj->setObjRect(rt);*/

				QFont font(QString::fromLocal8Bit("맑은 고딕"), 10);

				font.setBold(true);

				pt->setFont(font);
				pt->setFontColor(strokecolor);

				pt->setText(reader.readElementText());
				Obj->SetTextItem(pt);
				scene->addItem(pt._Get());
			}

		}
		else
			reader.readElementText();
		/*else*/
			//reader.readNext();

		pp->setObjName(Obj->GetObjName());
		Obj->SetPolygonItem(pp);

		scene->addItem(pp._Get());
	}
}
/*
	@breif			text 항목 파싱
	@param			QXmlStreamReader &reader		xml 데이터 형식의 계통정보
	@param			sp_SwInfoObj Obj			개폐기 개체 정보 저장하는 객체
	@param			agCanvasScene* scene		QGraphicsScene을 상속받는 클래스	
	@param			bool	bIsCaption			caption 정보의 유무를 파악하는 변수
*/
void agNetworkParser::ReadText(QXmlStreamReader &reader, sp_SwInfoObj Obj , agCanvasScene* scene /*, const QRect rect*/, bool bIsCaption )
{
	Q_ASSERT(reader.isStartElement() && (reader.name() == "mark" || reader.name() == "dlload" || reader.name() == "caption"));

	sp_TextItem pt( new agTextItem() );

	if( pt )
	{
		QColor color;
		ColculateColor(reader.attributes().value("color").toString().toInt() , color);
		pt->setFontColor(color);


		int nHeight = reader.attributes().value("height").toString().toInt();
		int nWidth = reader.attributes().value("width").toString().toInt();
		int nY = reader.attributes().value("y").toString().toInt();
		int nX = reader.attributes().value("x").toString().toInt();

		nX = nX - (nWidth/2);
		nY = nY - (nHeight/2);
		pt->setRect(nX , nY-5 , nWidth , nHeight);
		/*Obj->setObjRect(QRectF(nX , nY-5 , nWidth , nHeight));*/

		QFont font(QString::fromLocal8Bit("맑은 고딕"), (reader.attributes().value("size").toString().toInt()*0.7));
	
		font.setBold(true);
		pt->setFont(font);
		
		if( reader.name() == "mark" )
			pt->setMark(true);           
		else if( reader.name() == "caption" )
			pt->setCaption(true);

		pt->setText(reader.readElementText());
		Obj->SetTextItem(pt);
		scene->addItem(pt._Get());

	}

}

/*
	@breif			rect의 위치 정보를 파싱하는 함수
	@param			QRect& rect						위치정보를 저장하는 아웃버퍼
	@param			QXmlStreamReader &reader		xml 데이터 형식의 계통정보
*/
bool agNetworkParser::ReadRectPoint(QRect& rect , QXmlStreamReader &reader)
{
	if(reader.attributes().value("right").toString().isEmpty() )
		return false;
	/*Q_ASSERT(reader.attributes().value("right").toString().isEmpty());*/
		
		
	rect.setRight(reader.attributes().value("right").toString().toInt());
	rect.setLeft(reader.attributes().value("left").toString().toInt());
	rect.setBottom(reader.attributes().value("bottom").toString().toInt());
	rect.setTop(reader.attributes().value("top").toString().toInt());

	return true;
}

bool agNetworkParser::checkTargetCurtomer(int nTargetSw , sp_SwInfoObj& sp)
{
	bool bRet = false;
	QMap<int, sp_SwInfoObj>::iterator itr = m_mapCustomer.find(nTargetSw);

	if( itr != m_mapCustomer.end() )
	{
		sp = (*itr);

		/*if( sp->getSwId() == 3656 )
			int aa = 0;*/

		bRet = true;
	}

	return bRet;
}