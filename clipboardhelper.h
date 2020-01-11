#ifndef CLIPBOARDHELPER_H
#define CLIPBOARDHELPER_H

#include <QObject>

class ClipboardHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
public:
    explicit ClipboardHelper(QObject *parent = nullptr);

    QString text() const;
    void setText(const QString &text);

signals:
    void textChanged();

};

#endif // CLIPBOARDHELPER_H
