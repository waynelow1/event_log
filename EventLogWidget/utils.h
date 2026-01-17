#ifndef UTILS_H
#define UTILS_H

#include <QString>

static QString csvEscape(const QString& value)
{
    QString escaped = value;
    escaped.replace("\"", "\"\"");   // escape quotes
    return "\"" + escaped + "\"";    // always quote
}

#endif // UTILS_H
