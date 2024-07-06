#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "remote/remoterepository.h"

#include <QMainWindow>

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
	Ui::MainWindow * ui{};

	QScopedPointer<RemoteRepository> remoteRepository;

	void setupMenu();

	void editDictionary();

public:
	explicit MainWindow(QWidget * parent = nullptr);
	~MainWindow() override;

	[[nodiscard]] RemoteRepository * getRemoteRepository() const;
};

#endif // MAINWINDOW_H
