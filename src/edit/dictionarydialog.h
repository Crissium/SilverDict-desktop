<<<<<<< HEAD
#ifndef EDITDICTIONARY_H
#define EDITDICTIONARY_H

#include <QDialog>
#include <QTableWidget>
#include <QListWidget>
#include <QFutureWatcher>

#include "remote/remoterepository.h"
=======
#ifndef DICTIONARYDIALOG_H
#define DICTIONARYDIALOG_H

#include "edit/adddictionarydialog.h"
#include "remote/remoterepository.h"

#include <QDialog>
#include <QFutureWatcher>
#include <QInputDialog>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTimer>
>>>>>>> upstream/main

namespace Ui
{
class DictionaryDialog;
}

class DictionaryDialog : public QDialog
{
	Q_OBJECT

public:
	explicit DictionaryDialog(QWidget * parent, RemoteRepository * repo);
	~DictionaryDialog();

private slots:
	void onListWidgetItemClicked(QListWidgetItem * item);
	void updateTableWidget();

	void onRenameButtonClicked();
	void onDeleteButtonClicked();
	void onAddButtonClicked();

private:
	Ui::DictionaryDialog * ui;
	RemoteRepository * remoteRepository;
	QSharedPointer<Dictionary> currentDictionary;

	void setRemoteRepository(RemoteRepository * repo);
	void populateListWidget();
};

#endif // DICTIONARYDIALOG_H
