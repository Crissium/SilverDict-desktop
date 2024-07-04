#include "queryscreen.h"
#include "ui_queryscreen.h"

QueryScreen::QueryScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QueryScreen)
{
    ui->setupUi(this);
}

QueryScreen::~QueryScreen()
{
    delete ui;
}
