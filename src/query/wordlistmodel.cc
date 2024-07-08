#include "wordlistmodel.h"

#include <QBrush>

WordListModel::WordListModel(QObject * parent)
	: QAbstractListModel(parent)
	, lastSuggestionUpdateTime(QDateTime::currentDateTime())
	// , highlightedIndex(0)
	, displayingHistory(true)
{
}

int WordListModel::rowCount(const QModelIndex & parent) const
{
	// For list models only the root node (an invalid parent) should return the list's size. For all
	// other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
	if (parent.isValid())
		return 0;

	return words.size();
}

QVariant WordListModel::data(const QModelIndex & index, int role) const
{
	const int i = index.row();
	if (!index.isValid() || i >= words.size() || i < 0)
		return {};

	if (role == Qt::DisplayRole)
	{
		return words[i];
	}
	else
	{
		return {};
	}
}

bool WordListModel::isDisplayingHistory() const
{
	return displayingHistory;
}

void WordListModel::setWords(const History & history)
{
	beginResetModel();
	words = history;
	// highlightedIndex = 0;
	displayingHistory = true;
	endResetModel();
}

void WordListModel::setWords(const Suggestions & suggestions)
{
	if (suggestions.timestamp > lastSuggestionUpdateTime)
	{
		beginResetModel();
		words = suggestions.wordList;
		// highlightedIndex = 0;
		displayingHistory = false;
		endResetModel();
	}
}

// void WordListModel::setHighlight(int i)
// {
// 	if (highlightedIndex == i)
// 		return;
//
// 	highlightedIndex = i;
// 	emit dataChanged(index(i), index(i), {Qt::FontRole});
// }
