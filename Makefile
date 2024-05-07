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
    
cmocka.o: cmocka/cmocka.c
	gcc cmocka.c -o cmocka.o -Icmocka
	
testsuite.exe: test.o cmocka.o
	gcc test.o cmocka.o -o testsuite.exe
    
runtest: testsuite.exe main.exe
	./testsuite.exe

clean:
	rm -rf *.o *.exe
