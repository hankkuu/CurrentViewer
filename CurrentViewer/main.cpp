#include "stdafx.h"
#include "currentviewer.h"

#include <QtGui/QApplication>
#include <QtGui/QStyleFactory> 

int main(int argc, char *argv[])
{
	QApplication App(argc, argv);                  // 프레임웍 단위에서 QCore / QGUI 단위애서 사용할 수 있게 인터페이스 성격으 

	//qDbAccssor->Instance();
	//qDbAccssor->connectDB();
	//DBFunction->Instance();						// DB 연결
	DBFunction->setEnbox();							// Enbox 정보 수집
	DBFunction->LoadDataSubsAndCbName();            // 가급적이면 어플리케이션(DBhandler)에서 상속받아서 사용할 수 있다

	CurrentViewer MainW;									// QMainFrame 생성 : QWidget		
	QIcon icon(":/CurrentViewer/Resources/icons/app.ico");			
	MainW.setWindowIcon(icon);								// 프로그램 아이콘 삽입
	//MainW.show();
	MainW.showMaximized();									// 초기 활성화면 최대화

	return App.exec();
}
