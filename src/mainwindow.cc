#include "mainwindow.h"

#include "./ui_mainwindow.h"

#include "edit/editDictionary.h"

MainWindow::MainWindow(QWidget * parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, remoteRepository(new RemoteRepository(QStringLiteral("http://localhost:2628/api/"), this))
{
	ui->setupUi(this);

	setupMenu();

	ui->queryScreen->setRemoteRepository(remoteRepository.data());
}

MainWindow::~MainWindow()
{
	delete ui;
}

RemoteRepository * MainWindow::getRemoteRepository() const
{
	return remoteRepository.data();
}

void MainWindow::setupMenu(){
	QMenuBar *menuBar = this->menuBar();

	QMenu *fileMenu = menuBar->addMenu(QStringLiteral("File"));
	QAction *cutAction = fileMenu->addAction(QStringLiteral("Quit"));

	QMenu *editMenu = menuBar->addMenu(QStringLiteral("Edit"));
	QAction *dictionariesAction = editMenu->addAction(QStringLiteral("Dictionaries"));
	connect(dictionariesAction, &QAction::triggered, this, &MainWindow::editDictionary);
	QAction *groupsAction = editMenu->addAction(QStringLiteral("Groups"));
	QAction *sourcesAction = editMenu->addAction(QStringLiteral("Sources"));
	QAction *preferencesAction = editMenu->addAction(QStringLiteral("Preferences"));

	QMenu *helpMenu = menuBar->addMenu(QStringLiteral("Help"));
	QAction *aboutAction = helpMenu->addAction(QStringLiteral("About"));
	QAction *aboutQtAction = helpMenu->addAction(QStringLiteral("About Qt"));
}

void MainWindow::editDictionary() {
	DictionaryDialog *dictionaryDialog = new DictionaryDialog(this, this->getRemoteRepository());
	dictionaryDialog->exec();
}

