#ifndef SOURCEDIALOG_H
#define SOURCEDIALOG_H

#include <QDialog>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QListWidgetItem>

#include "remote/remoterepository.h"

namespace Ui
{
class SourceDialog;
}

class SourceDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SourceDialog(QWidget * parent = nullptr, RemoteRepository *repo = nullptr);
	~SourceDialog();

signals:
	void updateUISignal();

private slots:
	void onScanButtonClicked();
	void onAddButtonClicked();
	void onDeleteButtonClicked(const QString &source, QListWidgetItem *item);
	void addSourceItem(const QString &source);

private:
	Ui::SourceDialog * ui;
	RemoteRepository* remoteRepository;
	void setRemoteRepository(RemoteRepository * repo);
	void populateListWidget();
};

#endif // SOURCEDIALOG_H
