#ifndef STYLESHEETEDITOR_H
#define STYLESHEETEDITOR_H

#include <QObject>

class StyleSheetEditor : public QObject
{
	Q_OBJECT

public:
	StyleSheetEditor(QWidget *parent = 0);
	~StyleSheetEditor();

private:
	//Ui::StyleSheetEditor ui;
	void loadStyleSheet(const QString &sheetName);
};

#endif // STYLESHEETEDITOR_H
