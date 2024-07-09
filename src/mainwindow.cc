#include "mainwindow.h"

#include "./ui_mainwindow.h"
#include "edit/dictionarydialog.h"
#include "edit/groupdialog.h"

MainWindow::MainWindow(QWidget * parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, remoteRepository(new RemoteRepository(
		  QStringLiteral("http://localhost:2628/api/"), this))
{
	ui->setupUi(this);

	connect(ui->actionDictionaries, &QAction::triggered, this, &MainWindow::manageDictionaries);
	connect(ui->actionDictionaries, &QAction::triggered, this, &MainWindow::manageGroups);
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
	DictionaryDialog * dictionaryDialog =
		new DictionaryDialog(this, this->getRemoteRepository());
	dictionaryDialog->exec();
}

void MainWindow::manageGroups()
{
	GroupDialog * groupDialog =
		new GroupDialog(this, this->getRemoteRepository());
	groupDialog->exec();
}
