#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int mag_niet=1;	//AST error Error_Global_vars_used

void stoute_functie(int getal){
	while(1){	//AST error Error_while_loop_uses_constant_in_condition 
		if(getal==0){
			break;
		}
		getal--;
	}
	while(1==getal){	//NO AST error !
		if(getal==0){
			break;
		}
		getal--;
	}
}

int main(void)
{
    float num1, num2, res;
    char operand;

    printf("Enter an expression (without spaces!): ");
    scanf("%f%c%f%*c", &num1, &operand, &num2);

    switch (operand)
    {
    case '+':
    {
        res = num1 + num2;
        printf("%.2f %c %.2f = %.2f", num1, operand, num2, res);
        break;
    }
    case '-':
    {
        res = num1 - num2;
        printf("%.2f %c %.2f = %.2f", num1, operand, num2, res);
        break;
    }
    case '*':
    {
        res = num1 * num2;
        printf("%.2f %c %.2f = %.2f", num1, operand, num2, res);
        break;
    }
    case '/':
    {
        res = num1 / num2;
        printf("%.2f %c %.2f = %.2f", num1, operand, num2, res);
        break;
    }
    default:
        printf("%c is not a valid operand.", operand);
        break;
    }

    return 0;
}

