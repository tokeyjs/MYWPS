#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QFontComboBox>
#include<QFont>
#include<QComboBox>
#include<QPixmap>
#include<QDebug>
#include<QFileDialog>
#include<QFile>
#include<QTextDocumentWriter>
#include<QColorDialog>
#include<QMessageBox>
#include<QtPrintSupport/QPrinter>
#include<QtPrintSupport/QPrintDialog>
#include<QtPrintSupport/QPrintPreviewDialog>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->clear();
    ui->tabWidget->setTabsClosable(true);
    initMainWindow();
    //设置tab标签可以关闭
    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));

    //设置标题与图标
    setWindowTitle("WPS");
    QPixmap pic;
    pic.load(":/images/wps.png");
    this->setWindowIcon(pic);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initMainWindow()
{
    //初始化字号下拉框
    QFontDatabase fontdb;
    foreach(int fontsize, fontdb.standardSizes())
        ui->sizeComboBox->addItem(QString::number(fontsize));

    QFont defFont = QApplication::font(); //当前默认字体
    int defFontSize = defFont.pointSize(); //当前字体字号
    QString sFontSize = QString::number(defFontSize);
    int defFontIndex = ui->sizeComboBox->findText(sFontSize); //查找当前字号在框中的索引
    ui->sizeComboBox->setCurrentIndex(defFontIndex);

    //保证对齐方式的互斥
    QActionGroup *alignGroup = new QActionGroup(this);
    alignGroup->addAction(ui->leftAlineAction);
    alignGroup->addAction(ui->rightAlineAction);
    alignGroup->addAction(ui->centerAction);
    alignGroup->addAction(ui->justifyAction);

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //查看是否有未保存的tab
    if(ui->tabWidget->count()==0)
        event->accept();

    for (int i=0; i<ui->tabWidget->count();i++) {
        ChildWnd* cwnd = static_cast<ChildWnd*>(ui->tabWidget->widget(i));
        if(!cwnd->isSave){
            QMessageBox::StandardButton btn = QMessageBox::warning(this, "警告", "当前窗口有内容未保存，建议先保存后关闭，是否继续退出？", QMessageBox::StandardButton::Yes|QMessageBox::StandardButton::No, QMessageBox::StandardButton::No);
            if(btn == QMessageBox::StandardButton::Yes){
                break;
            }else{
                event->ignore();
                return;
            }
        }
    }

    event->accept();
}

//添加文件
void MainWindow::addNewFile()
{
    //tab上的名字
    QString fileName = QString("new_%1").arg(ChildWnd::newFileIndex++);
    ChildWnd *cwnd = new ChildWnd();
    cwnd->setFileName(fileName);

    //加入tab
    ui->tabWidget->addTab(cwnd, fileName);

    //焦点设置
    ui->tabWidget->setCurrentWidget(cwnd);
    //设置tab标题的‘*’
    connect(cwnd->document() ,&QTextDocument::contentsChanged, this, [&](){
        //更新tab名称
        if(ui->tabWidget->tabText(ui->tabWidget->currentIndex()).back()!='*'){
            ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), ui->tabWidget->tabText(ui->tabWidget->currentIndex())+"*");
            static_cast<ChildWnd*>(ui->tabWidget->currentWidget())->isSave = false;
        }
    });

}

void MainWindow::loadFile()
{
    //打开一个文件
    QString filename = QFileDialog::getOpenFileName(this, "打开一个文件", "./");
    if(!filename.isEmpty()){
        ChildWnd *cwnd = new ChildWnd(filename);
        //加入tab
        ui->tabWidget->addTab(cwnd, cwnd->getFileName());
        //焦点设置
        ui->tabWidget->setCurrentWidget(cwnd);
        bool ret = cwnd->loadFile();
        if(!ret){
            //load err
            qDebug()<<"load err";
            ui->tabWidget->removeTab(ui->tabWidget->currentIndex());
            return ;
        }
        //设置tab标题的‘*’
        connect(cwnd->document() ,&QTextDocument::contentsChanged, this, [=](){
            //更新tab名称
            ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), cwnd->getFileName()+"*");
            static_cast<ChildWnd*>(ui->tabWidget->currentWidget())->isSave = false;
        });
    }

}

void MainWindow::saveFile()
{
    if(ui->tabWidget->count()==0)
        return;
    //保存文件
    ChildWnd* cwnd = static_cast<ChildWnd*>(ui->tabWidget->currentWidget());
    bool ret = cwnd->saveFile();
    if(ret){
        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), cwnd->getFileName());
        cwnd->isSave = true;
    }
    else{
        qDebug()<<"save error";
    }

}

void MainWindow::saveAsFile()
{
    if(ui->tabWidget->count()==0)
        return;
    //保存文件
    ChildWnd* cwnd = static_cast<ChildWnd*>(ui->tabWidget->currentWidget());
    bool ret = cwnd->saveAsFile();
    if(ret)
        qDebug()<<"save as ok";
    else
        qDebug()<<"save as error";
}

void MainWindow::docUndo()
{
    if(ui->tabWidget->count()==0)
        return;
    ChildWnd* cwnd = static_cast<ChildWnd*>(ui->tabWidget->currentWidget());
    cwnd->undo();
}

void MainWindow::docRedo()
{
    if(ui->tabWidget->count()==0)
        return;
    ChildWnd* cwnd = static_cast<ChildWnd*>(ui->tabWidget->currentWidget());
    cwnd->redo();
}

void MainWindow::docCut()
{
    if(ui->tabWidget->count()==0)
        return;
    ChildWnd* cwnd = static_cast<ChildWnd*>(ui->tabWidget->currentWidget());
    cwnd->cut();
}

void MainWindow::docCopy()
{
    if(ui->tabWidget->count()==0)
        return;
    ChildWnd* cwnd = static_cast<ChildWnd*>(ui->tabWidget->currentWidget());
    cwnd->copy();
}

void MainWindow::docPaste()
{
    if(ui->tabWidget->count()==0)
        return;
    ChildWnd* cwnd = static_cast<ChildWnd*>(ui->tabWidget->currentWidget());
    cwnd->paste();
}

void MainWindow::docPrint()
{
    if(ui->tabWidget->count()==0)
        return;
    QPrinter pter(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&pter, this);
    dlg->setOption(QAbstractPrintDialog::PrintSelection, true);
    dlg->setWindowTitle("打印文档");

    ChildWnd* cwnd = static_cast<ChildWnd*>(ui->tabWidget->currentWidget());
    if(dlg->exec() == QDialog::Accepted){
        cwnd->print(&pter);
    }
    delete dlg;
}

void MainWindow::docPrintPreview()
{
    QPrinter pter;
    QPrintPreviewDialog preview(&pter, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)),this, SLOT(printPreview(QPrinter*)));
    preview.exec();
}

void MainWindow::textBold()
{
    if(ui->tabWidget->count()==0)
        return;
    QTextCharFormat fmt;
    fmt.setFontWeight(ui->boldAction->isChecked()?QFont::Bold : QFont::Normal);

    ChildWnd* cwnd = static_cast<ChildWnd*>(ui->tabWidget->currentWidget());
    cwnd->setFormatOnSelectedWord(fmt);
}

void MainWindow::textItalic()
{
    if(ui->tabWidget->count()==0)
        return;
    QTextCharFormat fmt;
    fmt.setFontItalic(ui->italicAction->isChecked());

    ChildWnd* cwnd = static_cast<ChildWnd*>(ui->tabWidget->currentWidget());
    cwnd->setFormatOnSelectedWord(fmt);
}

void MainWindow::textUnderline()
{
    if(ui->tabWidget->count()==0)
        return;
    QTextCharFormat fmt;
    fmt.setFontUnderline(ui->underLineAction->isChecked());
    ChildWnd* cwnd = static_cast<ChildWnd*>(ui->tabWidget->currentWidget());
    cwnd->setFormatOnSelectedWord(fmt);
}

void MainWindow::textFamily(const QString &family)
{
    if(ui->tabWidget->count()==0)
        return;
    QTextCharFormat fmt;
    fmt.setFontFamily(family);
    ChildWnd* cwnd = static_cast<ChildWnd*>(ui->tabWidget->currentWidget());
    cwnd->setFormatOnSelectedWord(fmt);
}

void MainWindow::textSize(const QString &ps)
{
    if(ui->tabWidget->count()==0)
        return;
    QTextCharFormat fmt;
    fmt.setFontPointSize(ps.toInt());
    ChildWnd* cwnd = static_cast<ChildWnd*>(ui->tabWidget->currentWidget());
    cwnd->setFormatOnSelectedWord(fmt);
}

void MainWindow::textColor()
{
    if(ui->tabWidget->count()==0)
        return;
    ChildWnd* cwnd = static_cast<ChildWnd*>(ui->tabWidget->currentWidget());
    QColor col = QColorDialog::getColor(cwnd->textColor(),this, "选择文本颜色");
    if(!col.isValid())
        return;

    QTextCharFormat fmt;
    fmt.setForeground(col);
    cwnd->setFormatOnSelectedWord(fmt);

    QPixmap pix(16,16);
    pix.fill(col);
    ui->colorAction->setIcon(pix);
}

void MainWindow::textParaStyle(int stytle)
{
    if(ui->tabWidget->count()==0)
        return;
    ChildWnd* cwnd = static_cast<ChildWnd*>(ui->tabWidget->currentWidget());
    cwnd->setParaStyle(stytle);
}

//关闭tab
void MainWindow::closeTab(int index)
{
    //看文件是否保存

    //未保存
    ChildWnd *cwnd =static_cast<ChildWnd*>(ui->tabWidget->widget(index));
    if(!cwnd->isSave){
        QMessageBox::StandardButton btn = QMessageBox::warning(this, "警告", "当前内容未保存，建议先保存后关闭，是否继续关闭当前文件？", QMessageBox::StandardButton::Yes|QMessageBox::StandardButton::No, QMessageBox::StandardButton::No);
        if(btn == QMessageBox::StandardButton::Yes){
            ui->tabWidget->widget(index)->close();
            ui->tabWidget->removeTab(index);
        }else if(btn == QMessageBox::StandardButton::No){
            return;
        }else{

        }

        return;
    }
    //已保存
    ui->tabWidget->widget(index)->close();
    ui->tabWidget->removeTab(index);

}


void MainWindow::on_newAction_triggered()
{
    this->addNewFile();
}

void MainWindow::on_saveAction_triggered()
{
    this->saveFile();

//    //更新tab名称
//    QString tabName = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
//    qDebug()<<tabName;

//    if(tabName.back()=='*'){
//        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), tabName.remove('*'));
//    }
}

void MainWindow::on_nextAction_triggered()
{
    //下一个tab
    int num = ui->tabWidget->count();
    if(num!=0){
        int index = ui->tabWidget->currentIndex();
        if(index<num-1){
            ui->tabWidget->setCurrentIndex(index+1);
        }
    }
}

void MainWindow::on_previousAction_triggered()
{
    //前一个tab
    int num = ui->tabWidget->count();
    if(num!=0){
        int index = ui->tabWidget->currentIndex();
        if(index>0){
            ui->tabWidget->setCurrentIndex(index-1);
        }
    }
}

void MainWindow::on_openAction_triggered()
{
    loadFile();
}

//另存为
void MainWindow::on_saveAsAction_triggered()
{
    saveAsFile();
}

void MainWindow::on_undoAction_triggered()
{
    docUndo();
}

void MainWindow::on_redoAction_triggered()
{
    docRedo();
}

void MainWindow::on_cutAction_triggered()
{
    docCut();
}

void MainWindow::on_copyAction_triggered()
{
    docCopy();
}

void MainWindow::on_pasteAction_triggered()
{
    docPaste();
}

void MainWindow::on_boldAction_triggered()
{
    textBold();
}

void MainWindow::on_italicAction_triggered()
{
    textItalic();
}

void MainWindow::on_underLineAction_triggered()
{
    textUnderline();
}

//选择字体家族
void MainWindow::on_fontComboBox_currentFontChanged(const QFont &f)
{
    textFamily(f.family());
}

void MainWindow::on_sizeComboBox_currentIndexChanged(const QString &arg1)
{}

//设置字体大小
void MainWindow::on_sizeComboBox_activated(const QString &arg1)
{
    textSize(arg1);
}

void MainWindow::on_leftAlineAction_triggered()
{
    //左对齐
    if(ui->tabWidget->count()==0)
        return;
    ChildWnd* cwnd = static_cast<ChildWnd*>(ui->tabWidget->currentWidget());
    cwnd->setAlignOfDocumentText(1);
}

void MainWindow::on_centerAction_triggered()
{
    //中对齐
    if(ui->tabWidget->count()==0)
        return;
    ChildWnd* cwnd = static_cast<ChildWnd*>(ui->tabWidget->currentWidget());
    cwnd->setAlignOfDocumentText(2);
}

void MainWindow::on_rightAlineAction_triggered()
{
    //右对齐
    if(ui->tabWidget->count()==0)
        return;
    ChildWnd* cwnd = static_cast<ChildWnd*>(ui->tabWidget->currentWidget());
    cwnd->setAlignOfDocumentText(3);
}

void MainWindow::on_justifyAction_triggered()
{
    //两端对齐
    if(ui->tabWidget->count()==0)
        return;
    ChildWnd* cwnd = static_cast<ChildWnd*>(ui->tabWidget->currentWidget());
    cwnd->setAlignOfDocumentText(4);
}

void MainWindow::on_colorAction_triggered()
{
    textColor();
}

void MainWindow::on_comboBox_activated(int index)
{
    textParaStyle(index);
}

void MainWindow::on_printAction_triggered()
{
    docPrint();
}

void MainWindow::printPreview(QPrinter *ptr)
{
    if(ui->tabWidget->count()==0)
        return;
    ChildWnd* cwnd = static_cast<ChildWnd*>(ui->tabWidget->currentWidget());
    cwnd->print(ptr);
}

void MainWindow::on_printPreviewAction_triggered()
{
    docPrintPreview();
}

void MainWindow::on_closeAction_triggered()
{
    //关闭当前tab
    if(ui->tabWidget->count()==0)
        return;
    ui->tabWidget->currentWidget()->close();
}

void MainWindow::on_closeAllAction_triggered()
{
    //关闭所有tab
    if(ui->tabWidget->count()==0)
        return;
    ui->tabWidget->clear();
}

void MainWindow::on_exitAction_triggered()
{
    this->close();
}

void MainWindow::on_aboutAction_triggered()
{
    HelpWnd *hp = new HelpWnd();
    hp->setWindowTitle("MYWPS 关于");
    hp->show();
}
