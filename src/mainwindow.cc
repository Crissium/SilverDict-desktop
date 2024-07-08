#include "mainwindow.h"

#include "./ui_mainwindow.h"
#include "edit/dictionarydialog.h"

#include <QCloseEvent>

void MainWindow::closeEvent(QCloseEvent * event)
{
	hide();
	event->ignore();
}

MainWindow::MainWindow(QWidget * parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, remoteRepository(new RemoteRepository(
		  QStringLiteral("http://localhost:2628/api/"), this))
	, trayIcon(new QSystemTrayIcon(this))
	, trayIconMenu(new QMenu(this))
	, actionRestore(new QAction(tr("Restore"), this))
{
	ui->setupUi(this);

	ui->queryScreen->setRemoteRepository(remoteRepository.data());

	// Set up actions
	connect(ui->actionCloseToTray, &QAction::triggered, this, &QMainWindow::hide);
	connect(ui->actionQuit, &QAction::triggered, qApp, &QCoreApplication::quit);
	connect(actionRestore.data(), &QAction::triggered, this, &QMainWindow::showNormal);
	connect(ui->actionDictionaries, &QAction::triggered, this, &MainWindow::manageDictionaries);

	// Set up tray icon
	trayIconMenu->addAction(actionRestore.data());
	trayIconMenu->addAction(ui->actionQuit);
	trayIcon->setContextMenu(trayIconMenu.data());
	trayIcon->setIcon(windowIcon());
	trayIcon->show();
}

MainWindow::~MainWindow()
{
	delete ui;
}

RemoteRepository * MainWindow::getRemoteRepository() const
{
	return remoteRepository.data();
}

void MainWindow::manageDictionaries()
{
	const QScopedPointer<DictionaryDialog> dictionaryDialog(
		new DictionaryDialog(this, remoteRepository.data()));
	dictionaryDialog->exec();
}
