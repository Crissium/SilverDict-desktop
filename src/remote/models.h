#ifndef MODELS_H
#define MODELS_H

#include <QJsonObject>
#include <QList>
#include <QLocale>
#include <QMap>
#include <QSet>
#include <QString>

/**
 * {
 *	"timestamp": 1719822227792, (ms since epoch)
 *	"suggestions": ["suggestion1", "suggestion2", ...]
 * }
 */
struct Suggestions
{
	QDateTime timestamp;
	QStringList wordList;

	static Suggestions fromJson(const QByteArray & json);
};

/*{
	"dictionary_name": "name",
	"dictionary_display_name": "displayName",
	"dictionary_format": "DSL",
	"dictionary_filename": "filename"
}*/
struct Dictionary
{
	QString name;
	QString displayName;
	QString format;
	QString filename;

	static Dictionary * fromJsonObj(const QJsonObject & obj);
	[[nodiscard]] QJsonObject toJsonObj() const;
};

/*{
	"found": true/false,
	"articles": "<p>...</p>",
	"dictionaries": ["dictName1", "dictName2", ...]
}*/
struct QueryResult
{
	bool found;
	QString articlesHtml;
	QList<const Dictionary *> dictionaries;

	static QueryResult fromJson(
		const QByteArray & json,
		const QList<QSharedPointer<Dictionary>> * availableDictionaries);
};

/*{
	"name": "name",
	"lang": ["en", "fr", ...]
}*/
struct Group
{
	QString name;
	QSet<QLocale::Language> languages;

	static Group * fromJsonObj(const QJsonObject & obj);
	[[nodiscard]] QJsonObject toJsonObj() const;
};

/*{
	"group1": ["dict1", "dict2", ...],
	"group2": ["dict1", "dict3", ...]
}*/
struct Groupings : public QMap<const Group *, QSet<const Dictionary *>>
{
	static Groupings * fromJsonObj(
		const QJsonObject & obj,
		const QList<QSharedPointer<Group>> * availableGroups,
		const QList<QSharedPointer<Dictionary>> * availableDictionaries);
	static Groupings * fromJson(
		const QByteArray & json,
		const QList<QSharedPointer<Group>> * availableGroups,
		const QList<QSharedPointer<Dictionary>> * availableDictionaries);
};

struct Formats : public QStringList
{
	explicit Formats(QStringList formats);
	static Formats * fromJson(const QByteArray & json);
};

struct History : public QStringList
{
	explicit History(QStringList history);
	static History * fromJson(const QByteArray & json);
};

struct Sources : public QStringList
{
	explicit Sources(QStringList sources);
	static Sources * fromJson(const QByteArray & json);
};

#endif // MODELS_H
