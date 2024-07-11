#include "sourcedialog.h"
#include "ui_sourcedialog.h"

SourceDialog::SourceDialog(QWidget * parent, RemoteRepository * repo)
	: QDialog(parent)
	, ui(new Ui::SourceDialog)
{
	ui->setupUi(this);

	setRemoteRepository(repo);

	populateListWidget();

	connect(ui->scanButton, &QPushButton::clicked, this, &SourceDialog::onScanButtonClicked);
	connect(ui->addButton, &QPushButton::clicked, this, &SourceDialog::onAddButtonClicked);
	connect(this, &SourceDialog::updateUISignal, this, &SourceDialog::populateListWidget);
}

SourceDialog::~SourceDialog()
{
	delete ui;
}

void SourceDialog::setRemoteRepository(RemoteRepository * repo)
{
	remoteRepository = repo;
}

void SourceDialog::populateListWidget()
{
	ui->listWidget->clear();

	for (const QString & source : remoteRepository->getSources())
	{
		QWidget * itemWidget = new QWidget();
		QLabel * label = new QLabel(source);
		QPushButton * deleteButton = new QPushButton("Delete");

		QHBoxLayout * layout = new QHBoxLayout();
		layout->addWidget(label);
		layout->addWidget(deleteButton);
		layout->setContentsMargins(0, 0, 0, 0); // 去除布局的外边距
		layout->setStretch(0, 5);
		layout->setStretch(1, 1);

		itemWidget->setLayout(layout);

		QListWidgetItem * listWidgetItem = new QListWidgetItem(ui->listWidget);
		ui->listWidget->setItemWidget(listWidgetItem, itemWidget);

		connect(deleteButton, &QPushButton::clicked, this, [=]() {
			onDeleteButtonClicked(source, listWidgetItem);
		});
	}
}

void SourceDialog::addSourceItem(const QString & source)
{
	QWidget * itemWidget = new QWidget();
	QLabel * label = new QLabel(source);
	QPushButton * deleteButton = new QPushButton("Delete");

	QHBoxLayout * layout = new QHBoxLayout();
	layout->addWidget(label);
	layout->addWidget(deleteButton);
	layout->setContentsMargins(0, 0, 0, 0); // 去除布局的外边距
	layout->setStretch(0, 5);
	layout->setStretch(1, 1);

	itemWidget->setLayout(layout);

	QListWidgetItem * listWidgetItem = new QListWidgetItem(ui->listWidget);
	ui->listWidget->setItemWidget(listWidgetItem, itemWidget);

	connect(deleteButton, &QPushButton::clicked, this, [=]() {
		onDeleteButtonClicked(source, listWidgetItem);
	});
}

void SourceDialog::onScanButtonClicked()
{
	remoteRepository->rescanSources()
		.then([=](bool result) {
			if (result)
			{
				emit this->updateUISignal();
			}
			else
			{
				qDebug() << "scan error";
			}
		});
}

void SourceDialog::onAddButtonClicked()
{
	bool ok;
	QString newSource = QInputDialog::getText(this, QStringLiteral("Add Source"), "", QLineEdit::Normal, "", &ok);

	if (ok && !newSource.isEmpty())
	{
		remoteRepository->addSource(newSource)
			.then([=](bool result) {
				if (result)
				{
					qDebug() << newSource;
					emit this->updateUISignal();
				}
				else
				{
					QMessageBox::warning(this, QStringLiteral("Add Source Failed"),
										 QStringLiteral("The route is incorrect."));
				}
			});
	}
}

void SourceDialog::onDeleteButtonClicked(const QString & source, QListWidgetItem * item)
{
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, "Delete", "Are you sure you want to delete this source?",
								  QMessageBox::Yes | QMessageBox::No);
	if (reply == QMessageBox::Yes)
	{
		remoteRepository->deleteSource(source)
			.then([=](bool result) {
				emit this->updateUISignal();
			});
	}
}
