#!/bin/sh

f=`find . -name \*.cpp -o -name \*.c -o -name \*.h`
# f="replace_test.cpp"

for file in $f 
do 

 echo "${file}"

 # sed 's/InfoFieldEnter\.h/infofieldenter\.h/g' ${file} > tmpfile && mv tmpfile ${file}
 # sed 's/resource\/pic\/.*\.svg/replace/g' ${file} > tmpfile && mv tmpfile ${file}

 # ssed --regexp-perl -i 's/DPF( *)\(( *)\((.*?)( *)\)( *)\)/logprint\(\3\)/g' $file
 ssed --regexp-perl -i 's/\"(resource\/pic\/.+?\.svg)\"/\":\/\1\"/g' $file

 
done