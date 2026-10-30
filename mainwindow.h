#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QLabel;
class QTimer;
class AboutDialog;
class FindDialog;
class ReplaceDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // 文件操作
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    bool on_actionSave_triggered();
    bool on_actionSaveAs_triggered();
    void on_actionExit_triggered();

    // 编辑操作
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    void on_actionCut_triggered();
    void on_actionCopy_triggered();
    void on_actionPaste_triggered();
    void on_actionSelectAll_triggered();

    // 格式操作
    void on_actionWrap_triggered(bool checked);
    void on_actionLineNumber_triggered(bool checked);
    void on_actionFont_triggered();
    void on_actionFontColor_triggered();
    void on_actionFontBgColor_triggered();
    void on_actionEditorBgColor_triggered();

    // 查看操作
    void on_actionToolBar_triggered(bool checked);
    void on_actionStatusBar_triggered(bool checked);

    // 查找与替换
    void on_actionFind_triggered();
    void on_actionReplace_triggered();
    void doFind(const QString &text, bool caseSensitive, bool forward);
    void doReplaceOne(const QString &findText, const QString &replaceText, bool caseSensitive, bool forward);
    void doReplaceAll(const QString &findText, const QString &replaceText, bool caseSensitive);

    // 帮助
    void on_actionAbout_triggered();

    // 状态更新与辅助槽
    void documentWasModified();
    void updateStatusBar();
    void updateClipboardState();
    void onAutoSaveTimerTimeout();

private:
    void initStatusBar();
    void initConnections();
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);

private:
    Ui::MainWindow *ui;
    QString currentFilePath;
    bool isUntitled;

    QLabel *statusLabel;
    QTimer *autoSaveTimer;

    FindDialog *findDlg;
    ReplaceDialog *replaceDlg;
};

#endif // MAINWINDOW_H
