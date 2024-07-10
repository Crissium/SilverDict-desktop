#ifndef ABOUTDIALOGUE_H
#define ABOUTDIALOGUE_H

#include <QDialog>

namespace Ui
{
class AboutDialogue;
}

class AboutDialogue : public QDialog
{
	Q_OBJECT

private:
	Ui::AboutDialogue * ui;

public:
	explicit AboutDialogue(QWidget * parent = nullptr);
	~AboutDialogue() override;
};

#endif // ABOUTDIALOGUE_H
