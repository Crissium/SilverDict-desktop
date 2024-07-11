#include "adddictionarydialog.h"
#include "ui_adddictionarydialog.h"

AddDictionaryDialog::AddDictionaryDialog(QWidget * parent,
										 RemoteRepository * repo)
	: QDialog(parent)
	, ui(new Ui::AddDictionaryDialog)
{
	ui->setupUi(this);

	setRemoteRepository(repo);

	this->addFormatItem();
	this->addGroupItem();

	connect(ui->buttonBox, &QDialogButtonBox::accepted, this,
			&AddDictionaryDialog::onButtonBoxAccepted);
}

AddDictionaryDialog::~AddDictionaryDialog()
{
	delete ui;
}

void AddDictionaryDialog::setRemoteRepository(RemoteRepository * repo)
{
	remoteRepository = repo;
}

void AddDictionaryDialog::addFormatItem()
{
	const Formats & formats = remoteRepository->getFormats();
	ui->formatComboBox->clear();

	for (const QString & formatName : formats)
	{
		ui->formatComboBox->addItem(formatName);
	}
}

void AddDictionaryDialog::addGroupItem()
{
	const QList<QSharedPointer<Group>> & groups = remoteRepository->getGroups();

	foreach(const QSharedPointer<Group> & group, groups)
	{
		ui->groupComboBox->addItem(group->name, QVariant::fromValue(group.data()));
	}
}

Dictionary AddDictionaryDialog::getNewDictionary()
{
	return newDictionary;
}

Group * AddDictionaryDialog::getSelectedGroup()
{
	return selectedGroup;
}

void AddDictionaryDialog::onButtonBoxAccepted()
{
	newDictionary = {ui->nameLineEdit->text(), ui->nameLineEdit->text(),
					 ui->formatComboBox->currentText(),
					 ui->filenameLineEdit->text()};

	selectedGroup = ui->groupComboBox->itemData(ui->groupComboBox->currentIndex())
						.value<Group *>();
}
