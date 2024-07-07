#include "dictionarydialog.h"
#include "ui_dictionarydialog.h"
#include "remote/models.h"

#include<QPushButton>
#include<QMessageBox>
#include<QInputDialog>

DictionaryDialog::DictionaryDialog(QWidget *parent, RemoteRepository *repo)
	: QDialog(parent)
	, ui(new Ui::DictionaryDialog)
{	
	ui->setupUi(this);

	setRemoteRepository(repo);

	populateListWidget();
}

DictionaryDialog::~DictionaryDialog()
{
	delete ui;
}

void DictionaryDialog::setRemoteRepository(RemoteRepository * repo)
{
	remoteRepository = repo;
}

void DictionaryDialog::populateListWidget() {
	QList<QSharedPointer<Dictionary>> dictionaries = remoteRepository->getDictionaries();
	QListWidget *listWidget = findChild<QListWidget *>(QStringLiteral("listWidget"));
	listWidget->clear();

	QListWidgetItem *orderItem = new QListWidgetItem(QStringLiteral("dictionary order:"));
	orderItem->setFlags(orderItem->flags() & ~Qt::ItemIsSelectable);
	listWidget->addItem(orderItem);

	connect(listWidget, &QListWidget::itemClicked, this, &DictionaryDialog::onListWidgetItemClicked);

	foreach (const QSharedPointer<Dictionary> &dictionary, dictionaries) {
		QListWidgetItem *nameItem = new QListWidgetItem(dictionary->displayName);
		nameItem->setData(Qt::UserRole, QVariant::fromValue(dictionary));
		listWidget->addItem(nameItem);
	}
}

void DictionaryDialog::onListWidgetItemClicked(QListWidgetItem* item) {
	QTableWidget *tableWidget = findChild<QTableWidget *>(QStringLiteral("tableWidget"));
	QVariant data = item->data(Qt::UserRole);
	if (data.canConvert<QSharedPointer<Dictionary>>()) {
		QSharedPointer<Dictionary> dictionary = data.value<QSharedPointer<Dictionary>>();
		updateTableWidget(dictionary);
	}
}

void DictionaryDialog::updateTableWidget(const QSharedPointer<Dictionary>& dictionary) {
	QTableWidget *tableWidget = findChild<QTableWidget *>(QStringLiteral("tableWidget"));
	tableWidget->clearContents();

	QTableWidgetItem *nameTitleItem = new QTableWidgetItem(QStringLiteral("Name"));
	QTableWidgetItem *filenameTitleItem = new QTableWidgetItem(QStringLiteral("Filename"));
	QTableWidgetItem *formatTitleItem = new QTableWidgetItem(QStringLiteral("Format"));
	QTableWidgetItem *countTitleItem = new QTableWidgetItem(QStringLiteral("Headword Count"));

	QTableWidgetItem *nameItem = new QTableWidgetItem(dictionary->displayName);
	QTableWidgetItem *filenameItem = new QTableWidgetItem(dictionary->filename);
	QTableWidgetItem *formatItem = new QTableWidgetItem(dictionary->format);
	QTableWidgetItem *countItem = new QTableWidgetItem(QStringLiteral("Loading..."));

	tableWidget->setItem(0, 0, nameTitleItem);
	tableWidget->setItem(1, 0, filenameTitleItem);
	tableWidget->setItem(2, 0, formatTitleItem);
	tableWidget->setItem(3, 0, countTitleItem);

	tableWidget->setItem(0, 1, nameItem);
	tableWidget->setItem(1, 1, filenameItem);
	tableWidget->setItem(2, 1, formatItem);
	tableWidget->setItem(3, 1, countItem);

	fetchHeadwordCount(countItem, dictionary.data());

	tableWidget->resizeColumnsToContents();
	tableWidget->resizeRowsToContents();
}

void DictionaryDialog::fetchHeadwordCount(QTableWidgetItem *countItem, Dictionary* dictionary) {
	QFuture<qsizetype> futureCount = remoteRepository->getHeadwordCount(dictionary);
	QFutureWatcher<qsizetype> *watcher = new QFutureWatcher<qsizetype>(this);

	connect(watcher, &QFutureWatcher<qsizetype>::finished, this, [countItem, watcher]() {
		if (!watcher->isCanceled()&&watcher->isFinished()) {
			qsizetype headwordCount = watcher->result();
			countItem->setText(QString::number(headwordCount));
		}
		watcher->deleteLater();
	});
	watcher->setFuture(futureCount);
}

void DictionaryDialog::on_renameButton_clicked()
{
	QListWidget *listWidget = findChild<QListWidget *>(QStringLiteral("listWidget"));
	QListWidgetItem *currentItem = listWidget->currentItem();

	if (currentItem) {
		QVariant data = currentItem->data(Qt::UserRole);
		QSharedPointer<Dictionary> dictionary = data.value<QSharedPointer<Dictionary>>();

		bool ok;
		QString newName = QInputDialog::getText(this
												, QStringLiteral("Rename dictionary")
												, ""
												, QLineEdit::Normal
												, ""
												, &ok);

		if (ok && !newName.isEmpty()) {
			QFuture<bool> renameResult = remoteRepository->renameDictionary(dictionary.data(), newName);
			QFutureWatcher<bool> *watcher = new QFutureWatcher<bool>(this);
			watcher->setFuture(renameResult);
			connect(watcher, &QFutureWatcher<bool>::finished, this, [this, watcher, newName]() {
				if (watcher->result()) {
					QListWidgetItem* item = ui->listWidget->currentItem();
					item->setText(newName);
					QTableWidget* tableWidget = findChild<QTableWidget *>(QStringLiteral("tableWidget"));
					QTableWidgetItem* newNameItem = new QTableWidgetItem(newName);
					tableWidget->setItem(0, 1, newNameItem);
				}
				watcher->deleteLater();
			}, Qt::QueuedConnection);
		}
	}
}

void DictionaryDialog::on_deleteButton_clicked() {
	QListWidgetItem *currentItem = ui->listWidget->currentItem();

	if (currentItem) {
		QVariant data = currentItem->data(Qt::UserRole);
		QSharedPointer<Dictionary> dictionary = data.value<QSharedPointer<Dictionary>>();

		QMessageBox msgBox(this);
		msgBox.setWindowTitle(QStringLiteral("Delete"));
		msgBox.setText(QStringLiteral("Are you sure you want to delete ‘%1’? The dictionary files won't be deleted.").arg(dictionary->displayName));
		msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Ok);

		int ret = msgBox.exec();
		if (ret == QMessageBox::Ok) {
			QFuture<bool> deletionResult = remoteRepository->deleteDictionary(dictionary.data());
			QFutureWatcher<bool> *watcher = new QFutureWatcher<bool>(this);
			watcher->setFuture(deletionResult);
			connect(watcher, &QFutureWatcher<bool>::finished, this, [this, watcher]() {
				if (watcher->result()) {
					QListWidgetItem *itemToDelete = ui->listWidget->currentItem();
					if (itemToDelete) {
						ui->listWidget->removeItemWidget(itemToDelete);
						delete itemToDelete;
					}
					QTableWidget *tableWidget = findChild<QTableWidget *>(QStringLiteral("tableWidget"));
					tableWidget->clearContents();
				}
				watcher->deleteLater();
			}, Qt::QueuedConnection);
		}
	}
}

void DictionaryDialog::on_addButton_clicked()
{
	AddDictionaryDialog *addDialog = new AddDictionaryDialog(this, remoteRepository);
	if (addDialog->exec() == QDialog::Accepted) {
		Dictionary newDictionary = addDialog->getNewDictionary();
		Group* selectGroup = addDialog->getSelectedGroup();

		QFuture<bool> addResult = remoteRepository->addDictionary(newDictionary, selectGroup);
		QFutureWatcher<bool> *watcher = new QFutureWatcher<bool>(this);
		watcher->setFuture(addResult);
		connect(watcher, &QFutureWatcher<bool>::finished, this, [this, watcher]() {
			if (watcher->result()) {
				populateListWidget();
			}
			watcher->deleteLater();
		}, Qt::QueuedConnection);
	}
	delete addDialog;
}
