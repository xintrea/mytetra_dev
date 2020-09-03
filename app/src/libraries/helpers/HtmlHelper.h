#ifndef HTMLHELPER_H
#define HTMLHELPER_H

class QString;


class HtmlHelper
{
public:
    HtmlHelper();

    static QString htmlSpecialChars(QString line);
    static QString htmlSpecialCharsDecode(QString line);

};

#endif // HTMLHELPER_H
