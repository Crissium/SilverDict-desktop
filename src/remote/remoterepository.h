#ifndef REMOTEREPOSITORY_H
#define REMOTEREPOSITORY_H

#include "models.h"

#include <QFuture>
#include <QJsonDocument>
#include <QMap>
#include <QString>
#include <QtNetwork/QNetworkAccessManager>

class RemoteRepository : public QObject
{
	Q_OBJECT

private:
	// QScopedPointer<QNetworkAccessManager> manager;
	[[nodiscard]] static QFuture<QByteArray> get(const QUrl & url);
	[[nodiscard]] static QFuture<QByteArray> post(const QUrl & url, const QJsonDocument & json);
	[[nodiscard]] static QFuture<QByteArray> put(const QUrl & url, const QJsonDocument & json);
	[[nodiscard]] static QFuture<QByteArray> del(const QUrl & url);
	[[nodiscard]] static QFuture<QByteArray> del(const QUrl & url, const QJsonDocument & json);

	static QList<QSharedPointer<Dictionary>> dictionariesFromJsonArrayStack(const QJsonArray & array);
	static QList<QSharedPointer<Dictionary>> * dictionariesFromJsonArray(const QJsonArray & array);
	static QList<QSharedPointer<Dictionary>> * dictionariesFromJson(const QByteArray & json);
	static QList<QSharedPointer<Group>> groupsFromJsonArrayStack(const QJsonArray & array);
	static QList<QSharedPointer<Group>> * groupsFromJsonArray(const QJsonArray & array);
	static QList<QSharedPointer<Group>> * groupsFromJson(const QByteArray & json);
	/* {"size": 10} */
	static qsizetype sizeFromJson(const QByteArray & json);
	static QJsonDocument sizeToJson(qsizetype size);
	/* {"name": "some name"} */
	static QJsonDocument nameToJson(const QString & name);
	/* {"source": "/path/to/source"} */
	static QJsonDocument sourceToJson(const QString & source);

	QUrl apiPrefix; // e.g. http://localhost:8080/api/
	[[nodiscard]] QUrl testConnectionEndpoint() const;
	[[nodiscard]] QUrl suggestionsEndpoint(const QString & groupName, const QString & key) const;
	[[nodiscard]] QUrl queryEndpoint(const QString & groupName, const QString & key, bool raw = false) const;
	[[nodiscard]] QUrl ankiEndpoint(const QString & groupName, const QString & key, bool raw = false) const;
	[[nodiscard]] QUrl formatsEndpoint() const;
	[[nodiscard]] QUrl dictionariesEndpoint() const;
	[[nodiscard]] QUrl dictionaryRenameEndpoint() const;
	[[nodiscard]] QUrl headwordCountEndpoint() const;
	[[nodiscard]] QUrl sourcesEndpoint() const;
	[[nodiscard]] QUrl scanEndpoint() const;
	[[nodiscard]] QUrl groupsEndpoint() const;
	[[nodiscard]] QUrl groupLangChangeEndpoint() const;
	[[nodiscard]] QUrl groupRenameEndpoint() const;
	[[nodiscard]] QUrl groupingsEndpoint() const;
	[[nodiscard]] QUrl historyEndpoint() const;
	[[nodiscard]] QUrl historySizeEndpoint() const;
	[[nodiscard]] QUrl suggestionsSizeEndpoint() const;
	[[nodiscard]] QUrl createNgramIndexEndpoint() const;
	[[nodiscard]] QUrl validatorDictionaryEndpoint() const;
	[[nodiscard]] QUrl validatorSourceEndpoint() const;

	QScopedPointer<QList<QSharedPointer<Dictionary>>> dictionaries;
	QScopedPointer<QList<QSharedPointer<Group>>> groups;
	const Group * activeGroup;
	QScopedPointer<Groupings> groupings;

	QMap<const Dictionary *, qsizetype> headwordCounts;

	QScopedPointer<History> history;

	qsizetype sizeSuggestions;
	qsizetype sizeHistory;

	QScopedPointer<Formats> formats;

	QScopedPointer<Sources> sources;

	/**
	 * A dictionary can be deleted or added.
	 * The list can also be reordered.
	 * When updating the list, we should first remove those no longer present,
	 * then add the new ones, and finally update the order.
	 * During this process, the pointers to the dictionaries should remain the same,
	 * that is, the objects themselves should not be recreated.
	 */
	void updateDictionaries(const QList<QSharedPointer<Dictionary>> & newDictionaries);

	/**
	 * A group can be added or deleted (renaming is essentially deleting and adding).
	 * The list can also be reordered.
	 * When updating the list, we should first remove those no longer present,
	 * then add the new ones, and finally update the order.
	 * During this process, the pointers to the groups should remain the same,
	 * that is, the objects themselves should not be recreated.
	 */
	void updateGroups(const QList<QSharedPointer<Group>> & newGroups);

	[[nodiscard]] QueryResult processQueryResult(const QByteArray & result) const;
	bool processHistory(const QByteArray & result);
	bool processDictionariesAndGroupings(const QByteArray & result);
	bool processSources(const QByteArray & result);
	bool processGroupsAndGroupings(const QByteArray & result);
	bool processGroups(const QByteArray & result);
	bool processGroupings(const QByteArray & result);

signals:
	void dictionariesChanged();
	void groupsChanged();

public:
	class InitialisationError final : public std::runtime_error
	{
	public:
		using std::runtime_error::runtime_error;
	};

	class NetworkError final : public std::runtime_error
	{
	public:
		using std::runtime_error::runtime_error;
	};

	bool initialise();

	explicit RemoteRepository(QUrl apiPrefix, QObject * parent = nullptr);

	[[nodiscard]] QUrl getBaseUrl() const;
	[[nodiscard]] const QUrl & getApiPrefix() const;
	bool setApiPrefix(const QUrl & url);

	[[nodiscard]] QFuture<Suggestions> getSuggestions(const QString & key) const;

	[[nodiscard]] QFuture<QueryResult> query(const QString & key);
	[[nodiscard]] QUrl getQueryUrl(const QString & key) const;

	[[nodiscard]] QFuture<QueryResult> queryAnki(const QString & word) const;
	[[nodiscard]] QUrl getQueryAnkiUrl(const QString & word) const;

	[[nodiscard]] const QList<QSharedPointer<Dictionary>> & getDictionaries() const;
	QFuture<bool> addDictionary(const Dictionary & dictionary, const Group * group);
	QFuture<bool> deleteDictionary(const Dictionary * dictionary);
	[[nodiscard]] QFuture<bool> reorderDictionaries(const QList<const Dictionary *> & dictionaries);
	QFuture<bool> renameDictionary(const Dictionary * dictionary, const QString & newDisplayName);

	QFuture<qsizetype> getHeadwordCount(const Dictionary * dictionary);

	[[nodiscard]] const Sources & getSources() const;
	QFuture<bool> addSource(const QString & source);
	QFuture<bool> deleteSource(const QString & source);

	QFuture<bool> rescanSources();

	[[nodiscard]] const QList<QSharedPointer<Group>> & getGroups() const;
	[[nodiscard]] const Group * getGroup(const QString & name) const;
	QFuture<bool> addGroup(const Group & group);
	QFuture<bool> deleteGroup(const Group * group);
	QFuture<bool> reorderGroups(const QList<const Group *> & groups);
	QFuture<bool> changeGroupLanguages(const Group * group, const QSet<QLocale::Language> & languages);
	QFuture<bool> renameGroup(const Group * group, const QString & newName);

	[[nodiscard]] const Group * getActiveGroup() const;
	void setActiveGroup(const Group * group);

	[[nodiscard]] const Groupings & getGroupings() const;
	QFuture<bool> addDictionaryToGroup(const Dictionary * dictionary, const Group * group);
	QFuture<bool> removeDictionaryFromGroup(const Dictionary * dictionary, const Group * group);

	[[nodiscard]] const History & getHistory() const;
	QFuture<bool> updateHistory();
	QFuture<bool> clearHistory();

	[[nodiscard]] qsizetype getSizeSuggestions() const;
	QFuture<bool> setSizeSuggestions(qsizetype size);

	[[nodiscard]] qsizetype getSizeHistory() const;
	QFuture<bool> setSizeHistory(qsizetype size);

	[[nodiscard]] const Formats & getFormats() const;

	[[nodiscard]] QFuture<bool> createNgramIndex() const;
};

#endif // REMOTEREPOSITORY_H
