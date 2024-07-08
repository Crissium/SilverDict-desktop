#include "mainwindow.h"

#include "./ui_mainwindow.h"
#include "edit/dictionarydialog.h"

MainWindow::MainWindow(QWidget * parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, remoteRepository(new RemoteRepository(
		  QStringLiteral("http://localhost:2628/api/"), this))
{
	ui->setupUi(this);

	connectActions();

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

void MainWindow::connectActions()
{
	QList<QAction *> actionsList = this->ui->menuEdit->actions();
	foreach(QAction * action, actionsList)
	{
		if (action->text() == "Dictionaries")
		{
			connect(action, &QAction::triggered, this, &MainWindow::editDictionary);
		}
	}
}

void MainWindow::editDictionary()
{
	DictionaryDialog * dictionaryDialog =
		new DictionaryDialog(this, this->getRemoteRepository());
	dictionaryDialog->exec();
}
