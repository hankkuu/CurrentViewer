#include "StdAfx.h"
#include "stylesheeteditor.h"

StyleSheetEditor::StyleSheetEditor(QWidget *parent)
	: QObject(parent)
{
	QRegExp regExp(".(.*)\\+?Style");
	QString defaultStyle = QApplication::style()->metaObject()->className();

	if (regExp.exactMatch(defaultStyle))
		defaultStyle = regExp.cap(1);

	loadStyleSheet("CurrentViewer");
}

StyleSheetEditor::~StyleSheetEditor()
{

}

void StyleSheetEditor::loadStyleSheet(const QString &sheetName)
{
	QFile file(":/StyleSheet/qss/" + sheetName.toLower() + ".qss");
	if( !file.open(QFile::ReadOnly) )
	{
		qDebug() << "could not open file for read!";
	}
	else
	{
		QString styleSheet = QLatin1String(file.readAll());
		qApp->setStyleSheet(styleSheet);
	}
}

