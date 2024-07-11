#ifndef QUERYSCREEN_H
#define QUERYSCREEN_H

#include "articleview.h"
#include "wordlistmodel.h"
#include "../remote/remoterepository.h"

#include <QHotkey>
#include <QStringListModel>
#include <QtWebEngineWidgets/QWebEngineView>

namespace Ui
{
class QueryScreen;
}

class QueryScreen : public QWidget
{
	Q_OBJECT
private:
	static const qsizetype ClipboardLengthMax;
	Ui::QueryScreen * ui;
	RemoteRepository * remoteRepository;
	Preferences * preferences;
	QScopedPointer<WordListModel> wordListModel;
	QAbstractItemModel * groupListModel; // within the combo box
	QScopedPointer<QStringListModel> dictListModel;
	QScopedPointer<QHotkey> clipboardHotkey; // Ctrl + C + C

	void selectFirstWord() const;
	void setDictionaries(const QList<const Dictionary *> & dictionaries) const;
	void setDictionaries(const QStringList & dictNames) const;
	void resetDictionaries(const Group * activeGroup) const;

	[[nodiscard]] ArticleView * getCurrentArticleView() const;
	[[nodiscard]] QWebEngineView * getCurrentWebView() const;

	void initialiseArticleView(ArticleView * articleView) const;
	ArticleView * createArticleView();

private slots:
	void onSearchTermChanged(const QString & searchTerm) const;
	void onWordClicked(const QModelIndex & index) const;
	void onReturnPressed() const;
	void onGroupsChanged() const;
	void onActiveGroupChanged(int i) const;
	void onDictionariesChanged() const;
	void onArticleLoaded(const QStringList & dictNames) const;
	void onHistoryUpdated() const;
	void onDictionaryClicked(const QModelIndex & index) const;

	void addTab();
	void closeTab(int i) const;
	void openLinkInNewTab(const QUrl & url);

	void searchForClipboardContent();

public:
	explicit QueryScreen(QWidget * parent = nullptr);
	~QueryScreen() override;

	void setup(RemoteRepository * repo, Preferences * preferences); // to be called after initialisation
};

#endif // QUERYSCREEN_H
