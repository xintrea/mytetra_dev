#include <QString>

#include "HtmlHelper.h"

HtmlHelper::HtmlHelper()
{

}


QString HtmlHelper::htmlSpecialChars(QString line)
{
  line.replace("\"", "&quot;");
  line.replace("<",  "&lt;");
  line.replace(">",  "&gt;");

  return line;
}


QString HtmlHelper::htmlSpecialCharsDecode(QString line)
{
  line.replace("&quot;", "\"");
  line.replace("&lt;", "<");
  line.replace("&gt;", ">");

  return line;
}
