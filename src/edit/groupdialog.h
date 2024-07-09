#ifndef GROUPDIALOG_H
#define GROUPDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QInputDialog>
#include <QFutureWatcher>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QMessageBox>

#include "remote/remoterepository.h"
#include "addgroupdialog.h"

namespace Ui
{
class GroupDialog;
}

class GroupDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GroupDialog(QWidget * parent = nullptr, RemoteRepository *repo = nullptr);
	~GroupDialog();

private slots:
	void updateTableWidget();
	void onUpdateDictionaries();

	void onEditLanguageButtonClicked();
	void onEditDictionaryButtonClicked();
	void onRenameButtonClicked();
	void onDeleteButtonClicked();
	void onAddButtonClicked();


private:
	Ui::GroupDialog * ui;
	RemoteRepository* remoteRepository;
	QSharedPointer<Group> currentGroup;
	void setRemoteRepository(RemoteRepository * repo);
	QString getDictionariesName();
	QSet<const Dictionary *> getGroupDictionaries();
	QList<QSharedPointer<Dictionary>> selectedDictionaries;
	QSet<QLocale::Language> getLanguageSetFromString(QString languageString);

	void populateListWidget();
	void onListWidgetItemClicked(QListWidgetItem* item);
};

#endif // GROUPDIALOG_H
