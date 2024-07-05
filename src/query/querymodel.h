#ifndef QUERYMODEL_H
#define QUERYMODEL_H

#include <QObject>

class QueryModel : public QObject
{
	Q_OBJECT
public:
	explicit QueryModel(QObject * parent = nullptr);
};

#endif // QUERYMODEL_H
