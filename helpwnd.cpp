#include "helpwnd.h"
#include "ui_helpwnd.h"
#include<QDebug>
HelpWnd::HelpWnd(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HelpWnd)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
}

HelpWnd::~HelpWnd()
{
//    qDebug()<<"help del....";
    delete ui;
}
