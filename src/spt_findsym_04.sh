
f=`find . -name \*.cpp -o -name \*.c -o -name \*.h`

for file in $f 
do 

 echo "${file}"
 
 sed 's/InfoFieldEnter\.h/infofieldenter\.h/g' ${file} > tmpfile && mv tmpfile ${file}
 
done