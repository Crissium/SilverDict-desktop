#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QLocale>
#include <QTranslator>

int main(int argc, char * argv[])
{
	QApplication a(argc, argv);

	QTranslator translator;
	const QStringList uiLanguages = QLocale::system().uiLanguages();
	for (const QString & locale : uiLanguages)
	{
		const QString baseName = "SilverDict_" + QLocale(locale).name();
		qDebug() << "Trying to load translation for" << baseName;
		if (translator.load("/data/translations/" + baseName))
		{
			a.installTranslator(&translator);
			qDebug() << "Loaded translation for" << locale;
			break;
		}
	}
	MainWindow w;
	w.show();
	return a.exec();
}
