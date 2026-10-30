#include "finddialog.h"
#include "ui_finddialog.h"
#include <QMessageBox>

FindDialog::FindDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindDialog)
{
    ui->setupUi(this);
    adjustSize();
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

FindDialog::~FindDialog()
{
    delete ui;
}

QString FindDialog::searchText() const
{
    return ui->lineEditFind->text();
}

bool FindDialog::isCaseSensitive() const
{
    return ui->checkBoxCase->isChecked();
}

bool FindDialog::isForward() const
{
    return ui->radioDown->isChecked();
}

void FindDialog::on_btnFindNext_clicked()
{
    QString text = ui->lineEditFind->text();
    if (text.isEmpty()) {
        QMessageBox::information(this, tr("提示"), tr("查找内容不能为空！"));
        return;
    }
    emit findNext(text, isCaseSensitive(), isForward());
}

void FindDialog::on_btnCancel_clicked()
{
    close();
}
