#include "models.h"

#include <QJsonArray>
#include <QJsonDocument>

Suggestions Suggestions::fromJson(const QByteArray & json)
{
	const QJsonDocument doc = QJsonDocument::fromJson(json);
	const QJsonObject obj = doc.object();

	const QJsonArray suggestionsArray = obj["suggestions"].toArray();
	QStringList suggestions(suggestionsArray.size());
	for (qsizetype i = 0; i < suggestionsArray.size(); ++i)
	{
		suggestions[i] = suggestionsArray[i].toString();
	}

	return {
		QDateTime::fromMSecsSinceEpoch(obj["timestamp"].toInteger()),
		suggestions};
}

Dictionary * Dictionary::fromJsonObj(const QJsonObject & obj)
{
	return new Dictionary({obj["dictionary_name"].toString(),
						   obj["dictionary_display_name"].toString(),
						   obj["dictionary_format"].toString(),
						   obj["dictionary_filename"].toString()});
}

QJsonObject Dictionary::toJsonObj() const
{
	QJsonObject obj;
	obj["dictionary_name"] = name;
	obj["dictionary_display_name"] = displayName;
	obj["dictionary_format"] = format;
	obj["dictionary_filename"] = filename;

	return obj;
}

QueryResult QueryResult::fromJson(const QByteArray & json, const QList<QSharedPointer<Dictionary>> * availableDictionaries)
{
	if (json.isEmpty())
	{
		QList<const Dictionary *> dictionaries;
		for (auto const & dict : *availableDictionaries)
		{
			dictionaries.append(dict.data());
		}
		return {false, QStringLiteral(""), dictionaries};
	}

	const QJsonDocument doc = QJsonDocument::fromJson(json);
	const QJsonObject obj = doc.object();

	const QJsonArray dictionariesArray = obj["dictionaries"].toArray();
	QList<const Dictionary *> dictionaries;
	for (auto const & dictName : dictionariesArray)
	{
		const QString name = dictName.toString();
		for (auto const & dict : *availableDictionaries)
		{
			if (dict->name == name)
			{
				dictionaries.append(dict.data());
				break;
			}
		}
	}

	return {
		obj["found"].toBool(),
		obj["articles"].toString(),
		dictionaries};
}

Group * Group::fromJsonObj(const QJsonObject & obj)
{
	const QJsonArray langArray = obj["lang"].toArray();
	QSet<QLocale::Language> languages;
	for (auto const & lang : langArray)
	{
		languages.insert(QLocale::codeToLanguage(lang.toString(), QLocale::ISO639Part1));
	}

	return new Group({obj["name"].toString(),
					  languages});
}

QJsonObject Group::toJsonObj() const
{
	QJsonObject obj;
	obj["name"] = name;

	QJsonArray langArray;
	for (const QLocale::Language lang : languages)
	{
		langArray.append(QLocale::languageToCode(lang, QLocale::ISO639Part1));
	}
	obj["lang"] = langArray;

	return obj;
}

Groupings * Groupings::fromJsonObj(
	const QJsonObject & obj,
	const QList<QSharedPointer<Group>> * availableGroups,
	const QList<QSharedPointer<Dictionary>> * availableDictionaries)
{
	auto * groupings = new Groupings();
	for (auto const & groupName : obj.keys())
	{
		const Group * group = nullptr;
		for (auto const & g : *availableGroups)
		{
			if (g->name == groupName)
			{
				group = g.data();
				break;
			}
		}

		groupings->insert(group, QSet<const Dictionary *>());
		for (auto const & dictName : obj[groupName].toArray())
		{
			const QString name = dictName.toString();
			for (auto const & dict : *availableDictionaries)
			{
				if (dict->name == name)
				{
					(*groupings)[group].insert(dict.data());
					break;
				}
			}
		}
	}

	return groupings;
}

Groupings * Groupings::fromJson(
	const QByteArray & json,
	const QList<QSharedPointer<Group>> * availableGroups,
	const QList<QSharedPointer<Dictionary>> * availableDictionaries)
{
	const QJsonDocument doc = QJsonDocument::fromJson(json);
	const QJsonObject obj = doc.object();
	return fromJsonObj(obj, availableGroups, availableDictionaries);
}

Formats::Formats(QStringList formats)
	: QStringList(std::move(formats))
{
}

Formats * Formats::fromJson(const QByteArray & json)
{
	const QJsonDocument doc = QJsonDocument::fromJson(json);
	const QJsonArray array = doc.array();

	QStringList formats(array.size());
	for (qsizetype i = 0; i < array.size(); ++i)
	{
		formats[i] = array[i].toString();
	}

	return new Formats(formats);
}

History::History(QStringList history)
	: QStringList(std::move(history))
{
}

History * History::fromJson(const QByteArray & json)
{
	const QJsonDocument doc = QJsonDocument::fromJson(json);
	const QJsonArray array = doc.array();

	QStringList history(array.size());
	for (qsizetype i = 0; i < array.size(); ++i)
	{
		history[i] = array[i].toString();
	}

	return new History(history);
}

Sources::Sources(QStringList sources)
	: QStringList(std::move(sources))
{
}

Sources * Sources::fromJson(const QByteArray & json)
{
	const QJsonDocument doc = QJsonDocument::fromJson(json);
	const QJsonArray array = doc.array();

	QStringList sources(array.size());
	for (qsizetype i = 0; i < array.size(); ++i)
	{
		sources[i] = array[i].toString();
	}

	return new Sources(sources);
}
