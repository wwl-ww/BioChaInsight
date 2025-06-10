#include "stdafx.h"
#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <QFile>
#include "table.h"
#include <iostream>


int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	MainWindow w;

	// 读取 style.qss
	QFile f("C:\\Users\\wwl\\source\\repos\\BioChaInsight\\styles\\style.qss");
	if (f.open(QIODevice::ReadOnly))
		a.setStyleSheet(QString::fromUtf8(f.readAll()));

	w.setWindowTitle("BioChaInsight");
	w.show();
	return a.exec();

	//std::cout << "Hello, World!" << Qt::endl;

}



