#include "selectdictionarydialog.h"
#include "ui_selectdictionarydialog.h"

SelectDictionaryDialog::SelectDictionaryDialog(QWidget * parent, RemoteRepository * repo, QSharedPointer<Group> cGroup)
	: QDialog(parent)
	, ui(new Ui::SelectDictionaryDialog)
{
	ui->setupUi(this);

	this->setWindowTitle(tr("Select Dictionaries"));

	setRemoteRepository(repo);
	setCurrentGroup(cGroup);

	showDictionaries();
}

SelectDictionaryDialog::~SelectDictionaryDialog()
{
	delete ui;
}

void SelectDictionaryDialog::setRemoteRepository(RemoteRepository * repo)
{
	remoteRepository = repo;
}

void SelectDictionaryDialog::setCurrentGroup(QSharedPointer<Group> cGroup)
{
	currentGroup = cGroup;
}

const QSet<const Dictionary *> SelectDictionaryDialog::getGroupDictionaries()
{
	const Groupings & groupings = remoteRepository->getGroupings();
	auto it = groupings.find(currentGroup.data());
	const QSet<const Dictionary *> & groupDictionaries = it.value();
	return groupDictionaries;
}

void SelectDictionaryDialog::onUpdateDictionaries(QList<QSharedPointer<Dictionary>> selectedDictionaries)
{
	const QSet<const Dictionary *> & groupDictionaries = getGroupDictionaries();

	QList<QSharedPointer<Dictionary>> toAdd;
	QList<QSharedPointer<Dictionary>> toRemove;

	for (const QSharedPointer<Dictionary> & selectedDict : selectedDictionaries)
	{
		if (!groupDictionaries.contains(selectedDict.data()))
		{
			toAdd.append(selectedDict);
		}
	}

	for (const Dictionary * groupDict : groupDictionaries)
	{
		if (!selectedDictionaries.contains(groupDict))
		{
			toRemove.append(QSharedPointer<Dictionary>(const_cast<Dictionary *>(groupDict)));
		}
	}

	for (const QSharedPointer<Dictionary> & dict : toAdd)
	{
		remoteRepository->addDictionaryToGroup(dict.data(), currentGroup.data())
			.then([=](bool result)
				  {
					  emit updateTableWidget();
				  });
	}

	for (const QSharedPointer<Dictionary> & dict : toRemove)
	{
		remoteRepository->removeDictionaryFromGroup(dict.data(), currentGroup.data())
			.then([=](bool result)
				  {
					  emit updateTableWidget();
				  });
	}
}

void SelectDictionaryDialog::showDictionaries()
{
	QGridLayout * layout = new QGridLayout(ui->scrollAreaWidgetContents);
	QList<QCheckBox *> checkBoxes;

	const QSet<const Dictionary *> & groupDictionaries = getGroupDictionaries();
	const QList<QSharedPointer<Dictionary>> & dictionaries = remoteRepository->getDictionaries();

	layout->setColumnStretch(0, 1);
	layout->setColumnStretch(1, 1);
	layout->setColumnStretch(2, 1);

	int row = 0;
	int column = 0;
	for (const QSharedPointer<Dictionary> & dictionarySharedPtr : dictionaries)
	{
		QCheckBox * checkBox = new QCheckBox(dictionarySharedPtr->displayName);
		checkBox->setProperty("dictionaryPointer", QVariant::fromValue(dictionarySharedPtr));

		if (groupDictionaries.contains(dictionarySharedPtr.data()))
		{
			checkBox->setChecked(true);
		}

		layout->addWidget(checkBox, row, column);
		checkBoxes.append(checkBox);
		column++;
		if (column == 3)
		{
			row++;
			column = 0;
		}
	}

	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [this, checkBoxes]
			{
				QList<QSharedPointer<Dictionary>> selectedDictionaries;
				foreach(QCheckBox * checkBox, checkBoxes)
				{
					if (checkBox->isChecked())
					{
						QVariant dictVariant = checkBox->property("dictionaryPointer");
						if (dictVariant.canConvert<QSharedPointer<Dictionary>>())
						{
							selectedDictionaries.append(dictVariant.value<QSharedPointer<Dictionary>>());
						}
					}
				}
				onUpdateDictionaries(selectedDictionaries);
			});

	connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}
