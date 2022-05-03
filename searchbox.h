#ifndef SEARCHBOX_H
#define SEARCHBOX_H

#include <QDialog>

class QCheckBox;
class QLineEdit;
class QTextEdit;
class SearchBox : public QDialog
{
    Q_OBJECT
public:
    explicit SearchBox(QWidget *parent = nullptr);

private:
    int lastStart;
    int index;
    QVector<QPair<int, int>> results;
    QString previousQuery;
    QCheckBox *isCaseSensitive;
    QLineEdit *findField;
    QLineEdit *replaceField;
    QTextEdit *editor;

    void highlightText(int start, int end);
    void findText();
    void replaceText();

private slots:
    void on_FindNextButton_Clicked();
    void on_FindPrevButton_Clicked();
    void on_ReplaceButton_Clicked();
    void on_ReplaceAllButton_Clicked();

signals:
    void shouldSetFocus();
};

#endif // SEARCHBOX_H
