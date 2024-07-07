#ifndef ADDDICTIONARYDIALOG_H
#define ADDDICTIONARYDIALOG_H

#include <QDialog>

#include "remote/models.h"
#include "remote/remoterepository.h"

namespace Ui
{
class AddDictionaryDialog;
}

class AddDictionaryDialog : public QDialog
{
	Q_OBJECT

public:
	explicit AddDictionaryDialog(QWidget * parent = nullptr, RemoteRepository* repo = nullptr);
	~AddDictionaryDialog();

	Dictionary getNewDictionary();
	Group* getSelectedGroup();

private slots:
	void on_buttonBox_accepted();

private:
	Ui::AddDictionaryDialog * ui;
	RemoteRepository* remoteRepository;
	Dictionary newDictionary;
	Group* selectedGroup = nullptr;

	void setRemoteRepository(RemoteRepository * repo);
	void addFormatItem();
	void addGroupItem();
};

#endif // ADDDICTIONARYDIALOG_H
