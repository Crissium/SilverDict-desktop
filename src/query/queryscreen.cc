#include "queryscreen.h"
#include "ui_queryscreen.h"

void QueryScreen::selectFirstWord()
{
	if (wordListModel->rowCount() > 0)
	{
		const QModelIndex firstItemIndex = wordListModel->index(0);
		ui->wordListView->setCurrentIndex(firstItemIndex);
		ui->wordListView->selectionModel()->select(firstItemIndex, QItemSelectionModel::Select);
	}
}

void QueryScreen::resetDictionaries(const Group * activeGroup)
{
	const QSet<const Dictionary *> & currentDicts = remoteRepository->getGroupings().value(activeGroup);
	QStringList dictNames;
	dictNames.reserve(currentDicts.size());
	for (auto const & dict : remoteRepository->getDictionaries())
	{
		if (currentDicts.contains(dict.data()))
		{
			dictNames.append(dict->displayName);
		}
	}
	dictListModel->setStringList(dictNames);
}

void QueryScreen::onSearchTermChanged(const QString & searchTerm)
{
	if (searchTerm.isEmpty())
	{
		wordListModel->setWords(remoteRepository->getHistory());
		selectFirstWord();
	}
	else
	{
		remoteRepository->getSuggestions(searchTerm)
			.then([this](const Suggestions & suggestions)
				  {
					  wordListModel->setWords(suggestions);
					  selectFirstWord(); // FIXME: doesn't work
				  });
	}
}

void QueryScreen::onWordClicked(const QModelIndex & index)
{
	if (index.isValid())
	{
		int i = index.row();
		if (wordListModel->isDisplayingHistory())
		{
		}
		else
		{
			// wordListModel->setHighlight(i);
		}
	}
}

void QueryScreen::onGroupsChanged()
{
	ui->dictSelectionBox->clear();
	const QList<QSharedPointer<Group>> & groups = remoteRepository->getGroups();
	QStringList groupNames;
	groupNames.reserve(groups.size());
	for (const QSharedPointer<Group> & group : groups)
	{
		groupNames.append(group->name);
	}
	ui->dictSelectionBox->addItems(groupNames);

	if (const Group * activeGroup = remoteRepository->getActiveGroup())
	{
		const int activeGroupIndex = groupNames.indexOf(activeGroup->name);
		if (activeGroupIndex != -1)
		{
			ui->dictSelectionBox->setCurrentIndex(activeGroupIndex);
		}

		resetDictionaries(activeGroup);
	}
}

void QueryScreen::onActiveGroupChanged(int i)
{
	const Group * group = remoteRepository->getGroups().at(i).data();
	remoteRepository->setActiveGroup(group);

	resetDictionaries(group);
}

void QueryScreen::onDictionariesChanged()
{
	resetDictionaries(remoteRepository->getActiveGroup());
}

QueryScreen::QueryScreen(QWidget * parent)
	: QWidget(parent)
	, ui(new Ui::QueryScreen)
	, remoteRepository(nullptr)
	, wordListModel(new WordListModel(this))
	, groupListModel(nullptr)
	, dictListModel(new QStringListModel(this))
{
	ui->setupUi(this);
	groupListModel = ui->dictSelectionBox->model();
	ui->dictListView->setModel(dictListModel.get());

	connect(ui->searchTermEdit, &QLineEdit::textEdited, this, &QueryScreen::onSearchTermChanged);
	connect(ui->wordListView, &QListView::clicked, this, &QueryScreen::onWordClicked);
	connect(ui->dictSelectionBox, &QComboBox::currentIndexChanged, this, &QueryScreen::onActiveGroupChanged);
}

QueryScreen::~QueryScreen()
{
	delete ui;
}

void QueryScreen::setRemoteRepository(RemoteRepository * repo)
{
	remoteRepository = repo;

	// wordListModel->setWords(remoteRepository->getHistory());
	ui->wordListView->setModel(wordListModel.get());
	onSearchTermChanged(QStringLiteral());

	onGroupsChanged();

	connect(remoteRepository, &RemoteRepository::groupsChanged, this, &QueryScreen::onGroupsChanged);
	connect(remoteRepository, &RemoteRepository::dictionariesChanged, this, &QueryScreen::onDictionariesChanged);
}
