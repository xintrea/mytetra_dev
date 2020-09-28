TEMPLATE = subdirs

RC_ICONS = app/bin/resource/pic/icon.ico

# For desktop
!android {
SUBDIRS = app mimetex

app.file = app/app.pro
app.depends = mimetex

mimetex.file = thirdParty/mimetex/mimetex.pro
}

# For Android
android {
SUBDIRS = app

app.file = app/app.pro
}

