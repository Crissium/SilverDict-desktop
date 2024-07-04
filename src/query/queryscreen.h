#ifndef QUERYSCREEN_H
#define QUERYSCREEN_H

#include <QWidget>

namespace Ui {
class QueryScreen;
}

class QueryScreen : public QWidget
{
    Q_OBJECT

public:
    explicit QueryScreen(QWidget *parent = nullptr);
    ~QueryScreen() override;

private:
    Ui::QueryScreen *ui;
};

#endif // QUERYSCREEN_H
