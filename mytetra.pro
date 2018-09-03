TEMPLATE = subdirs
 
SUBDIRS = app mimetex

app.file = app/app.pro
app.depends = mimetex

mimetex.file = thirdParty/mimetex/mimetex.pro

