#ifndef QUERYSCREEN_H
#define QUERYSCREEN_H

#include "wordlistmodel.h"
#include "../remote/remoterepository.h"

#include <QStringListModel>
#include <QWidget>

namespace Ui
{
class QueryScreen;
}

class QueryScreen : public QWidget
{
	Q_OBJECT
private:
	Ui::QueryScreen * ui;
	RemoteRepository * remoteRepository;
	QScopedPointer<WordListModel> wordListModel;
	QAbstractItemModel * groupListModel; // within the combo box
	QScopedPointer<QStringListModel> dictListModel;

	void selectFirstWord();
	void resetDictionaries(const Group * activeGroup);

private slots:
	void onSearchTermChanged(const QString & searchTerm);
	void onWordClicked(const QModelIndex & index);
	void onGroupsChanged();
	void onActiveGroupChanged(int i);
	void onDictionariesChanged();

public:
	explicit QueryScreen(QWidget * parent = nullptr);
	~QueryScreen() override;

	void setRemoteRepository(RemoteRepository * repo); // to be called after initialisation
};

#endif // QUERYSCREEN_H
