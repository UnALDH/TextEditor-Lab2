#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "finddialog.h"
#include "replacedialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QFontDialog>
#include <QColorDialog>
#include <QClipboard>
#include <QMimeData>
#include <QTimer>
#include <QDateTime>
#include <QTextBlock>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentFilePath("")
    , isUntitled(true)
    , statusLabel(nullptr)
    , autoSaveTimer(nullptr)
    , findDlg(nullptr)
    , replaceDlg(nullptr)
{
    ui->setupUi(this);

    // 设置初始窗口大小
    resize(900, 650);

    initStatusBar();
    initConnections();

    setCurrentFile("");

    // 自动保存定时器（选做功能：每隔 60 秒检查并自动保存）
    autoSaveTimer = new QTimer(this);
    connect(autoSaveTimer, &QTimer::timeout, this, &MainWindow::onAutoSaveTimerTimeout);
    autoSaveTimer->start(60000); // 60s
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initStatusBar()
{
    statusLabel = new QLabel(this);
    statusLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    statusLabel->setText("length: 0   lines: 1    Ln: 1   Col: 1    赖俊达");
    ui->statusBar->addPermanentWidget(statusLabel);
}

void MainWindow::initConnections()
{
    // 内容变化与光标位置变化
    connect(ui->codeEditor->document(), &QTextDocument::modificationChanged,
            this, &MainWindow::documentWasModified);
    connect(ui->codeEditor->document(), &QTextDocument::contentsChanged,
            this, &MainWindow::documentWasModified);
    connect(ui->codeEditor, &QPlainTextEdit::cursorPositionChanged,
            this, &MainWindow::updateStatusBar);

    // Action 状态联动
    connect(ui->codeEditor, &QPlainTextEdit::copyAvailable,
            ui->actionCut, &QAction::setEnabled);
    connect(ui->codeEditor, &QPlainTextEdit::copyAvailable,
            ui->actionCopy, &QAction::setEnabled);
    connect(ui->codeEditor, &QPlainTextEdit::undoAvailable,
            ui->actionUndo, &QAction::setEnabled);
    connect(ui->codeEditor, &QPlainTextEdit::redoAvailable,
            ui->actionRedo, &QAction::setEnabled);

    // 剪贴板监听
    connect(QApplication::clipboard(), &QClipboard::dataChanged,
            this, &MainWindow::updateClipboardState);

    // 初始 Action 使能状态
    ui->actionUndo->setEnabled(false);
    ui->actionRedo->setEnabled(false);
    ui->actionCut->setEnabled(false);
    ui->actionCopy->setEnabled(false);
    updateClipboardState();
}

void MainWindow::updateClipboardState()
{
    const QMimeData *mimeData = QApplication::clipboard()->mimeData();
    bool hasText = mimeData && mimeData->hasText();
    ui->actionPaste->setEnabled(hasText);
}

void MainWindow::documentWasModified()
{
    QString shownName = isUntitled ? tr("新建文本文件") : QFileInfo(currentFilePath).fileName();
    if (ui->codeEditor->document()->isModified()) {
        setWindowTitle(QString("* %1 - 编辑器").arg(shownName));
    } else {
        setWindowTitle(QString("%1 - 编辑器").arg(shownName));
    }
    updateStatusBar();
}

void MainWindow::updateStatusBar()
{
    QTextCursor cursor = ui->codeEditor->textCursor();
    int lineNum = cursor.blockNumber() + 1;
    int colNum = cursor.columnNumber() + 1;
    int totalLength = ui->codeEditor->toPlainText().length();
    int totalLines = ui->codeEditor->blockCount();

    QString statusText = QString("length: %1   lines: %2    Ln: %3   Col: %4    赖俊达")
                             .arg(totalLength)
                             .arg(totalLines)
                             .arg(lineNum)
                             .arg(colNum);
    statusLabel->setText(statusText);
}

bool MainWindow::maybeSave()
{
    if (!ui->codeEditor->document()->isModified())
        return true;

    QString docName = isUntitled ? tr("新建文本文件") : QFileInfo(currentFilePath).fileName();
    QMessageBox::StandardButton ret = QMessageBox::warning(
        this,
        tr("简单文本编辑器"),
        tr("文件 \"%1\" 已被修改。\n是否保存更改？").arg(docName),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save
    );

    if (ret == QMessageBox::Save) {
        return on_actionSave_triggered();
    } else if (ret == QMessageBox::Cancel) {
        return false;
    }
    return true;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    currentFilePath = fileName;
    isUntitled = fileName.isEmpty();
    ui->codeEditor->document()->setModified(false);

    QString shownName = isUntitled ? tr("新建文本文件") : QFileInfo(currentFilePath).fileName();
    setWindowTitle(QString("%1 - 编辑器").arg(shownName));
    updateStatusBar();
}

void MainWindow::on_actionNew_triggered()
{
    if (maybeSave()) {
        ui->codeEditor->clear();
        setCurrentFile("");
    }
}

void MainWindow::on_actionOpen_triggered()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(
            this,
            tr("打开文件"),
            QString(),
            tr("文本文件 (*.txt);;所有文件 (*.*)")
        );
        if (!fileName.isEmpty()) {
            loadFile(fileName);
        }
    }
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.exists()) {
        QMessageBox::critical(this, tr("错误"), tr("文件不存在: %1").arg(fileName));
        return;
    }

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::critical(this, tr("错误"), tr("无法读取文件 %1:\n%2.").arg(fileName, file.errorString()));
        return;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");
    ui->codeEditor->setPlainText(in.readAll());
    file.close();

    setCurrentFile(fileName);
    ui->statusBar->showMessage(tr("文件已成功打开"), 2000);
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::critical(this, tr("错误"), tr("无法写入文件 %1:\n%2.").arg(fileName, file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << ui->codeEditor->toPlainText();
    file.close();

    setCurrentFile(fileName);
    ui->statusBar->showMessage(tr("文件已保存"), 2000);
    return true;
}

bool MainWindow::on_actionSave_triggered()
{
    if (isUntitled) {
        return on_actionSaveAs_triggered();
    } else {
        return saveFile(currentFilePath);
    }
}

bool MainWindow::on_actionSaveAs_triggered()
{
    QString initialPath = isUntitled ? "新建文本文件.txt" : currentFilePath;
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("另存为"),
        initialPath,
        tr("文本文件 (*.txt);;所有文件 (*.*)")
    );
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionUndo_triggered()
{
    ui->codeEditor->undo();
}

void MainWindow::on_actionRedo_triggered()
{
    ui->codeEditor->redo();
}

void MainWindow::on_actionCut_triggered()
{
    ui->codeEditor->cut();
}

void MainWindow::on_actionCopy_triggered()
{
    ui->codeEditor->copy();
}

void MainWindow::on_actionPaste_triggered()
{
    ui->codeEditor->paste();
}

void MainWindow::on_actionSelectAll_triggered()
{
    ui->codeEditor->selectAll();
}

void MainWindow::on_actionWrap_triggered(bool checked)
{
    if (checked) {
        ui->codeEditor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    } else {
        ui->codeEditor->setLineWrapMode(QPlainTextEdit::NoWrap);
    }
}

void MainWindow::on_actionLineNumber_triggered(bool checked)
{
    ui->codeEditor->setLineNumberVisible(checked);
}

void MainWindow::on_actionFont_triggered()
{
    bool ok = false;
    QFont font = QFontDialog::getFont(&ok, ui->codeEditor->font(), this, tr("选择字体"));
    if (ok) {
        ui->codeEditor->setFont(font);
    }
}

void MainWindow::on_actionFontColor_triggered()
{
    QColor color = QColorDialog::getColor(Qt::black, this, tr("选择字体颜色"));
    if (color.isValid()) {
        QPalette p = ui->codeEditor->palette();
        p.setColor(QPalette::Text, color);
        ui->codeEditor->setPalette(p);
    }
}

void MainWindow::on_actionFontBgColor_triggered()
{
    QColor color = QColorDialog::getColor(Qt::yellow, this, tr("选择字体背景色"));
    if (color.isValid()) {
        QTextCursor cursor = ui->codeEditor->textCursor();
        if (cursor.hasSelection()) {
            QTextCharFormat format;
            format.setBackground(color);
            cursor.mergeCharFormat(format);
        } else {
            QPalette p = ui->codeEditor->palette();
            p.setColor(QPalette::Highlight, color);
            ui->codeEditor->setPalette(p);
        }
    }
}

void MainWindow::on_actionEditorBgColor_triggered()
{
    QColor color = QColorDialog::getColor(Qt::white, this, tr("选择编辑器背景色"));
    if (color.isValid()) {
        QPalette p = ui->codeEditor->palette();
        p.setColor(QPalette::Base, color);
        ui->codeEditor->setPalette(p);
    }
}

void MainWindow::on_actionToolBar_triggered(bool checked)
{
    ui->mainToolBar->setVisible(checked);
}

void MainWindow::on_actionStatusBar_triggered(bool checked)
{
    ui->statusBar->setVisible(checked);
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dlg(this);
    dlg.exec();
}

void MainWindow::on_actionFind_triggered()
{
    if (!findDlg) {
        findDlg = new FindDialog(this);
        connect(findDlg, &FindDialog::findNext, this, &MainWindow::doFind);
    }
    findDlg->show();
    findDlg->raise();
    findDlg->activateWindow();
}

void MainWindow::doFind(const QString &text, bool caseSensitive, bool forward)
{
    if (text.isEmpty()) {
        QMessageBox::information(this, tr("提示"), tr("查找内容不能为空！"));
        return;
    }

    QTextDocument::FindFlags flags;
    if (!forward)
        flags |= QTextDocument::FindBackward;
    if (caseSensitive)
        flags |= QTextDocument::FindCaseSensitively;

    QTextCursor origCursor = ui->codeEditor->textCursor();
    bool found = ui->codeEditor->find(text, flags);
    if (!found) {
        // 尝试从文档两端循环查找
        QTextCursor cursor = origCursor;
        if (forward) {
            cursor.movePosition(QTextCursor::Start);
        } else {
            cursor.movePosition(QTextCursor::End);
        }
        ui->codeEditor->setTextCursor(cursor);

        found = ui->codeEditor->find(text, flags);
        if (!found) {
            // 恢复原光标位置，防止光标丢失
            ui->codeEditor->setTextCursor(origCursor);
            QMessageBox::information(this, tr("查找"), tr("找不到 \"%1\"").arg(text));
        }
    }
}

void MainWindow::on_actionReplace_triggered()
{
    if (!replaceDlg) {
        replaceDlg = new ReplaceDialog(this);
        connect(replaceDlg, &ReplaceDialog::findNext, this, &MainWindow::doFind);
        connect(replaceDlg, &ReplaceDialog::replaceOne, this, &MainWindow::doReplaceOne);
        connect(replaceDlg, &ReplaceDialog::replaceAll, this, &MainWindow::doReplaceAll);
    }
    replaceDlg->show();
    replaceDlg->raise();
    replaceDlg->activateWindow();
}

void MainWindow::doReplaceOne(const QString &findText, const QString &replaceText, bool caseSensitive, bool forward)
{
    if (findText.isEmpty()) {
        QMessageBox::information(this, tr("提示"), tr("查找内容不能为空！"));
        return;
    }
    QTextCursor cursor = ui->codeEditor->textCursor();
    QString selected = cursor.selectedText();

    Qt::CaseSensitivity cs = caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
    if (!selected.isEmpty() && selected.compare(findText, cs) == 0) {
        cursor.insertText(replaceText);
        ui->codeEditor->setTextCursor(cursor);
    }
    doFind(findText, caseSensitive, forward);
}

void MainWindow::doReplaceAll(const QString &findText, const QString &replaceText, bool caseSensitive)
{
    if (findText.isEmpty()) {
        QMessageBox::information(this, tr("提示"), tr("查找内容不能为空！"));
        return;
    }
    QTextCursor cursor = ui->codeEditor->textCursor();
    cursor.movePosition(QTextCursor::Start);
    ui->codeEditor->setTextCursor(cursor);

    QTextDocument::FindFlags flags;
    if (caseSensitive)
        flags |= QTextDocument::FindCaseSensitively;

    int count = 0;
    while (ui->codeEditor->find(findText, flags)) {
        QTextCursor c = ui->codeEditor->textCursor();
        c.insertText(replaceText);
        ui->codeEditor->setTextCursor(c);
        ++count;
    }

    QMessageBox::information(this, tr("替换"), tr("全部替换完成，共替换了 %1 处。").arg(count));
}

void MainWindow::onAutoSaveTimerTimeout()
{
    // 定时自动保存（选做功能）
    if (!isUntitled && ui->codeEditor->document()->isModified()) {
        if (saveFile(currentFilePath)) {
            QString timeStr = QDateTime::currentDateTime().toString("HH:mm:ss");
            ui->statusBar->showMessage(tr("已定时自动保存 (%1)").arg(timeStr), 3000);
        }
    }
}
