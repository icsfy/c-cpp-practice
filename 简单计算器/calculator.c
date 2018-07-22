#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_SIZE 1024

double num_stack[MAX_SIZE];
int num_top = -1;

char op_stack[MAX_SIZE];
int op_top = -1;

void push_num(double num)
{
	num_stack[num_top++] = num;
	//printf("push num: %f\n", num_stack[num_top - 1]);
}

int pop_num(double *num)
{
	if (num_top == -1)
		return -1;
	*num = num_stack[--num_top];
	//printf("pop num: %f\n", *num);
	return 0;
}

void push_op(char op)
{
	op_stack[op_top++] = op;
	//printf("push op: %c\n", op_stack[op_top - 1]);
}

int pop_op(char *op)
{
	if (op_top == -1)
		return -1;
	*op = op_stack[--op_top];
	//printf("pop op: %c\n", *op);
	return 0;
}

void cal_top_two()
{
	char op;
	double num1, num2, val = 0;
	pop_num(&num1);
	pop_num(&num2);
	pop_op(&op);

	//printf("cal_two: %f %c %f\n", num2, op, num1);

	switch (op) {
	case '+':
		val = num2 + num1;
		break;
	case '-':
		val = num2 - num1;
		break;
	case '*':
		val = num2 * num1;
		break;
	case '/':
		val = num2 / num1;
		break;
	default:
		fprintf(stderr, "unsupported operator: %c\nwill exit...", op);
		getchar();
		exit(EXIT_FAILURE);
		break;
	}
	push_num(val);
}

int main()
{
	char expr[MAX_SIZE];
	char numStr[MAX_SIZE];
	char *temp, *p;

	printf("--- a simple calculator ---\n"
		"example:\n"
		">>(1.2+3.4)*(5.6+7.8)\n");

LABEL_START:
	printf(">>");
	fgets(expr, MAX_SIZE, stdin);
	p = expr;

	while (*p != '\0')
	{
		temp = numStr;
		while (isdigit(*p) || isblank(*p) || *p == '.') {
			*temp = *p;
			p++;
			temp++;
		}

		if (*p != '(' && *(temp - 1) != '\0') {
			*temp = '\0';
			push_num(atof(numStr));
		}

		while (1) {
			char top_op;
			if (pop_op(&top_op) != -1) {
				if (((top_op == '-' || top_op == '+') && (*p == '*' || *p == '/')) ||
					*p == '(' || (top_op == '(' && *p != ')')) {
					push_op(top_op);
					push_op(*p);
					break;
				} else if (top_op == '(' && *p == ')') {
					p++;
				}
				else {
					push_op(top_op);
					cal_top_two();
				}
			} else {
				push_op(*p);
				break;
			}
		}
		p++;
	}

	double result;
	pop_num(&result);
	printf("result = %f\n", result);

	{
		double num;
		char op;
		//printf("--- value in stack ---\n");
		while (pop_num(&num) != -1) {
			//printf("num: %f\n", num);
		}
		while (pop_op(&op) != -1) {
			//printf("op: %c\n", op);
		}
	}
	goto LABEL_START;
	return 0;
}
