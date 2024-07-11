#include "groupdialog.h"
#include "ui_groupdialog.h"

GroupDialog::GroupDialog(QWidget * parent, RemoteRepository * repo)
	: QDialog(parent)
	, ui(new Ui::GroupDialog)
{
	ui->setupUi(this);

	setRemoteRepository(repo);

	populateListWidget();

	connect(ui->listWidget, &QListWidget::itemClicked, this,
			&GroupDialog::onListWidgetItemClicked);
	connect(ui->editLanguageButton, &QPushButton::clicked, this,
			&GroupDialog::onEditLanguageButtonClicked);
	connect(ui->renameButton, &QPushButton::clicked, this,
			&GroupDialog::onRenameButtonClicked);
	connect(ui->editDictionaryButton, &QPushButton::clicked, this,
			&GroupDialog::onEditDictionaryButtonClicked);
	connect(ui->deleteButton, &QPushButton::clicked, this,
			&GroupDialog::onDeleteButtonClicked);
	connect(ui->addButton, &QPushButton::clicked, this,
			&GroupDialog::onAddButtonClicked);
	connect(this, &GroupDialog::updateListSignal, this, &GroupDialog::populateListWidget);
}

GroupDialog::~GroupDialog()
{
	delete ui;
}

void GroupDialog::setRemoteRepository(RemoteRepository * repo)
{
	remoteRepository = repo;
}

void GroupDialog::populateListWidget()
{
	ui->listWidget->clear();

	connect(ui->listWidget, &QListWidget::itemClicked, this, &GroupDialog::onListWidgetItemClicked);

	foreach(const QSharedPointer<Group> & group, remoteRepository->getGroups())
	{
		QListWidgetItem * nameItem = new QListWidgetItem(group->name);
		nameItem->setData(Qt::UserRole, QVariant::fromValue(group));
		ui->listWidget->addItem(nameItem);
	}

	ui->tableWidget->resizeColumnsToContents();
	ui->tableWidget->resizeRowsToContents();
}

void GroupDialog::onListWidgetItemClicked(QListWidgetItem * item)
{
	if (item)
	{
		currentGroup = item->data(Qt::UserRole).value<QSharedPointer<Group>>();
		updateTableWidget();
	}
}

void GroupDialog::updateTableWidget()
{
	bool isDefaultGroup = currentGroup->name == QStringLiteral("Default Group");

	if (isDefaultGroup)
	{
		ui->renameButton->setEnabled(false);
		ui->deleteButton->setEnabled(false);
	}
	else
	{
		ui->renameButton->setEnabled(true);
		ui->deleteButton->setEnabled(true);
	}

	QTableWidgetItem * nameItem = new QTableWidgetItem(currentGroup->name);

	QString languageList;
	QStringList languageStrings;
	for (QLocale::Language language : currentGroup->languages)
	{
		languageStrings.append(QLocale::languageToString(language));
	}
	languageList = languageStrings.join(", ");

	QTableWidgetItem * languageItem = new QTableWidgetItem(languageList);

	QString dictionariesName = getDictionariesName();
	QTableWidgetItem * dictionariesItem = new QTableWidgetItem(dictionariesName);

	ui->tableWidget->setItem(0, 1, nameItem);
	ui->tableWidget->setItem(1, 1, languageItem);
	ui->tableWidget->setItem(2, 1, dictionariesItem);

	ui->tableWidget->resizeColumnsToContents();
	ui->tableWidget->resizeRowsToContents();
}

QString GroupDialog::getDictionariesName()
{
	const Groupings & groupings = remoteRepository->getGroupings();
	auto it = groupings.find(currentGroup.data());
	QString dictionariesName;
	if (it != groupings.end())
	{
		const QSet<const Dictionary *> & dictionaries = it.value();
		for (const Dictionary * dictionary : dictionaries)
		{
			if (!dictionariesName.isEmpty())
			{
				dictionariesName.append("\n");
			}
			dictionariesName.append(dictionary->displayName);
		}
	}
	return dictionariesName;
}

QSet<QLocale::Language> GroupDialog::getLanguageSetFromString(QString languageString)
{
	QSet<QLocale::Language> languages;
	QStringList languageList = languageString.split(',', Qt::SkipEmptyParts);

	for (const QString & langString : languageList)
	{
		QLocale locale3(langString);

		languages.insert(locale3.language());
	}
	return languages;
}

const QSet<const Dictionary *> GroupDialog::getGroupDictionaries()
{
	const Groupings & groupings = remoteRepository->getGroupings();
	auto it = groupings.find(currentGroup.data());
	const QSet<const Dictionary *> & groupDictionaries = it.value();
	return groupDictionaries;
}

void GroupDialog::onRenameButtonClicked()
{
	bool ok;
	QString newName = QInputDialog::getText(this, QStringLiteral("Rename dictionary"), "", QLineEdit::Normal, "", &ok);

	if (ok && !newName.isEmpty())
	{
		remoteRepository->renameGroup(currentGroup.data(), newName)
			.then([=](bool result)
				  {
					  if (result)
					  {
						  ui->listWidget->currentItem()->setText(newName);
						  QTableWidgetItem * nameItem = new QTableWidgetItem(newName);
						  ui->tableWidget->setItem(0, 1, nameItem);
					  }
				  });
	}
}

void GroupDialog::onEditLanguageButtonClicked()
{
	bool ok;
	QString newLanguage = QInputDialog::getText(this, QStringLiteral("Edit languages"), "", QLineEdit::Normal, "", &ok);
	QSet<QLocale::Language> languages = getLanguageSetFromString(newLanguage);

	if (ok && !newLanguage.isEmpty())
	{
		remoteRepository->changeGroupLanguages(currentGroup.data(), languages)
			.then([=](bool result)
				  {
					  if (result)
					  {
						  QStringList languageStrings;
						  for (QLocale::Language language : languages)
						  {
							  languageStrings.append(QLocale::languageToString(language));
						  }
						  QTableWidgetItem * languageItem = new QTableWidgetItem(languageStrings.join(", "));
						  ui->tableWidget->setItem(1, 1, languageItem);
					  }
				  });
	}
}

void GroupDialog::onEditDictionaryButtonClicked()
{
	SelectDictionaryDialog * selectDictionaryDialog = new SelectDictionaryDialog(this, remoteRepository, currentGroup);
	connect(selectDictionaryDialog, &SelectDictionaryDialog::updateTableWidget, this, &GroupDialog::updateTableWidget);
	selectDictionaryDialog->exec();
}

void GroupDialog::onDeleteButtonClicked()
{
	QMessageBox msgBox(this);
	msgBox.setWindowTitle(QStringLiteral("Delete group"));
	msgBox.setText(QStringLiteral("Are you sure you want to delete ‘%1’?").arg(currentGroup->name));
	msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Ok);

	int ret = msgBox.exec();
	if (ret == QMessageBox::Ok)
	{
		remoteRepository->deleteGroup(currentGroup.data())
			.then([=](bool result)
				  {
					  if (result)
					  {
						  emit updateListSignal();
						  for (int row = 0; row < ui->tableWidget->rowCount(); ++row)
						  {
							  ui->tableWidget->item(row, 1)->setText("");
						  }
					  }
				  });
	}
}

void GroupDialog::onAddButtonClicked()
{
	AddGroupDialog * addDialog =
		new AddGroupDialog(this, remoteRepository);
	if (addDialog->exec() == QDialog::Accepted)
	{
		QString groupName = addDialog->getGroupName();
		QString groupLanguages = addDialog->getGroupLanguages();

		const Group newGroup = {
			groupName,
			getLanguageSetFromString(groupLanguages)};

		remoteRepository->addGroup(newGroup)
			.then([=](bool result)
				  {
					  if (result)
					  {
						  emit updateListSignal();
					  }
				  });
	}
	delete addDialog;
}
