#ifndef WORDLISTMODEL_H
#define WORDLISTMODEL_H

#include "../remote/models.h"

#include <QAbstractListModel>

class WordListModel : public QAbstractListModel
{
	Q_OBJECT
private:
	QStringList words;
	QDateTime lastSuggestionUpdateTime;
	// int highlightedIndex;
	bool displayingHistory;

public:
	explicit WordListModel(QObject * parent = nullptr);

	[[nodiscard]] int rowCount(const QModelIndex & parent = QModelIndex()) const override;

	[[nodiscard]] QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

	[[nodiscard]] bool isDisplayingHistory() const;
	void setWords(const History & history); // Search term cleared, or a history item clicked
	void setWords(const Suggestions & suggestions); // Search term entered or changed
	// void setHighlight(int i); // A suggestion clicked
};

#endif // WORDLISTMODEL_H
