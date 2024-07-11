#ifndef SELECTDICTIONARYDIALOG_H
#define SELECTDICTIONARYDIALOG_H

#include "remote/models.h"
#include "remote/remoterepository.h"

#include <QCheckBox>
#include <QDialog>

namespace Ui
{
class SelectDictionaryDialog;
}

class SelectDictionaryDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SelectDictionaryDialog(QWidget * parent, RemoteRepository * repo, QSharedPointer<Group> cGroup);
	~SelectDictionaryDialog();
	void setCurrentGroup(QSharedPointer<Group> currentGroup);

private:
	Ui::SelectDictionaryDialog * ui;
	RemoteRepository * remoteRepository;
	QSharedPointer<Group> currentGroup;
	// QSet<const Dictionary *> currentGroupDictionaries;

	void setRemoteRepository(RemoteRepository * repo);
	const QSet<const Dictionary *> getGroupDictionaries();
	void showDictionaries();
	void onUpdateDictionaries(QList<QSharedPointer<Dictionary>> selectedDictionaries);

signals:
	void updateTableWidget();
};

#endif // SELECTDICTIONARYDIALOG_H
