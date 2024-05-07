#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <setjmp.h>

#include <regex.h>
#include <cmocka.h>


#define UNUSED(x) (void)(x)
#define BUFFER_SIZE 256

/**
 * @brief runAppWithInput zal main.exe uitvoeren.
 *
 * De main.exe die gestart word krijgt @p input door via de stdin van main.exe
 * Alles wat main.exe terug stuurt via stdout komt terug via @p output
 * 
 * @param input Een string die wordt doorgegeven aan main.exe
 * @param output Een string die output van main.exe bevat. 
 * the caller moet deze array aanmaken en byref doorgeven.
 *
 * @return void
 * @note Er is niet nagekeken op race condities
 */
static void runAppWithInput(char * input, char * output){
    // Command to start
    const char* command = "./main.exe";

    // File pointers for pipes
    FILE *pipe_in, *pipe_out;
    char buffer[BUFFER_SIZE];

    // Open pipes for reading and writing
    int pipe_to_child[2], pipe_from_child[2];

    if (pipe(pipe_to_child) == -1 || pipe(pipe_from_child) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Child process
        // Close unused ends of pipes
        close(pipe_to_child[1]);
        close(pipe_from_child[0]);

        // Redirect stdin and stdout to pipes
        dup2(pipe_to_child[0], STDIN_FILENO);
        dup2(pipe_from_child[1], STDOUT_FILENO);

        // Execute the existing application
        execl(command, command, NULL);
        perror("execl"); // execl doesn't return unless there's an error
        exit(EXIT_FAILURE);
    } else { // Parent process
        // Close unused ends of pipes
        close(pipe_to_child[0]);
        close(pipe_from_child[1]);

        // Open pipes for reading and writing
        pipe_in = fdopen(pipe_from_child[0], "r");
        pipe_out = fdopen(pipe_to_child[1], "w");

        if (pipe_in == NULL || pipe_out == NULL) {
            perror("fdopen");
            exit(EXIT_FAILURE);
        }

        // Send data to the existing application
        fprintf(pipe_out, "%s", input);
        fflush(pipe_out);

        // Read data from the existing application
        fgets(output, BUFFER_SIZE, pipe_in);
        //printf("Received from existing application: %s", buffer);
        
        // Close pipes
        fclose(pipe_in);
        fclose(pipe_out);
    }  
}

/**
 * @brief regexFindsMatch returned true bij match.
 *
 * Deze functie zal in een gegeven @p string zoeken naar een match.
 * Naar wat gezocht word kan je instellen via @p regexString
 * 
 * @param string Een string waarin gezocht moet worden
 * @param regexString De reguliere expressie. 
 * 
 * @return true indien er een match is anders false
 * @note Als er iets fout is met de regexString stopt de applicatie
 */
static bool regexFindsMatch(char * string, char * regexString){
	regex_t    preg;
	int        rc;
	size_t     nmatch = 1;
	regmatch_t pmatch[1];
	bool retValue = false;
	
	if (0 != (rc = regcomp(&preg, regexString, REG_EXTENDED))) {
    	printf("regcomp() failed, returning nonzero (%d): %s\n", rc, regexString);
    	exit(EXIT_FAILURE);
   	}
   	
   	if (0 != (rc = regexec(&preg, string, nmatch, pmatch, 0))) {
   		//REGEX heeft niets gevonden
    	//printf("Failed to match '%s' with '%s',returning %d.\n", string, regexString, rc);
    	retValue = false;
	}else{
		//REGEX heeft een match
		//printf("Found match! \"%.*s\" is found at position %d to %d.\n",
        //     pmatch[0].rm_eo - pmatch[0].rm_so, &string[pmatch[0].rm_so],
        //     pmatch[0].rm_so, pmatch[0].rm_eo - 1);
		retValue = true;	  
    }
    regfree(&preg);
    return retValue;
}

static void checkPlusOperand(void **state)
{
    UNUSED(state); 
    char inputbuffer[BUFFER_SIZE] ="2.5+8.4\n";
    char outputbuffer[BUFFER_SIZE]; 
    //Opmerking: de opgave heeft geen enter in de output!
    char expectedoutput[] = "Enter an expression (without spaces!): 2.50 + 8.40 = 10.90";
	runAppWithInput(inputbuffer, outputbuffer);
	assert_string_equal(outputbuffer, expectedoutput); 
    //Opgelet de lijnen na een gefaalde assert worden niet uitgevoerd!
}

static void checkMinOperand(void **state)
{
    UNUSED(state); 
    
    char inputbuffer[BUFFER_SIZE] ="2.4-8.7\n";
    char outputbuffer[BUFFER_SIZE]; 
    //Opmerking: de opgave heeft geen enter in de output!
    char expectedoutput[] = "Enter an expression (without spaces!): 2.40 - 8.70 = -6.30";
   	char *pattern = "2\\.40? ?- ?8\\.70? ?= ?-6\\.30?";

	runAppWithInput(inputbuffer, outputbuffer);

	if (regexFindsMatch(outputbuffer, pattern)){
		assert_true(1); //test is gelukt
	}else{
		assert_string_equal(outputbuffer, expectedoutput); 
	}
}

static void checkMulOperand(void **state)
{
    UNUSED(state); 
    
    char inputbuffer[BUFFER_SIZE] ="-25.3*0.58\n";
    char outputbuffer[BUFFER_SIZE]; 
    //Opmerking: de opgave heeft geen enter in de output!
    char expectedoutput[] = "Enter an expression (without spaces!): -25.30 * 0.58 = -14.67";
   	char *pattern = "-25\\.30? ?\\* ?0\\.58 ?= ?-14\\.67";

	runAppWithInput(inputbuffer, outputbuffer);

	if (regexFindsMatch(outputbuffer, pattern)){
		assert_true(1); //test is gelukt
	}else{
		assert_string_equal(outputbuffer, expectedoutput); 
	}
}

static void checkDivOperand(void **state)
{
    UNUSED(state); 
    
    char inputbuffer[BUFFER_SIZE] ="245/16\n";
    char outputbuffer[BUFFER_SIZE]; 
    //Opmerking: de opgave heeft geen enter in de output!
    char expectedoutput[] = "Enter an expression (without spaces!): 245.00 / 16.00 = 15.31";
   	char *pattern = "245.?0?0? ?/ ?16.?0?0? ?= ?15.31";

	runAppWithInput(inputbuffer, outputbuffer);

	if (regexFindsMatch(outputbuffer, pattern)){
		assert_true(1); //test is gelukt
	}else{
		assert_string_equal(outputbuffer, expectedoutput); 
	}
}

static void checkNotAValidOperand(void **state)
{
    UNUSED(state); 
    
    char inputbuffer[BUFFER_SIZE] ="25%3\n";
    char outputbuffer[BUFFER_SIZE]; 
    //Opmerking: de opgave heeft geen enter in de output!
    char expectedoutput[] = "Enter an expression (without spaces!): Output: % is not a valid operand.";
   	char *pattern = "not a valid operand";

	runAppWithInput(inputbuffer, outputbuffer);

	if (regexFindsMatch(outputbuffer, pattern)){
		assert_true(1); //test is gelukt
	}else{
		assert_string_equal(outputbuffer, expectedoutput); 
	}
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(checkPlusOperand),
        cmocka_unit_test(checkMinOperand),
        cmocka_unit_test(checkMulOperand),
        cmocka_unit_test(checkDivOperand),
        cmocka_unit_test(checkNotAValidOperand),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
