#ifndef HELPWND_H
#define HELPWND_H

#include <QWidget>

namespace Ui {
class HelpWnd;
}

class HelpWnd : public QWidget
{
    Q_OBJECT

public:
    explicit HelpWnd(QWidget *parent = nullptr);
    ~HelpWnd();

private:
    Ui::HelpWnd *ui;
};

#endif // HELPWND_H
