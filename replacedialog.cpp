#include "replacedialog.h"
#include "ui_replacedialog.h"
#include <QMessageBox>

ReplaceDialog::ReplaceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReplaceDialog)
{
    ui->setupUi(this);
    adjustSize();
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

ReplaceDialog::~ReplaceDialog()
{
    delete ui;
}

QString ReplaceDialog::findText() const
{
    return ui->lineEditFind->text();
}

QString ReplaceDialog::replaceText() const
{
    return ui->lineEditReplace->text();
}

bool ReplaceDialog::isCaseSensitive() const
{
    return ui->checkBoxCase->isChecked();
}

bool ReplaceDialog::isForward() const
{
    return ui->radioDown->isChecked();
}

void ReplaceDialog::on_btnFindNext_clicked()
{
    QString text = ui->lineEditFind->text();
    if (text.isEmpty()) {
        QMessageBox::information(this, tr("提示"), tr("查找内容不能为空！"));
        return;
    }
    emit findNext(text, isCaseSensitive(), isForward());
}

void ReplaceDialog::on_btnReplace_clicked()
{
    QString fText = ui->lineEditFind->text();
    if (fText.isEmpty()) {
        QMessageBox::information(this, tr("提示"), tr("查找内容不能为空！"));
        return;
    }
    emit replaceOne(fText, ui->lineEditReplace->text(), isCaseSensitive(), isForward());
}

void ReplaceDialog::on_btnReplaceAll_clicked()
{
    QString fText = ui->lineEditFind->text();
    if (fText.isEmpty()) {
        QMessageBox::information(this, tr("提示"), tr("查找内容不能为空！"));
        return;
    }
    emit replaceAll(fText, ui->lineEditReplace->text(), isCaseSensitive());
}

void ReplaceDialog::on_btnCancel_clicked()
{
    close();
}
