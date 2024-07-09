#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QSettings>

class Preferences final : public QObject
{
	Q_OBJECT
private:
	static const QString OrganisationName;
	static const QString ApplicationName;
	QScopedPointer<QSettings> settings;

public:
	enum Theme
	{
		Light,
		Dark
	};
	Q_ENUM(Theme)

	explicit Preferences(QObject * parent = nullptr);
	Q_DISABLE_COPY(Preferences)

	[[nodiscard]] QString getServerAddress() const; // e.g., http://localhost:2628
	void setServerAddress(const QString & serverAddress) const;

	[[nodiscard]] qreal getZoomFactor() const;
	void setZoomFactor(qreal zoomFactor) const;

	[[nodiscard]] Theme getTheme() const;
	void setTheme(Theme theme) const;

	[[nodiscard]] QByteArray getGeometry() const;
	void setGeometry(const QByteArray & geometry) const;
};

#endif // PREFERENCES_H
