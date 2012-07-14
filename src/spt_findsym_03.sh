
f=`find . -maxdepth 1 -name \*.cpp -o -name \*.c -o -name \*.h`

for file in $f 
do 

 echo "${file}"
 
 sed 's/include "editor/include "editor\/editor/g' ${file} > tmpfile && mv tmpfile ${file}
 
done