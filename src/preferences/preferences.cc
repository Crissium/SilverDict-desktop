#include "preferences.h"

const QString Preferences::OrganisationName("SilverDict");
const QString Preferences::ApplicationName("SilverDict");

Preferences::Preferences(QObject * parent)
	: QObject{parent}
	, settings(new QSettings(OrganisationName, ApplicationName))
{
}

QString Preferences::getServerAddress() const
{
	return settings->value("ServerAddress", "http://localhost:2628").toString();
}

void Preferences::setServerAddress(const QString & serverAddress) const
{
	settings->setValue("ServerAddress", serverAddress);
}

qreal Preferences::getZoomFactor() const
{
	return settings->value("ZoomFactor", 1.0).toReal();
}

void Preferences::setZoomFactor(qreal zoomFactor) const
{
	settings->setValue("ZoomFactor", zoomFactor);
}

Preferences::Theme Preferences::getTheme() const
{
	return static_cast<Theme>(settings->value("Theme", Light).toInt());
}

void Preferences::setTheme(Theme theme) const
{
	settings->setValue("Theme", theme);
}

QByteArray Preferences::getGeometry() const
{
	return settings->value("Geometry").toByteArray();
}

void Preferences::setGeometry(const QByteArray & geometry) const
{
	settings->setValue("Geometry", geometry);
}
