#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>
#include <QLabel>
#include <QSysInfo>
#include <QTextBlock>
#include <searchbox.h>
#include <QDockWidget>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isSaved(true)
{
    ui->setupUi(this);
    setCentralWidget(ui->textEdit);

    ui->actionStatus_Bar->setChecked(true);
    lineInfo = new QLabel("Ln " + QString::number(currentLine) + " Col " + QString::number(currentColumn));
    lineInfo->setAlignment(Qt::AlignRight);
    statusBar()->addWidget(lineInfo, 30);

    zoomAmount = new QLabel(QString::number(zoomPercentage) + "%         ");
    zoomAmount->setAlignment(Qt::AlignRight);
    statusBar()->addWidget(zoomAmount, 6);

    QString osName = QSysInfo::kernelType();
    osName[0] = osName[0].toUpper();
    QString osInfo = osName + "(" + QSysInfo::buildCpuArchitecture() + ")";
    QLabel *stat = new QLabel(osInfo + "         ");
    stat->setAlignment(Qt::AlignRight);
    statusBar()->addWidget(stat, 1);

    QLabel *encoding = new QLabel("UTF-8                  ");
    encoding->setAlignment(Qt::AlignRight);
    statusBar()->addWidget(encoding, 0);

    ui->actionSave->setEnabled(false);
    setWindowTitle("Untitled-1");

    font = QFont();
    font.setPointSize(defaultFontSize);
    ui->textEdit->setFont(font);
    connect(ui->textEdit, &QTextEdit::textChanged, this, &MainWindow::on_TextEdit_changed);
    connect(ui->textEdit, &QTextEdit::cursorPositionChanged, this, &MainWindow::on_TextEdit_cursorPositionChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_TextEdit_cursorPositionChanged()
{
    QTextEdit *te = qobject_cast<QTextEdit*>(sender());
    Q_ASSERT(te);
    QTextCursor cursor = te->textCursor();
    currentColumn = cursor.columnNumber() + 1;

    cursor.movePosition(QTextCursor::StartOfLine);
    int lines = 1;
    while(cursor.positionInBlock()>0)
    {
        cursor.movePosition(QTextCursor::Up);
        lines++;
    }

    QTextBlock block = cursor.block().previous();

    while(block.isValid())
    {
        lines += block.lineCount();
        block = block.previous();
    }

    currentLine = lines;
    lineInfo->setText("Ln " + QString::number(currentLine) + " Col " + QString::number(currentColumn));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!isSaved)
    {
        QMessageBox::StandardButton box = QMessageBox::question(this, "Quit anyway?",
                                                                "You have a unsaved file. Quit anyway?",
                                                                QMessageBox::Yes | QMessageBox::No);
        if (box == QMessageBox::Yes)
        {
            QApplication::quit();
        }

        if (box == QMessageBox::No)
        {
            return;
        }
    }

    else
    {
        QApplication::quit();
    }
}

void MainWindow::on_TextEdit_changed()
{
    if (isSaved)
    {
        isSaved = false;
        if (!windowTitle().endsWith("*")) setWindowTitle(windowTitle() + "*");
    }
}

void MainWindow::on_actionNew_triggered()
{
    currentFile.clear();
    ui->textEdit->setText(QString());

    ui->actionSave->setEnabled(false);
    isSaved = false;
    setWindowTitle("Untitled-1*");
}


void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open a file");
    currentFile = fileName;
    QFile file(currentFile);
    if (!file.open(QIODevice::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Error", "Failed to open file");
        return;
    }

    QFileInfo info(currentFile);
    setWindowTitle(info.fileName());
    QTextStream inputStream(&file);
    ui->textEdit->setText(inputStream.readAll());

    isSaved = true;
    ui->actionSave->setEnabled(true);
    file.close();
}


void MainWindow::on_actionSave_triggered()
{
    QFile file(currentFile);
    if (!file.open(QIODevice::WriteOnly | QFile::Text | QFile::Truncate))
    {
        QMessageBox::warning(this, "Error", "Failed to save file");
        return;
    }

    QTextStream outputStream(&file);
    QString text = ui->textEdit->toPlainText();

    outputStream << text;
    isSaved = true;

    if (windowTitle().endsWith("*"))
    {
        QString newTitle = windowTitle();
        newTitle.truncate(windowTitle().length()-1);
        setWindowTitle(newTitle);
    }

    file.close();
}

void MainWindow::on_actionSave_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Select a Location");
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QFile::Text | QFile::Truncate))
    {
        QMessageBox::warning(this, "Error", "Failed to save file");
        return;
    }

    currentFile = fileName;
    QFileInfo info(currentFile);
    setWindowTitle(info.fileName());

    QTextStream outputStream(&file);
    QString text = ui->textEdit->toPlainText();
    outputStream << text;

    isSaved = true;
    ui->actionSave->setEnabled(true);

    if (windowTitle().endsWith("*"))
    {
        QString newTitle = windowTitle();
        newTitle.truncate(windowTitle().length()-1);
        setWindowTitle(newTitle);
    }

    file.close();
}

void MainWindow::on_actionPrint_triggered()
{
    QPrinter printer;
    printer.setPrinterName("Print");
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Rejected)
    {
        QMessageBox::warning(this, "Failed", "Failed to print");
        return;
    }

    ui->textEdit->print(&printer);
}

void MainWindow::on_actionExit_triggered()
{
    if (!isSaved)
    {
        QMessageBox::StandardButton box = QMessageBox::question(this, "Quit anyway?",
                                                                "You have a unsaved file. Quit anyway?",
                                                                QMessageBox::Yes | QMessageBox::No);
        if (box == QMessageBox::Yes)
        {
            QApplication::quit();
        }

        if (box == QMessageBox::No)
        {
            return;
        }
    }

    else
    {
        QApplication::quit();
    }
}


void MainWindow::on_actionCopy_triggered()
{
    ui->textEdit->copy();
}


void MainWindow::on_actionPaste_triggered()
{
    ui->textEdit->paste();
}


void MainWindow::on_actionCut_triggered()
{
    ui->textEdit->cut();
}


void MainWindow::on_actionUndo_triggered()
{
    ui->textEdit->undo();
}


void MainWindow::on_actionRedo_triggered()
{
    ui->textEdit->redo();
}


void MainWindow::on_actionSelect_All_triggered()
{
    ui->textEdit->selectAll();
}

void MainWindow::on_actionZoom_In_triggered()
{
    font.setPointSize(font.pointSize() + 3);
    zoomPercentage += 10;
    zoomAmount->setText(QString::number(zoomPercentage) + "%         ");

    ui->textEdit->setFont(font);
}

void MainWindow::on_actionZoom_Out_triggered()
{
    if (font.pointSize() <= 3) return;
    font.setPointSize(font.pointSize() - 3);
    zoomPercentage -= 10;
    zoomAmount->setText(QString::number(zoomPercentage) + "%         ");

    ui->textEdit->setFont(font);
}

void MainWindow::on_actionRestore_Default_triggered()
{
    font.setPointSize(defaultFontSize);
    zoomPercentage = 100;
    zoomAmount->setText(QString::number(zoomPercentage) + "%         ");
    ui->textEdit->setFont(font);
}

void MainWindow::on_actionSearch_triggered()
{
    SearchBox *box = new SearchBox(ui->textEdit);
    connect(box, &SearchBox::shouldSetFocus, this, &MainWindow::on_textFound);
    box->show();
}

void MainWindow::on_textFound()
{
    activateWindow();
    raise();
}

void MainWindow::on_actionStatus_Bar_toggled(bool arg1)
{
    arg1 ? statusBar()->show() : statusBar()->hide();
}
