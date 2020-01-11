#include "clipboardhelper.h"

#include <QClipboard>
#include <QGuiApplication>

ClipboardHelper::ClipboardHelper(QObject *parent) : QObject(parent)
{

}

QString ClipboardHelper::text() const
{
    return QGuiApplication::clipboard()->text();
}

void ClipboardHelper::setText(const QString &text)
{
    QGuiApplication::clipboard()->setText(text);
    emit textChanged();
}
