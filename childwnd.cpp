#include "childwnd.h"
#include<QDebug>
#include<QFile>
#include<QFileDialog>
#include<QTextDocumentWriter>
#include<QTextBlockFormat>
#include<QTextListFormat>
#include<QTextList>
#include<QCloseEvent>
//新打开文件的标号
int ChildWnd::newFileIndex = 1;

ChildWnd::ChildWnd(QString dirFileName)
    :isSave(true)
    ,fileName_("")
    ,dirFileName_(dirFileName)
{

    //设置自动释放
    setAttribute(Qt::WA_DeleteOnClose);

    if(dirFileName_.isEmpty())
        return;
    //分割文件名
    splitFileName();
}

ChildWnd::~ChildWnd()
{
    qDebug()<<"del============";
}

QString ChildWnd::getFileName()
{
    return fileName_;
}

void ChildWnd::setFileName(QString name)
{
    fileName_ = name;
}

QString ChildWnd::getDirFileName()
{
    return dirFileName_;
}

void ChildWnd::setDirFileName(QString name)
{
    dirFileName_ = name;
}

void ChildWnd::splitFileName()
{
    fileName_ = QString(dirFileName_).split('/').back();
}


bool ChildWnd::loadFile()
{
    QFile file(dirFileName_);
    bool ret = file.open(QFile::ReadOnly);
    if(!ret)
        return false;
    QByteArray text = file.readAll();
    if(Qt::mightBeRichText(text))
        setHtml(text);
    else
        setPlainText(text);
    file.close();
    return true;
}

bool ChildWnd::saveFile()
{
    //判断是否为已经存在的文件
    if(dirFileName_.isEmpty()){
        //需要指引用户保存
        QString dirfilename = QFileDialog::getSaveFileName(this, "选择保存文件路径及名字","./"+getFileName());
        qDebug()<<dirfilename;

        if(dirfilename.isEmpty()){
            return false;
        }
        setDirFileName(dirfilename);
        splitFileName();
    }
    //可以直接保存
//    QTextDocument *doc = document();
//    QTextDocumentWriter wr(dirFileName_);
//    return wr.write(doc);

    QFile file(dirFileName_);
    file.open(QFile::Truncate|QFile::WriteOnly);
    bool ret = file.write(this->toPlainText().toUtf8());
    file.close();

    isSave = true;

    return  ret;
}

bool ChildWnd::saveAsFile()
{
    //需要指引用户保存
    QString dirfilename = QFileDialog::getSaveFileName(this, "选择保存文件路径及名字","./"+fileName_);
    if(dirfilename.isEmpty()){
        return false;
    }

//    QTextDocument *doc = document();
//    QTextDocumentWriter wr(dirfilename);
//    return wr.write(doc);

    QFile file(dirfilename);
    file.open(QFile::Truncate|QFile::WriteOnly);
    bool ret = file.write(this->toPlainText().toUtf8());
    file.close();

    isSave = true;

    return  ret;
}

//设置选中字体格式
void ChildWnd::setFormatOnSelectedWord(const QTextCharFormat &fmt)
{
    QTextCursor tcur = textCursor(); //光标
    if(!tcur.hasSelection())
//        tcur.select(QTextCursor::WordUnderCursor);
        return;
    tcur.mergeCharFormat(fmt);
    mergeCurrentCharFormat(fmt);
}

void ChildWnd::setAlignOfDocumentText(int aligntype)
{
    switch (aligntype) {
        case 1:
            setAlignment(Qt::AlignLeft|Qt::AlignAbsolute); break;
        case 2:
            setAlignment(Qt::AlignCenter|Qt::AlignAbsolute); break;
        case 3:
            setAlignment(Qt::AlignRight|Qt::AlignAbsolute); break;
        case 4:
            setAlignment(Qt::AlignJustify|Qt::AlignAbsolute); break;
        default:
            break;
    }
}

void ChildWnd::setParaStyle(int style)
{
    QTextCursor cur = textCursor();

    QTextListFormat::Style sname;
    switch (style) {
        case 1:
            sname = QTextListFormat::ListDisc; //黑实心圆
            break;
        case 2:
            sname = QTextListFormat::ListCircle; //黑空心圆
            break;
        case 3:
            sname = QTextListFormat::ListDecimal; //数字
            break;
        case 4:
            sname = QTextListFormat::ListLowerAlpha; //小写字母
            break;
        case 5:
            sname = QTextListFormat::ListUpperAlpha; //大写字母
            break;
        case 6:
            sname = QTextListFormat::ListLowerRoman; //罗马小写
            break;
        default:
            return;
    }
    cur.beginEditBlock();
    QTextBlockFormat tBfmt = cur.blockFormat();
    QTextListFormat tListFmt;
    if(cur.currentList()){
        tListFmt = cur.currentList()->format();
    }
    else{
        tListFmt.setIndent(tBfmt.indent()+1);
        tBfmt.setIndent(0);
        cur.setBlockFormat(tBfmt);
    }
    tListFmt.setStyle(sname);
    cur.createList(tListFmt);

    cur.endEditBlock();

}


