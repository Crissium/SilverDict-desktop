#include "addgroupdialog.h"
#include "src/edit/ui_addgroupdialog.h"

AddGroupDialog::AddGroupDialog(QWidget * parent,
										 RemoteRepository * repo)
	: QDialog(parent)
	, ui(new Ui::AddGroupDialog)
{
	ui->setupUi(this);

	setRemoteRepository(repo);

	this->setWindowTitle(tr("New Group"));

	connect(ui->buttonBox, &QDialogButtonBox::accepted, this,
			&AddGroupDialog::onButtonBoxAccepted);
}

AddGroupDialog::~AddGroupDialog()
{
	delete ui;
}

void AddGroupDialog::setRemoteRepository(RemoteRepository * repo)
{
	remoteRepository = repo;
}

QString AddGroupDialog::getGroupName()
{
	return groupName;
}

QString AddGroupDialog::getGroupLanguages()
{
	return groupLanguages;
}

void AddGroupDialog::onButtonBoxAccepted()
{
	groupName = ui->nameLineEdit->text();
	groupLanguages = ui->languageLineEdit->text();
}
