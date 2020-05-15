#include "stdafx.h"
#include "currentviewer.h"

#include <QtGui/QApplication>
#include <QtGui/QStyleFactory> 

int main(int argc, char *argv[])
{
	QApplication App(argc, argv);                  // �����ӿ� �������� QCore / QGUI �����ּ� ����� �� �ְ� �������̽� ������ 

	//qDbAccssor->Instance();
	//qDbAccssor->connectDB();
	//DBFunction->Instance();						// DB ����
	DBFunction->setEnbox();							// Enbox ���� ����
	DBFunction->LoadDataSubsAndCbName();            // �������̸� ���ø����̼�(DBhandler)���� ��ӹ޾Ƽ� ����� �� �ִ�

	CurrentViewer MainW;									// QMainFrame ���� : QWidget		
	QIcon icon(":/CurrentViewer/Resources/icons/app.ico");			
	MainW.setWindowIcon(icon);								// ���α׷� ������ ����
	//MainW.show();
	MainW.showMaximized();									// �ʱ� Ȱ��ȭ�� �ִ�ȭ

	return App.exec();
}
