# find -name '*.cpp' -exec cat {} | 
# cat copyright.txt > cat alco.cpp > alco.cpp
# cat alco.cpp | sed '1rcopyright.txt' > alco.cpp
# file="alco.cpp"

countsym()
{
 count=`echo $1 | wc -m`
 c=`expr ${count} - 1`
 echo ${c}

 mod=`expr ${c} % 2`
 if [ ${mod} = 1 ]
 then
  echo "Warning! In file $1 found uncorrect symbol"
 fi 
}


f=`find . -name \*.cpp -or -name \*.c -or -name \*.h`

for file in $f 
do 

# filtfile=`cat ${file} | grep \" | sed -e 's/[^"]//g' | tr -d [:space:]`
# countsym ${filtfile}
 
 filtfile=`cat ${file} | grep \( | sed -e 's/[^\(]//g' | tr -d [:space:]`
 countsym ${filtfile}
 
done