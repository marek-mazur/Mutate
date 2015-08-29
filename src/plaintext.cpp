#include "plaintext.h"
#include "setting.h"
#include "keyboard_layout.h"
#include "config_parse.h"
#include <iostream>
#include <QTextEdit>
#include <QProcess>
#include <QClipboard>
#include <fstream>
#include <memory>

using namespace std;

PlainText::PlainText(const char *file, QWidget *parent):
    QPlainTextEdit(parent),
    fparse(file)
{
    try
    {
        auto config = new ConfigParse(CONFPATH);
        this->keyboardLayout = new KeyboardLayout(config);
    }
    catch(...)
    {
        this->keyboardLayout = new KeyboardLayout(NULL);
    }
}
PlainText::~PlainText()
{
    delete this->keyboardLayout;
}

void PlainText::enterCurrentRow()
{
    enterListWidget(listWidget->currentRow());
}


void PlainText::enterListWidget(int row)
{
    if (row < listWidget->count() && row >= 0)
    {
        if ((*val)[row].icon == INTERNETICON)
        try
        {
            std::shared_ptr<ConfigParse> C(new ConfigParse(URLPATH));
            auto tmp = C->getSections();
            if (std::find(tmp.begin(), tmp.end(), (*val)[row].text) == tmp.end()){
                std::ofstream out(URLPATH, std::ios::app);
                out << '[' << (*val)[row].text << ']' << std::endl;
            }
        }catch(...)
        {
            std::ofstream out(URLPATH, std::ios::app);
            out << '[' << (*val)[row].text << ']' << std::endl;
        }
        if ((*val)[row].command == "copy")
        {
            QClipboard* clipboard = QGuiApplication::clipboard();
            this->parent->hide();
            clipboard->setText(QString::fromStdString((*val)[row].text), QClipboard::Clipboard);
            clipboard->setText(QString::fromStdString((*val)[row].text), QClipboard::Selection);
            return;
        }
        if ((*val)[row].command == "setting")
        {
            Setting * s = new Setting;
            s->show();
            return;
        }
        if ((*val)[row].command == "complete")
        {
            int pos = (*val)[row].text.size();
            this->setPlainText(QString::fromStdString((*val)[row].text + ' '));
            QTextCursor t(this->document());
            t.setPosition(pos + 1);
            this->setTextCursor(t);
            return;
        }

        if ((*val)[row].command == "miss")
        {
            return;
        }

        if ((*val)[row].command == "quit")
        {
            (*aptr).quit();
            return;
        }

        this->parent->hide();
        if (!(*val)[row].command.empty())
        {
            QProcess::startDetached(QString::fromStdString((*val)[row].command));
            fparse.addValue((*val)[row].text, 1);
            fparse.storeValue();
        }
    }

}

void PlainText::keyPressEvent(QKeyEvent* event)
{
    Qt::KeyboardModifiers modifiers = event->modifiers();
    int uKey = event->key();
    Qt::Key key = static_cast<Qt::Key>(uKey);
    if(modifiers & Qt::AltModifier) {
        if (key >= Qt::Key_1 && key <= Qt::Key_1 + std::min(this->listWidget->count(), MAXPRINTSIZE) - 1)
        {
            this->enterListWidget(key - Qt::Key_1);
            return;
        }
    }
    if (key == 16777220 || key == Qt::Key_Enter)
    {
        enterListWidget(listWidget->currentRow());
        return;
    }
    if (listWidget->count() > 0)
    {
        bool emacsLayout = this->keyboardLayout->layout() == KeyboardLayout::Emacs;
        if (key == Qt::Key_Up || key == Qt::Key_Backtab ||
            (emacsLayout && (modifiers & Qt::ControlModifier) && key == Qt::Key_P))
        {
            listWidget->setCurrentRow((listWidget->currentRow() + listWidget->count() - 1) % listWidget->count());
            return;
        }
        if (key == Qt::Key_Down || key == Qt::Key_Tab ||
            (emacsLayout && (modifiers & Qt::ControlModifier) && key == Qt::Key_N))
        {
            listWidget->setCurrentRow((listWidget->currentRow() + 1) % listWidget->count());
            return;
        }
    }
    if (this->handleKey(key, modifiers))
    {
        return;
    }
    QPlainTextEdit::keyPressEvent(event);
}

bool PlainText::handleKey(Qt::Key key, Qt::KeyboardModifiers modifiers)
{
    if (this->keyboardLayout->layout() == KeyboardLayout::Default)
    {
        if ((modifiers & Qt::AltModifier) || (modifiers & Qt::MetaModifier))
        {
            return true;
        }
        if (modifiers & Qt::ControlModifier)
        {
            if (key == Qt::Key_C || key == Qt::Key_V || key == Qt::Key_A || key == Qt::Key_X)
            {
                return false;
            }
            return true;
        }
        return false;
    }

    // emacs like shortcuts
    QClipboard* clipboard = QGuiApplication::clipboard();
    if (modifiers & Qt::ControlModifier)
    {
        switch (key)
        {
            case Qt::Key_H:
                this->textCursor().deletePreviousChar();
                break;
            case Qt::Key_D:
                this->textCursor().deleteChar();
                break;
            case Qt::Key_B:
                this->moveCursor(QTextCursor::Left);
                break;
            case Qt::Key_F:
                this->moveCursor(QTextCursor::Right);
                break;
            case Qt::Key_A:
                this->moveCursor(QTextCursor::StartOfLine);
                break;
            case Qt::Key_E:
                this->moveCursor(QTextCursor::EndOfLine);
                break;
            case Qt::Key_K:
                this->moveCursor(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
                clipboard->setText(this->textCursor().selectedText());
                this->textCursor().removeSelectedText();
                break;
            case Qt::Key_Y:
                this->textCursor().insertText(clipboard->text());
                break;
            default:
                break;
        }
        return true;
    }
    else if (modifiers & Qt::AltModifier)
    {
        switch (key)
        {
            case Qt::Key_B:
                this->moveCursor(QTextCursor::WordLeft);
                break;
            case Qt::Key_F:
                this->moveCursor(QTextCursor::WordRight);
                break;
            case Qt::Key_D:
                this->moveCursor(QTextCursor::WordRight, QTextCursor::KeepAnchor);
                clipboard->setText(this->textCursor().selectedText());
                this->textCursor().removeSelectedText();
                break;
            case Qt::Key_H:
                this->moveCursor(QTextCursor::WordLeft, QTextCursor::KeepAnchor);
                clipboard->setText(this->textCursor().selectedText());
                this->textCursor().removeSelectedText();
            default:
                break;
        }
        return true;

    }
    else if (modifiers & Qt::MetaModifier)
    {
        return true;
    }
    return false;
}


void PlainText::focusOutEvent(QFocusEvent*)
{
    //this->parent->hide();
}

void PlainText::setApp(SingleApplication* app)
{
    aptr = app;
}
