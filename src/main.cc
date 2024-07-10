#include "mainwindow.h"
#include "runguard.h"

#include <QApplication>
#include <QDebug>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>

int main(int argc, char * argv[])
{
	RunGuard guard("SilverDict");
	if (!guard.tryToRun())
		return 0;

	QApplication a(argc, argv);

	QTranslator translator;
	QTranslator qtTranslator; // library strings
	for (const QString & locale : QLocale::system().uiLanguages())
	{
		if (translator.load(
				QLocale(locale),
				QStringLiteral("SilverDict"),
				QStringLiteral("_"),
				QStringLiteral("./translations")))
		{
			qDebug() << "Loaded translation for" << locale;
			a.installTranslator(&translator);

			if (qtTranslator.load(
					QLocale(locale),
					QStringLiteral("qtbase"),
					QStringLiteral("_"),
					QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
			{
				qDebug() << "Loaded Qt translation for" << locale;
				a.installTranslator(&qtTranslator);
			}

			break; // Library strings I think could be guaranteed to be loaded
		}
	}

	MainWindow w;
	w.show();
	return a.exec();
}
