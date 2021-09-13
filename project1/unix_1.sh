
if [ $# -eq 0 ] #if no argument is given
then
	cat > input	#read from console till Ctrl-D is pressed and write to file input
	set input	#set the input as position parameter
fi

text=$(cat $1)		#assign content of file to text

for word in $text	#read text word by word 
	do
		#initializations
		upper=0
		lower=0
		digit=0
		other=0
		len=${#word} #Length of the string
		#loop to read and check char by char till length of the string
		for((i=0;i<$len;i++))	
		do
			char=${word:$i:1}	#take corresponding character from word
			if [[ "$char" =~ [A-Z] ]]; then	#if it is an uppercase letter increment upper variable
				upper=$((upper + 1)) 
			elif [[ "$char" =~ [a-z] ]]; then #if it is a lowercase letter increment lower variable
				lower=$((lower + 1)) 
			elif [[ "$char" =~ [0-9] ]]; then #if it is a digit increment digit variable
				digit=$((digit + 1)) 
			else				#for any other character increment other variable
				other=$((other+1))
			fi		
		done
		str=$str""$upper"."$lower"."$digit"."$other" "	#after finding corresponding values for all variables concatenate results in proper format for each word
	done
echo $str	#print result



