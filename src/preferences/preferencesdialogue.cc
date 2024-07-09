#include "preferencesdialogue.h"
#include "ui_preferencesdialogue.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSaveFile>
#include <QTextStream>

const QStringList PreferencesDialogue::ThemesNames{tr("Light"), tr("Dark")};

void PreferencesDialogue::saveLocalPreferences() const
{
	auto theme = static_cast<Preferences::Theme>(ui->themeComboBox->currentIndex());
	if (theme != preferences->getTheme())
	{
		preferences->setTheme(theme);
	}

	qreal zoomFactor = ui->zoomFactorSpinBox->value();
	if (zoomFactor != preferences->getZoomFactor())
	{
		preferences->setZoomFactor(zoomFactor);
	}
}

void PreferencesDialogue::saveRemotePreferences()
{
	int sizeHistory = ui->historySizeSpinBox->value();
	if (sizeHistory != remoteRepository->getSizeHistory())
	{
		remoteRepository->setSizeHistory(sizeHistory)
			.then([this](bool success)
				  {
					  if (!success)
					  {
						  QMessageBox::critical(this,
												tr("Error"),
												tr("Failed to update history size."));
					  }
				  });
	}

	int sizeSuggestions = ui->suggestionSizeSpinBox->value();
	if (sizeSuggestions != remoteRepository->getSizeSuggestions())
	{
		remoteRepository->setSizeSuggestions(sizeSuggestions)
			.then([this](bool success)
				  {
					  if (!success)
					  {
						  QMessageBox::critical(this,
												tr("Error"),
												tr("Failed to update suggestion size."));
					  }
				  });
	}
}

void PreferencesDialogue::clearHistory()
{
	remoteRepository->clearHistory()
		.then([this](bool success)
			  {
				  if (!success)
				  {
					  QMessageBox::critical(this,
											tr("Error"),
											tr("Failed to clear history."));
				  }
			  });
}

void PreferencesDialogue::exportHistory()
{
	const QString fileName = QFileDialog::getSaveFileName(this,
														  tr("Export History"),
														  tr("history.txt"),
														  tr("Text Files (*.txt)"));

	QSaveFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QMessageBox::critical(this,
							  tr("Error"),
							  tr("Failed to open file for writing."));
		return;
	}

	QTextStream out(&file);
	for (const QString & word : remoteRepository->getHistory())
	{
		out << word << '\n';
	}

	file.commit();
}

void PreferencesDialogue::createNgramIndex()
{
	// This one has to be blocking because the back-end is not responsive when the ngram index is being created.
	if (!remoteRepository->createNgramIndex().result())
	{
		QMessageBox::critical(this,
							  tr("Error"),
							  tr("Failed to create n-gram index."));
	}
}

void PreferencesDialogue::accept()
{
	saveRemotePreferences();
	saveLocalPreferences();
	QDialog::accept();
}

PreferencesDialogue::PreferencesDialogue(RemoteRepository * repo, Preferences * preferences, QWidget * parent)
	: QDialog(parent)
	, ui(new Ui::PreferencesDialogue)
	, remoteRepository(repo)
	, preferences(preferences)
{
	ui->setupUi(this);

	ui->zoomFactorSpinBox->setValue(preferences->getZoomFactor());

	ui->themeComboBox->addItems(ThemesNames);
	ui->themeComboBox->setCurrentIndex(preferences->getTheme());

	ui->historySizeSpinBox->setValue(remoteRepository->getSizeHistory());
	ui->suggestionSizeSpinBox->setValue(remoteRepository->getSizeSuggestions());

	connect(ui->exportHistoryButton, &QPushButton::clicked, this, &PreferencesDialogue::exportHistory);
	connect(ui->clearHistoryButton, &QPushButton::clicked, this, &PreferencesDialogue::clearHistory);
	connect(ui->createNgramButton, &QPushButton::clicked, this, &PreferencesDialogue::createNgramIndex);
}

PreferencesDialogue::~PreferencesDialogue()
{
	delete ui;
}
