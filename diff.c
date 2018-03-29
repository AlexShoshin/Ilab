#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define FUNCTION_LENGTH 200

struct Node
{
	char * s;
	struct Node * left;
	struct Node * right;
};


void greetings();
char * read_function();
char * check_function_structure(char * function);
char * read_file();
void dump_tree(struct Node * tree, FILE * output);
char * check_for_spaces(char * function);
struct Node * create_tree(char * function);
void check_functions(struct Node * tree, char * function);
struct Node * copy(struct Node * root);
struct Node * diff_tree(struct Node * root);
void printfunc(struct Node * tree, FILE * output);
void close_tex(FILE * output);
FILE * prepare_tex();
void tree_destroy (struct Node * tree);
int optimise(struct Node * tree, FILE * output);


int main()
{
	greetings();

	char * function = calloc(FUNCTION_LENGTH, sizeof(char)); //reading function from file and checking it's structure
	function = read_function();
	assert(check_function_structure(function));

	struct Node *root = calloc(1, sizeof(struct Node));     //creating tree from function-string
	root = create_tree(function);


	FILE * output = prepare_tex(); 				//printf to pdf oringinal function
	fprintf(output, "\\left(");
	printfunc(root, output);
	fprintf(output, "\\right)^{'} = \\\\ \\nonumber\\]\n\n\\[");

	root = diff_tree(root);					//differentiating function


	printfunc(root, output);                                //print to pdf function after differentiating

	int change_flag = 1;
	while(change_flag != 0)					//optimising structure of differentiated function
	{
		change_flag = optimise(root, output);
		fprintf(output, " = \\\\ \\nonumber\\]\n\n\\[");
   		printfunc(root, output);
	}
    	

	FILE * dump_output = fopen("dump.txt", "w");		//dump to txt
	dump_tree(root, dump_output);
	fclose(dump_output);

	close_tex(output);
	free(function);
	tree_destroy(root);
	printf("All right, check diff.pdf for results of differentiation!\n");
	return 0;
}

void greetings()
{
	printf("Differetiator 2.0\n\nHello! This program will differentiate your function!\n");
	printf("Please, use only following functions:\n+, -, *, ()/()\nexp(...)\nln(...)\n(a)^(...)\n(...)^(c)\nsin(...), cos(...), tg(...)\narcsin(...), arccos(...), arctg(...)\n");
	printf("Please, separate signes + and - with spaces!\nThe structure of function must be correct!\n");
	printf("Example: (cos(2*x) + exp(arcsin(x))) + 54/ln(x)\n\n");
}

char * read_function()					//reading function from file
{
	char * function = calloc(FUNCTION_LENGTH, sizeof(char));
	char symbol;
	function = read_file();

	return function;
}

char * check_function_structure(char * function)			//checking for right brackets structure
{
	char * result = calloc(10, sizeof(char));
	result = "okay";
	int len = strlen(function);
	int i;
	int opening_brackets_counter = 0;
	int closing_brackets_counter = 0;
	for(i = 0; i < len; i++)
	{
		if(function[i] == '(')
		{
			opening_brackets_counter++;
		}
		else if(function[i] == ')')
		{
			closing_brackets_counter++;
		}
	}
	assert(check_for_spaces(function));
	if(opening_brackets_counter != closing_brackets_counter)
	{
		result = NULL;
	}

	return result;
}

char * read_file()				//reading file
{
	FILE * input = fopen("function.txt", "r");
	if(input == NULL)
	{
		printf("File |function.txt| not found!\n");
	}
	assert(input);
	char * function = calloc(FUNCTION_LENGTH, sizeof(char));
	char * symbol = calloc(100, sizeof(char));
	function[0] = '\0';
	while(fscanf(input, "%s", symbol) == 1)
	{
		function = strcat(function, symbol);
		function = strcat(function, " ");
	}
	int len = strlen(function);
	function[len - 1] = '\0';
	fclose(input);
	return function;
}

void dump_tree(struct Node * tree, FILE * output)		//dump to txt
{
	fprintf(output, "%s", tree->s);
	if(tree->left != NULL)
	{
		fprintf(output, "%s", "{");
		dump_tree(tree->left, output);
		fprintf(output, "%s", "}");
	}
	if(tree->right != NULL)
	{
		fprintf(output, "%s", "{");
		dump_tree(tree->right, output);
		fprintf(output, "%s", "}");
	}
}

char * check_for_spaces(char * function)			//check for right spaces structure
{
	 char * checkstr = calloc(FUNCTION_LENGTH, sizeof(char));
	 char * printstr = calloc(FUNCTION_LENGTH, sizeof(char));
	 int i;
	 int len = strlen(function);
	 int assert_flag = 0;
	 for(i = 1; i < len - 1; i++)
	 {
		 if(function[i] == ' ')
		 {
			 if(function[i - 1] == '+' || function[i - 1] == '-' || function[i + 1] == '+' || function[i + 1] == '-')
			 {
				 checkstr = "okay";
			 }
			 else
			 {
				 printstr[0] = '\0';
				 printstr = strncat(printstr, function, i);
				 printf("Mistake:\nYou have an extra space after %s\n", printstr);
				 assert_flag = 1;
			 }
		 }

	 }

	 for(i = 1; i < len - 1; i++)
	 {
		if(function[i] == '+' || (function[i] == '-' && (function[i + 1] < 48 || function[i + 1] > 57)))
		{
			if(function[i - 1] == ' ')
			{
				checkstr = "okay";
			}
			else
			{
			       	printstr[0] = '\0';
				 printstr = strncat(printstr, function, i);
				 printf("Mistake:\nThere is not enough space after %s\n", printstr);
				 checkstr = NULL;
				 assert_flag = 1;
			}


			if(function[i + 1] == ' ')
			{
				checkstr = "okay";
			}
			else
			{
			       	printstr[0] = '\0';
				 printstr = strncat(printstr, function, i + 1);
				 printf("Mistake:\nThere is not enough space after %s\n", printstr);
				 checkstr = NULL;
				assert_flag = 1;
			}


		}
	 }
	 if(assert_flag == 1)
	 {
		 return NULL;
	 }
	 else
	 {
	 	return checkstr;
	 }

	 free(checkstr);
	 free(printstr);
}


struct Node * create_tree(char * function)			//creating tree from function-string
{
	struct Node * tree = calloc(1, sizeof(struct Node));
	int len = strlen(function);
	char * left_function = calloc(FUNCTION_LENGTH, sizeof(char));
	char * right_function = calloc(FUNCTION_LENGTH, sizeof(char));
	int in_brackets = 0;     //if ==0 it means that we are not in brackets
	int brack;
	char * checkstr = calloc(FUNCTION_LENGTH, sizeof(char));
	int i, j, k;
	char c;    //for easywrite
	for(i = 0; i < len; i++)
	{
		c = function[i];
		if(c == '(')
		{
			in_brackets++;
		}
		else if(c == ')')
		{
			in_brackets--;
		}
		else if(c == '+' && in_brackets == 0)
		{
			tree->s = "+";

			for(j = 0; j < i - 1; j++)
			{
				left_function[j] = function[j];
			}
			left_function[i - 1] = '\0';
			tree->left = create_tree(left_function);

			for(j = i + 2; j < len; j++)
			{
				right_function[j - i - 2] = function[j];
			}
			right_function[len - i - 2] = '\0';
			tree->right = create_tree(right_function);

			return tree;

		}
	}

	in_brackets = 0;
	for(i = 0; i < len; i++)
	{
		c = function[i];
		if(c == '(')
		{
			in_brackets++;
		}
		else if(c == ')')
		{
			in_brackets--;
		}
		else if(c == '-' && in_brackets == 0)
		{

			if(function[i + 1] < 47 || function[i + 1] > 58)
			{

				tree->s = "-";
				for(j = 0; j < i - 1; j++)
				{
					left_function[j] = function[j];
				}
				left_function[i - 1] = '\0';
				tree->left = create_tree(left_function);

				for(j = i + 2; j < len; j++)
				{
					right_function[j - i - 2] = function[j];
				}
				right_function[len - i - 2] = '\0';

				tree->right = create_tree(right_function);
			}
			else
			{
				tree->s = "-";
				left_function = "0";
				tree->left = create_tree(left_function);

				for(j = i + 1; j < len; j++)
				{
					right_function[j - i - 1] = function[j];
				}
				right_function[len - i - 1] = '\0';
				tree->right = create_tree(right_function);
				//tree->s = function;
			}

			return tree;

		}
	}

	in_brackets = 0;
	for(i = 0; i < len; i++)
	{
		c = function[i];
		if(c == '(')
		{
			in_brackets++;
		}
		else if(c == ')')
		{
			in_brackets--;
		}
		else if(c == '*' && in_brackets == 0)
		{
			tree->s = "*";

			if(function[i - 1] != ')')
			{
				for(j = 0; j < i; j++)
				{
					left_function[j] = function[j];
				}
				left_function[i] = '\0';

			}
			else
			{
				brack = 0;
				for(j = i - 1; j >= 0; j--)
				{
					if(function[j] == ')')
					{
						brack++;
					}
					else if(function[j] == '(')
					{
						brack--;
					}
					if(function[j] == '(' && brack == 0)
					{
						k = j;
						break;
					}

				}

				if(k == 0)
				{
					for(j = 1; j < i - 1; j++)
					{
						left_function[j - 1] = function[j];
					}
					left_function[i - 2] = '\0';
				}
				else
				{
					for(j = 0; j < i; j++)
					{
						left_function[j] = function[j];
					}
					left_function[i] = '\0';

				}
			}

			tree->left = create_tree(left_function);



			if(function[i + 1] != '(')
			{
				for(j = i + 1; j < len; j++)
				{
					right_function[j - i - 1] = function[j];
				}
				right_function[len - i - 1] = '\0';
			}
			else
			{
				brack = 0;
				for(j = i + 1; j < len; j++)
				{
					if(function[j] == '(')
					{
						brack++;
					}
					else if(function[j] == ')')
					{
						brack--;
					}
					if(function[j] == ')' && brack == 0)
					{
						k = j;
						break;
					}
				}

				if(k == len - 1)
				{
					for(j = i + 2; j < len - 1; j++)
					{
						right_function[j - i - 2] = function[j];
					}
					right_function[len - i - 3] = '\0';

				}
				else
				{
					for(j = i + 1; j < len; j++)
					{
						right_function[j - i - 1] = function[j];
					}
					right_function[len - i - 1] = '\0';

				}

			}

			tree->right = create_tree(right_function);

			return tree;

		}
	}

	in_brackets = 0;
	for(i = 0; i < len; i++)
	{
		c = function[i];
		if(c == '(')
		{
			in_brackets++;
		}
		else if(c == ')')
		{
			in_brackets--;
		}
		else if(c == '/' && in_brackets == 0)
		{
			tree->s = "/";

			if(function[i - 1] == ')')
			{
				for(j = 1; j < i - 1; j++)
				{
					left_function[j - 1] = function[j];
				}
				left_function[i - 2] = '\0';
			}
			else
			{
				for(j = i; j < len; j++)
				{
					checkstr[j - i] = function[j];
				}
				checkstr[len - i] = '\0';

				printf("Mistake!You have wrong construction with / before |%s| !Right construction is ()/().\n", checkstr);
				checkstr = NULL;
				assert(checkstr);
			}
			tree->left = create_tree(left_function);

			if(function[i + 1] == '(')
			{
				for(j = i + 2; j < len - 1; j++)
				{
					right_function[j - i - 2] = function[j];
				}
				right_function[len - i - 3] = '\0';
			}
			else
			{
				for(j = 0; j < i + 1; j++)
				{
					checkstr[j] = function[j];
				}
				checkstr[i + 1] = '\0';
				printf("Mistake!You have wrong construction with / after |%s| !Right construction is ()/().\n", checkstr);
				checkstr = NULL;
				assert(checkstr);

			}
			tree->right = create_tree(right_function);

			return tree;

		}
	}

	in_brackets = 0;
	for(i = 0; i < len; i++)
	{
		c = function[i];
		if(c == '(')
		{
			in_brackets++;
		}
		else if(c == ')')
		{
			in_brackets--;
		}
		else if(c == '^' && in_brackets == 0)
		{
			tree->s = "^";

			if(function[i - 1] == ')')
			{
				for(j = 1; j < i - 1; j++)
				{
					left_function[j - 1] = function[j];
				}
				left_function[i - 2] = '\0';
			}
			else
			{
				for(j = i; j < len; j++)
				{
					checkstr[j - i] = function[j];
				}
				checkstr[len - i] = '\0';

				printf("Mistake!You have wrong construction with ^ before |%s| !Right construction is ()^().\n", checkstr);
				checkstr = NULL;
				assert(checkstr);
			}
			tree->left = create_tree(left_function);

			if(function[i + 1] == '(')
			{
				for(j = i + 2; j < len - 1; j++)
				{
					right_function[j - i - 2] = function[j];
				}
				right_function[len - i - 3] = '\0';
			}
			else
			{
				for(j = 0; j < i + 1; j++)
				{
					checkstr[j] = function[j];
				}
				checkstr[i + 1] = '\0';

				printf("Mistake!You have wrong construction with ^ after |%s| !Right construction is ()^().\n", checkstr);
				checkstr = NULL;
				assert(checkstr);


			}
			tree->right = create_tree(right_function);

			return tree;

		}
	}
 	check_functions(tree, function);
	free(checkstr);
	free(left_function);
	free(right_function);
	return tree;
}

void check_functions(struct Node * tree, char * function)	//check structure of other function and integrate them to tree
{
	int i, j;
	char * checkstr = calloc(FUNCTION_LENGTH, sizeof(char));
	char * insidef = calloc(FUNCTION_LENGTH, sizeof(char));
	int assertflag = 0; // is 0 if all right, is 1 if assertation needed
	int len = strlen(function);
	if(function[0] > 47 && function[0] < 58)
	{
		if(len != 1)
		{
			for(i = 1; i < len; i++)
			{
				if(function[0] > 57 || function[0] < 47)
				{
					printf("Mistake!What did you mean here |%s|?May be it is a number?\n", function);
					checkstr = NULL;
					assert(checkstr);
				}
			}
			tree->s = function;
		}
		else
		{
			tree->s = function;
		}
	}
	else if(function[0] > 96 && function[0] < 123 && len == 1)
	{
		tree->s = function;
	}
	else if(function[0] == 'x')
	{
		if(len == 1)
		{
			tree->s = function;
		}
		else
		{
			printf("Mistake!What did you mean here |%s|?May be it is |x|?\n", function);
			checkstr = NULL;
			assert(checkstr);

		}
	}
	else if(function[0] == 'e')
	{
		checkstr = strncat(checkstr, function, 3);
		if(strcmp(checkstr, "exp") == 0 && function[3] == '(')
		{
			tree->s = "exp";
			for(j = 4; j < len - 1; j++)
			{
				insidef[j - 4] = function[j];
			}
			insidef[len - 5] = '\0';
			tree->left = create_tree(insidef);
			tree->right = create_tree("0");
		}
		else
		{
			printf("Mistake!What did you mean here |%s|?May be it is |exp()|?\n", function);
			checkstr = NULL;
			assert(checkstr);

		}
	}
	else if(function[0] == 'l')
	{
		checkstr = strncat(checkstr, function, 2);
		if(strcmp(checkstr, "ln") == 0 && function[2] == '(')
		{
			tree->s = "ln";
			for(j = 3; j < len - 1; j++)
			{
				insidef[j - 3] = function[j];
			}
			insidef[len - 4] = '\0';
			tree->left = create_tree(insidef);
			tree->right = create_tree("0");

		}
		else
		{
			printf("Mistake!What did you mean here |%s|?May be it is |ln()|?\n", function);
			checkstr = NULL;
			assert(checkstr);

		}
	}
	else if(function[0] == 's')
	{
		checkstr = strncat(checkstr, function, 3);
		if(strcmp(checkstr, "sin") == 0 && function[3] == '(')
		{
			tree->s = "sin";
			for(j = 4; j < len - 1; j++)
			{
				insidef[j - 4] = function[j];
			}
			insidef[len - 5] = '\0';
			tree->left = create_tree(insidef);
			tree->right = create_tree("0");

		}
		else
		{
			printf("Mistake!What did you mean here |%s|?May be it is |sin()|?\n", function);
			checkstr = NULL;
			assert(checkstr);

		}
	}
	else if(function[0] == 'c')
	{
		checkstr = strncat(checkstr, function, 3);
		if(strcmp(checkstr, "cos") == 0 && function[3] == '(')
		{
			tree->s = "cos";
			for(j = 4; j < len - 1; j++)
			{
				insidef[j - 4] = function[j];
			}
			insidef[len - 5] = '\0';
			tree->left = create_tree(insidef);
			tree->right = create_tree("0");

		}
		else
		{
			printf("Mistake!What did you mean here |%s|?May be it is |cos()|?\n", function);
			checkstr = NULL;
			assert(checkstr);

		}
	}
	else if(function[0] == 't')
	{
		checkstr = strncat(checkstr, function, 2);
		if(strcmp(checkstr, "tg") == 0 && function[2] == '(')
		{
			tree->s = "tg";
			for(j = 3; j < len - 1; j++)
			{
				insidef[j - 3] = function[j];
			}
			insidef[len - 4] = '\0';
			tree->left = create_tree(insidef);
			tree->right = create_tree("0");

		}
		else
		{
			printf("Mistake!What did you mean here |%s|?May be it is |tg()|?\n", function);
			checkstr = NULL;
			assert(checkstr);

		}
	}
	else if(function[0] == 'a')
	{
		checkstr = strncat(checkstr, function, 6);
		if(strcmp(checkstr, "arcsin") == 0 && function[5] == 'n')
		{
			tree->s = "arcsin";
			for(j = 7; j < len - 1; j++)
			{
				insidef[j - 7] = function[j];
			}
			insidef[len - 8] = '\0';
			tree->left = create_tree(insidef);
			tree->right = create_tree("0");

		}
		else if(strcmp(checkstr, "arccos") == 0 && function[6] == '(')
		{
			tree->s = "arccos";
			for(j = 7; j < len - 1; j++)
			{
				insidef[j - 7] = function[j];
			}
			insidef[len - 8] = '\0';
			tree->left = create_tree(insidef);
			tree->right = create_tree("0");

		}
		else if(strcmp(checkstr, "arctg(") == 0 && function[5] == '(')
		{
			tree->s = "arctg";
			for(j = 6; j < len - 1; j++)
			{
				insidef[j - 6] = function[j];
			}
			insidef[len - 7] = '\0';
			tree->left = create_tree(insidef);
			tree->right = create_tree("0");

		}
		else
		{
			printf("Mistake!What did you mean here |%s|   %s    %c?\n", function, checkstr, function[6]);
			checkstr = NULL;
			assert(checkstr);

		}
	}
	else
	{
		printf("Mistake!What did you mean here |%s|?\n", function);
		checkstr = NULL;
		assert(checkstr);
	}

	free(checkstr);
}


struct Node * copy(struct Node * root)    		//copying part of tree
{
	struct Node * tree = calloc(1, sizeof(struct Node));
	tree->s = root->s;

	if(root->left != NULL && root->right != NULL)
	{
		tree->left = copy(root->left);
		tree->right = copy(root->right);
	}

	return tree;
}

struct Node * diff_tree(struct Node * root)		//differentiating tree
{
	struct Node * tree = calloc(1, sizeof(struct Node));
	int value;
	if(root->s[0] == '+')
	{
		tree->s = "+";
		tree->left = diff_tree(root->left);
		tree->right = diff_tree(root->right);
	}
	else if((root->s[0] > 47 && root->s[0] < 58))
	{
		tree->s = "0";
	}
	else if(root->s[0] > 96 && root->s[0] < 123 && strlen(root->s) == 1 && root->s[0] != 'x')
	{
		tree->s = "0";
	}
	else if(root->s[0] == 'x')
	{
		tree->s = "1";
	}
	else if(root->s[0] == '-')
	{
		tree->s = "-";
		tree->left = diff_tree(root->left);
		tree->right = diff_tree(root->right);
	}
	else if(root->s[0] == '*')
	{
		tree->s = "+";

		tree->left = calloc(1, sizeof(struct Node));
		tree->left->s = "*";
		tree->left->left = diff_tree(root->left);
		tree->left->right = copy(root->right);

		tree->right = calloc(1, sizeof(struct Node));
		tree->right->s = "*";
		tree->right->left = copy(root->left);
		tree->right->right = diff_tree(root->right);

	}
	else if(root->s[0] == '/')
	{
		tree->s = "/";

		tree->left = calloc(1, sizeof(struct Node));
		tree->left->s = "-";

		tree->left->left = calloc(1, sizeof(struct Node));
		tree->left->left->s = "*";
		tree->left->left->left = diff_tree(root->left);
		tree->left->left->right = copy(root->right);

		tree->left->right = calloc(1, sizeof(struct Node));
		tree->left->right->s = "*";
		tree->left->right->left = copy(root->left);
		tree->left->right->right= diff_tree(root->right);

		tree->right = calloc(1, sizeof(struct Node));
		tree->right->s = "^";
		tree->right->left = copy(root->right);
		tree->right->right = calloc(1, sizeof(struct Node));
		tree->right->right->s = "2";
	}
	else if(root->s[0] == '^')
	{
		if(root->right->s[0] == '-')
		{
			assert(NULL);
		}
		if((root->right->s[0] > 47 && root->right->s[0] < 58) && (root->left->s[0] < 48 || root->left->s[0] > 57))
		{
			tree->s = "*";

			tree->left = calloc(1, sizeof(struct Node));
			tree->left->s = root->right->s;

			tree->right = calloc(1, sizeof(struct Node));
			tree->right->s = "*";
			tree->right->left = calloc(1, sizeof(struct Node));
			tree->right->left->s = "^";
			tree->right->left->left = copy(root->left);
			tree->right->left->right = calloc(1, sizeof(struct Node));
			tree->right->left->right->s = calloc(10, sizeof(char));
			sprintf(tree->right->left->right->s, "%d", atoi(root->right->s) - 1);
			tree->right->right = diff_tree(root->left);
		}
		else if((root->right->s[0] == '/') && (root->left->s[0] < 48 || root->left->s[0] > 57))
		{
			if(atoi(root->right->left->s) > atoi(root->right->right->s))
			{
				tree->s = "*";

				tree->left = calloc(1, sizeof(struct Node));
				tree->left->s = "*";

				tree->left->left = copy(root->right);
				tree->left->right = calloc(1, sizeof(struct Node));
				tree->left->right->s = "^";
				tree->left->right->left = copy(root->left);
				tree->left->right->right = calloc(1, sizeof(struct Node));
				tree->left->right->right->s = "/";
				tree->left->right->right->left = calloc(1, sizeof(struct Node));
				tree->left->right->right->left->s = calloc(10, sizeof(char));
				sprintf(tree->left->right->right->left->s, "%d", atoi(root->right->left->s) - atoi(root->right->right->s));
				tree->left->right->right->right = calloc(1, sizeof(struct Node));
				tree->left->right->right->right->s = calloc(10, sizeof(char));
				sprintf(tree->left->right->right->right->s, "%d", atoi(root->right->right->s));


				tree->right = diff_tree(root->left);

			}
			else
			{
				tree->s = "/";

				tree->left = calloc(1, sizeof(struct Node));
				tree->left->s = "*";
				tree->left->left = copy(root->right->left);
				tree->left->right = diff_tree(root->left);

				tree->right = calloc(1, sizeof(struct Node));
				tree->right->s = "*";
				tree->right->left = copy(root->right->right);
				tree->right->right = calloc(1, sizeof(struct Node));
				tree->right->right->s = "^";
				tree->right->right->left = copy(root->left);
				tree->right->right->right = calloc(1, sizeof(struct Node));
				tree->right->right->right->s = "/";
				tree->right->right->right->left = calloc(1, sizeof(struct Node));
				tree->right->right->right->left->s = calloc(10, sizeof(char));
				sprintf(tree->right->right->right->left->s, "%d", atoi(root->right->right->s) - atoi(root->right->left->s));
				tree->right->right->right->right = copy(root->right->right);

			}
		}
		else if(root->left->s[0] > 47 && root->left->s[0] < 58)
		{
			tree->s = "*";

			tree->left = copy(root);

			tree->right = calloc(1, sizeof(struct Node));
			tree->right->s = "ln";
			tree->right->left = copy(root->left);
			tree->right->right = calloc(1, sizeof(struct Node));
			tree->right->right->s = "0";
		}
		else
		{
			tree->s = "*";

			tree->left = copy(root);

			tree->right = calloc(1, sizeof(struct Node));
			tree->right->s = "+";
			tree->right->left = calloc(1, sizeof(struct Node));
			tree->right->left->s = "*";
			tree->right->left->left = diff_tree(root->right);
			tree->right->left->right = calloc(1, sizeof(struct Node));
			tree->right->left->right->s = "ln";
			tree->right->left->right->left = copy(root->left);
			tree->right->left->right->right = calloc(1, sizeof(struct Node));
			tree->right->left->right->right->s = "0";
			tree->right->right = calloc(1, sizeof(struct Node));
			tree->right->right->s = "/";
			tree->right->right->left = copy(root->right);
			tree->right->right->right = copy(root->left);


		}


	}
	else if(root->s[0] == 'l')
	{
		tree->s = "/";
		tree->left = diff_tree(root->left);
		tree->right = copy(root->left);
	}
	else if(root->s[0] == 'e')
	{
		tree->s = "*";
		tree->left = copy(root);
		tree->right = diff_tree(root->left);
	}
	else if(root->s[0] == 's')
	{
		tree->s = "*";

		tree->left = calloc(1, sizeof(struct Node));
		tree->left->s = "cos";
		tree->left->left = copy(root->left);
		tree->left->right = calloc(1, sizeof(struct Node));
		tree->left->right->s = "0";

		tree->right = diff_tree(root->left);
	}
	else if(root->s[0] == 'c')
	{
		tree->s = "*";
		tree->left = calloc(1, sizeof(struct Node));
		tree->left->s = "-";
		tree->left->left = calloc(1, sizeof(struct Node));
		tree->left->left->s = "0";
		tree->left->right = calloc(1, sizeof(struct Node));
		tree->left->right->s = "sin";
		tree->left->right->left = copy(root->left);
		tree->left->right->right = calloc(1, sizeof(struct Node));
		tree->left->right->right->s = "0";

		tree->right = diff_tree(root->left);
	}
	else if(root->s[0] == 't')
	{
		tree->s = "/";

		tree->left = diff_tree(root->left);

		tree->right = calloc(1, sizeof(struct Node));
		tree->right->s = "^";
		tree->right->left = calloc(1, sizeof(struct Node));
		tree->right->left->s = "cos";
		tree->right->left->left = copy(root->left);
		tree->right->left->right = calloc(1, sizeof(struct Node));
		tree->right->left->right->s = "0";
		tree->right->right = calloc(1, sizeof(struct Node));
		tree->right->right->s = "2";
	}
	else if(root->s[0] == 'a')
	{
		if(strcmp(root->s, "arcsin") == 0)
		{
			tree->s = "/";
			tree->left = diff_tree(root->left);

			tree->right = calloc(1, sizeof(struct Node));
			tree->right->s = "^";
			tree->right->left = calloc(1, sizeof(struct Node));
			tree->right->left->s = "-";
			tree->right->left->left = calloc(1, sizeof(struct Node));
			tree->right->left->left->s = "1";
			tree->right->left->right = calloc(1, sizeof(struct Node));
			tree->right->left->right->s = "^";
			tree->right->left->right->left = copy(root->left);
			tree->right->left->right->right = calloc(1, sizeof(struct Node));
			tree->right->left->right->right->s = "2";
			tree->right->right = calloc(1, sizeof(struct Node));
			tree->right->right->s = "/";
			tree->right->right->left = calloc(1, sizeof(struct Node));
			tree->right->right->left->s = "1";
			tree->right->right->right = calloc(1, sizeof(struct Node));
			tree->right->right->right->s = "2";
		}
		else if(strcmp(root->s, "arccos") == 0)
		{
			tree->s = "/";
			tree->left = calloc(1, sizeof(struct Node));
			tree->left->s = "-";
			tree->left->left = calloc(1, sizeof(struct Node));
			tree->left->left->s = "0";
			tree->left->right = diff_tree(root->left);

			tree->right = calloc(1, sizeof(struct Node));
			tree->right->s = "^";
			tree->right->left = calloc(1, sizeof(struct Node));
			tree->right->left->s = "-";
			tree->right->left->left = calloc(1, sizeof(struct Node));
			tree->right->left->left->s = "1";
			tree->right->left->right = calloc(1, sizeof(struct Node));
			tree->right->left->right->s = "^";
			tree->right->left->right->left = copy(root->left);
			tree->right->left->right->right = calloc(1, sizeof(struct Node));
			tree->right->left->right->right->s = "2";
			tree->right->right = calloc(1, sizeof(struct Node));
			tree->right->right->s = "/";
			tree->right->right->left = calloc(1, sizeof(struct Node));
			tree->right->right->left->s = "1";
			tree->right->right->right = calloc(1, sizeof(struct Node));
			tree->right->right->right->s = "2";

		}
		else if(strcmp(root->s, "arctg") == 0)
		{
			tree->s = "/";

			tree->left = diff_tree(root->left);
			tree->right = calloc(1, sizeof(struct Node));
			tree->right->s = "+";
			tree->right->left = calloc(1, sizeof(struct Node));
			tree->right->left->s = "1";
			tree->right->right = calloc(1, sizeof(struct Node));
			tree->right->right->s = "^";
			tree->right->right->right = calloc(1, sizeof(struct Node));
			tree->right->right->right->s = "2";
			tree->right->right->left = copy(root->left);
		}
	}
	return tree;
}


void printfunc(struct Node * tree, FILE * output)			//print function to pdf
{
	if(tree->s[0] == '+')
	{
		printfunc(tree->left, output);
		fprintf(output, " + ");
		printfunc(tree->right, output);
	}
	else if(strcmp(tree->s, "-") == 0 && tree->left->s[0] != '0')
	{
		if((tree->left->s[0] == '+' || tree->left->s[0] == '-')  && (tree->right->s[0] != '+' && tree->right->s[0] != '-'))
		{
			fprintf(output, "\\left(");
			printfunc(tree->left, output);
			fprintf(output, "\\right)");
			fprintf(output, "-");
			printfunc(tree->right, output);
		}
		else if((tree->right->s[0] == '+' || tree->right->s[0] == '-')  && (tree->left->s[0] != '+' && tree->left->s[0] != '-' ))
		{
			printfunc(tree->left, output);
			fprintf(output, "-");
			fprintf(output, "\\left(");
			printfunc(tree->right, output);
			fprintf(output, "\\right)");

		}
		else if((tree->right->s[0] == '+' || tree->right->s[0] == '-')  &&  (tree->left->s[0] == '+' || tree->left->s[0] == '-' ))
		{
			fprintf(output, "\\left(");
			printfunc(tree->left, output);
			fprintf(output, "\\right)");
			fprintf(output, "-");
			fprintf(output, "\\left(");
			printfunc(tree->right, output);
			fprintf(output, "\\right)");
		}
		else
		{
			printfunc(tree->left, output);
			fprintf(output, "-");
			printfunc(tree->right, output);
		}

	}
	else if(strcmp(tree->s, "-") == 0 && tree->left->s[0] == '0')
	{
		if(tree->right->s[0] > 47 && tree->right->s[0] < 58)
		{
			fprintf(output, "-");
			printfunc(tree->right, output);
		}
		else
		{
			fprintf(output, " - \\left(");
			printfunc(tree->right, output);
			fprintf(output, " \\right)");
		}
	}

	else if(tree->s[0] > 47 && tree->s[0] < 58)
	{
		fprintf(output, "%s", tree->s);
	}
	else if(tree->s[0] > 96 && tree->s[0] < 123 && strlen(tree->s) == 1)
	{
		fprintf(output, "%s", tree->s);
	}
	else if(tree->s[0] == 'x')
	{
		fprintf(output, "%s", tree->s);
	}
	else if(tree->s[0] == '*')
	{
		if((tree->left->s[0] == '+' || tree->left->s[0] == '-')  && (tree->right->s[0] != '+' && tree->right->s[0] != '-'))
		{
			fprintf(output, "\\left(");
			printfunc(tree->left, output);
			fprintf(output, "\\right)");
			fprintf(output, "\\cdot ");
			printfunc(tree->right, output);
		}
		else if((tree->right->s[0] == '+' || tree->right->s[0] == '-')  && (tree->left->s[0] != '+' && tree->left->s[0] != '-' ))
		{
			printfunc(tree->left, output);
			fprintf(output, "\\cdot ");
			fprintf(output, "\\left(");
			printfunc(tree->right, output);
			fprintf(output, "\\right)");

		}
		else if((tree->right->s[0] == '+' || tree->right->s[0] == '-')  &&  (tree->left->s[0] == '+' || tree->left->s[0] == '-' ))
		{
			fprintf(output, "\\left(");
			printfunc(tree->left, output);
			fprintf(output, "\\right)");
			fprintf(output, "\\cdot ");
			fprintf(output, "\\left(");
			printfunc(tree->right, output);
			fprintf(output, "\\right)");
		}
		else
		{
			printfunc(tree->left, output);
			fprintf(output, "\\cdot ");
			printfunc(tree->right, output);
		}

	}
	else if(tree->s[0] == '/')
	{
		fprintf(output, "\\frac {");
		printfunc(tree->left, output);
		fprintf(output, "}{");
		printfunc(tree->right, output);
		fprintf(output, "}");

	}
	else if(tree->s[0] == '^')
	{
		if(tree->right->s[0] != '/')
		{
			fprintf(output, "\\left(");
			printfunc(tree->left, output);
			if(strcmp(tree->right->s, "1") != 0)
			{
				fprintf(output, "\\right)^{");
				printfunc(tree->right, output);
				fprintf(output, "}");
			}
			else
			{
				fprintf(output, "\\right)");
			}

		}
		else
		{
			fprintf(output, "\\sqrt [");
			printfunc(tree->right->right, output);
			if(strcmp(tree->right->left->s, "1") != 0)
			{
				fprintf(output, "]{\\left(");
				printfunc(tree->left, output);
				fprintf(output, "\\right)^{");
				printfunc(tree->right->left, output);
				fprintf(output, "}}");
			}
			else
			{
				fprintf(output, "]{");
				printfunc(tree->left, output);
				fprintf(output, "}");
			}
		}
	}
	else if(tree->s[0] == 'l')
	{
		fprintf(output, "\\ln {\\left(");
		printfunc(tree->left, output);
		fprintf(output, "\\right)}");
	}
	else if(tree->s[0] == 'e')
	{
		fprintf(output, "\\exp {\\left(");
		printfunc(tree->left, output);
		fprintf(output, "\\right)}");
	}
	else if(tree->s[0] == 'c')
	{
		fprintf(output, "\\cos {\\left(");
		printfunc(tree->left, output);
		fprintf(output, "\\right)}");
	}
	else if(tree->s[0] == 's')
	{
		fprintf(output, "\\sin {\\left(");
		printfunc(tree->left, output);
		fprintf(output, "\\right)}");
	}
	else if(tree->s[0] == 't')
	{
		fprintf(output, "\\tan {\\left(");
		printfunc(tree->left, output);
		fprintf(output, "\\right)}");
	}
	else if(strcmp(tree->s, "arcsin") == 0)
	{
		fprintf(output, "\\arcsin {\\left(");
		printfunc(tree->left, output);
		fprintf(output, "\\right)}");
	}
	else if(strcmp(tree->s, "arccos") == 0)
	{
		fprintf(output, "\\arccos {\\left(");
		printfunc(tree->left, output);
		fprintf(output, "\\right)}");
	}
	else if(strcmp(tree->s, "arctg") == 0)
	{
		fprintf(output, "\\arctan {\\left(");
		printfunc(tree->left, output);
		fprintf(output, "\\right)}");
	}


}

void close_tex(FILE * output)		//close output file
{
	fprintf(output, "\\]\n");
	fprintf(output, "\\end{landscape}\n");
	fprintf(output, "\\end{document}\n");

	fclose(output);
	system("pdflatex diff.tex");

}


FILE * prepare_tex()			//open output file
{
	FILE * output = fopen("diff.tex", "w");
	if(output == NULL)
	{
		printf("File |diff.tex| not found!\n");
		assert(NULL);
	}

	fprintf(output, "\\documentclass[a2paper,5pt]{article}\n");
	fprintf(output, "\\usepackage{amsmath,amsfonts,amssymb,amsthm}\n\n");
	fprintf(output, "\\usepackage[left=1cm,right=1cm, top=1cm,bottom=1cm]{geometry}\n\n");
	fprintf(output, "\\usepackage[pdftex]{lscape}\n\n");
	fprintf(output, "\\begin{document}\n");
	fprintf(output, "\\begin{landscape}\n");
	fprintf(output, "\\[");

	return output;
}

void tree_destroy (struct Node * tree)          //frees memory
{
	if(tree != NULL)
	{
		tree_destroy(tree->left);
		tree_destroy(tree->right);
		struct Node * p;
		p = realloc(tree, 0);
		tree = p;
	}
}

int optimise(struct Node * tree, FILE * output)	//optimising function structure
{
	int change_flag = 0;
	int flag1 = 0;
	int flag2 = 0;
	int value = 0;
	struct Node * tmp;



	if(tree->s[0] == '+' && strcmp(tree->left->s, "0") == 0) // 0 + 5 = 5
	{

		tmp = copy(tree->right);

		tree_destroy(tree);

		tree = copy(tmp);

		change_flag = 1;

	}
	else if(tree->s[0] == '*' && strcmp(tree->left->s, "1") == 0)  //1 * 5 = 5
	{

		tmp = copy(tree->right);
		tree_destroy(tree);
		tree = copy(tmp);
		change_flag = 1;

	}

	else if(tree->s[0] == '+' && strcmp(tree->right->s, "0") == 0) // 5 + 0 = 5
	{
		tmp = copy(tree->left);
		tree_destroy(tree);
		tree = copy(tmp);
		change_flag = 1;
	}
	else if(tree->s[0] == '*' && (tree->left->s[0] > 47 && tree->left->s[0] < 58) && tree->right->s[0] == '/' && (tree->right->left->s[0] > 47 && tree->right->left->s[0] < 58)) // 7*(1/x) = 7/x
	{
		tmp = copy(tree->right->right);
		value = atoi(tree->left->s) * atoi(tree->right->left->s);
		tree_destroy(tree);
		tree = calloc(1, sizeof(struct Node));
		tree->s = "/";
		tree->left = calloc(1, sizeof(struct Node));
		tree->left->s = calloc(10, sizeof(char));
		sprintf(tree->left->s, "%d", value);
		tree->right = copy(tmp);
		change_flag = 1;
	}
	else if(tree->s[0] == '*' && (tree->left->s[0] > 49 && tree->left->s[0] < 58) && (tree->right->s[0] > 49 && tree->right->s[0] < 58)) //2*5 = 10
	{
		value = atoi(tree->left->s) * atoi(tree->right->s);
		tree_destroy(tree);
		tree = calloc(1, sizeof(struct Node));
		tree->s = calloc(10, sizeof(char));
		sprintf(tree->s, "%d", value);
		change_flag = 1;
	}
	else if(tree->s[0] == '*' && (tree->left->s[0] == 48 || tree->right->s[0] == 48))  //5 * 0 = 0
	{
	    	value = 0;
		tree->left = NULL;
		tree->right = NULL;
		//tree = calloc(1, sizeof(struct Node));
		tree->s = calloc(10, sizeof(char));
		sprintf(tree->s, "%d", value);
		change_flag = 1;

   	}
	else if(tree->s[0] == '+' && (tree->left->s[0] > 48 && tree->left->s[0] < 58) && (tree->right->s[0] > 48 && tree->right->s[0] < 58)) //2 + 5 = 7
	{
		value = atoi(tree->left->s) + atoi(tree->right->s);
		tree_destroy(tree);
		tree = calloc(1, sizeof(struct Node));
		tree->s = calloc(10, sizeof(char));
		sprintf(tree->s, "%d", value);
		change_flag = 1;
	}
	else if(tree->s[0] == '*' && (tree->left->s[0] > 47 && tree->left->s[0] < 58) && (tree->right->s[0] == '*') && (tree->right->left->s[0] > 47 && tree->right->left->s[0] < 58)) //2*3*x = 6*x
	{
		tmp = copy(tree->right->right);
		value = atoi(tree->left->s) * atoi(tree->right->left->s);
		tree_destroy(tree);
		tree = calloc(1, sizeof(struct Node));
		tree->s = "*";
		tree->left = calloc(1, sizeof(struct Node));
		tree->left->s = calloc(10, sizeof(char));
		sprintf(tree->left->s, "%d", value);

		tree->right = copy(tmp);
		change_flag = 1;
	}
	else if(tree->s[0] == '*' && (tree->left->s[0] > 47 && tree->left->s[0] < 58) && (tree->right->s[0] == '*') && (tree->right->right->s[0] > 47 && tree->right->right->s[0] < 58)) //3 * 2 * x = 6 *x
	{
		tmp = copy(tree->right->left);
		value = atoi(tree->left->s) * atoi(tree->right->right->s);
		tree_destroy(tree);
		tree = calloc(1, sizeof(struct Node));
		tree->s = "*";
		tree->left = calloc(1, sizeof(struct Node));
		tree->left->s = calloc(10, sizeof(char));
		sprintf(tree->left->s, "%d", value);

		tree->right = copy(tmp);
		change_flag = 1;
	}
	else if(tree->s[0] == '*' && (tree->right->s[0] > 47 && tree->right->s[0] < 58) && (tree->left->s[0] == '*') && (tree->left->left->s[0] > 47 && tree->left->left->s[0] < 58)) // 2 * x * 2 = 4
	{
		tmp = copy(tree->left->right);
		value = atoi(tree->right->s) * atoi(tree->left->left->s);
		tree_destroy(tree);
		tree = calloc(1, sizeof(struct Node));
		tree->s = "*";
		tree->left = calloc(1, sizeof(struct Node));
		tree->left->s = calloc(10, sizeof(char));
		sprintf(tree->left->s, "%d", value);

		tree->right = copy(tmp);
		change_flag = 1;
	}
	else if(tree->s[0] == '*' && (tree->right->s[0] > 47 && tree->right->s[0] < 58) && (tree->left->s[0] == '*') && (tree->left->right->s[0] > 47 && tree->left->right->s[0] < 58)) // x * 2 * 3 = 6 * x
	{
		tmp = copy(tree->left->left);
		value = atoi(tree->right->s) * atoi(tree->left->right->s);
		tree_destroy(tree);
		tree = calloc(1, sizeof(struct Node));
		tree->s = "*";
		tree->left = calloc(1, sizeof(struct Node));
		tree->left->s = calloc(10, sizeof(char));
		sprintf(tree->left->s, "%d", value);

		tree->right = copy(tmp);
		change_flag = 1;
	}
	else if(tree->s[0] == '-' && (tree->left->s[0] > 48 && tree->left->s[0] < 58) && (tree->right->s[0] > 48 && tree->right->s[0] < 58)) //2 - 5 = -3
	{
		value = atoi(tree->right->s) - atoi(tree->left->s);
		tree_destroy(tree);
		tree = calloc(1, sizeof(struct Node));
		tree->s = "-";
		tree->left = calloc(1, sizeof(struct Node));
		tree->left->s = "0";
		tree->right = calloc(1, sizeof(struct Node));
		tree->right->s = calloc(10,sizeof(struct Node));
		sprintf(tree->right->s, "%d", value);
		change_flag = 1;
	}
	else if(tree->s[0] == '-' && tree->left->s[0] == '0' && tree->right->s[0] == '0') //0 - 0 = 0
	{
		tree_destroy(tree);
		tree = calloc(1, sizeof(struct Node));
		tree->s = "0";
		change_flag = 1;
	}
	else if(tree->s[0] == '+' && (tree->left->s[0] == '-') && (tree->left->left->s[0] == '0') &&
			(tree->left->right->s[0] > 47 && tree->left->right->s[0] < 58) && (tree->right->s[0] > 47 && tree->right->s[0] < 58)) //(-2) + 5 = -3
	{
		if(atoi(tree->right->s) >= atoi(tree->left->right->s))
		{
			value = atoi(tree->right->s) - atoi(tree->left->right->s);
			tree_destroy(tree);
			tree = calloc(1, sizeof(struct Node));
			tree->s = calloc(10, sizeof(char));
			sprintf(tree->s, "%d", value);
		}
		else
		{
			value = atoi(tree->left->right->s) - atoi(tree->right->s);
			tree_destroy(tree);
			tree = calloc(1, sizeof(struct Node));
			tree->s = "-";
			tree->left = calloc(1, sizeof(struct Node));
			tree->left->s = "0";
			tree->right = calloc(1, sizeof(struct Node));
			tree->right->s = calloc(10, sizeof(char));
			sprintf(tree->right->s, "%d", value);
		}

		change_flag = 1;
	}
	else if(tree->s[0] == '+' && (tree->right->s[0] == '-') && (tree->right->left->s[0] == '0') &&
			(tree->right->right->s[0] > 47 && tree->right->right->s[0] < 58) && (tree->left->s[0] > 47 && tree->left->s[0] < 58)) //5 + (-2) = -3
	{
		if(atoi(tree->left->s) >= atoi(tree->right->right->s))
		{
			value = atoi(tree->left->s) - atoi(tree->right->right->s);
			tree_destroy(tree);
			tree = calloc(1, sizeof(struct Node));
			tree->s = calloc(10, sizeof(char));
			sprintf(tree->s, "%d", value);
		}
		else
		{
			value = atoi(tree->right->right->s) - atoi(tree->left->s);
			tree_destroy(tree);
			tree = calloc(1, sizeof(struct Node));
			tree->s = "-";
			tree->left = calloc(1, sizeof(struct Node));
			tree->left->s = "0";
			tree->right = calloc(1, sizeof(struct Node));
			tree->right->s = calloc(10, sizeof(char));
			sprintf(tree->right->s, "%d", value);
		}

		change_flag = 1;
	}
	else if(tree->s[0] == '-' && (tree->right->s[0] == '-') && (tree->right->left->s[0] == '0') &&
			(tree->right->right->s[0] > 48 && tree->right->right->s[0] < 58) && (tree->left->s[0] > 48 && tree->left->s[0] < 58)) //5 - (-2) = -3
	{
		value = atoi(tree->left->s) + atoi(tree->right->right->s);
		tree_destroy(tree);
		tree = calloc(1, sizeof(struct Node));
		tree->s = calloc(10, sizeof(char));
		sprintf(tree->s, "%d", value);

		change_flag = 1;
	}
	else if(tree->s[0] == '-' && (tree->left->s[0] == '-') && (tree->left->left->s[0] == '0') &&
			(tree->left->right->s[0] > 48 && tree->left->right->s[0] < 58) && (tree->right->s[0] > 48 && tree->right->s[0] < 58)) //(-2) - 5 = -7
	{
		value = atoi(tree->left->right->s) + atoi(tree->right->s);
		tree_destroy(tree);
		tree = calloc(1, sizeof(struct Node));
		tree->s = "-";
		tree->left = calloc(1, sizeof(struct Node));
		tree->left->s = "0";
		tree->right = calloc(1, sizeof(struct Node));
		tree->right->s = calloc(10, sizeof(char));
		sprintf(tree->right->s, "%d", value);

		change_flag = 1;
	}
	else if(tree->s[0] == '*' && (tree->left->s[0] == '-') && (tree->left->left->s[0] == '0') &&
			(tree->left->right->s[0] > 48 && tree->left->right->s[0] < 58) && (tree->right->s[0] > 48 && tree->right->s[0] < 58)) //(-2)*5 = -10
	{
		value = atoi(tree->left->right->s) * atoi(tree->right->s);
		tree_destroy(tree);
		tree = calloc(1, sizeof(struct Node));
		tree->s = "-";
		tree->left = calloc(1, sizeof(struct Node));
		tree->left->s = "0";
		tree->right = calloc(1, sizeof(struct Node));
		tree->right->s = calloc(10, sizeof(char));
		sprintf(tree->right->s, "%d", value);

		change_flag = 1;
	}
	else if(tree->s[0] == '*' && (tree->right->s[0] == '-') && (tree->right->left->s[0] == '0') &&
			(tree->right->right->s[0] > 48 && tree->right->right->s[0] < 58) && (tree->left->s[0] > 48 && tree->left->s[0] < 58)) //(-2)*5 = -10
	{
		value = atoi(tree->right->right->s) * atoi(tree->left->s);
		tree_destroy(tree);
		tree = calloc(1, sizeof(struct Node));
		tree->s = "-";
		tree->left = calloc(1, sizeof(struct Node));
		tree->left->s = "0";
		tree->right = calloc(1, sizeof(struct Node));
		tree->right->s = calloc(10, sizeof(char));
		sprintf(tree->right->s, "%d", value);

		change_flag = 1;
	}




	if(tree->left != NULL && tree->right != NULL)
	{
		flag1 = optimise(tree->left, output);
		flag2 = optimise(tree->right, output);
		if(flag1 == 1 || flag2 == 1)
		{
			change_flag = 1;
		}
	}
	return change_flag;
}

