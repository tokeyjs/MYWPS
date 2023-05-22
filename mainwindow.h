#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"childwnd.h"
#include<QtPrintSupport/QPrinter>
#include<QtPrintSupport/QPrintDialog>
#include<QtPrintSupport/QPrintPreviewDialog>
#include<QCloseEvent>
#include "helpwnd.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //初始化
    void initMainWindow();

protected:
    //关闭wps事件
    void closeEvent(QCloseEvent *event);



private:
    //新建文件
    void addNewFile();
    //打开已经存在的文件
    void loadFile();
    //保存当前文件
    void saveFile();
    //另存为
    void saveAsFile();

    void docUndo();
    void docRedo();
    void docCut();
    void docCopy();
    void docPaste();

    void docPrint(); //打印
    void docPrintPreview(); //打印预览

    void textBold(); //加粗
    void textItalic(); //倾斜
    void textUnderline(); //下划线

    void textFamily(const QString &family);
    void textSize(const QString &ps);
    void textColor();

    void textParaStyle(int stytle);

private slots:
    //关闭tab
    void closeTab(int index);

    void on_newAction_triggered();

    void on_saveAction_triggered();

    void on_nextAction_triggered();

    void on_previousAction_triggered();

    void on_openAction_triggered();

    void on_saveAsAction_triggered();

    void on_undoAction_triggered();

    void on_redoAction_triggered();

    void on_cutAction_triggered();

    void on_copyAction_triggered();

    void on_pasteAction_triggered();

    void on_boldAction_triggered();

    void on_italicAction_triggered();

    void on_underLineAction_triggered();

    void on_fontComboBox_currentFontChanged(const QFont &f);

    void on_sizeComboBox_currentIndexChanged(const QString &arg1);

    void on_sizeComboBox_activated(const QString &arg1);

    void on_leftAlineAction_triggered();

    void on_centerAction_triggered();

    void on_rightAlineAction_triggered();

    void on_justifyAction_triggered();

    void on_colorAction_triggered();

    void on_comboBox_activated(int index);

    void on_printAction_triggered();

    void printPreview(QPrinter* ptr);
    void on_printPreviewAction_triggered();

    void on_closeAction_triggered();

    void on_closeAllAction_triggered();

    void on_exitAction_triggered();

    void on_aboutAction_triggered();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
