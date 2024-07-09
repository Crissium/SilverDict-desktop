#ifndef ARTICLEVIEW_H
#define ARTICLEVIEW_H

#include "../remote/remoterepository.h"
#include "preferences/preferences.h"

#include <QRegularExpression>
#include <QToolButton>
#include <QtWebEngineWidgets/QWebEngineView>

namespace Ui
{
class ArticleView;
}

class ArticleView : public QWidget
{
	Q_OBJECT

private:
	static const QRegularExpression DictNamePattern;
	static const qreal ZoomStep;

	Ui::ArticleView * ui;
	RemoteRepository * remoteRepository;
	Preferences * preferences;
	std::function<void(const QString &)> updateTabTitle;

	void extractDictNames(const QString & html) const;

	void injectCSS(const QString & css) const;

	void saveMedia(const QUrl & url);

private slots:
	void injectJavaScript() const;
	void onLoadStarted() const;
	void onLoadFinished(bool ok) const;
	void zoomIn() const;
	void zoomOut() const;
	void zoomReset() const;
	void playFirstAudio() const;
	void createContextMenu(const QPoint & pos);

signals:
	void articleLoaded(QStringList dictNames) const;
	void historyUpdated() const;
	void openLinkInNewTabRequested(const QUrl & url) const;

public:
	explicit ArticleView(QWidget * parent = nullptr); // the parent is NOT the tab widget
	~ArticleView() override;

	void setup(RemoteRepository * repo, Preferences * preferences); // to be called after initialisation

	[[nodiscard]] QWebEngineView * getWebView() const;
	[[nodiscard]] QToolButton * getNewTabButton() const;

	[[nodiscard]] bool isInAnkiMode() const;

	void lookup(const QString & word) const;

	void navigateTo(const QString & id) const;
};

#endif // ARTICLEVIEW_H
