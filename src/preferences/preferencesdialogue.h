#ifndef PREFERENCESDIALOGUE_H
#define PREFERENCESDIALOGUE_H

#include "preferences.h"
#include "remote/remoterepository.h"

#include <QDialog>

namespace Ui
{
class PreferencesDialogue;
}

class PreferencesDialogue : public QDialog
{
	Q_OBJECT

private:
	const static QStringList ThemesNames;
	Ui::PreferencesDialogue * ui;
	RemoteRepository * remoteRepository;
	Preferences * preferences;

	void saveLocalPreferences() const;
	void saveRemotePreferences();

private slots:
	void clearHistory();
	void exportHistory();
	void createNgramIndex();

	void accept() override;

public:
	explicit PreferencesDialogue(RemoteRepository * repo, Preferences * preferences, QWidget * parent = nullptr);
	~PreferencesDialogue() override;
};

#endif // PREFERENCESDIALOGUE_H
