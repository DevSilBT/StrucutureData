// INFIX.C - Portable Version
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include "list.h"
#include "stack.h"
#include "queue.h"
#include "dlist.h"

#define MAX_EXPR 256
#define MAX_PATH 512

// --- VT100 Sequence Definitions ---
#define RESET_COLOR "\033[0m"
#define COLOR_RED "\033[1;31m"
#define COLOR_GREEN "\033[1;32m"
#define COLOR_YELLOW "\033[1;33m"
#define COLOR_BLUE "\033[1;34m"

// --- Portable screen clear function ---
void clear_screen() {
    printf("\033[2J\033[H");
    fflush(stdout);
}

/*
    Restore original color
*/
void reset_color() {
    printf(RESET_COLOR);
}

/*
    Set blue color
*/
void set_blue() {
    printf(COLOR_BLUE);
}

/*
    Set green color
*/
void set_green() {
    printf(COLOR_GREEN);
}

void set_yellow() {
    printf(COLOR_YELLOW);
}

/*
    Set red color
*/
void set_red() {
    printf(COLOR_RED);
}

// Structure for tokens
typedef struct {
    char type;      // 'N' = number, 'O' = operator, 'P' = parenthesis
    double value;
    char operator;
} Token;

// Structure for evaluation steps
typedef struct {
    double operand1;
    double operand2;
    char operator;
    double result;
} Step;

// Prototypes
int validate_syntax(const char *expr);
void tokenize(const char *expr, DList *tokens);
double evaluate_expression(DList *tokens, Queue *steps);
int precedence(char op);
int is_operator(char c);
double apply_operation(char op, double a, double b);
void show_steps(Queue *steps);
void free_token(void *data);
void free_step(void *data);

// NEW FUNCTIONS FOR SAVING FILE
void save_operations_to_file(Queue *steps, const char *expression, double result, const char *filename);
void show_steps_in_file(Queue *steps, FILE *file);
int get_file_path(char *path);

// Function to show table with format
void show_evaluation_table(DList *tokens);

// Main function
int main() {
    char expression[MAX_EXPR];
    char file_path[MAX_PATH];
    DList tokens;
    Queue steps;
    double result;
    
    clear_screen();
    
    printf("\n\n");
    set_green();
    printf("+===============================================================================================+\n");
    printf("|                                                                                               |\n");
    printf("|                       INFIX EXPRESSION CALCULATOR                                             |\n");
    printf("|                         EVALUATION STEP BY STEP                                               |\n");
    printf("|                                                                                               |\n");
    printf("+===============================================================================================+\n");
    reset_color();
    
    printf("\n");
    set_green();
    printf("+===============================================================================================+\n");
    printf("| OPERATOR HIERARCHY (from highest to lowest precedence):                                       |\n");
    printf("+===============================================================================================|\n");
    printf("|  1. ( )         Parentheses                                                                   |\n");
    printf("|  2. ^           Exponents                                                                     |\n");
    printf("|  3. * /         Multiplication and Division                                                   |\n");
    printf("|  4. + -         Addition and Subtraction                                                      |\n");
    printf("+===============================================================================================+\n");
    reset_color();
    
    printf("\n");
    set_green();
    printf("+===============================================================================================+\n");
    printf("| EVALUATION ALGORITHM:                                                                         |\n");
    printf("+===============================================================================================|\n");
    printf("|                                                                                               |\n");
    printf("|   Expression is read from LEFT TO RIGHT                                                      |\n");
    printf("|   Two stacks are used: one for numbers and one for operators                                 |\n");
    printf("|   Operators are processed according to their precedence                                      |\n");
    printf("|   Parentheses change the evaluation order                                                    |\n");
    printf("|                                                                                               |\n");
    printf("+===============================================================================================+\n");
    reset_color();
    
    printf("\n");
    set_green();
    printf("+===============================================================================================+\n");
    printf("| EXAMPLES:                                                                                     |\n");
    printf("+===============================================================================================|\n");
    printf("|   3+4*5           ->    23                                                                   |\n");
    printf("|   (3+4)*5         ->    35                                                                   |\n");
    printf("|   2^3+4*5         ->    28                                                                   |\n");
    printf("+===============================================================================================+\n");
    reset_color();

    while(1) {
        printf("\n");
        set_yellow();
        printf("  -> Enter the expression (or 'exit' to finish): ");
        set_blue();
        printf("");
        reset_color();
        fgets(expression, MAX_EXPR, stdin);

        // Remove newline
        expression[strcspn(expression, "\n")] = 0;

        // Check for exit
        if(strcmp(expression, "exit") == 0) {
            printf("\n");
            set_green();
            printf("  Thank you for using the calculator! Goodbye!\n");
            reset_color();
            printf("\n");
            break;
        }

        // Validate syntax
        printf("\n");
        set_yellow();
        printf("[1] Validating syntax...\n");
        reset_color();
        if(!validate_syntax(expression)) {
            set_red();
            printf("    ERROR: The expression has syntax errors.\n\n");
            reset_color();
            
            set_yellow();
            printf("  Do you want to try another expression? (y/n): ");
            reset_color();
            char answer;
            scanf(" %c", &answer);
            getchar();
            
            if(answer != 'y' && answer != 'Y') {
                break;
            }
            continue;
        }
        
        set_green();
        printf("    Syntax correct!\n");
        reset_color();

        // Tokenize
        printf("\n");
        set_yellow();
        printf("[2] Tokenizing expression...\n");
        reset_color();
        dlist_init(&tokens, free_token);
        tokenize(expression, &tokens);
        set_blue();
        printf("    Tokens processed: %d\n", dlist_size(&tokens));
        reset_color();

        // Evaluate expression step by step
        printf("\n");
        set_yellow();
        printf("[3] Evaluating expression with operation hierarchy...\n");
        reset_color();
        
        set_green();
        printf("+-------------------------------------------------------------------------------------------------+\n");
        reset_color();

        queue_init(&steps, free_step);
        result = evaluate_expression(&tokens, &steps);

        printf("\n");
        set_yellow();
        printf("[4] Evaluation steps (binary operations):\n");
        reset_color();
        
        set_green();
        printf("+-------------------------------------------------------------------------------------------------+\n");
        reset_color();
        
        show_steps(&steps);
        
        set_green();
        printf("+-------------------------------------------------------------------------------------------------+\n");
        reset_color();
        
        printf("\n");
        set_green();
        printf("[FINAL RESULT] = ");
        set_blue();
        printf("%.4f\n", result);
        reset_color();

        // NEW FEATURE: SAVE TO FILE
        printf("\n");
        set_yellow();
        printf("==============================================\n");
        printf("Do you want to save the operations to a file? (y/n): ");
        reset_color();
        char answer;
        scanf(" %c", &answer);
        getchar(); // Clear the buffer

        if(answer == 'y' || answer == 'Y') {
            if(get_file_path(file_path)) {
                save_operations_to_file(&steps, expression, result, file_path);
                set_green();
                printf("Operations saved to: %s\n", file_path);
                reset_color();
            }
        }
        
        set_yellow();
        printf("==============================================\n");
        reset_color();

        // Free memory
        dlist_destroy(&tokens);
        queue_destroy(&steps);
    }

    return 0;
}

// Validate expression syntax
int validate_syntax(const char *expr) {
    int i, len = strlen(expr);
    int parenthesis = 0;
    int expect_operand = 1;
    char previous = '\0';
    int in_number = 0;

    if(len == 0) {
        set_red();
        printf("    Error: Empty expression\n");
        reset_color();
        return 0;
    }

    for(i = 0; i < len; i++) {
        char c = expr[i];

        if(isspace(c)) {
            in_number = 0;
            continue;
        }

        if(c == '(') {
            if(!expect_operand && !is_operator(previous) && previous != '(' && previous != '\0') {
                set_red();
                printf("    Error: Unexpected opening parenthesis at position %d\n", i);
                reset_color();
                return 0;
            }
            parenthesis++;
            expect_operand = 1;
            in_number = 0;
            previous = c;
        }
        else if(c == ')') {
            if(expect_operand) {
                set_red();
                printf("    Error: Closing parenthesis after operator at position %d\n", i);
                reset_color();
                return 0;
            }
            parenthesis--;
            if(parenthesis < 0) {
                set_red();
                printf("    Error: Closing parenthesis without opening at position %d\n", i);
                reset_color();
                return 0;
            }
            expect_operand = 0;
            in_number = 0;
            previous = c;
        }
        else if(isdigit(c) || c == '.') {
            if(!expect_operand && !in_number) {
                set_red();
                printf("    Error: Unexpected number at position %d\n", i);
                reset_color();
                return 0;
            }
            expect_operand = 0;
            in_number = 1;
        }
        else if(is_operator(c)) {
            if((c == '-' || c == '+') && expect_operand) {
                expect_operand = 1;
            }
            else if(expect_operand) {
                set_red();
                printf("    Error: Unexpected operator '%c' at position %d\n", c, i);
                reset_color();
                return 0;
            }
            else {
                expect_operand = 1;
            }
            in_number = 0;
            previous = c;
        }
        else {
            set_red();
            printf("    Error: Invalid character '%c' at position %d\n", c, i);
            reset_color();
            return 0;
        }
    }

    if(parenthesis != 0) {
        set_red();
        printf("    Error: Unclosed parentheses\n");
        reset_color();
        return 0;
    }

    if(expect_operand) {
        set_red();
        printf("    Error: Incomplete expression\n");
        reset_color();
        return 0;
    }

    return 1;
}

// Tokenize the expression
void tokenize(const char *expr, DList *tokens) {
    int i = 0, len = strlen(expr);

    while(i < len) {
        if(isspace(expr[i])) {
            i++;
            continue;
        }

        Token *token = (Token*)malloc(sizeof(Token));

        if(isdigit(expr[i]) || expr[i] == '.') {
            char num_str[50];
            int j = 0;

            while(i < len && (isdigit(expr[i]) || expr[i] == '.')) {
                num_str[j++] = expr[i++];
            }
            num_str[j] = '\0';

            token->type = 'N';
            token->value = atof(num_str);

            if(dlist_size(tokens) == 0) {
                dlist_ins_next(tokens, NULL, token);
            } else {
                dlist_ins_next(tokens, dlist_tail(tokens), token);
            }
        }
        else {
            token->type = (expr[i] == '(' || expr[i] == ')') ? 'P' : 'O';
            token->operator = expr[i];

            if(dlist_size(tokens) == 0) {
                dlist_ins_next(tokens, NULL, token);
            } else {
                dlist_ins_next(tokens, dlist_tail(tokens), token);
            }
            i++;
        }
    }
}

// Evaluate expression using two stacks (numbers and operators)
double evaluate_expression(DList *tokens, Queue *steps) {
    Stack number_stack, operator_stack;
    DListNode *current;

    stack_init(&number_stack, NULL);
    stack_init(&operator_stack, NULL);

    current = dlist_head(tokens);

    while(current != NULL) {
        Token *token = (Token*)dlist_data(current);

        if(token->type == 'N') {
            // Number: push directly
            double *num = (double*)malloc(sizeof(double));
            *num = token->value;
            stack_push(&number_stack, num);
        }
        else if(token->type == 'O') {
            // Operator: process according to precedence
            while(stack_size(&operator_stack) > 0) {
                char *top_op = (char*)stack_peek(&operator_stack);

                if(*top_op == '(') break;

                // Check precedence
                if(precedence(token->operator) > precedence(*top_op)) break;

                // Right associativity for ^
                if(token->operator == '^' && *top_op == '^') break;

                // Perform operation
                char *op;
                stack_pop(&operator_stack, (void**)&op);

                double *num2, *num1;
                stack_pop(&number_stack, (void**)&num2);
                stack_pop(&number_stack, (void**)&num1);

                double result = apply_operation(*op, *num1, *num2);

                // Save step
                Step *step = (Step*)malloc(sizeof(Step));
                step->operand1 = *num1;
                step->operand2 = *num2;
                step->operator = *op;
                step->result = result;
                queue_enqueue(steps, step);

                // Push result
                double *res = (double*)malloc(sizeof(double));
                *res = result;
                stack_push(&number_stack, res);

                free(op);
                free(num1);
                free(num2);
            }

            char *new_op = (char*)malloc(sizeof(char));
            *new_op = token->operator;
            stack_push(&operator_stack, new_op);
        }
        else if(token->type == 'P') {
            if(token->operator == '(') {
                char *par = (char*)malloc(sizeof(char));
                *par = '(';
                stack_push(&operator_stack, par);
            }
            else if(token->operator == ')') {
                // Process until '(' is found
                while(stack_size(&operator_stack) > 0) {
                    char *op;
                    stack_pop(&operator_stack, (void**)&op);

                    if(*op == '(') {
                        free(op);
                        break;
                    }

                    double *num2, *num1;
                    stack_pop(&number_stack, (void**)&num2);
                    stack_pop(&number_stack, (void**)&num1);

                    double result = apply_operation(*op, *num1, *num2);

                    // Save step
                    Step *step = (Step*)malloc(sizeof(Step));
                    step->operand1 = *num1;
                    step->operand2 = *num2;
                    step->operator = *op;
                    step->result = result;
                    queue_enqueue(steps, step);

                    double *res = (double*)malloc(sizeof(double));
                    *res = result;
                    stack_push(&number_stack, res);

                    free(op);
                    free(num1);
                    free(num2);
                }
            }
        }

        current = dlist_next(current);
    }

    // Process remaining operators
    while(stack_size(&operator_stack) > 0) {
        char *op;
        stack_pop(&operator_stack, (void**)&op);

        double *num2, *num1;
        stack_pop(&number_stack, (void**)&num2);
        stack_pop(&number_stack, (void**)&num1);

        double result = apply_operation(*op, *num1, *num2);

        // Save step
        Step *step = (Step*)malloc(sizeof(Step));
        step->operand1 = *num1;
        step->operand2 = *num2;
        step->operator = *op;
        step->result = result;
        queue_enqueue(steps, step);

        double *res = (double*)malloc(sizeof(double));
        *res = result;
        stack_push(&number_stack, res);

        free(op);
        free(num1);
        free(num2);
    }

    // Get final result
    double *final_result;
    stack_pop(&number_stack, (void**)&final_result);
    double value = *final_result;
    free(final_result);

    stack_destroy(&number_stack);
    stack_destroy(&operator_stack);

    return value;
}

// Apply operation
double apply_operation(char op, double a, double b) {
    switch(op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/':
            if(b == 0) {
                set_red();
                printf("\nERROR: Division by zero!\n");
                reset_color();
                exit(1);
            }
            return a / b;
        case '^': return pow(a, b);
        default: return 0;
    }
}

// Show evaluation steps
void show_steps(Queue *steps) {
    ListNode *current = list_head(steps);
    int step_number = 1;

    // Table header
    set_green();
    printf("| %-6s | %-15s | %-10s | %-15s | %-15s |\n", 
           "Step", "Operand 1", "Operator", "Operand 2", "Result");
    reset_color();
    set_green();
    printf("+--------+-----------------+------------+-----------------+-----------------+\n");
    reset_color();

    while(current != NULL) {
        Step *step = (Step*)list_data(current);
        if (step != NULL) {
            printf("| ");
            set_blue();
            printf("%-6d", step_number++);
            reset_color();
            printf(" | ");
            set_yellow();
            printf("%-15.4f", step->operand1);
            reset_color();
            printf(" | ");
            set_red();
            printf("%-10c", step->operator);
            reset_color();
            printf(" | ");
            set_yellow();
            printf("%-15.4f", step->operand2);
            reset_color();
            printf(" | ");
            set_green();
            printf("%-15.4f", step->result);
            reset_color();
            printf(" |\n");
        }
        current = list_next(current);
    }
}

// Get operator precedence
int precedence(char op) {
    switch(op) {
        case '+':
        case '-': return 1;
        case '*':
        case '/': return 2;
        case '^': return 3;
        default: return 0;
    }
}

// Check if it's an operator
int is_operator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^');
}

// Free token
void free_token(void *data) {
    free(data);
}

// Free step
void free_step(void *data) {
    free(data);
}

// NEW FUNCTIONS FOR FILE HANDLING

void save_operations_to_file(Queue *steps, const char *expression, double result, const char *filename) {
    FILE *file = fopen(filename, "a");
    if(file == NULL) {
        set_red();
        printf("Error: Could not create/open the file '%s'\n", filename);
        reset_color();
        return;
    }

    // Write header with date and time
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char buffer[80];
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(file, "==============================================\n");
    fprintf(file, "SAVED OPERATION: %s\n", buffer);
    fprintf(file, "Expression: %s\n", expression);
    fprintf(file, "----------------------------------------------\n");
    fprintf(file, "Evaluation steps:\n");

    // Write steps
    show_steps_in_file(steps, file);

    fprintf(file, "----------------------------------------------\n");
    fprintf(file, "FINAL RESULT: %.4f\n", result);
    fprintf(file, "==============================================\n\n");

    fclose(file);
}

void show_steps_in_file(Queue *steps, FILE *file) {
    if (steps == NULL || file == NULL) return;

    ListNode *current = list_head(steps);
    int step_number = 1;

    while(current != NULL) {
        Step *step = (Step*)list_data(current);
        if (step != NULL) {
            fprintf(file, "Step %d: %.4f %c %.4f = %.4f\n",
                   step_number++,
                   step->operand1,
                   step->operator,
                   step->operand2,
                   step->result);
        }
        current = list_next(current);
    }
}

int get_file_path(char *path) {
    set_yellow();
    printf("Enter the file path and name (e.g., operations.txt or C:/my_operations.txt):\n> ");
    reset_color();

    if(fgets(path, MAX_PATH, stdin) == NULL) {
        set_red();
        printf("Error reading the path.\n");
        reset_color();
        return 0;
    }

    // Remove newline
    path[strcspn(path, "\n")] = 0;

    // Verify the path is not empty
    if(strlen(path) == 0) {
        set_red();
        printf("Error: The path cannot be empty.\n");
        reset_color();
        return 0;
    }

    // Verify it ends with .txt, if not, add it
    if(strlen(path) < 4 || strcmp(path + strlen(path) - 4, ".txt") != 0) {
        strcat(path, ".txt");
    }

    return 1;
}
