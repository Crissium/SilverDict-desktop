#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "preferences/preferences.h"
#include "remote/remoterepository.h"

#include <QMainWindow>
#include <QSystemTrayIcon>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT
private:
	Ui::MainWindow * ui;

	QScopedPointer<RemoteRepository> remoteRepository;
	QScopedPointer<Preferences> preferences;

	QScopedPointer<QSystemTrayIcon> trayIcon;
	QScopedPointer<QMenu> trayIconMenu;
	QScopedPointer<QAction> actionRestore;

protected:
	void closeEvent(QCloseEvent * event) override;

private slots:
	void onQuit() const;
	void manageDictionaries();
	void manageGroups();
	void openPreferencesDialogue();
	void manageSources();
	void openAboutDialogue();
	void openAboutQtDialogue();

public:
	explicit MainWindow(QWidget * parent = nullptr);
	~MainWindow() override;

	[[nodiscard]] RemoteRepository * getRemoteRepository() const;
};

#endif // MAINWINDOW_H
