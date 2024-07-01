#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QLocale>
#include <QTranslator>

int main(int argc, char * argv[])
{
	QApplication a(argc, argv);

	QTranslator translator;
	for (const QString & locale : QLocale::system().uiLanguages())
	{
		if (translator.load(QLocale(locale), "SilverDict", "_", "./translations"))
		{
			qDebug() << "Loaded translation for" << locale;
			a.installTranslator(&translator);
			break;
		}
	}

	MainWindow w;
	w.show();
	return a.exec();
}
