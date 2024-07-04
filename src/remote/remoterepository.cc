#include "remoterepository.h"

#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtConcurrent/QtConcurrentRun>
#include <QtNetwork/QNetworkReply>

QFuture<QByteArray> RemoteRepository::get(const QUrl & url) const
{
	auto networkOperation = [this, url]()
	{
		QNetworkReply * reply = manager->get(QNetworkRequest(url));
		QEventLoop loop;
		QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
		loop.exec();

		if (reply->error() == QNetworkReply::NoError)
		{
			QByteArray result = reply->readAll();
			reply->deleteLater();
			return result;
		}
		else
		{
			reply->deleteLater();
			return QByteArrayLiteral("");
		}
	};

	QFuture<QByteArray> future = QtConcurrent::run(networkOperation);

	return future;
}

QFuture<QByteArray> RemoteRepository::post(const QUrl & url, const QJsonDocument & json) const
{
	auto networkOperation = [this, url, json]()
	{
		QNetworkReply * reply = manager->post(QNetworkRequest(url), json.toJson());
		QEventLoop loop;
		QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
		loop.exec();

		if (reply->error() == QNetworkReply::NoError)
		{
			QByteArray result = reply->readAll();
			reply->deleteLater();
			return result;
		}
		else
		{
			reply->deleteLater();
			return QByteArrayLiteral("");
		}
	};

	QFuture<QByteArray> future = QtConcurrent::run(networkOperation);

	return future;
}

QFuture<QByteArray> RemoteRepository::put(const QUrl & url, const QJsonDocument & json) const
{
	auto networkOperation = [this, url, json]()
	{
		QNetworkReply * reply = manager->put(QNetworkRequest(url), json.toJson());
		QEventLoop loop;
		QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
		loop.exec();

		if (reply->error() == QNetworkReply::NoError)
		{
			QByteArray result = reply->readAll();
			reply->deleteLater();
			return result;
		}
		else
		{
			reply->deleteLater();
			return QByteArrayLiteral("");
		}
	};

	QFuture<QByteArray> future = QtConcurrent::run(networkOperation);

	return future;
}

QFuture<QByteArray> RemoteRepository::del(const QUrl & url) const
{
	auto networkOperation = [this, url]()
	{
		QNetworkReply * reply = manager->deleteResource(QNetworkRequest(url));
		QEventLoop loop;
		QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
		loop.exec();

		if (reply->error() == QNetworkReply::NoError)
		{
			QByteArray result = reply->readAll();
			reply->deleteLater();
			return result;
		}
		else
		{
			reply->deleteLater();
			return QByteArrayLiteral("");
		}
	};

	QFuture<QByteArray> future = QtConcurrent::run(networkOperation);

	return future;
}

QFuture<QByteArray> RemoteRepository::del(const QUrl & url, const QJsonDocument & json) const
{
	auto networkOperation = [this, url, json]()
	{
		QNetworkRequest request(url);
		request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
		QNetworkReply * reply = manager->sendCustomRequest(
			QNetworkRequest(url),
			QByteArrayLiteral("DELETE"),
			json.toJson());
		QEventLoop loop;
		QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
		loop.exec();

		if (reply->error() == QNetworkReply::NoError)
		{
			QByteArray result = reply->readAll();
			reply->deleteLater();
			return result;
		}
		else
		{
			reply->deleteLater();
			return QByteArrayLiteral("");
		}
	};

	QFuture<QByteArray> future = QtConcurrent::run(networkOperation);

	return future;
}

QList<QSharedPointer<Dictionary>> RemoteRepository::dictionariesFromJsonArrayStack(const QJsonArray & array)
{
	QList<QSharedPointer<Dictionary>> dictionaries;
	for (auto const & value : array)
	{
		dictionaries.append(QSharedPointer<Dictionary>(Dictionary::fromJsonObj(value.toObject())));
	}
	return dictionaries;
}

QList<QSharedPointer<Dictionary>> * RemoteRepository::dictionariesFromJsonArray(const QJsonArray & array)
{
	auto * dictionaries = new QList<QSharedPointer<Dictionary>>();
	for (auto const & value : array)
	{
		dictionaries->append(QSharedPointer<Dictionary>(Dictionary::fromJsonObj(value.toObject())));
	}
	return dictionaries;
}

QList<QSharedPointer<Dictionary>> * RemoteRepository::dictionariesFromJson(const QByteArray & json)
{
	const QJsonDocument doc = QJsonDocument::fromJson(json);
	const QJsonArray array = doc.array();
	return dictionariesFromJsonArray(array);
}

QList<QSharedPointer<Group>> RemoteRepository::groupsFromJsonArrayStack(const QJsonArray & array)
{
	QList<QSharedPointer<Group>> groups;
	for (auto const & value : array)
	{
		groups.append(QSharedPointer<Group>(Group::fromJsonObj(value.toObject())));
	}
	return groups;
}

QList<QSharedPointer<Group>> * RemoteRepository::groupsFromJsonArray(const QJsonArray & array)
{
	auto * groups = new QList<QSharedPointer<Group>>();
	for (auto const & value : array)
	{
		groups->append(QSharedPointer<Group>(Group::fromJsonObj(value.toObject())));
	}
	return groups;
}

QList<QSharedPointer<Group>> * RemoteRepository::groupsFromJson(const QByteArray & json)
{
	const QJsonDocument doc = QJsonDocument::fromJson(json);
	const QJsonArray array = doc.array();
	return groupsFromJsonArray(array);
}

qsizetype RemoteRepository::sizeFromJson(const QByteArray & json)
{
	const QJsonDocument doc = QJsonDocument::fromJson(json);
	return doc.object().value(QStringLiteral("size")).toInt();
}

QJsonDocument RemoteRepository::sizeToJson(qsizetype size)
{
	QJsonObject obj;
	obj["size"] = size;
	return QJsonDocument(obj);
}

QJsonDocument RemoteRepository::nameToJson(const QString & name)
{
	QJsonObject obj;
	obj["name"] = name;
	return QJsonDocument(obj);
}

QJsonDocument RemoteRepository::sourceToJson(const QString & source)
{
	QJsonObject obj;
	obj["source"] = source;
	return QJsonDocument(obj);
}

bool RemoteRepository::initialise()
{
	// First test connection. The endpoint always returns {"success": true}
	const QByteArray testResult = get(testConnectionEndpoint()).result();
	qDebug() << "Test result" << testResult;
	if (!QJsonDocument::fromJson(testResult).object().value(QStringLiteral("success")).toBool())
	{
		return false;
	}

	const QFuture<QByteArray> dictionariesDataFuture = get(dictionariesEndpoint());
	const QFuture<QByteArray> groupsDataFuture = get(groupsEndpoint());
	const QFuture<QByteArray> groupingsDataFuture = get(groupingsEndpoint());
	const QFuture<QByteArray> historyDataFuture = get(historyEndpoint());
	const QFuture<QByteArray> sizeSuggestionsDataFuture = get(suggestionsSizeEndpoint());
	const QFuture<QByteArray> sizeHistoryDataFuture = get(historySizeEndpoint());
	const QFuture<QByteArray> formatsDataFuture = get(formatsEndpoint());
	const QFuture<QByteArray> sourcesDataFuture = get(sourcesEndpoint());

	// Groupings depends on groups and dictionaries
	const QByteArray dictionariesData = dictionariesDataFuture.result();
	if (dictionariesData.isEmpty())
	{
		return false;
	}
	dictionaries.reset(dictionariesFromJson(dictionariesData));

	const QByteArray groupsData = groupsDataFuture.result();
	if (groupsData.isEmpty())
	{
		return false;
	}
	groups.reset(groupsFromJson(groupsData));
	if (groups->isEmpty())
	{
		return false;
	}
	activeGroup = groups->first().data();

	const QByteArray groupingsData = groupingsDataFuture.result();
	if (groupingsData.isEmpty())
	{
		return false;
	}
	groupings.reset(
		Groupings::fromJson(
			groupingsData,
			groups.data(),
			dictionaries.data()));

	// Now initialise the rest
	const QByteArray historyData = historyDataFuture.result();
	if (historyData.isEmpty())
	{
		return false;
	}
	history.reset(History::fromJson(historyData));

	const QByteArray sizeSuggestionsData = sizeSuggestionsDataFuture.result();
	if (sizeSuggestionsData.isEmpty())
	{
		return false;
	}
	sizeSuggestions = sizeFromJson(sizeSuggestionsData);

	const QByteArray sizeHistoryData = sizeHistoryDataFuture.result();
	if (sizeHistoryData.isEmpty())
	{
		return false;
	}
	sizeHistory = sizeFromJson(sizeHistoryData);

	const QByteArray formatsData = formatsDataFuture.result();
	if (formatsData.isEmpty())
	{
		return false;
	}
	formats.reset(Formats::fromJson(formatsData));

	const QByteArray sourcesData = sourcesDataFuture.result();
	if (sourcesData.isEmpty())
	{
		return false;
	}
	sources.reset(Sources::fromJson(sourcesData));

	return true;
}

void RemoteRepository::updateDictionaries(const QList<QSharedPointer<Dictionary>> & newDictionaries) const
{
	// Fist find those no longer present, then delete them.
	// We'd better not do this in one go.
	QStringList deletedNames;
	for (auto const & d : *dictionaries)
	{
		bool found = false;
		for (auto const & nD : newDictionaries)
		{
			if (d->name == nD->name)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			deletedNames.append(d->name);
		}
	}

	for (const QString & name : deletedNames)
	{
		erase_if(*dictionaries, [&name](const QSharedPointer<Dictionary> & d)
				 {
					 return d->name == name;
				 });
	}

	// Now find the new ones and add them
	for (auto const & nD : newDictionaries)
	{
		bool found = false;
		for (auto const & d : *dictionaries)
		{
			if (d->name == nD->name)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			dictionaries->append(nD);
		}
	}

	// Then reorder them according to the new list
	for (qsizetype i = 0; i < newDictionaries.size(); ++i)
	{
		for (qsizetype j = 0; j < dictionaries->size(); ++j)
		{
			if (dictionaries->at(j)->name == newDictionaries[i]->name)
			{
				if (j != i)
				{
					dictionaries->move(j, i);
				}
				break;
			}
		}
	}
}

void RemoteRepository::updateGroups(const QList<QSharedPointer<Group>> & newGroups)
{
	// Fist find those no longer present, then delete them.
	// We'd better not do this in one go.
	QStringList deletedNames;
	for (auto const & g : *groups)
	{
		bool found = false;
		for (auto const & nG : newGroups)
		{
			if (g->name == nG->name)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			deletedNames.append(g->name);
		}
	}

	if (deletedNames.contains(activeGroup->name))
	{
		activeGroup = nullptr;
	}

	for (const QString & name : deletedNames)
	{
		erase_if(*groups, [&name](const QSharedPointer<Group> & g)
				 {
					 return g->name == name;
				 });
	}

	// Now find the new ones and add them
	for (auto const & nG : newGroups)
	{
		bool found = false;
		for (auto const & g : *groups)
		{
			if (g->name == nG->name)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			groups->append(nG);
		}
	}

	// Then reorder them according to the new list
	for (qsizetype i = 0; i < newGroups.size(); ++i)
	{
		for (qsizetype j = 0; j < groups->size(); ++j)
		{
			if (groups->at(j)->name == newGroups[i]->name)
			{
				if (j != i)
				{
					groups->move(j, i);
				}
				break;
			}
		}
	}

	if (!activeGroup)
	{
		activeGroup = groups->first().data();
	}
}

QueryResult RemoteRepository::processQueryResult(const QByteArray & result) const
{
	return QueryResult::fromJson(result, dictionaries.data());
}

bool RemoteRepository::processDictionariesAndGroupings(const QByteArray & result)
{
	if (result.isEmpty())
	{
		return false;
	}

	const QJsonDocument doc = QJsonDocument::fromJson(result);
	const QJsonObject obj = doc.object();
	updateDictionaries(dictionariesFromJsonArrayStack(obj["dictionaries"].toArray()));
	groupings.reset(
		Groupings::fromJsonObj(
			obj["groupings"].toObject(),
			groups.data(),
			dictionaries.data()));

	return true;
}

bool RemoteRepository::processSources(const QByteArray & result)
{
	if (result.isEmpty())
	{
		return false;
	}

	sources.reset(Sources::fromJson(result));
	return true;
}

bool RemoteRepository::processGroupsAndGroupings(const QByteArray & result)
{
	if (result.isEmpty())
	{
		return false;
	}

	const QJsonDocument doc = QJsonDocument::fromJson(result);
	const QJsonObject obj = doc.object();
	updateGroups(groupsFromJsonArrayStack(obj["groups"].toArray()));
	groupings.reset(
		Groupings::fromJsonObj(
			obj["groupings"].toObject(),
			groups.data(),
			dictionaries.data()));

	return true;
}

bool RemoteRepository::processGroups(const QByteArray & result)
{
	if (result.isEmpty())
	{
		return false;
	}

	updateGroups(groupsFromJsonArrayStack(QJsonDocument::fromJson(result).array()));
	return true;
}

bool RemoteRepository::processGroupings(const QByteArray & result)
{
	if (result.isEmpty())
	{
		return false;
	}

	groupings.reset(
		Groupings::fromJson(
			result,
			groups.data(),
			dictionaries.data()));

	return true;
}

QUrl RemoteRepository::testConnectionEndpoint() const
{
	return baseUrl.resolved(QUrl(QStringLiteral("validator/test_connection")));
}

QUrl RemoteRepository::suggestionsEndpoint(const QString & groupName, const QString & key) const
{
	return baseUrl.resolved(QUrl(QStringLiteral("suggestions/%1/%2").arg(groupName, key)));
}

QUrl RemoteRepository::queryEndpoint(const QString & groupName, const QString & key) const
{
	return baseUrl.resolved(QUrl(QStringLiteral("query/%1/%2").arg(groupName, key)));
}

QUrl RemoteRepository::ankiEndpoint(const QString & groupName, const QString & key) const
{
	return baseUrl.resolved(QUrl(QStringLiteral("anki/%1/%2").arg(groupName, key)));
}

QUrl RemoteRepository::formatsEndpoint() const
{
	return baseUrl.resolved(QUrl(QStringLiteral("management/formats")));
}

QUrl RemoteRepository::dictionariesEndpoint() const
{
	return baseUrl.resolved(QUrl(QStringLiteral("management/dictionaries")));
}

QUrl RemoteRepository::dictionaryRenameEndpoint() const
{
	return baseUrl.resolved(QUrl(QStringLiteral("management/dictionary_name")));
}

QUrl RemoteRepository::headwordCountEndpoint() const
{
	return baseUrl.resolved(QUrl(QStringLiteral("management/headword_count")));
}

QUrl RemoteRepository::sourcesEndpoint() const
{
	return baseUrl.resolved(QUrl(QStringLiteral("management/sources")));
}

QUrl RemoteRepository::scanEndpoint() const
{
	return baseUrl.resolved(QUrl(QStringLiteral("management/scan")));
}

QUrl RemoteRepository::groupsEndpoint() const
{
	return baseUrl.resolved(QUrl(QStringLiteral("management/groups")));
}

QUrl RemoteRepository::groupLangChangeEndpoint() const
{
	return baseUrl.resolved(QUrl(QStringLiteral("management/group_lang")));
}

QUrl RemoteRepository::groupRenameEndpoint() const
{
	return baseUrl.resolved(QUrl(QStringLiteral("management/group_name")));
}

QUrl RemoteRepository::groupingsEndpoint() const
{
	return baseUrl.resolved(QUrl(QStringLiteral("management/dictionary_groupings")));
}

QUrl RemoteRepository::historyEndpoint() const
{
	return baseUrl.resolved(QUrl(QStringLiteral("management/history")));
}

QUrl RemoteRepository::historySizeEndpoint() const
{
	return baseUrl.resolved(QUrl(QStringLiteral("management/history_size")));
}

QUrl RemoteRepository::suggestionsSizeEndpoint() const
{
	return baseUrl.resolved(QUrl(QStringLiteral("management/num_suggestions")));
}

QUrl RemoteRepository::createNgramIndexEndpoint() const
{
	return baseUrl.resolved(QUrl(QStringLiteral("management/create_ngram_table")));
}

QUrl RemoteRepository::validatorDictionaryEndpoint() const
{
	return baseUrl.resolved(QUrl(QStringLiteral("validator/dictionary")));
}

QUrl RemoteRepository::validatorSourceEndpoint() const
{
	return baseUrl.resolved(QUrl(QStringLiteral("validator/source")));
}

RemoteRepository::RemoteRepository(QUrl baseUrl, QObject * parent)
	: QObject(parent)
	, manager(new QNetworkAccessManager(this))
	, baseUrl(std::move(baseUrl))
	, dictionaries(nullptr)
	, groups(nullptr)
	, activeGroup(nullptr)
	, groupings(nullptr)
	, history(nullptr)
	, sizeSuggestions(10)
	, sizeHistory(100)
	, formats(nullptr)
	, sources(nullptr)
{
	if (!initialise())
	{
		throw InitialisationError("Failed to initialise RemoteRepository");
	}
}

const QUrl & RemoteRepository::getBaseUrl() const
{
	return baseUrl;
}

bool RemoteRepository::setBaseUrl(const QUrl & url)
{
	const QUrl original = baseUrl;
	const bool alreadyInitialised = dictionaries && groups && groupings && history && formats && sources;
	baseUrl = url;
	if (!initialise())
	{
		baseUrl = original;
		if (alreadyInitialised)
		{
			initialise();
		}
		return false;
	}

	return true;
}

QFuture<Suggestions> RemoteRepository::getSuggestions(const QString & key) const
{
	return get(suggestionsEndpoint(activeGroup->name, key))
		.then([this](const QByteArray & result)
			  {
				  return Suggestions::fromJson(result);
			  });
}

QFuture<QueryResult> RemoteRepository::query(const QString & key) const
{
	return get(queryEndpoint(activeGroup->name, key))
		.then(std::bind(&RemoteRepository::processQueryResult, this, std::placeholders::_1));
}

QFuture<QueryResult> RemoteRepository::queryAnki(const QString & word) const
{
	return get(ankiEndpoint(activeGroup->name, word))
		.then(std::bind(&RemoteRepository::processQueryResult, this, std::placeholders::_1));
}

const QList<QSharedPointer<Dictionary>> & RemoteRepository::getDictionaries() const
{
	return *dictionaries;
}

QFuture<bool> RemoteRepository::addDictionary(const Dictionary & dictionary, const Group * group)
{
	QJsonObject dictObj = dictionary.toJsonObj();
	// Validate
	const QByteArray validationResult = post(validatorDictionaryEndpoint(), QJsonDocument(dictObj)).result();
	if (validationResult.isEmpty())
	{
		return QtFuture::makeReadyFuture(false);
	}
	// It must be {"valid": true}
	if (!QJsonDocument::fromJson(validationResult).object().value(QStringLiteral("valid")).toBool())
	{
		return QtFuture::makeReadyFuture(false);
	}

	dictObj["group_name"] = group->name;
	return post(dictionariesEndpoint(), QJsonDocument(dictObj))
		.then(std::bind(&RemoteRepository::processDictionariesAndGroupings, this, std::placeholders::_1));
}

QFuture<bool> RemoteRepository::deleteDictionary(const Dictionary * dictionary)
{
	return del(dictionariesEndpoint(), nameToJson(dictionary->name))
		.then(std::bind(&RemoteRepository::processDictionariesAndGroupings, this, std::placeholders::_1));
}

QFuture<bool> RemoteRepository::reorderDictionaries(const QList<const Dictionary *> & dictionaries) const
{
	QJsonArray array;
	for (auto const & d : dictionaries)
	{
		array.append(d->toJsonObj());
	}

	return put(dictionariesEndpoint(), QJsonDocument(array))
		.then([this](const QByteArray & result)
			  {
				  if (!result.isEmpty())
				  {
					  updateDictionaries(dictionariesFromJsonArrayStack(QJsonDocument::fromJson(result).array()));
					  return true;
				  }
				  else
				  {
					  return false;
				  }
			  });
}

QFuture<bool> RemoteRepository::renameDictionary(
	const Dictionary * dictionary,
	const QString & newDisplayName) const
{
	QJsonObject obj;
	obj["name"] = dictionary->name;
	obj["display"] = newDisplayName;
	return put(dictionariesEndpoint(), QJsonDocument(obj))
		.then([this, dictionary, &newDisplayName](const QByteArray & result)
			  {
				  if (!result.isEmpty())
				  {
					  // It must be {"success": true}
					  if (QJsonDocument::fromJson(result).object().value(QStringLiteral("success")).toBool())
					  {
						  // Note the pointer is const, and we don't want a const_cast
						  for (auto const & d : *dictionaries)
						  {
							  if (d->name == dictionary->name)
							  {
								  d->displayName = newDisplayName;
								  return true;
							  }
						  }
						  return false;
					  }
					  else
					  {
						  return false;
					  }
				  }
				  else
				  {
					  return false;
				  }
			  });
}

QFuture<qsizetype> RemoteRepository::getHeadwordCount(const Dictionary * dictionary)
{
	if (headwordCounts.contains(dictionary))
	{
		return QtFuture::makeReadyFuture(headwordCounts[dictionary]);
	}

	return post(headwordCountEndpoint(), nameToJson(dictionary->name))
		.then([this, dictionary](const QByteArray & result)
			  {
				  if (!result.isEmpty())
				  {
					  const qsizetype count = QJsonDocument::fromJson(result).object().value(QStringLiteral("count")).toInt();
					  headwordCounts[dictionary] = count;
					  return count;
				  }
				  else
				  {
					  return 0ll;
				  }
			  });
}

const Sources & RemoteRepository::getSources() const
{
	return *sources;
}

QFuture<bool> RemoteRepository::addSource(const QString & source)
{
	const QJsonDocument doc = sourceToJson(source);
	// Validate
	const QByteArray validationResult = post(validatorSourceEndpoint(), doc).result();
	if (validationResult.isEmpty())
	{
		return QtFuture::makeReadyFuture(false);
	}
	// It must be {"valid": true}
	if (!QJsonDocument::fromJson(validationResult).object().value(QStringLiteral("valid")).toBool())
	{
		return QtFuture::makeReadyFuture(false);
	}

	return post(sourcesEndpoint(), doc)
		.then(std::bind(&RemoteRepository::processSources, this, std::placeholders::_1));
}

QFuture<bool> RemoteRepository::deleteSource(const QString & source)
{
	return del(sourcesEndpoint(), sourceToJson(source))
		.then(std::bind(&RemoteRepository::processSources, this, std::placeholders::_1));
}

QFuture<bool> RemoteRepository::rescanSources()
{
	return post(scanEndpoint(), QJsonDocument())
		.then(std::bind(&RemoteRepository::processDictionariesAndGroupings, this, std::placeholders::_1));
}

const Group * RemoteRepository::getGroup(const QString & name) const
{
	for (auto const & g : *groups)
	{
		if (g->name == name)
		{
			return g.data();
		}
	}
	return nullptr;
}

const QList<QSharedPointer<Group>> & RemoteRepository::getGroups() const
{
	return *groups;
}

QFuture<bool> RemoteRepository::addGroup(const Group & group)
{
	return post(groupsEndpoint(), QJsonDocument(group.toJsonObj()))
		.then(std::bind(&RemoteRepository::processGroupsAndGroupings, this, std::placeholders::_1));
}

QFuture<bool> RemoteRepository::deleteGroup(const Group * group)
{
	return del(groupsEndpoint(), nameToJson(group->name))
		.then(std::bind(&RemoteRepository::processGroupsAndGroupings, this, std::placeholders::_1));
}

QFuture<bool> RemoteRepository::reorderGroups(const QList<const Group *> & groups)
{
	QJsonArray array;
	for (auto const & g : groups)
	{
		array.append(g->toJsonObj());
	}

	return put(groupsEndpoint(), QJsonDocument(array))
		.then(std::bind(&RemoteRepository::processGroups, this, std::placeholders::_1));
}

QFuture<bool> RemoteRepository::changeGroupLanguages(const Group * group, const QSet<QLocale::Language> & languages)
{
	QJsonObject obj;
	obj["name"] = group->name;

	QJsonArray langArray;
	for (const QLocale::Language lang : languages)
	{
		langArray.append(QLocale::languageToCode(lang, QLocale::ISO639Part1));
	}
	obj["lang"] = langArray;

	return put(groupLangChangeEndpoint(), QJsonDocument(obj))
		.then(std::bind(&RemoteRepository::processGroups, this, std::placeholders::_1));
}

QFuture<bool> RemoteRepository::renameGroup(const Group * group, const QString & newName)
{
	QJsonObject obj;
	obj["old"] = group->name;
	obj["new"] = newName;
	return put(groupRenameEndpoint(), QJsonDocument(obj))
		.then(std::bind(&RemoteRepository::processGroupsAndGroupings, this, std::placeholders::_1));
}

const Group * RemoteRepository::getActiveGroup() const
{
	return activeGroup;
}

void RemoteRepository::setActiveGroup(const Group * group)
{
	activeGroup = group;
}

const Groupings & RemoteRepository::getGroupings() const
{
	return *groupings;
}

QFuture<bool> RemoteRepository::addDictionaryToGroup(const Dictionary * dictionary, const Group * group)
{
	QJsonObject obj;
	obj["dictionary_name"] = dictionary->name;
	obj["group_name"] = group->name;
	return post(groupingsEndpoint(), QJsonDocument(obj))
		.then(std::bind(&RemoteRepository::processGroupings, this, std::placeholders::_1));
}

QFuture<bool> RemoteRepository::removeDictionaryFromGroup(const Dictionary * dictionary, const Group * group)
{
	QJsonObject obj;
	obj["dictionary_name"] = dictionary->name;
	obj["group_name"] = group->name;
	return del(groupingsEndpoint(), QJsonDocument(obj))
		.then(std::bind(&RemoteRepository::processGroupings, this, std::placeholders::_1));
}

const History & RemoteRepository::getHistory() const
{
	return *history;
}

QFuture<bool> RemoteRepository::clearHistory()
{
	return del(historyEndpoint())
		.then([this](const QByteArray & result)
			  {
				  if (!result.isEmpty())
				  {
					  history.reset(History::fromJson(result));
					  return true;
				  }
				  else
				  {
					  return false;
				  }
			  });
}

qsizetype RemoteRepository::getSizeSuggestions() const
{
	return sizeSuggestions;
}

QFuture<bool> RemoteRepository::setSizeSuggestions(qsizetype size)
{
	return put(suggestionsSizeEndpoint(), sizeToJson(size))
		.then([this](const QByteArray & result)
			  {
				  if (!result.isEmpty())
				  {
					  sizeSuggestions = sizeFromJson(result);
					  return true;
				  }
				  else
				  {
					  return false;
				  }
			  });
}

qsizetype RemoteRepository::getSizeHistory() const
{
	return sizeHistory;
}

QFuture<bool> RemoteRepository::setSizeHistory(qsizetype size)
{
	return put(historySizeEndpoint(), sizeToJson(size))
		.then([this, size](const QByteArray & result)
			  {
				  if (!result.isEmpty())
				  {
					  sizeHistory = size;
					  history.reset(History::fromJson(result));
					  return true;
				  }
				  else
				  {
					  return false;
				  }
			  });
}

const Formats & RemoteRepository::getFormats() const
{
	return *formats;
}

QFuture<bool> RemoteRepository::createNgramIndex() const
{
	return get(createNgramIndexEndpoint())
		.then([this](const QByteArray & result)
			  {
				  if (!result.isEmpty())
				  {
					  // It must be {"success": true}
					  return QJsonDocument::fromJson(result).object().value(QStringLiteral("success")).toBool();
				  }
				  else
				  {
					  return false;
				  }
			  });
}
