#include "utils.h"
#include <QFile>
#include <QFile>
#include <QTextDocument>
#include <QCoreApplication>

QString Utils::getOrigin()
{
    return "https://cppdiscord.com";
}

QString Utils::getFrontendPath()
{
    QString path = QCoreApplication::applicationDirPath() + "/frontend";
    return path;
}

QString Utils::markdownToHtml(QString file)
{
    QFile mdFile(file);
    if (mdFile.open(QIODevice::ReadOnly))
    {
        QByteArray md = mdFile.readAll();
        mdFile.close();

        QTextDocument document;
        document.setMarkdown(QString::fromUtf8(md));
        
        return document.toHtml();
    }
    return "";
}