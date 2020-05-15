#pragma once
#include "stdafx.h"

class ItemMetaData
{
public:
	ItemMetaData(void);
	virtual ~ItemMetaData(void);

public:
	int nCBId;
	int nTreeLevel;
	QString ServerCode;
	QString strSubStation;

	QString strDPName;
	QString strType;
	QString strReleam;
};
