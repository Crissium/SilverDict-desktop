#include "queryscreen.h"
#include "ui_queryscreen.h"

#include "articleview.h"

void QueryScreen::selectFirstWord() const
{
	if (wordListModel->rowCount() > 0)
	{
		const QModelIndex firstItemIndex = wordListModel->index(0);
		ui->wordListView->setCurrentIndex(firstItemIndex);
		ui->wordListView->selectionModel()->select(firstItemIndex, QItemSelectionModel::Select);
	}
}

void QueryScreen::setDictionaries(const QList<const Dictionary *> & dictionaries) const
{
	QStringList dictNames;
	dictNames.reserve(dictionaries.size());
	for (const Dictionary * dict : dictionaries)
	{
		dictNames.append(dict->displayName);
	}
	dictListModel->setStringList(dictNames);
}

void QueryScreen::setDictionaries(const QStringList & dictNames) const
{
	QStringList displayNames;
	displayNames.reserve(dictNames.size());
	for (auto const & d : remoteRepository->getDictionaries())
	{
		if (dictNames.contains(d->name))
		{
			displayNames.append(d->displayName);
		}
	}
	dictListModel->setStringList(displayNames);
}

void QueryScreen::resetDictionaries(const Group * activeGroup) const
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

ArticleView * QueryScreen::getCurrentArticleView() const
{
	return qobject_cast<ArticleView *>(ui->tabWidget->currentWidget());
}

QWebEngineView * QueryScreen::getCurrentWebView() const
{
	return qobject_cast<ArticleView *>(ui->tabWidget->currentWidget())->getWebView();
}

void QueryScreen::initialiseArticleView(ArticleView * articleView) const
{
	articleView->setRemoteRepository(remoteRepository);
	connect(articleView, &ArticleView::articleLoaded, this, &QueryScreen::onArticleLoaded);
	connect(articleView, &ArticleView::historyUpdated, this, &QueryScreen::onHistoryUpdated);
	connect(articleView->getNewTabButton(), &QToolButton::clicked, this, &QueryScreen::addTab);
}

ArticleView * QueryScreen::createArticleView()
{
	auto * view = new ArticleView(this);
	initialiseArticleView(view);
	return view;
}

void QueryScreen::onSearchTermChanged(const QString & searchTerm) const
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

void QueryScreen::onWordClicked(const QModelIndex & index) const
{
	if (index.isValid())
	{
		const QString word = wordListModel->data(index, Qt::DisplayRole).toString();
		ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), word);
		const ArticleView * currentArticleView = getCurrentArticleView();
		QWebEngineView * currentWebView = currentArticleView->getWebView();
		if (currentArticleView->isInAnkiMode())
		{
			currentWebView->load(remoteRepository->getQueryAnkiUrl(word));
		}
		else
		{
			currentWebView->load(remoteRepository->getQueryUrl(word));
		}
	}
}

void QueryScreen::onReturnPressed() const
{
	const QModelIndex i = ui->wordListView->currentIndex();
	if (i.isValid())
	{
		onWordClicked(i);
	}
	else
	{
		onWordClicked(wordListModel->index(0, 0));
	}
}

void QueryScreen::onGroupsChanged() const
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

void QueryScreen::onActiveGroupChanged(int i) const
{
	const Group * group = remoteRepository->getGroups().at(i).data();
	remoteRepository->setActiveGroup(group);

	resetDictionaries(group);
}

void QueryScreen::onDictionariesChanged() const
{
	resetDictionaries(remoteRepository->getActiveGroup());
}

void QueryScreen::onArticleLoaded(const QStringList & dictNames) const
{
	setDictionaries(dictNames);
}

void QueryScreen::onHistoryUpdated() const
{
	if (wordListModel->isDisplayingHistory())
	{
		wordListModel->setWords(remoteRepository->getHistory());
		selectFirstWord();
	}
}

void QueryScreen::onDictionaryClicked(const QModelIndex & index) const
{
	if (index.isValid())
	{
		for (auto const & d : remoteRepository->getDictionaries())
		{
			// FIXME: display name is not unique
			if (d->displayName == dictListModel->data(index, Qt::DisplayRole).toString())
			{
				getCurrentArticleView()->navigateTo(d->name);
			}
		}
	}
}

void QueryScreen::addTab()
{
	auto * newArticleView = createArticleView();
	ui->tabWidget->addTab(newArticleView, QStringLiteral());
	ui->tabWidget->setCurrentWidget(newArticleView);
}

void QueryScreen::closeTab(int i) const
{
	if (ui->tabWidget->count() != 1)
	{
		ui->tabWidget->removeTab(i);
	}
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
	connect(ui->searchTermEdit, &QLineEdit::returnPressed, this, &QueryScreen::onReturnPressed);
	connect(ui->wordListView, &QListView::clicked, this, &QueryScreen::onWordClicked);
	connect(ui->dictSelectionBox, &QComboBox::currentIndexChanged, this, &QueryScreen::onActiveGroupChanged);
	connect(ui->dictListView, &QListView::clicked, this, &QueryScreen::onDictionaryClicked);
	connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &QueryScreen::closeTab);
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

	if (auto * currentArticleView = qobject_cast<ArticleView *>(ui->tabWidget->currentWidget()))
	{
		initialiseArticleView(currentArticleView);
	}

	connect(remoteRepository, &RemoteRepository::groupsChanged, this, &QueryScreen::onGroupsChanged);
	connect(remoteRepository, &RemoteRepository::dictionariesChanged, this, &QueryScreen::onDictionariesChanged);
}
