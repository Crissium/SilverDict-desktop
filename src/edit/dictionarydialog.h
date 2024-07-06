#ifndef EDITDICTIONARY_H
#define EDITDICTIONARY_H

#include <QDialog>
#include <QTableWidget>
#include <QListWidget>
#include <QFutureWatcher>

#include "remote/remoterepository.h"

namespace Ui
{
class DictionaryDialog;
}

class DictionaryDialog : public QDialog
{
	Q_OBJECT

public:
	explicit DictionaryDialog(QWidget *parent, RemoteRepository *repo);
	~DictionaryDialog();

private slots:
	void onListWidgetItemClicked(QListWidgetItem* item);
	void updateTableWidget(const QSharedPointer<Dictionary>& dictionary);

	void on_renameButton_clicked();
	void on_deleteButton_clicked();

private:
	Ui::DictionaryDialog * ui;
	RemoteRepository* remoteRepository;

	void setRemoteRepository(RemoteRepository * repo);
	void fetchHeadwordCount(QTableWidgetItem *countItem, Dictionary* dictionary);
	void populateListWidget();
};

#endif // EDITDICTIONARY_H
