#ifndef ADDGROUPDIALOG_H
#define ADDGROUPDIALOG_H

#include "remote/models.h"
#include "remote/remoterepository.h"

#include <QDialog>

namespace Ui
{
class AddGroupDialog;
}

class AddGroupDialog : public QDialog
{
	Q_OBJECT

public:
	explicit AddGroupDialog(QWidget * parent = nullptr, RemoteRepository * repo = nullptr);
	~AddGroupDialog();

	QString getGroupName();
	QString getGroupLanguages();

private slots:
	void onButtonBoxAccepted();

private:
	Ui::AddGroupDialog * ui;
	RemoteRepository * remoteRepository;
	QString groupName;
	QString groupLanguages;

	void setRemoteRepository(RemoteRepository * repo);
};

#endif // ADDGROUPDIALOG_H
