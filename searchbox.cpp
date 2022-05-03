#include "searchbox.h"

#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QGridLayout>
#include <QTextEdit>
#include <QMessageBox>

SearchBox::SearchBox(QWidget *parent) : QDialog(parent), lastStart(0), index(0), previousQuery("")
{
    setGeometry(300, 300, 360, 250);
    setWindowTitle("Find and Replace...");
    setModal(false);

    QPushButton *findNextButton = new QPushButton("Find Next", this);
    connect(findNextButton, &QPushButton::clicked, this, &SearchBox::on_FindNextButton_Clicked);

    QPushButton *findPrevButton = new QPushButton("Find Previous", this);
    connect(findPrevButton, &QPushButton::clicked, this, &SearchBox::on_FindPrevButton_Clicked);

    QPushButton *replaceButton = new QPushButton("Replace", this);
    connect(replaceButton, &QPushButton::clicked, this, &SearchBox::on_ReplaceButton_Clicked);

    QPushButton *replaceAllButton = new QPushButton("Replace All", this);
    connect(replaceAllButton, &QPushButton::clicked, this, &SearchBox::on_ReplaceAllButton_Clicked);

    isCaseSensitive = new QCheckBox("Case Sensitive", this);
    isCaseSensitive->setChecked(true);

    findField = new QLineEdit(this);
    findField->resize(250, 50);
    findField->setFocus();

    replaceField = new QLineEdit(this);
    replaceField->resize(250, 50);

    QGridLayout *gl = new QGridLayout();
    gl->addWidget(findField, 1, 0, 1, 4);
    gl->addWidget(isCaseSensitive, 2, 0);
    gl->addWidget(findPrevButton, 3, 2, 1, 2);
    gl->addWidget(findNextButton, 3, 0, 1, 2);

    gl->addWidget(replaceField, 4, 0, 1, 4);
    gl->addWidget(replaceButton, 5, 0, 1, 2);
    gl->addWidget(replaceAllButton, 5, 2, 1, 2);

    setLayout(gl);
}

void SearchBox::findText()
{
    QTextEdit *editor = qobject_cast<QTextEdit*>(parent());
    Q_ASSERT(editor);

    std::string text = editor->toPlainText().toStdString();
    std::string queryText = findField->text().toStdString();
    previousQuery = findField->text();

    if (queryText == "") return;

    if (!isCaseSensitive->isChecked())
    {
        std::for_each(text.begin(), text.end(), [](char &c) {
                c = ::tolower(c);
            });

        std::for_each(queryText.begin(), queryText.end(), [](char &c) {
                c = ::tolower(c);
            });
    }

    while (lastStart != -1)
    {
        lastStart = (int)text.find(queryText, lastStart == 0 ? lastStart : lastStart + 1);

        if (lastStart != -1)
        {
            int end = lastStart + (int)queryText.length();
            results.push_back({lastStart, end});
            lastStart = end;
        }
    }

    lastStart = 0;
}

void SearchBox::replaceText()
{
    QTextEdit *editor = qobject_cast<QTextEdit*>(parent());
    Q_ASSERT(editor);

    QTextCursor cs = editor->textCursor();
    if (cs.hasSelection())
    {
        cs.insertText(replaceField->text());
        editor->setTextCursor(cs);
    }
}

void SearchBox::on_FindNextButton_Clicked()
{
    if (previousQuery != findField->text())
    {
        findText();
        if (results.size() == 0) {previousQuery = ""; return;}
        index = 0;
        highlightText(results[index].first, results[index].second);
        emit shouldSetFocus();
        return;
    }

    index++;
    if (index >= results.size())
    {
        index = 0;
        highlightText(results[index].first, results[index].second);
        emit shouldSetFocus();
        return;
    }

    highlightText(results[index].first, results[index].second);
    emit shouldSetFocus();
}

void SearchBox::on_FindPrevButton_Clicked()
{
    if (index == 0)
    {
        index = results.size()-1;
        highlightText(results[index].first, results[index].second);
        emit shouldSetFocus();
        return;
    }

    index--;
    highlightText(results[index].first, results[index].second);
    emit shouldSetFocus();
}

void SearchBox::on_ReplaceButton_Clicked()
{
    replaceText();
    previousQuery = "";
    results.clear();
    index = 0;
}

void SearchBox::on_ReplaceAllButton_Clicked()
{
    if (results.empty()) findText();
    if (results.empty())
        QMessageBox::information(this, "Failed", "No occurrence found");

    int oc = 0;
    foreach (auto &i, results)
    {
        highlightText(i.first, i.second);
        replaceText();
        oc++;
    }

    QMessageBox::information(this, "Success", "Replaced " + QString::number(oc) + " occurrence(s)");
    previousQuery = "";
    results.clear();
    index = 0;
}

void SearchBox::highlightText(int start, int end)
{
    QTextEdit *editor = qobject_cast<QTextEdit*>(parent());
    Q_ASSERT(editor);

    QTextCursor cs = editor->textCursor();
    cs.setPosition(start);

    cs.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, end - start);

    editor->setTextCursor(cs);
}
