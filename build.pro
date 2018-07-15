TEMPLATE = subdirs
 
SUBDIRS = mimetex app

app.file = app/mytetra.pro
app.depends = mimetex

mimetex.file = thirdParty/mimetex/mimetex.pro

