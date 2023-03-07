cd ..\..\
rm -fr buildlinux




check_return(){
	if [ $1 -ne 0 ]
	then
		echo "failed............................."
		exit 1
	fi
}

cmake -S . -B buildlinux  --trace-expand
check_return $?

make
check_return $?


