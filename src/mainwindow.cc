#include "mainwindow.h"

#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget * parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, remoteRepository(new RemoteRepository(QStringLiteral("http://localhost:2628/api/"), this))
{
	ui->setupUi(this);
	remoteRepository->getSuggestions("word")
		.then([](const Suggestions & suggestions)
		{
			qDebug() << suggestions.wordList;
		});
	remoteRepository->setActiveGroup(remoteRepository->getGroup("French"));
	remoteRepository->getSuggestions("word")
		.then([](const Suggestions & suggestions)
		{
			qDebug() << suggestions.wordList;
		});
}

MainWindow::~MainWindow()
{
	delete ui;
}

RemoteRepository * MainWindow::getRemoteRepository() const
{
	return remoteRepository.data();
}
