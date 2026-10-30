#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H

#include <QDialog>

namespace Ui {
class ReplaceDialog;
}

class ReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReplaceDialog(QWidget *parent = nullptr);
    ~ReplaceDialog();

    QString findText() const;
    QString replaceText() const;
    bool isCaseSensitive() const;
    bool isForward() const;

signals:
    void findNext(const QString &findText, bool caseSensitive, bool forward);
    void replaceOne(const QString &findText, const QString &replaceText, bool caseSensitive, bool forward);
    void replaceAll(const QString &findText, const QString &replaceText, bool caseSensitive);

private slots:
    void on_btnFindNext_clicked();
    void on_btnReplace_clicked();
    void on_btnReplaceAll_clicked();
    void on_btnCancel_clicked();

private:
    Ui::ReplaceDialog *ui;
};

#endif // REPLACEDIALOG_H
