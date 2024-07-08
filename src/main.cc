#include "mainwindow.h"
#include "runguard.h"

#include <QApplication>
#include <QDebug>
#include <QLocale>
#include <QTranslator>

int main(int argc, char * argv[])
{
	RunGuard guard("SilverDict");
	if (!guard.tryToRun())
		return 0;

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
