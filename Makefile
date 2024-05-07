#By default, cmocka prints human-readable test output to stderr. It is possible to configure several other output formats. The configuration is done using the CMOCKA_MESSAGE_OUTPUT environment variable. The supported values are:
#    STDOUT for the default standard output printer
#    SUBUNIT for subunit output
#    TAP for Test Anything Protocol (TAP) output
#    XML for JUnit XML format The case doesn't matter.

all: runtest

main.exe: main.c
	gcc main.c -o main.exe 
    
test.o: test.c
	gcc -c test.c -o test.o
    
testsuite.exe: test.o 
	gcc test.o -lcmocka -o testsuite.exe
    
runtest: testsuite.exe main.exe
	export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/local/lib" && ./testsuite.exe

clean:
	rm -rf *.o *.exe
