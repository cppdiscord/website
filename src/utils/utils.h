
#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QSettings>

class Utils
{
public:
    static QString getOrigin();
    static QString getFrontendPath();
    static QString markdownToHtml(QString file);
};

#endif // UTILS_H