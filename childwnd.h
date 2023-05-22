#ifndef CHILDWND_H
#define CHILDWND_H
#include<QTextEdit>

//子窗口类
class ChildWnd : public QTextEdit
{
    Q_OBJECT
public:
    ChildWnd(QString dirFileName = "");
    ~ChildWnd();

    QString getFileName();
    void setFileName(QString name);

    QString getDirFileName();
    void setDirFileName(QString name);


    void splitFileName(); //从路径全称中提取文件名

    static int newFileIndex;


    //打开已经存在的文件
    bool loadFile();
    //保存当前文件
    bool saveFile();
    //另存为
    bool saveAsFile();


    //设置选中文本的字体格式
    void setFormatOnSelectedWord(const QTextCharFormat &fmt);
    void setAlignOfDocumentText(int aligntype); //对齐方式

    //设置项目符号
    void setParaStyle(int style);
    bool isSave; //当前是否保存
private:

    QString fileName_; //当前文件名字
    QString dirFileName_; //路径及名字全称

protected:
//窗口关闭事件



};


#endif // CHILDWND_H
