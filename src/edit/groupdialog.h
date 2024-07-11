#ifndef GROUPDIALOG_H
#define GROUPDIALOG_H

#include "addgroupdialog.h"
#include "selectdictionarydialog.h"
#include "remote/remoterepository.h"

#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFutureWatcher>
#include <QInputDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>

namespace Ui
{
class GroupDialog;
}

class GroupDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GroupDialog(QWidget * parent = nullptr, RemoteRepository * repo = nullptr);
	~GroupDialog();

signals:
	void updateListSignal();
	void updateTableSignal();

private slots:
	void updateTableWidget();

	void onEditLanguageButtonClicked();
	void onEditDictionaryButtonClicked();
	void onRenameButtonClicked();
	void onDeleteButtonClicked();
	void onAddButtonClicked();

private:
	Ui::GroupDialog * ui;
	RemoteRepository * remoteRepository;
	QSharedPointer<Group> currentGroup;
	void setRemoteRepository(RemoteRepository * repo);
	QString getDictionariesName();
	const QSet<const Dictionary *> getGroupDictionaries();
	QSet<QLocale::Language> getLanguageSetFromString(QString languageString);

	void populateListWidget();
	void onListWidgetItemClicked(QListWidgetItem * item);
};

#endif // GROUPDIALOG_H
