#ifndef AGOBJECTBASE_H
#define AGOBJECTBASE_H

/**
	@class		agObjectBase
	@date		2015/10/06
	@author		이지수
	@brief		개체 그림 정보가 저장될 base 클래스
	@warning
*/

#include <QObject>
#include "agOLD/agGraphicsClass/agCircleItem.h"
#include "agOLD/agGraphicsClass/agPolygonItem.h"
#include "agOLD/agGraphicsClass/agTextItem.h"
#include "agOLD/agGraphicsClass/agRectItem.h"
#include "agOLD/agGraphicsClass/agLineItem.h"
#include "agOLD/agGraphicsClass/agPolyLineItem.h"

typedef std::tr1::shared_ptr<agCircleItem>		sp_CircleItem;
typedef std::tr1::shared_ptr<agRectItem>			sp_RectItem;
typedef std::tr1::shared_ptr<agPolygonItem>		sp_PolygonItem;
typedef std::tr1::shared_ptr<agTextItem>			sp_TextItem;
typedef std::tr1::shared_ptr<agLineItem>			sp_LineItem;
typedef std::tr1::shared_ptr<agPolyLineItem>		sp_PolyLineItem;

class agObjectBase : public QObject
{
	Q_OBJECT

public:
	agObjectBase(QObject *parent = NULL);
	~agObjectBase();


	void 			DeleteItems();
	void 			DeletePolygonItems();
	void 			DeleteTextItems();
	void 			DeleteCircleItems();
	void 			DeleteRectItems();
	void			DeleteLineItems();
	void			DeletePolyLineItems();

	void			SetCircleItem(const sp_CircleItem& pc);
	void			SetPolygonItem(const sp_PolygonItem& pp);
	void			SetRectItem(const sp_RectItem &pr);
	void			SetTextItem(const sp_TextItem &pt);
	void			SetLineItem(const sp_LineItem &pl);
	void			SetPolyLineItem(const sp_PolyLineItem &pp);

	QList<sp_CircleItem>& getCircleItem();
	QList<sp_PolygonItem>& getPolygonItem();
	QList<sp_RectItem>& getRectItem();
	QList<sp_TextItem>& getTextItem();
	QList<sp_LineItem>& getLineItem();
	QList<sp_PolyLineItem>& getPolyLineItem();


	void			paint(QPainter &painter);	

private:
	QList<sp_TextItem>		m_ListTextItem;
	QList<sp_CircleItem>	m_ListCircleItem;
	QList<sp_RectItem>		m_ListRectItem;
	QList<sp_PolygonItem>	m_ListPolygonItem;
	QList<sp_LineItem>		m_ListLineItem;
	QList<sp_PolyLineItem>	m_ListPolyLineItem;
};

#endif // AGOBJECTBASE_H
