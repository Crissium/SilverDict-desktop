#include "mainwindow.h"

#include "./ui_mainwindow.h"
#include "edit/dictionarydialog.h"
#include "edit/groupdialog.h"
#include "preferences/preferencesdialogue.h"

#include <QCloseEvent>

void MainWindow::closeEvent(QCloseEvent * event)
{
	hide();
	event->ignore();
}

void MainWindow::onQuit() const
{
	preferences->setGeometry(saveGeometry());
}

MainWindow::MainWindow(QWidget * parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, remoteRepository(new RemoteRepository(this))
	, preferences(new Preferences(this))
	, trayIcon(new QSystemTrayIcon(this))
	, trayIconMenu(new QMenu(this))
	, actionRestore(new QAction(tr("Restore"), this))
{
	ui->setupUi(this);
	restoreGeometry(preferences->getGeometry());
	connect(qApp, &QCoreApplication::aboutToQuit, this, &MainWindow::onQuit);

	// Set up actions
	connect(ui->actionCloseToTray, &QAction::triggered, this, &QMainWindow::hide);
	connect(ui->actionQuit, &QAction::triggered, qApp, &QCoreApplication::quit);
	connect(actionRestore.data(), &QAction::triggered, this, &QMainWindow::showNormal);
	connect(ui->actionDictionaries, &QAction::triggered, this, &MainWindow::manageDictionaries);
	connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::openPreferencesDialogue);

	connect(ui->actionDictionaries, &QAction::triggered, this, &MainWindow::manageDictionaries);
	connect(ui->actionGroups, &QAction::triggered, this, &MainWindow::manageGroups);
    
	// Set up tray icon
	trayIconMenu->addAction(actionRestore.data());
	trayIconMenu->addAction(ui->actionQuit);
	trayIcon->setContextMenu(trayIconMenu.data());
	trayIcon->setIcon(windowIcon());
	trayIcon->show();

	// Set up remote repository
	QString serverAddress = preferences->getServerAddress();
	QUrl apiPrefix = QUrl(serverAddress).resolved(QStringLiteral("/api/"));
	while (!RemoteRepository::apiPrefixValid(apiPrefix).result())
	{
		bool ok;
		serverAddress = QInputDialog::getText(
			this,
			tr("Please enter the server address"),
			QStringLiteral(),
			QLineEdit::Normal,
			serverAddress,
			&ok);
		if (ok && !serverAddress.isEmpty())
		{
			apiPrefix = QUrl(serverAddress).resolved(QStringLiteral("/api/"));
		}
	}
	preferences->setServerAddress(serverAddress);
	if (!remoteRepository->setApiPrefix(apiPrefix))
	{
		qDebug() << "Failed to initialise remote repository even after validating the API prefix" << apiPrefix;
		QMessageBox::critical(this, tr("Error"), tr("Failed to initialise application."));
		close();
	}
	ui->queryScreen->setup(remoteRepository.data(), preferences.data());
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

void MainWindow::openPreferencesDialogue()
{
	PreferencesDialogue dialogue(remoteRepository.data(), preferences.data(), this);
	dialogue.exec();
}

void MainWindow::manageGroups()
{
	GroupDialog * groupDialog =
		new GroupDialog(this, this->getRemoteRepository());
	groupDialog->exec();
}
