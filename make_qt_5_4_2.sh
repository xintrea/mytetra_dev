export QTDIR=/opt/Qt_5_4_2/5.4/gcc
 
LD_LIBRARY_PATH=$QTDIR:$QTDIR/lib:$QTDIR/bin:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH

# PATH=/usr/local/Trolltech/Qt-4.7.1/include:$PATH
PATH=/opt/Qt_5_4_2/5.4/gcc/bin:/usr/bin:/usr/local/bin:/usr/bin:/bin:/usr/games:/usr/lib/opera:/usr/lib/opera/plugins
export PATH

env

rm -f ./bin/mytetra

#/opt/qt_4_5_3/bin/qmake mytetra.pro
qmake mytetra.pro

# colormake -j 4
# make -j 4
make