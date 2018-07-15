TEMPLATE = subdirs
 
SUBDIRS = mimetex app

app.file = app/app.pro
app.depends = mimetex

mimetex.file = thirdParty/mimetex/mimetex.pro

