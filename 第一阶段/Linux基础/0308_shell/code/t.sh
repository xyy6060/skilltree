read n
i=2
while [ $i -lt $n ]
do
	 re=`expr $n % $i`
	 if [ $re -eq 0 ] ;then
		echo "no"
		break
	 else
		i=`expr $i + 1`
	 fi
done

if [ $i -eq $n ] ;then
	echo "yes"
fi
