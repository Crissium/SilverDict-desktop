#include "adddictionarydialog.h"
#include "ui_adddictionarydialog.h"

AddDictionaryDialog::AddDictionaryDialog(QWidget * parent, RemoteRepository *repo)
	: QDialog(parent)
	, ui(new Ui::AddDictionaryDialog)
{
	ui->setupUi(this);

	setRemoteRepository(repo);

	this->setWindowTitle("New dictionary");
	this->addFormatItem();
	this->addGroupItem();
}

AddDictionaryDialog::~AddDictionaryDialog()
{
	delete ui;
}

void AddDictionaryDialog::setRemoteRepository(RemoteRepository * repo)
{
	remoteRepository = repo;
}

void AddDictionaryDialog::addFormatItem(){
	QComboBox *formatCombo = findChild<QComboBox *>(QStringLiteral("formatComboBox"));

	formatCombo->addItem("MDict (.mdx)");
	formatCombo->addItem("StarDict (.ifo)");
	formatCombo->addItem("DSL (.dsl/.dsl.dz)");
}

void AddDictionaryDialog::addGroupItem(){
	QList<QSharedPointer<Group>> groups = remoteRepository->getGroups();
	QComboBox *groupCombo = findChild<QComboBox *>(QStringLiteral("groupComboBox"));

	foreach (const QSharedPointer<Group> &group, groups) {
		groupCombo->addItem(group->name, QVariant::fromValue(group.data()));
	}
}

Dictionary AddDictionaryDialog::getNewDictionary(){
	return newDictionary;
}

Group* AddDictionaryDialog::getSelectedGroup(){
	return selectedGroup;
}

void AddDictionaryDialog::on_buttonBox_accepted()
{
	QString name = ui->nameLineEdit->text();
	QString filename = ui->filenameLineEdit->text();
	QString format = ui->formatComboBox->currentText();
	QString displayName = name;

	Dictionary dictionary = {
		name,
		displayName,
		format,
		filename
	};

	newDictionary = dictionary;

	QVariant selectedData = ui->groupComboBox->itemData(ui->groupComboBox->currentIndex());
	selectedGroup = selectedData.value<Group*>();
}

