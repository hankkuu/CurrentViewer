#include "StdAfx.h"
#include "agcanvaswidget.h"
#include "document.h"

agCanvasWidget::agCanvasWidget(QWidget *parent , QGridLayout* layout, /*agMainWindowBase* window,*/ int nMode) : QGraphicsView(parent)
{
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(ShowContextMenu(const QPoint &)));
	
	m_nCurView = 2;
	if(nMode == agCanvasWidget::OLDMode )
	{
		for( int i = 0; i < 3; i++ )
		{
			m_oldscene[i] = NULL;
			m_oldscene[i] = new agCanvasScene(this);

			m_oldobj[i].reset(new agDLNetworkInfo());			
		}
		setScene(m_oldscene[0]);
	}
	else
	{
		m_scene = new agCanvasScene(this);
		setScene(m_scene);

		sp_DlInfoObj obj(new agDLNetworkInfo() );
		m_obj = obj;
	}

	setCacheMode(CacheBackground);
	setTransformationAnchor(AnchorUnderMouse);

	QBrush brush(Qt::black);
	setBackgroundBrush(brush);

 	setDragMode(QGraphicsView::ScrollHandDrag);						// 마우스 우클릭시 드래그 하는 녀석 RubberBandDrag - 이건 영역 드래그
 	setInteractive(true);
	setViewportUpdateMode(FullViewportUpdate);
	setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	
	m_nViewMode = nMode;
	m_parent = parent;

	m_bFirstShow = true;
	m_bIsFullScreen = false;

	m_nOfficeCode = 0;
	m_nDlCode = 0;
	m_objSp.reset();
	
	m_ShiftDown = false;
	m_CtrDown = false;
}

agCanvasWidget::~agCanvasWidget()
{	
	m_strXmlData.clear();
	if( m_obj.use_count() != 0 )
		m_obj.reset();	

	if( m_objSp.use_count() != 0 )
		m_objSp.reset();
		

	for( int i = 0; i < 3; i++ )
	{
		if( m_oldobj[i].use_count() != 0 )
			m_oldobj[i].reset();

		if( m_oldObjSp[i].use_count() != 0 )
			m_oldObjSp[i].reset();
	}
}

void agCanvasWidget::showChangeScreen()
{
	if( m_bIsFullScreen )
	{
		this->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOn );
		this->setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOn );
		m_bIsFullScreen = false;
	}
	else
	{
		this->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
		this->setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
		m_bIsFullScreen = true;
	}
}

void agCanvasWidget::resizeEvent(QResizeEvent *event)
{	
	QGraphicsView::resizeEvent(event);
	//setViewSize();
}

void agCanvasWidget::setXmlData(QString &strXml)
{
	m_strXmlData = strXml;
}

bool agCanvasWidget::SaveOldNetwork(int nIdx)
{
	if( m_strXmlData.size() == 0 )
	{
		//QString str;
		//str.sprintf("전송받은 계통 데이터 포맷이 다릅니다. 해당 사업소를 확인하여 주십시오.");
		QString str = QString::fromLocal8Bit("전송받은 계통 데이터 포맷이 다릅니다. 해당 사업소를 확인하여 주십시오.");
		QMessageBox::warning(NULL , QString::fromLocal8Bit("데이터 포맷 오류") , QString::fromLocal8Bit(str.toLocal8Bit()),QMessageBox::NoButton);
		return false;
	}

	agNetworkParser agReader(NULL , this);
	bool bRet = false;

	agReader.setViewType(m_nViewMode);

	if( agReader.ReadXmlNetwork(m_strXmlData , m_oldobj[nIdx] , m_oldscene[nIdx]) == true )
	{
		int nWidth = m_oldobj[nIdx]->getNetworkWidth();
		int nHeight = m_oldobj[nIdx]->getNeworkHeight();

		if( nIdx == 2 )
			setScene(m_oldscene[nIdx]);

		//m_fiOldDrawer[nIdx].setScene(m_oldscene[nIdx]);	
		//m_fiOldDrawer[nIdx].setItem(m_fiOldDrawer[nIdx].getItem());


		bRet = true;
	}

	m_strXmlData.clear();
	return  bRet;
}

void agCanvasWidget::deleteOldScene(int nIdx)
{
	if( m_oldscene[nIdx] != NULL )
	{
		delete m_oldscene[nIdx];
		m_oldscene[nIdx] = new agCanvasScene(this);
	}
}
void agCanvasWidget::deleteNzedScene()
{
	if( m_scene != NULL )
	{
		delete m_scene;
		m_scene = new agCanvasScene(this);
	}	
}


bool agCanvasWidget::SaveNzedNetwork()
{
	agNetworkParser agReader(NULL , this);
	bool bRet = false;

	agReader.setViewType(m_nViewMode);

	if( agReader.ReadXmlNetwork(m_strXmlData , m_obj , m_scene ) == true )
	{
		int nWidth = m_obj->getNetworkWidth();
		int nHeight = m_obj->getNeworkHeight();
		setScene(m_scene);
		//m_fiDrawer.setScene(m_scene);
		//m_fiDrawer.setItem(m_fiDrawer.getItem());
		bRet = true;
	}
	
	m_strXmlData.clear();

	return bRet;
}

agCanvasScene* agCanvasWidget::getCanvasScene()
{
	return m_scene;
}

agCanvasScene* agCanvasWidget::getOldCanvasScene()
{
	agCanvasScene* canvas = qobject_cast<agCanvasScene*>(scene());

	if( canvas != NULL )
		return canvas;
	else 
		return NULL;
}

sp_DlInfoObj&	agCanvasWidget::getInfoObj()
{
	return m_obj;
}

sp_DlInfoObj&	agCanvasWidget::getInfoObj(int nIdx)
{
	return m_oldobj[nIdx];
}

void agCanvasWidget::deleteObj()
{
	if( m_obj )
	{
		m_obj.reset();

		sp_DlInfoObj obj(new agDLNetworkInfo() );
		m_obj = obj;
	}
}

void agCanvasWidget::deleteObj(int nIdx)
{
	if( m_oldobj[nIdx] )
	{
		m_oldobj[nIdx].reset();

		sp_DlInfoObj obj(new agDLNetworkInfo() );
		m_oldobj[nIdx] = obj;
	}
}

void agCanvasWidget::SaveNzedNetwork(agCanvasScene* scene)
{
	m_scene = scene;
	setScene(m_scene);
}
bool agCanvasWidget::getSwObjPos(int nSwId, QPointF &pos, int &dir)
{
	return m_obj->getSwObjPos(nSwId, pos, dir);
}
bool agCanvasWidget::getOld1SwObjPos(int nSwId, QPointF &pos, int &dir)
{
	return m_oldobj[0]->getSwObjPos(nSwId, pos, dir);
}
bool agCanvasWidget::getOld2SwObjPos(int nSwId, QPointF &pos, int &dir)
{
	return m_oldobj[1]->getSwObjPos(nSwId, pos, dir);
}
bool agCanvasWidget::getOld3SwObjPos(int nSwId, QPointF &pos, int &dir)
{
	return m_oldobj[2]->getSwObjPos(nSwId, pos, dir);
}
void agCanvasWidget::getOldCanvasSenes(agCanvasScene*	oldscenes[3])
{
	oldscenes[0] = m_oldscene[0];
	oldscenes[1] = m_oldscene[1];
	oldscenes[2] = m_oldscene[2];
}
/*
void agCanvasWidget::addFI(QString ID, int FiType, QPointF pos, int dir)
{
	m_fiDrawer.addItem(ID,FiType, pos, dir);
}
void agCanvasWidget::revertFI(QString ID, int FiType)
{
	m_fiDrawer.revertFI(ID,FiType);
}
void agCanvasWidget::setFtCurrent(QString ID, QMap<QString,QVariant> &current)
{
	m_fiDrawer.setCurrentValue(ID, current);
}
void agCanvasWidget::addOLD1FI(QString ID, int FiType, QPointF pos, int dir)
{
	m_fiOldDrawer[0].addItem(ID,FiType, pos, dir);
}
void agCanvasWidget::revertOLD1FI(QString ID, int FiType)
{
	m_fiOldDrawer[0].revertFI(ID,FiType);
}
void agCanvasWidget::setOLD1FtCurrent(QString ID, QMap<QString,QVariant> &current)
{
	m_fiOldDrawer[0].setCurrentValue(ID, current);
}
void agCanvasWidget::addOLD2FI(QString ID, int FiType, QPointF pos , int dir)
{	
	m_fiOldDrawer[1].addItem(ID,FiType, pos, dir);
}
void agCanvasWidget::revertOLD2FI(QString ID, int FiType)
{
	m_fiOldDrawer[1].revertFI(ID,FiType);
}
void agCanvasWidget::setOLD2FtCurrent(QString ID, QMap<QString,QVariant> &current)
{
	m_fiOldDrawer[1].setCurrentValue(ID, current);
}
void agCanvasWidget::addOLD3FI(QString ID, int FiType, QPointF pos, int dir)
{
	m_fiOldDrawer[2].addItem(ID,FiType, pos, dir);
}
void agCanvasWidget::revertOLD3FI(QString ID, int FiType)
{
	m_fiOldDrawer[2].revertFI(ID,FiType);
}
void agCanvasWidget::setOLD3FtCurrent(QString ID, QMap<QString,QVariant> &current)
{
	m_fiOldDrawer[2].setCurrentValue(ID, current);
}*/

void agCanvasWidget::showEvent(QShowEvent * event)
{	
	if( m_bFirstShow )
	{
		setViewSize();
		showWidgetView();
		m_bFirstShow = false;
// 		if(scene())
//  		{
//  			scene()->setSceneRect(scene()->itemsBoundingRect());
//  			fitInView(scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
//  		}
	}
}

void agCanvasWidget::setViewSize()
{
	setMinimumHeight(m_parent->geometry().height()/2.2);		// width 도 변경 시키면 너비가 침범하지 않을 영역이 나올 것 같다
	setMaximumHeight(m_parent->geometry().height()/2.2);
	//setMinimumWidth(m_parent->geometry().width()/2);
	//setMaximumWidth(m_parent->geometry().width()/2);

	QRect parentRt = m_parent->geometry();

	int nWidth = parentRt.width();
	int nHeight = parentRt.height();

	this->resize(nWidth , nHeight);
}

void agCanvasWidget::showWidgetView()
{	
	if( m_nViewMode == agCanvasWidget::NetWorkMode )
	//if( m_nViewMode == agCanvasWidget::OLDMode )
	{
		//QRectF rt = sceneRect();
		//int nWidth = m_obj->getNetworkWidth();
		//int nHeight = m_obj->getNeworkHeight();
		
		//rt.setWidth(nWidth);
		//rt.setHeight(nHeight);
		//fitInView(rt , Qt::KeepAspectRatio);
		//centerOn(rt.center());

		QRectF rt = sceneRect();

		//if( m_oldobj[m_nCurView] )							// 임시로 열어둠 KHK
		//	return;
		int nWidth = m_obj->getNetworkWidth();
		int nHeight = m_obj->getNeworkHeight();

		QRectF cbRt = m_obj->getCBRect();

		if( cbRt.center().y() > (nHeight/2) && nWidth != 0 && nHeight != 0)
		{
			int nAdjust = cbRt.center().y() - (nHeight/2.5);


			if( nAdjust > (nHeight/2) )
			{
				rt.setY(nAdjust - (nHeight/2));
			}
			rt.setWidth(nWidth);
			rt.setHeight(nHeight + nAdjust);
		}
		else
		{
			rt.setWidth(nWidth);
			rt.setHeight(nHeight);
		}

		if( nWidth != 0 && nHeight != 0 )
		{
			setSceneRect(rt);
		}

		//fitInView(cbRt.center().x() , rt.y() , 500 , 500 , Qt::KeepAspectRatio);
		fitInView(rt.x() , rt.y() - 100 , 500 , 500 , Qt::KeepAspectRatio);
		//fitInView(rt , Qt::KeepAspectRatio);
		//ensureVisible(cbRt, 50, 0);
		//setTransform(transform());
		centerOn(cbRt.center());
		setResizeAnchor(QGraphicsView::AnchorViewCenter);
		setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	}
	else
	{
		QRectF rt = sceneRect();

		if( m_oldobj[m_nCurView] )							// 임시로 바뀜? 몰라 나도 나중에 3단보기 다시 바꺼 KHK
			return;
		int nWidth = m_oldobj[m_nCurView]->getNetworkWidth();
		int nHeight = m_oldobj[m_nCurView]->getNeworkHeight();

		QRectF cbRt = m_oldobj[m_nCurView]->getCBRect();

		if( cbRt.center().y() > (nHeight/2) && nWidth != 0 && nHeight != 0)
		{
			int nAdjust = cbRt.center().y() - (nHeight/2);
			

			if( nAdjust > (nHeight/2) )
			{
				rt.setY(nAdjust - (nHeight/2));
			}
			rt.setWidth(nWidth);
			rt.setHeight(nHeight + nAdjust);
		}
		else
		{
			rt.setWidth(nWidth);
			rt.setHeight(nHeight);
		}

		if( nWidth != 0 && nHeight != 0 )
		{
			setSceneRect(rt);
		}

		fitInView(rt , Qt::KeepAspectRatio);
		centerOn(rt.center());
		setResizeAnchor(QGraphicsView::AnchorViewCenter);
		setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	}

	
}


void agCanvasWidget::receiveCanvasPos(QRectF rt)
{
	//fitInView(rt.x() , rt.y() , 1500 , 1500 , Qt::KeepAspectRatio);
	fitInView(rt.x() , rt.y() , 500 , 500 , Qt::KeepAspectRatio);
	centerOn(rt.center());

}

void agCanvasWidget::scaleView(qreal scaleFactor )
{
	qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if (factor < 0.01 || factor > 100)
		return;

	scale(scaleFactor, scaleFactor);
}

void agCanvasWidget::wheelEvent(QWheelEvent *event)
{
	QPointF BeforeScale(mapToScene(event->pos()));

	scaleView(pow((double)2, event->delta() / 480.0));

	QStyleOptionGraphicsItem::levelOfDetailFromTransform(viewportTransform());

	event->accept();
	
}

QRectF agCanvasWidget::setSceneRectSizeUp()
{
	QRectF rt = sceneRect();

	rt.setHeight(rt.height() + 100);
	rt.setWidth(rt.width() + 100);

	setSceneRect(rt);

	return rt;
}

void	agCanvasWidget::dragMoveEvent ( QDragMoveEvent * event )
{
	QGraphicsView::dragMoveEvent(event);
}

// void			agCanvasWidget::mousePressEvent(QMouseEvent * e)
// {
// 	if (e->button() == Qt::MidButton)
// 	{
// 		QMouseEvent fake(e->type(), e->pos(), Qt::LeftButton, Qt::LeftButton, e->modifiers());
// 		QGraphicsView::mousePressEvent(&fake);
// 	}
// 	else QGraphicsView::mousePressEvent(e);
// 
// }
// void			agCanvasWidget::mouseReleaseEvent(QMouseEvent * e)
// {
// 	if (e->button() == Qt::MidButton)
// 	{
// 		QMouseEvent fake(e->type(), e->pos(), Qt::LeftButton, Qt::LeftButton, e->modifiers());
// 		QGraphicsView::mouseReleaseEvent(&fake);
// 	}
// 	else QGraphicsView::mouseReleaseEvent(e);
// 
// }

void agCanvasWidget::keyPressEvent(QKeyEvent *event)			// 20151210 KHK 키 입력 받아서 처리하는 부분
{
	//event->ignore();
	int nKey = event->key();

	switch (nKey)
	{
	case Qt::Key_Shift:
		{
			m_ShiftDown = true;
			break;
		}
	case Qt::Key_Control:
		{
			m_CtrDown = true;
			break;
		}

	default:
		QGraphicsView::keyPressEvent(event);
	}
}	

void agCanvasWidget::setViewMode()
{
	if( m_nCurView == 2 )
		m_nCurView = 0;
	else if( m_nCurView == 0 )
		m_nCurView = 1;
	else if( m_nCurView == 1)
		m_nCurView = 2;

	//WriteDkepcoLog->writeLog("1");

	if(m_oldscene[m_nCurView] != NULL)
	{
		//WriteDkepcoLog->writeLog("2. %d",m_nCurView);
		this->setScene(m_oldscene[m_nCurView]);
		//WriteDkepcoLog->writeLog("3");
		showWidgetView();
		//WriteDkepcoLog->writeLog("4");
	}
}


void agCanvasWidget::setViewMode(int nIdx)
{
	m_nCurView = nIdx;

	setScene(m_oldscene[m_nCurView]);
}

void agCanvasWidget::receiveSwObj(int nSwId , QString strObjName)
{
	if( m_nViewMode == agCanvasWidget::OLDMode )
	{
		//agOldWindow* old = GlobalData->getOldWindow();

		//if( old )
		{
			//if( old->focusSwObj(nSwId , m_nOfficeCode , strObjName) != true )
			{
				// 계통 불러오기
				//int nCbId = CommThrQueue->getCbId(m_nOfficeCode , nSwId , m_oldNetwork.first->getCenterIP() , m_oldNetwork.first->getSdsPort() , m_oldNetwork.first->getOfficeName());

				//if( nCbId != 0 )
				{
					//old->setFocusSwObj(m_nOfficeCode , nSwId , strObjName , nCbId);

					//agDLInfo* spt_dl = NULL;
					//GlobalData->findCbInfo(m_oldNetwork.first._Get() , nCbId , &spt_dl);

					//if( spt_dl != NULL )
					{
						//spt_DLInfo dl;
						//dl.reset(new agDLInfo(*(spt_dl)));
						//CommThrQueue->setOldNetworkDataQueue(m_oldNetwork.first , dl);

						//if( dl )
						//	dl.reset();
					}
					//else
					{
						QString strErr;
						strErr = QString::fromLocal8Bit("선택하신 %1 사업소의 네트워크 정보를 찾을 수 없습니다. 컨트롤러를 이용해 주세요.").arg(QString::number(m_nOfficeCode));
						// 해당 사업소가 검색되지 않으면 메세지 박스 출력
						QMessageBox::warning(NULL , QString::fromLocal8Bit("사업소 검색 실패") , strErr,QMessageBox::NoButton);
					}
				}
				//else
				{
					QString strErr;
					strErr = QString::fromLocal8Bit("선택하신 개폐기의 소속 DL을 찾을 수 없습니다. 컨트롤러를 이용해 주세요.");
					// 해당 사업소가 검색되지 않으면 메세지 박스 출력
					QMessageBox::warning(NULL , QString::fromLocal8Bit("사업소 검색 실패") , strErr,QMessageBox::NoButton);
				}
			}
		}
	}
	else if( m_nViewMode == agCanvasWidget::NetWorkMode )
	{
		//agNetworkWindow* network = GlobalData->getNetworkWindow();
		
		//if( network )
		{
			//if( network->focusSwObj(nSwId , m_nOfficeCode , strObjName) != true )
			{
				// 계통 불러오기
				checkFocusSwObj(nSwId , strObjName);
				//network->setFocusSwObj(m_nOfficeCode , nSwId , strObjName);
				//CommThrQueue->setDataQueue(m_oldNetwork.first);
			}
		}
	}
}

bool agCanvasWidget::checkFocusSwObj(int nSwId, QString strObjName)
{
	if( m_nViewMode == agCanvasWidget::OLDMode )
	{
		if( m_oldobj[m_nCurView]->checkSwObj(nSwId , m_oldObjSp[m_nCurView] , strObjName) == true )
		{
			// 깜빡임, 포커스.
			m_oldObjSp[m_nCurView]->startBlinkObj();
			return true;
		}
	}
	else
	{
		if( m_obj->checkSwObj(nSwId , m_objSp , strObjName) == true )
		{
			// 깜빡임, 포커스.					//20151210 KHK 나중에 개폐기와 선로이벤트 구분 할 수 있어야 함
			m_objSp->startBlinkObj();
			return true;
		}
	}
	
	//WriteDkepcoLog->writeLog("검색 대상 %d 개폐기를 찾을 수 없습니다.",nSwId);

	return false;
}

bool agCanvasWidget::checkFocusSwObj(int nSwId, QString strObjName , int nCurView)
{
	if( m_nViewMode == agCanvasWidget::OLDMode )
	{
		if( m_oldobj[nCurView]->checkSwObj(nSwId , m_oldObjSp[nCurView] , strObjName) == true )
		{
			// 깜빡임, 포커스.
			m_oldObjSp[nCurView]->startBlinkObj();
			return true;
		}
	}
	else
	{
		if( m_obj->checkSwObj(nSwId , m_objSp , strObjName) == true )
		{
			// 깜빡임, 포커스.
			m_objSp->startBlinkObj();
			return true;
		}
	}

	//WriteDkepcoLog->writeLog("검색 대상 %d 개폐기를 찾을 수 없습니다.",nSwId);

	return false;
}

bool agCanvasWidget::checkFocusSwObj(int nSwId)
{
	if( m_nViewMode == agCanvasWidget::OLDMode )
		return false;

	if( m_obj->checkSwObj(nSwId , m_objSp) == true )
	{
		// 깜빡임, 포커스.
		m_objSp->startBlinkObj();
		return true;
	}

	//WriteDkepcoLog->writeLog("검색 대상 %d 개폐기를 찾을 수 없습니다.",nSwId);

	return false;
}

bool agCanvasWidget::checkFocusSwObjToCaption(int nSwId, QString strCaption)
{
	if( m_nViewMode == agCanvasWidget::OLDMode )
	{
		if( m_oldobj[m_nCurView]->checkSwObjToCaption(nSwId , m_oldObjSp[m_nCurView] , strCaption) == true )
		{
			// 깜빡임, 포커스.
			m_oldObjSp[m_nCurView]->startBlinkObj();
			return true;
		}
	}
	else
	{
		if( m_obj->checkSwObjToCaption(nSwId , m_objSp , strCaption) == true )
		{
			// 깜빡임, 포커스.
			m_objSp->startBlinkObj();
			return true;
		}
	}

	//WriteDkepcoLog->writeLog("검색 대상 %d 개폐기를 찾을 수 없습니다.",nSwId);

	return false;
}

bool agCanvasWidget::checkFocusSwObjToCaption(int nSwId, QString strCaption , int nIdx)
{
	if( m_nViewMode == agCanvasWidget::OLDMode )
	{
		if( m_oldobj[nIdx]->checkSwObjToCaption(nSwId , m_oldObjSp[nIdx] , strCaption) == true )
		{
			// 깜빡임, 포커스.
			m_oldObjSp[nIdx]->startBlinkObj();
			return true;
		}
	}
	else
	{
		if( m_obj->checkSwObjToCaption(nSwId , m_objSp , strCaption) == true )
		{
			// 깜빡임, 포커스.
			m_objSp->startBlinkObj();
			return true;
		}
	}

	//WriteDkepcoLog->writeLog("검색 대상 %d 개폐기를 찾을 수 없습니다.",nSwId);

	return false;
}

/*
void agCanvasWidget::setDlQueueData(agCommThrQueue::DLNetwork &network)
{
	m_oldNetwork.first.reset(new agLowOfficeInfo(*(network.first)));

	/ *m_oldNetwork.first = network.first;* /
	
	if(network.second != 0 )
		m_oldNetwork.second.reset(new agDLInfo(*(network.second)));
	/ *if( network.second != NULL )
		m_oldNetwork.second = network.second;* /
}

agCommThrQueue::DLNetwork& agCanvasWidget::getDlQueueData()
{
	if( m_nViewMode  == agCanvasWidget::OLDMode )
	{
		if(  m_oldNetwork.first.use_count() == 0 || m_oldNetwork.second.use_count() == 0  ) 
		{
			WriteDkepcoLog->writeLog("해당 계통에 대한 사업소, DL 정보를 찾을 수 없습니다.");
		}
	}
	else
	{
		if(  m_oldNetwork.first.use_count() == 0 ) 
		{
			WriteDkepcoLog->writeLog("해당 계통에 대한 사업소, DL 정보를 찾을 수 없습니다.");
		}
	}
	
	return m_oldNetwork;
}
*/

void agCanvasWidget::stopBlinkObj()
{
	m_objSp->stopBlinkObj();
	//sp_DlInfoObj->checkSwObj()
// 	if( m_nViewMode == agCanvasWidget::OLDMode )
// 	{
// 		for( int i = 0; i < 3; i++ )
// 		{
// 			if( m_oldObjSp[i] )
// 			{
// 				m_oldObjSp[i]->stopBlinkObj();
// 				m_oldObjSp[i].reset();
// 			}
// 		}
// 	}
// 	else
// 	{
// 		if( m_objSp )
// 		{
// 			m_objSp->stopBlinkObj();
// 			m_objSp.reset();
// 		}
// 	}
}

void	agCanvasWidget::reverseBlackColor()
{
	QBrush brush(Qt::black);
	setBackgroundBrush(brush);
}
void	agCanvasWidget::reverseWhiteColor()
{
	QBrush brush(Qt::white);
	setBackgroundBrush(brush);
}

void agCanvasWidget::makeMainLine(QString strServer , int nCBId)
{
	QList<int>	lstTotalLineID;
	agCanvasScene* canvasScene = ((agCanvasScene*)scene());
// 	agLineItem* CBLine = canvasScene->mainCBTargetLine();
// 	agLineItem* LastLine = canvasScene->mainLastLine();	
// 
// 	if( !CBLine )
// 		return;
// 
// 	QString SLineId = CBLine->lineId();
// 	int nStartLineId = SLineId.toInt();
// 
// 	QString ELineId = LastLine->lineId();
// 	int nEndLineId = ELineId.toInt();
// 	// CBLine 부터 저장하면서 구간 검색
// 	//탐색 알고리즘 실행	 	
// 	m_obj->buildAdjList(nStartLineId, nEndLineId);							// 정확한 깊이 우선 탐색이 아직 안됨 
// 	lstTotalLineID = m_obj->getTotalLine();									// 알고리즘이 없는 걸 하루만에 만들라고? 다 죽여버리고 싶다..
// 	m_SectionData = m_obj->getTotalSectionList();							// 내가 회사를 잘못왔구나.. 더 작고 더 허접해 보이는 곳이면.. 내가 원하는 걸 얻을 수 있나 했는데... 
// 	m_BranchData =  m_obj->getBranchSectionList();							// 하기 싫다...... 이 회사 수준을 알만하네.. 여기는 평생 이 정도에서 발전이 없는 곳이다 이래서.. 천재나 뛰어난 애들이 나랑 다른 걸 하는 구나... 그냥 말이면 다인줄 아는구나.. 하루만에 해본적도 없는 걸 만들어오라니...
	
	QList<agLineItem*> lines = canvasScene->lineItems();					// 전체 라인 객체를 가져 옴
	lstTotalLineID = canvasScene->getLineIDList();							// 전체라인 아이디를 얻어 옴  // lineItems 함수에서 멤버를 만든다
	DBFunction->SetChartLineData(lstTotalLineID);							// 쿼리할 전체 라인을 보낸다	
	DBFunction->CreateTotalCurrent(strServer, nCBId);						// 선로의 전류 데이터를 DB로부터 가져 온다

	TypeChartDataMap totalLineDBData;
	totalLineDBData = DBFunction->GetChartSortLineData();	
	// 전류값 각 구간에(각각의 라인) 대입	
	ChartData chartData;
	foreach(int lineId, totalLineDBData.keys())
	{
		foreach(agLineItem* line, lines) 
		{
			if(line->lineId().toInt() == lineId) 
			{
				chartData = totalLineDBData.value(lineId);
				line->setChartData(chartData.fAvg_Current_a, chartData.fAvg_Current_b, chartData.fAvg_Current_c, chartData.sDatetime);
				break;
			}
		}
	}

	setMaxPVlaue(lines);			
}

void agCanvasWidget::setMaxPVlaue(QList<agLineItem*> lineItems)
{
	float fMaxValue = 0.0;
	foreach( agLineItem* line, lineItems )
	{
		float fLinePVlaue = line->getChartPValue();
		if( fLinePVlaue > fMaxValue )
		{
			fMaxValue = fLinePVlaue;
		}
	}
	m_nMaxPValue = fMaxValue;	
}

void agCanvasWidget::makeChartXData(QList<agLineItem*> lineItem)
{
	sp_DlInfoObj obj = managerObj();
	
	int nIx = 0;
	QString strSwCaption;
	int nSize = lineItem.count();

	//for( int i = nSize - 1 ; i >= 0 ; --i )
	for(int i = 0 ; i < lineItem.count() ; i++)
	{
		int nSWId = lineItem.at(i)->lineSourceId().toInt();
		if( nSWId != 0 )
		{			
			sp_SwInfoObj Sw = obj->GetObj(nSWId);

			if( Sw->GetObjName().toLower() == "dummy" && i != (nSize -1) )			// 예외 처리하다 죽을 듯... 중간 더미는 저장 안해 시발 그냥 다음것 찾기
				continue;

			agSWInfoObject* pPadParent = Sw->getPadParentData();
		
			if( pPadParent != NULL )
			{
				strSwCaption = pPadParent->GetCaptionTextName();
			}
			else
			{
				if( Sw->GetObjName().toLower() != "dummy")
					strSwCaption = Sw->GetCaptionTextName();
				else
				{
					sp_SwInfoObj dummyTargetLine = obj->GetObj(Sw->getTargetSecId());				// 더미명은 캡션으로 저장하지 않는다
					sp_SwInfoObj dummyTargetSw = obj->GetObj(dummyTargetLine->getTargetSwitch());
					strSwCaption = dummyTargetSw->GetCaptionTextName();
				}
			}			

			if( !strSwCaption.isEmpty())
			{
				makeSectionXData(nIx, strSwCaption);				
				nIx++;

				if( i == (nSize -1) )										// 마지막 전에 저장을 위해서... 마지막 선로는 무조건 타겟획득
				{
					nSWId = lineItem.at(i)->lineTargetId().toInt();
					if( nSWId != 0 )
					{
						sp_SwInfoObj LastSw = obj->GetObj(nSWId);
						agSWInfoObject* pLastPadParent = LastSw->getPadParentData();

						if( pLastPadParent != NULL )
						{
							strSwCaption = pLastPadParent->GetCaptionTextName();
						}
						else
						{
							strSwCaption = LastSw->GetCaptionTextName();
						}

						if( !strSwCaption.isEmpty())
						{
							makeSectionXData(nIx, strSwCaption);
							nIx++;
						}
					}
				}
			}
		}	
		else					// 개시발 ID = 0 인 녀석 때문에 예외 처리 개시발개끼
		{
			nSWId = lineItem.at(i)->lineTargetId().toInt();
			if( nSWId != 0 )
			{
				sp_SwInfoObj zeroIDDummyTargetSw = obj->GetObj(nSWId);
				agSWInfoObject* pZeroIDDummyTargetSwPadParent = zeroIDDummyTargetSw->getPadParentData();

				if( pZeroIDDummyTargetSwPadParent != NULL )
				{
					strSwCaption = pZeroIDDummyTargetSwPadParent->GetCaptionTextName();
				}
				else
				{
					strSwCaption = zeroIDDummyTargetSw->GetCaptionTextName();
				}

				if( !strSwCaption.isEmpty())
				{
					makeSectionXData(nIx, strSwCaption);
					nIx++;
				}
			}
		}
	}
}
// 이미 저장된 녀석인지 중복을 제거한다.. 왜냐고? 개시팔 id 0 처리하다 중복으로 들어가니까
void agCanvasWidget::makeSectionXData(int nIx, QString strSwCaption)	
{
// 	if(	nIx == 0 )
// 	{
// 		m_sectionXData.insert(nIx, strSwCaption);
// 		return;
// 	}
// 	// 해당 DL에 이름이 같은 녀석이 없다는 가정..
// 	QList<QString>::iterator compareStr = m_sectionXData.values().find(strSwCaption);
// 	if( compareStr == m_sectionXData.values().end() )
		m_sectionXData.insert(nIx, strSwCaption);
}

void agCanvasWidget::makeChartYData(QList<agLineItem*> MainLineItem)
{

}

void agCanvasWidget::ShowContextMenu(const QPoint &pos) 
{
	agCanvasScene* canvasScene = ((agCanvasScene*)scene());
	QList<agLineItem*> selItem = canvasScene->selectedLineItems();
	agCanvasWidget* view = ((agCanvasWidget*)scene()->views().first());

	QMenu contextMenu(tr("Context menu"), view);

	QAction action1(QString::fromLocal8Bit("선택차트 출력"), this);
	connect(&action1, SIGNAL(triggered()),  SIGNAL(printChart()));
	contextMenu.addAction(&action1);		
	QAction action4(QString::fromLocal8Bit("검색시간 변경"), this);
	connect(&action4, SIGNAL(triggered()),  SIGNAL(reTimeSettings()));
	contextMenu.addAction(&action4);
	QAction action2(QString::fromLocal8Bit("전체구간 차트출력"), this);
	connect(&action2, SIGNAL(triggered()),  SIGNAL(printCurrentDayChart()));
	contextMenu.addAction(&action2);
	QAction action3(QString::fromLocal8Bit("간선구간 차트출력"), this);
	connect(&action3, SIGNAL(triggered()),  SIGNAL(printMainLineChart()));
	contextMenu.addAction(&action3);
	QAction action5(QString::fromLocal8Bit("개체선택 모드전환"), this);
	connect(&action5, SIGNAL(triggered()),  SLOT(onDragMouse()));
	contextMenu.addAction(&action5);
	QAction action6(QString::fromLocal8Bit("화면끌기 모드전환"), this);
	connect(&action6, SIGNAL(triggered()),  SLOT(onGrabMaouse()));
	contextMenu.addAction(&action6);

	if( selItem.isEmpty() )
	{
		action1.setVisible(false);
		action4.setVisible(false);
	}
	else
	{
		action2.setVisible(false);
		action3.setVisible(false);
		action5.setVisible(false);
		action6.setVisible(false);
	}

	contextMenu.exec(view->mapToGlobal(pos));
}

void agCanvasWidget::onDragMouse()
{
	setDragMode(QGraphicsView::RubberBandDrag);						// 마우스 우클릭시 드래그 하는 녀석 RubberBandDrag - 이건 영역 드래그
	setInteractive(true);
}

void agCanvasWidget::onGrabMaouse()
{
	setDragMode(QGraphicsView::ScrollHandDrag);						// 마우스 우클릭시 드래그 하는 녀석 RubberBandDrag - 이건 영역 드래그
	setInteractive(true);
}