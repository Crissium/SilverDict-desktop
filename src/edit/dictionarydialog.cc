#include "dictionarydialog.h"
#include "ui_dictionarydialog.h"

#include "remote/models.h"

DictionaryDialog::DictionaryDialog(QWidget * parent, RemoteRepository * repo)
	: QDialog(parent)
	, ui(new Ui::DictionaryDialog)
{
	ui->setupUi(this);

	setRemoteRepository(repo);

	populateListWidget();

	connect(ui->listWidget, &QListWidget::itemClicked, this,
			&DictionaryDialog::onListWidgetItemClicked);
	connect(ui->renameButton, &QPushButton::clicked, this,
			&DictionaryDialog::onRenameButtonClicked);
	connect(ui->deleteButton, &QPushButton::clicked, this,
			&DictionaryDialog::onDeleteButtonClicked);
	connect(ui->addButton, &QPushButton::clicked, this,
			&DictionaryDialog::onAddButtonClicked);
}

DictionaryDialog::~DictionaryDialog()
{
	delete ui;
}

void DictionaryDialog::setRemoteRepository(RemoteRepository * repo)
{
	remoteRepository = repo;
}

void DictionaryDialog::populateListWidget()
{
	const QList<QSharedPointer<Dictionary>> & dictionaries =
		remoteRepository->getDictionaries();
	ui->listWidget->clear();

	connect(ui->listWidget, &QListWidget::itemClicked, this,
			&DictionaryDialog::onListWidgetItemClicked);

	foreach(const QSharedPointer<Dictionary> & dictionary, dictionaries)
	{
		QListWidgetItem * nameItem = new QListWidgetItem(dictionary->displayName);
		nameItem->setData(Qt::UserRole, QVariant::fromValue(dictionary));
		ui->listWidget->addItem(nameItem);
	}

	ui->tableWidget->resizeColumnsToContents();
	ui->tableWidget->resizeRowsToContents();
}

void DictionaryDialog::onListWidgetItemClicked(QListWidgetItem * item)
{
	if(item){
		currentDictionary = item->data(Qt::UserRole).value<QSharedPointer<Dictionary>>();
		updateTableWidget();
	}
}

void DictionaryDialog::updateTableWidget()
{
	// ui->tableWidget->clearContents();

	QTableWidgetItem * nameItem =
		new QTableWidgetItem(currentDictionary->displayName);
	QTableWidgetItem * filenameItem =
		new QTableWidgetItem(currentDictionary->filename);
	QTableWidgetItem * formatItem =
		new QTableWidgetItem(currentDictionary->format);
	QTableWidgetItem * countItem = new QTableWidgetItem(tr("Loading..."));

	ui->tableWidget->setItem(0, 1, nameItem);
	ui->tableWidget->setItem(1, 1, filenameItem);
	ui->tableWidget->setItem(2, 1, formatItem);
	ui->tableWidget->setItem(3, 1, countItem);

	remoteRepository->getHeadwordCount(currentDictionary.data())
		.then([=](qsizetype headwordCount) {
			qDebug() << QString::number(headwordCount);
			ui->tableWidget->setItem(3,1,new QTableWidgetItem(QString::number(headwordCount)));
		});

	ui->tableWidget->resizeColumnsToContents();
	ui->tableWidget->resizeRowsToContents();
}

void DictionaryDialog::onRenameButtonClicked()
{
	bool ok;
	QString newName = QInputDialog::getText(this, tr("Rename dictionary"), "",
											QLineEdit::Normal, "", &ok);

	if (ok && !newName.isEmpty())
	{
		QFutureWatcher<bool> *watcher = new QFutureWatcher<bool>(this);
		connect(watcher, &QFutureWatcher<bool>::finished, watcher, &QFutureWatcher<bool>::deleteLater);
		QFuture<bool> future = remoteRepository->renameDictionary(currentDictionary.data(), newName);
		watcher->setFuture(future);
		if (watcher->result())
		{
			ui->listWidget->currentItem()->setText(newName);
			ui->tableWidget->setItem(0, 1, new QTableWidgetItem(newName));
		}
	}
}

void DictionaryDialog::onDeleteButtonClicked()
{
	QListWidgetItem * currentItem = ui->listWidget->currentItem();

	if (currentItem)
	{
		QMessageBox msgBox(this);
		msgBox.setWindowTitle(tr("Delete"));
		msgBox.setText(tr("Are you sure you want to delete ‘%1’? The dictionary "
						  "files won't be deleted.")
						   .arg(currentDictionary->displayName));
		msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Ok);

		int ret = msgBox.exec();
		if (ret == QMessageBox::Ok)
		{
			remoteRepository->deleteDictionary(currentDictionary.data())
				.then([=](bool result) {
					if (result)
					{
						ui->listWidget->takeItem(ui->listWidget->currentRow());
						for (int row = 0; row < ui->tableWidget->rowCount(); ++row)
						{
							ui->tableWidget->item(row, 1)->setText("");
						}
					}
				});
		}
	}
}

void DictionaryDialog::onAddButtonClicked()
{
	AddDictionaryDialog * addDialog =
		new AddDictionaryDialog(this, remoteRepository);
	if (addDialog->exec() == QDialog::Accepted)
	{
		Dictionary newDictionary = addDialog->getNewDictionary();
		Group * selectGroup = addDialog->getSelectedGroup();

		remoteRepository->addDictionary(newDictionary, selectGroup)
			.then([=](bool result) {
				if (result)
				{
					populateListWidget();
				}
			});
	}
	delete addDialog;
}
