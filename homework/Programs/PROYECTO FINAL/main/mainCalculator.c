// mainCalculator.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h> // Added for timestamp
#include "stack.h"
#include "dlist.h"
#include "list.h"
#include "queue.h"

#define MAX_EXPR 256
// #define MAX_PATH 512 // COMENTADO: MAX_PATH ya está definido en stdlib.h

// --- VT100 Color Definitions ---
#define RESET_COLOR "\033[0m"
#define COLOR_RED "\033[1;31m"
#define COLOR_GREEN "\033[1;32m"
#define COLOR_YELLOW "\033[1;33m"
#define COLOR_BLUE "\033[1;34m"

void clear_screen() {
    printf("\033[2J\033[H");
    fflush(stdout);
}

void color_normal() { printf(RESET_COLOR); }
void color_blue() { printf(COLOR_BLUE); }
void color_green() { printf(COLOR_GREEN); }
void color_yellow() { printf(COLOR_YELLOW); }
void color_red() { printf(COLOR_RED); }

// --- Structures for storing evaluation steps ---
typedef struct {
    double op1;
    double op2;
    char op;
    double result;
} StepNumeric;

// --- NEW CENTRALIZED FUNCTION FOR SAVING ALL OPERATIONS - DECLARATION ADDED HERE ---
void save_all_operations_to_file(const char *original_expr, const char *type_original, const char *converted_expr, const char *type_converted, Queue *steps_queue, double final_numeric_result);

// --- Functions from POST-NUM.c logic ---
int extract_number_postnum(const char *expr, int start, double *num) {
    int i = start;
    while (expr[i] == ' ') i++;
    int start_num = i;
    int has_decimal = 0;
    while (isdigit(expr[i]) || (expr[i] == '.' && !has_decimal)) {
        if (expr[i] == '.') has_decimal = 1;
        i++;
    }
    if (i == start_num) return 0;
    char num_str[MAX_EXPR];
    strncpy(num_str, expr + start_num, i - start_num);
    num_str[i - start_num] = '\0';
    *num = atof(num_str);
    return i - start_num;
}

int precedence_postnum(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    return 0;
}

int is_operator_postnum(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^');
}

int validate_syntax_postnum(const char *infix) {
    int i;
    int len = strlen(infix);
    int parenthesis_balance = 0;
    int last_was_operand = 0;
    int last_was_operator = 0;
    int inside_number = 0;
    char c;
    char last_char = '\0';

    if (len == 0) {
        color_red();
        printf("\n  ERROR: The expression is empty\n");
        color_normal();
        return 0;
    }

    for (i = 0; i < len; i++) {
        c = infix[i];

        if (c == ' ') {
            inside_number = 0;
            continue;
        }

        if (!isdigit(c) && !isalpha(c) && !is_operator_postnum(c) && c != '(' && c != ')') {
            color_red();
            printf("\n  ERROR: Invalid character '%c' at position %d\n", c, i + 1);
            color_normal();
            return 0;
        }

        if (c == '(') {
            parenthesis_balance++;
            if (last_was_operand) {
                color_red();
                printf("\n  ERROR: Missing operator before parenthesis '(' at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            last_was_operator = 0;
            last_was_operand = 0;
            inside_number = 0;
        }
        else if (c == ')') {
            parenthesis_balance--;
            if (parenthesis_balance < 0) {
                color_red();
                printf("\n  ERROR: Closing parenthesis ')' without opening at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            if (last_was_operator) {
                color_red();
                printf("\n  ERROR: Missing operand before parenthesis ')' at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            if (last_char == '(') {
                color_red();
                printf("\n  ERROR: Empty parentheses '()' at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            last_was_operator = 0;
            last_was_operand = 1;
            inside_number = 0;
        }
        else if (isdigit(c)) {
            if (inside_number) {
                // Continues in the same number
            } else {
                if (last_was_operand) {
                    color_red();
                    printf("\n  ERROR: Missing operator between operands at position %d\n", i + 1);
                    color_normal();
                    return 0;
                }
                last_was_operand = 1;
                inside_number = 1;
            }
            last_was_operator = 0;
        }
        else if (isalpha(c)) {
            if (last_was_operand) {
                color_red();
                printf("\n  ERROR: Missing operator between operands at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            last_was_operand = 1;
            last_was_operator = 0;
            inside_number = 0;
        }
        else if (is_operator_postnum(c)) {
            if (last_was_operator) {
                color_red();
                printf("\n  ERROR: Two consecutive operators at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            if (i == 0) {
                color_red();
                printf("\n  ERROR: Expression cannot start with an operator\n");
                color_normal();
                return 0;
            }
            if (last_char == '(') {
                color_red();
                printf("\n  ERROR: Operator '%c' after parenthesis '(' at position %d\n", c, i + 1);
                color_normal();
                return 0;
            }
            last_was_operator = 1;
            last_was_operand = 0;
            inside_number = 0;
        }

        last_char = c;
    }

    if (parenthesis_balance > 0) {
        color_red();
        printf("\n  ERROR: Missing %d closing parentheses ')'\n", parenthesis_balance);
        color_normal();
        return 0;
    }

    if (last_was_operator) {
        color_red();
        printf("\n  ERROR: Expression cannot end with an operator\n");
        color_normal();
        return 0;
    }

    return 1;
}

double evaluate_postfix_numeric_main(const char *postfix, Queue *steps) {
    Stack stack;
    stack_init(&stack, NULL);

    char token_str[MAX_EXPR];
    int token_len = 0;
    int i = 0;

    while (postfix[i] != '\0') {
        char c = postfix[i];

        if (c == ' ') {
            i++;
            continue;
        }

        if (isdigit(c) || c == '.') {
            int num_len = extract_number_postnum(postfix, i, NULL);
            if (num_len > 0) {
                char num_str[MAX_EXPR];
                strncpy(num_str, postfix + i, num_len);
                num_str[num_len] = '\0';
                double *num_ptr = (double*)malloc(sizeof(double));
                *num_ptr = atof(num_str);
                stack_push(&stack, num_ptr);
                i += num_len;
                continue;
            }
        }
        else if (is_operator_postnum(c)) {
            if (stack_size(&stack) < 2) {
                color_red();
                printf("\nERROR: Not enough operands for operator '%c'\n", c);
                color_normal();
                while (stack_size(&stack) > 0) {
                    double *val;
                    stack_pop(&stack, (void**)&val);
                    free(val);
                }
                stack_destroy(&stack);
                return 0.0 / 0.0;
            }

            double *op2_ptr, *op1_ptr;
            stack_pop(&stack, (void**)&op2_ptr);
            stack_pop(&stack, (void**)&op1_ptr);

            double result;
            switch (c) {
                case '+': result = *op1_ptr + *op2_ptr; break;
                case '-': result = *op1_ptr - *op2_ptr; break;
                case '*': result = *op1_ptr * *op2_ptr; break;
                case '/':
                    if (*op2_ptr == 0.0) {
                        color_red();
                        printf("\nERROR: Division by zero\n");
                        color_normal();
                        free(op1_ptr); free(op2_ptr);
                        while (stack_size(&stack) > 0) {
                            double *val;
                            stack_pop(&stack, (void**)&val);
                            free(val);
                        }
                        stack_destroy(&stack);
                        return 0.0 / 0.0;
                    }
                    result = *op1_ptr / *op2_ptr;
                    break;
                case '^': result = pow(*op1_ptr, *op2_ptr); break;
                default: result = 0.0; break;
            }

            if (steps) { // Only record if a queue is passed
                StepNumeric *step = (StepNumeric*)malloc(sizeof(StepNumeric));
                step->op1 = *op1_ptr;
                step->op2 = *op2_ptr;
                step->op = c;
                step->result = result;
                queue_enqueue(steps, step);
            }

            free(op1_ptr);
            free(op2_ptr);

            double *res_ptr = (double*)malloc(sizeof(double));
            *res_ptr = result;
            stack_push(&stack, res_ptr);
        }
        else {
            color_red();
            printf("\nERROR: Invalid character '%c' in postfix expression\n", c);
            color_normal();
            while (stack_size(&stack) > 0) {
                double *val;
                stack_pop(&stack, (void**)&val);
                free(val);
            }
            stack_destroy(&stack);
            return 0.0 / 0.0;
        }
        i++;
    }

    if (stack_size(&stack) != 1) {
        color_red();
        printf("\nERROR: Invalid postfix expression structure\n");
        color_normal();
        while (stack_size(&stack) > 0) {
            double *val;
            stack_pop(&stack, (void**)&val);
            free(val);
        }
        stack_destroy(&stack);
        return 0.0 / 0.0;
    }

    double *final_result_ptr;
    stack_pop(&stack, (void**)&final_result_ptr);
    double final_result = *final_result_ptr;
    free(final_result_ptr);

    stack_destroy(&stack);
    return final_result;
}

void infix_to_postfix_main(const char *infix, char *postfix) {
    Stack stack;
    int i, j = 0, step = 1;
    char temp_operation[MAX_EXPR] = "";
    int length = strlen(infix);

    stack_init(&stack, free);

    temp_operation[0] = '\0';
    length = strlen(infix);

    for (i = 0; i < length; i++) {
        char c = infix[i];
        if (c == ' ') continue;

        if (isdigit(c)) {
            char number[50];
            int num_idx = 0;
            while (i < length && (isdigit(infix[i]) || infix[i] == '.')) {
                number[num_idx++] = infix[i++];
            }
            number[num_idx] = '\0';
            i--;
            strcat(temp_operation, number);
            strcat(temp_operation, " ");
        }
        else if (isalpha(c)) {
            temp_operation[j++] = c;
            temp_operation[j++] = ' ';
            temp_operation[j] = '\0';
        }
        else if (c == '(') {
            char *op_ptr = (char *)malloc(sizeof(char));
            *op_ptr = c;
            stack_push(&stack, op_ptr);
        }
        else if (c == ')') {
            char *op_ptr;
            while (stack_size(&stack) > 0) {
                char *top = (char *)stack_peek(&stack);
                if (top && *top == '(') {
                    stack_pop(&stack, (void **)&op_ptr);
                    free(op_ptr);
                    break;
                } else {
                    stack_pop(&stack, (void **)&op_ptr);
                    strcat(temp_operation, (char[]){*op_ptr, ' ', '\0'});
                    free(op_ptr);
                }
            }
        }
        else if (is_operator_postnum(c)) {
            while (stack_size(&stack) > 0) {
                char *top_op = (char *)stack_peek(&stack);
                if (top_op && *top_op != '(' && precedence_postnum(*top_op) >= precedence_postnum(c)) {
                    char *op_ptr;
                    stack_pop(&stack, (void **)&op_ptr);
                    strcat(temp_operation, (char[]){*op_ptr, ' ', '\0'});
                    free(op_ptr);
                } else {
                    break;
                }
            }
            char *op_ptr = (char *)malloc(sizeof(char));
            *op_ptr = c;
            stack_push(&stack, op_ptr);
        }
    }

    while (stack_size(&stack) > 0) {
        char *op_ptr;
        stack_pop(&stack, (void **)&op_ptr);
        strcat(temp_operation, (char[]){*op_ptr, ' ', '\0'});
        free(op_ptr);
    }

    strcpy(postfix, temp_operation);
    stack_destroy(&stack);
}

void run_postnum() {
    char infix[MAX_EXPR];
    char postfix[MAX_EXPR];
    char continue_choice;

    do {
        clear_screen();
        printf("\n\n");
        color_green();
        printf("+-------------------------------------------------------------------------------------------------+\n");
        printf("|                                                                                                 |\n");
        printf("|                    INFIX TO POSTFIX (NUMBERS) & EVALUATION                                      |\n");
        printf("|                                                                                                 |\n");
        printf("+-------------------------------------------------------------------------------------------------+\n");
        color_normal();

        printf("\n");
        color_yellow();
        printf("  -> Enter the infix expression (with numbers and operators): ");
        color_normal();
        fgets(infix, MAX_EXPR, stdin);
        infix[strcspn(infix, "\n")] = '\0';

        if (!validate_syntax_postnum(infix)) {
            printf("\n");
            color_red();
            printf("  The expression contains errors. Please correct the syntax.\n");
            color_normal();
            printf("\n");
            color_yellow();
            printf("  Press Enter to continue...");
            color_normal();
            getchar();
            continue;
        }

        infix_to_postfix_main(infix, postfix);
        printf("\n  Postfix Expression: %s\n", postfix);

        Queue steps;
        queue_init(&steps, free);
        double result = evaluate_postfix_numeric_main(postfix, &steps);
        if (isnan(result)) {
            printf("\n");
            color_red();
            printf("  Evaluation failed due to an error in the expression.\n");
            color_normal();
        } else {
            printf("\n");
            color_green();
            printf("  Numeric Result: %.4f\n", result);
            color_normal();
        }

        // --- SAVE ALL OPERATIONS ---
        save_all_operations_to_file(infix, "Infix", postfix, "Postfix", &steps, result);

        queue_destroy(&steps);

        printf("\n");
        color_yellow();
        printf("  Press Enter to continue...");
        color_normal();
        getchar();
    } while (0); // Only one iteration for this example
}

// --- Functions from PRE-NUM.c logic ---
int precedence_prefix(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    if (op == 's') return 4;  /* sqrt */
    return 0;
}

int is_operator_prefix(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == 's');
}

int validate_syntax_prefix(const char *infix) {
    int i;
    int len = strlen(infix);
    int parenthesis_balance = 0;
    int last_was_operand = 0;
    int last_was_operator = 0;
    int inside_number = 0;
    char c;
    char last_char = '\0';

    if (len == 0) {
        color_red();
        printf("\n  ERROR: The expression is empty\n");
        color_normal();
        return 0;
    }

    for (i = 0; i < len; i++) {
        c = infix[i];

        if (c == ' ') {
            inside_number = 0;
            continue;
        }

        if (!isdigit(c) && !isalpha(c) && !is_operator_prefix(c) && c != '(' && c != ')') {
            color_red();
            printf("\n  ERROR: Invalid character '%c' at position %d\n", c, i + 1);
            color_normal();
            return 0;
        }

        if (c == '(') {
            parenthesis_balance++;
            if (last_was_operand && !inside_number) {
                color_red();
                printf("\n  ERROR: Missing operator before parenthesis '(' at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            last_was_operator = 0;
            last_was_operand = 0;
            inside_number = 0;
        }
        else if (c == ')') {
            parenthesis_balance--;
            if (parenthesis_balance < 0) {
                color_red();
                printf("\n  ERROR: Closing parenthesis ')' without opening at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            if (last_was_operator) {
                color_red();
                printf("\n  ERROR: Missing operand before parenthesis ')' at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            if (last_char == '(') {
                color_red();
                printf("\n  ERROR: Empty parentheses '()' at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            last_was_operator = 0;
            last_was_operand = 1;
            inside_number = 0;
        }
        else if (isdigit(c)) {
            if (inside_number) {
                // Continues in the same number
            } else {
                if (last_was_operand) {
                    color_red();
                    printf("\n  ERROR: Missing operator between numbers at position %d\n", i + 1);
                    color_normal();
                    return 0;
                }
                last_was_operand = 1;
                inside_number = 1;
            }
            last_was_operator = 0;
        }
        else if (isalpha(c)) {
            if (last_was_operand && !inside_number) {
                color_red();
                printf("\n  ERROR: Missing operator between operands at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            last_was_operand = 1;
            last_was_operator = 0;
            inside_number = 0;
        }
        else if (is_operator_prefix(c)) {
            if (last_was_operator) {
                color_red();
                printf("\n  ERROR: Two consecutive operators at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            if (i == 0) {
                color_red();
                printf("\n  ERROR: Expression cannot start with an operator\n");
                color_normal();
                return 0;
            }
            if (last_char == '(') {
                color_red();
                printf("\n  ERROR: Operator '%c' after parenthesis '(' at position %d\n", c, i + 1);
                color_normal();
                return 0;
            }
            last_was_operator = 1;
            last_was_operand = 0;
            inside_number = 0;
        }

        last_char = c;
    }

    if (parenthesis_balance > 0) {
        color_red();
        printf("\n  ERROR: Missing %d closing parenthesis(es) ')'\n", parenthesis_balance);
        color_normal();
        return 0;
    }

    if (last_was_operator) {
        color_red();
        printf("\n  ERROR: Expression cannot end with an operator\n");
        color_normal();
        return 0;
    }

    return 1;
}

double evaluate_prefix_numeric_main(const char *prefix, Queue *steps) {
    char expression[MAX_EXPR];
    char result_str[MAX_EXPR];
    int i, j, k;
    int changes = 1;
    int length;
    char num1_str[MAX_EXPR], num2_str[MAX_EXPR];
    int advance_num1, advance_num2;
    int pos_after_num1, pos_after_num2;
    int pos_num1; // --- CORRECCIÓN: Declarar pos_num1 aquí ---
    double operation_result;

    strcpy(expression, prefix);

    while (changes) {
        changes = 0;
        length = strlen(expression);

        for (i = 0; i < length; i++) {
            if (expression[i] == ' ') continue;

            if (is_operator_prefix(expression[i])) {
                int pos_op = i;
                pos_num1 = i + 1; // --- CORRECCIÓN: Ahora pos_num1 está declarada ---
                while (pos_num1 < length && expression[pos_num1] == ' ') pos_num1++;
                if (pos_num1 < length && isdigit(expression[pos_num1])) {
                    advance_num1 = extract_number_postnum(expression, pos_num1, NULL); // Reuse function from POST-NUM
                    if (advance_num1 > 0) {
                        strncpy(num1_str, expression + pos_num1, advance_num1);
                        num1_str[advance_num1] = '\0';
                        pos_after_num1 = pos_num1 + advance_num1;
                        while (pos_after_num1 < length && expression[pos_after_num1] == ' ') pos_after_num1++;
                        if (pos_after_num1 < length && isdigit(expression[pos_after_num1])) {
                            advance_num2 = extract_number_postnum(expression, pos_after_num1, NULL);
                            if (advance_num2 > 0) {
                                strncpy(num2_str, expression + pos_after_num1, advance_num2);
                                num2_str[advance_num2] = '\0';
                                pos_after_num2 = pos_after_num1 + advance_num2;

                                double a = atof(num1_str);
                                double b = atof(num2_str);

                                switch (expression[pos_op]) {
                                    case '+': operation_result = a + b; break;
                                    case '-': operation_result = a - b; break;
                                    case '*': operation_result = a * b; break;
                                    case '/': operation_result = (b != 0.0) ? a / b : 0.0; break;
                                    case '^': operation_result = pow(a, b); break;
                                    case 's': operation_result = sqrt(a); pos_after_num2 = pos_after_num1; break; // Unary
                                    default: operation_result = 0.0; break;
                                }

                                if (steps && expression[pos_op] != 's') { // Only record binary operations
                                    StepNumeric *step = (StepNumeric*)malloc(sizeof(StepNumeric));
                                    step->op1 = a;
                                    step->op2 = b;
                                    step->op = expression[pos_op];
                                    step->result = operation_result;
                                    queue_enqueue(steps, step);
                                }

                                j = 0;
                                for (k = 0; k < pos_op; k++) result_str[j++] = expression[k];
                                if (expression[pos_op] == 's') { // For 's', replace 's' + num1 only
                                    sprintf(result_str + j, "%.4f", operation_result);
                                    j = strlen(result_str);
                                    for (k = pos_after_num2; k < length; k++) result_str[j++] = expression[k];
                                } else { // For binary
                                    sprintf(result_str + j, "%.4f", operation_result);
                                    j = strlen(result_str);
                                    for (k = pos_after_num2; k < length; k++) result_str[j++] = expression[k];
                                }
                                result_str[j] = '\0';

                                strcpy(expression, result_str);
                                changes = 1;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    int only_number = 1;
    int has_digit = 0;
    for (i = 0; i < strlen(expression); i++) {
        if (isdigit(expression[i]) || expression[i] == '.') {
            has_digit = 1;
        } else if (expression[i] != ' ') {
            only_number = 0;
            break;
        }
    }

    if (only_number && has_digit) {
        return atof(expression);
    } else {
        return 0.0 / 0.0; // NaN
    }
}

void infix_to_prefix_main(const char *infix, char *prefix) {
    Stack stack;
    int i;
    int j = 0;
    char c;
    char *op_ptr;
    char temp_operation[MAX_EXPR];
    int length;

    stack_init(&stack, free);
    temp_operation[0] = '\0';
    length = strlen(infix);

    for (i = length - 1; i >= 0; i--) {
        c = infix[i];
        if (c == ' ') continue;

        if (isdigit(c)) {
            int start_num = i;
            while (start_num >= 0 && (isdigit(infix[start_num]) || infix[start_num] == '.')) {
                start_num--;
            }
            start_num++;
            int num_len = i - start_num + 1;
            for (int k = start_num; k <= i; k++) {
                temp_operation[j++] = infix[k];
            }
            temp_operation[j++] = ' ';
            i = start_num;
        }
        else if (isalpha(c)) {
            temp_operation[j++] = c;
            temp_operation[j++] = ' ';
        }
        else if (c == ')') {
            op_ptr = (char *)malloc(sizeof(char));
            *op_ptr = c;
            stack_push(&stack, op_ptr);
        }
        else if (c == '(') {
            char *op_ptr_internal; // --- CORRECCIÓN: Declarar op_ptr_internal aquí ---
            while (stack_size(&stack) > 0) {
                stack_pop(&stack, (void **)&op_ptr_internal); // --- CORRECCIÓN: op_ptr_internal ya está declarada ---
                if (*op_ptr_internal == ')') {
                    free(op_ptr_internal);
                    break;
                } else {
                    temp_operation[j++] = *op_ptr_internal;
                    temp_operation[j++] = ' ';
                    free(op_ptr_internal);
                }
            }
        }
        else if (is_operator_prefix(c)) {
            while (stack_size(&stack) > 0) {
                char *top_op = (char *)stack_peek(&stack);
                if (top_op && *top_op != '(' && precedence_prefix(*top_op) >= precedence_prefix(c)) {
                    char *op_ptr_internal; // --- CORRECCIÓN: Declarar op_ptr_internal aquí ---
                    stack_pop(&stack, (void **)&op_ptr_internal); // --- CORRECCIÓN: op_ptr_internal ya está declarada ---
                    temp_operation[j++] = *op_ptr_internal;
                    temp_operation[j++] = ' ';
                    free(op_ptr_internal);
                } else {
                    break;
                }
            }
            op_ptr = (char *)malloc(sizeof(char));
            *op_ptr = c;
            stack_push(&stack, op_ptr);
        }
    }

    while (stack_size(&stack) > 0) {
        stack_pop(&stack, (void **)&op_ptr);
        temp_operation[j++] = *op_ptr;
        temp_operation[j++] = ' ';
        free(op_ptr);
    }

    temp_operation[j] = '\0';

    DList list;
    dlist_init(&list, NULL);
    for (i = 0; i < j; i++) {
        if (temp_operation[i] != ' ') {
            char *character = (char *)malloc(sizeof(char));
            *character = temp_operation[i];
            dlist_ins_next(&list, dlist_tail(&list), character);
        }
    }

    int k = 0;
    DListNode *node = dlist_tail(&list);
    while (node != NULL) {
        prefix[k++] = *(char *)dlist_data(node);
        node = dlist_prev(node);
    }
    prefix[k] = '\0';

    dlist_destroy(&list);
    stack_destroy(&stack);
}

void run_prenum() {
    char infix[MAX_EXPR];
    char prefix[MAX_EXPR];
    char continue_choice;

    do {
        clear_screen();
        printf("\n\n");
        color_green();
        printf("+-------------------------------------------------------------------------------------------------+\n");
        printf("|                                                                                                 |\n");
        printf("|                    INFIX TO PREFIX (NUMBERS) & EVALUATION                                       |\n");
        printf("|                                                                                                 |\n");
        printf("+-------------------------------------------------------------------------------------------------+\n");
        color_normal();

        printf("\n");
        printf("+-------------------------------------------------------------------------------------------------+\n");
        printf("| OPERATOR HIERARCHY (from highest to lowest precedence):                                         |\n");
        printf("+-------------------------------------------------------------------------------------------------|\n");
        printf("|  1. ( )         Parentheses                                                                     |\n");
        printf("|  2. s           Square roots                                                                     |\n");
        printf("|  3. ^           Exponents                                                                      |\n");
        printf("|  4. * /         Multiplication and Division                                                     |\n");
        printf("|  5. + -         Addition and Subtraction                                                        |\n");
        printf("+-------------------------------------------------------------------------------------------------+\n");

        printf("\n");
        color_yellow();
        printf("  -> Enter the infix expression (without spaces): ");
        color_normal();
        fgets(infix, MAX_EXPR, stdin);
        infix[strcspn(infix, "\n")] = '\0';

        if (!validate_syntax_prefix(infix)) {
            printf("\n");
            color_red();
            printf("  The expression contains errors. Please correct the syntax.\n");
            color_normal();
            printf("\n");
            color_yellow();
            printf("  Press Enter to continue...");
            color_normal();
            getchar();
            continue;
        }

        infix_to_prefix_main(infix, prefix);
        printf("\n  Prefix Expression: %s\n", prefix);

        Queue steps;
        queue_init(&steps, free);
        double result = evaluate_prefix_numeric_main(prefix, &steps);
        if (isnan(result)) {
            printf("\n");
            color_red();
            printf("  Evaluation failed due to an error in the expression.\n");
            color_normal();
        } else {
            printf("\n");
            color_green();
            printf("  Numeric Result: %.4f\n", result);
            color_normal();
        }

        // --- SAVE ALL OPERATIONS ---
        save_all_operations_to_file(infix, "Infix", prefix, "Prefix", &steps, result);

        queue_destroy(&steps);

        printf("\n");
        color_yellow();
        printf("  Press Enter to continue...");
        color_normal();
        getchar();
    } while (0); // Only one iteration for this example
}

// --- Functions from POSTFIX LETTERS.c logic ---
void verify_postfix_letters_main(const char *postfix, char *final_result) {
    char expression[MAX_EXPR];
    char result[MAX_EXPR];
    int i, j, k;
    int step = 1;
    char new_letter = 'Z';

    strcpy(expression, postfix);

    printf("\n");
    color_green();
    printf("+-------------------------------------------------------------------------------------------------+\n");
    printf("|                          POSTFIX EXPRESSION VERIFICATION (Letters)                              |\n");
    printf("|                              STEP BY STEP (LEFT -> RIGHT)                                       |\n");
    printf("+-------------------------------------------------------------------------------------------------+\n");
    color_normal();

    printf("\n");
    color_yellow();
    printf("  Postfix Expression to Verify: %s\n", expression);
    color_normal();

    printf("\n");
    color_green();
    printf("+-------------------------------------------------------------------------------------------------+\n");
    printf("| STEP |         OPERATION FOUND              |         RESULTING EXPRESSION                    |\n");
    printf("+-------------------------------------------------------------------------------------------------|\n");

    while (1) {
        int found = 0;
        int len = strlen(expression);

        for (i = 0; i < len - 2; i++) {
            if (isalpha(expression[i]) && isalpha(expression[i+1]) && is_operator_postnum(expression[i+2])) {
                found = 1;
                break;
            }
        }

        if (!found) break;

        printf("|  %3d |   %c%c%c = %c                         |   ", step, expression[i], expression[i+1], expression[i+2], new_letter);

        j = 0;
        for (k = 0; k < i; k++) result[j++] = expression[k];
        result[j++] = new_letter;
        for (k = i + 3; k < len; k++) result[j++] = expression[k];
        result[j] = '\0';

        printf("%s", result);
        for (k = 0; k < 45 - strlen(result); k++) printf(" ");
        printf("|\n");

        strcpy(expression, result);
        new_letter--;
        if (new_letter < 'A') new_letter = 'Z';
        step++;
    }

    printf("+-------------------------------------------------------------------------------------------------+\n");

    printf("\n");
    if (strlen(expression) == 1 && isalpha(expression[0])) {
        color_green();
        printf("  Status: SUCCESSFUL VERIFICATION - Expression reduced to a single letter: %c\n", expression[0]);
        color_normal();
        strcpy(final_result, expression); // Copy final letter to result string
    } else {
        color_red();
        printf("  Status: INCONCLUSIVE VERIFICATION - Expression not completely reduced: %s\n", expression);
        color_normal();
        strcpy(final_result, expression); // Copy incomplete result
    }
}

int validate_syntax_postfix_letters(const char *infix) {
    int i;
    int len = strlen(infix);
    int parenthesis_balance = 0;
    int last_was_operand = 0;
    int last_was_operator = 0;
    char c;
    char last_char = '\0';

    if (len == 0) {
        color_red();
        printf("\n  ERROR: The expression is empty\n");
        color_normal();
        return 0;
    }

    for (i = 0; i < len; i++) {
        c = infix[i];

        if (c == ' ') continue;

        if (!isalpha(c) && !is_operator_postnum(c) && c != '(' && c != ')') {
            color_red();
            printf("\n  ERROR: Invalid character '%c' at position %d\n", c, i + 1);
            color_normal();
            return 0;
        }

        if (c == '(') {
            parenthesis_balance++;
            if (last_was_operand) {
                color_red();
                printf("\n  ERROR: Missing operator before parenthesis '(' at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            last_was_operator = 0;
            last_was_operand = 0;
        }
        else if (c == ')') {
            parenthesis_balance--;
            if (parenthesis_balance < 0) {
                color_red();
                printf("\n  ERROR: Closing parenthesis ')' without opening at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            if (last_was_operator) {
                color_red();
                printf("\n  ERROR: Missing operand before parenthesis ')' at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            if (last_char == '(') {
                color_red();
                printf("\n  ERROR: Empty parentheses '()' at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            last_was_operator = 0;
            last_was_operand = 1;
        }
        else if (isalpha(c)) {
            if (last_was_operand) {
                color_red();
                printf("\n  ERROR: Two consecutive operands at position %d (missing operator)\n", i + 1);
                color_normal();
                return 0;
            }
            last_was_operand = 1;
            last_was_operator = 0;
        }
        else if (is_operator_postnum(c)) {
            if (last_was_operator) {
                color_red();
                printf("\n  ERROR: Two consecutive operators at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            if (i == 0) {
                color_red();
                printf("\n  ERROR: Expression cannot start with an operator\n");
                color_normal();
                return 0;
            }
            if (last_char == '(') {
                color_red();
                printf("\n  ERROR: Operator '%c' after parenthesis '(' at position %d\n", c, i + 1);
                color_normal();
                return 0;
            }
            last_was_operator = 1;
            last_was_operand = 0;
        }

        last_char = c;
    }

    if (parenthesis_balance > 0) {
        color_red();
        printf("\n  ERROR: Missing %d closing parentheses ')'\n", parenthesis_balance);
        color_normal();
        return 0;
    }

    if (last_was_operator) {
        color_red();
        printf("\n  ERROR: Expression cannot end with an operator\n");
        color_normal();
        return 0;
    }

    return 1;
}

void run_postfix_letters() {
    char infix[MAX_EXPR];
    char postfix[MAX_EXPR];
    char final_verification_result[MAX_EXPR]; // To store the final letter or incomplete expression
    char continue_choice;

    do {
        clear_screen();
        printf("\n\n");
        color_green();
        printf("+-------------------------------------------------------------------------------------------------+\n");
        printf("|                                                                                                 |\n");
        printf("|                 INFIX TO POSTFIX (LETTERS) & VERIFICATION                                       |\n");
        printf("|                                                                                                 |\n");
        printf("+-------------------------------------------------------------------------------------------------+\n");
        color_normal();

        printf("\n");
        color_yellow();
        printf("  -> Enter the infix expression (letters and operators only): ");
        color_normal();
        fgets(infix, MAX_EXPR, stdin);
        infix[strcspn(infix, "\n")] = '\0';

        if (!validate_syntax_postfix_letters(infix)) {
            printf("\n");
            color_red();
            printf("  The expression contains errors. Please correct the syntax.\n");
            color_normal();
            printf("\n");
            color_yellow();
            printf("  Press Enter to continue...");
            color_normal();
            getchar();
            continue;
        }

        infix_to_postfix_main(infix, postfix); // Reuse conversion from numbers, works the same for letters
        printf("\n  Postfix Expression: %s\n", postfix);

        verify_postfix_letters_main(postfix, final_verification_result); // Pass result string

        // --- SAVE ALL OPERATIONS ---
        save_all_operations_to_file(infix, "Infix", postfix, "Postfix", NULL, 0.0); // No numeric steps, pass 0.0

        printf("\n");
        color_yellow();
        printf("  Press Enter to continue...");
        color_normal();
        getchar();
    } while (0); // Only one iteration for this example
}

// --- Functions from PRE-LETTERS.c logic ---
void verify_prefix_letters_main(const char *prefix, char *final_result) {
    char expression[MAX_EXPR];
    char result[MAX_EXPR];
    int i, j, k;
    int step = 1;
    char new_var = 'Z';
    int changes = 1;

    strcpy(expression, prefix);

    printf("\n");
    color_green();
    printf("+-------------------------------------------------------------------------------------------------+\n");
    printf("|                          PREFIX EXPRESSION VERIFICATION (Letters)                               |\n");
    printf("|                              STEP BY STEP (LEFT -> RIGHT)                                       |\n");
    printf("+-------------------------------------------------------------------------------------------------+\n");
    color_normal();

    printf("\n");
    color_yellow();
    printf("  Prefix Expression to Verify: %s\n", expression);
    color_normal();

    printf("\n");
    color_green();
    printf("+-------------------------------------------------------------------------------------------------+\n");
    printf("| STEP |         OPERATION FOUND              |         RESULTING EXPRESSION                    |\n");
    printf("+-------------------------------------------------------------------------------------------------|\n");

    while (changes) {
        changes = 0;
        int length = strlen(expression);

        for (i = 0; i < length - 2; i++) {
            if (is_operator_prefix(expression[i]) && isalpha(expression[i+1]) && isalpha(expression[i+2])) {
                printf("|  %3d |   %c%c%c = %c                         |   ", step, expression[i], expression[i+1], expression[i+2], new_var);

                j = 0;
                for (k = 0; k < i; k++) result[j++] = expression[k];
                result[j++] = new_var;
                for (k = i + 3; k < length; k++) result[j++] = expression[k];
                result[j] = '\0';

                printf("%s", result);
                for (k = 0; k < 45 - strlen(result); k++) printf(" ");
                printf("|\n");

                strcpy(expression, result);
                new_var--;
                if (new_var < 'A') new_var = 'Z';
                changes = 1;
                step++;
                break; // Restart search
            }
        }
    }

    printf("+-------------------------------------------------------------------------------------------------+\n");

    printf("\n");
    if (strlen(expression) == 1 && isalpha(expression[0])) {
        color_green();
        printf("  Status: SUCCESSFUL VERIFICATION - Expression reduced to a single letter: %c\n", expression[0]);
        color_normal();
        strcpy(final_result, expression); // Copy final letter to result string
    } else {
        color_red();
        printf("  Status: INCONCLUSIVE VERIFICATION - Expression not completely reduced: %s\n", expression);
        color_normal();
        strcpy(final_result, expression); // Copy incomplete result
    }
}

int validate_syntax_prefix_letters(const char *infix) {
    int i;
    int len = strlen(infix);
    int parenthesis_balance = 0;
    int last_was_operand = 0;
    int last_was_operator = 0;
    int inside_number = 0; // Not applicable here
    char c;
    char last_char = '\0';

    if (len == 0) {
        color_red();
        printf("\n  ERROR: The expression is empty\n");
        color_normal();
        return 0;
    }

    for (i = 0; i < len; i++) {
        c = infix[i];

        if (c == ' ') continue;

        if (!isalpha(c) && !is_operator_prefix(c) && c != '(' && c != ')') {
            color_red();
            printf("\n  ERROR: Invalid character '%c' at position %d\n", c, i + 1);
            color_normal();
            return 0;
        }

        if (c == '(') {
            parenthesis_balance++;
            if (last_was_operand) {
                color_red();
                printf("\n  ERROR: Missing operator before parenthesis '(' at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            last_was_operator = 0;
            last_was_operand = 0;
        }
        else if (c == ')') {
            parenthesis_balance--;
            if (parenthesis_balance < 0) {
                color_red();
                printf("\n  ERROR: Closing parenthesis ')' without opening at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            if (last_was_operator) {
                color_red();
                printf("\n  ERROR: Missing operand before parenthesis ')' at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            if (last_char == '(') {
                color_red();
                printf("\n  ERROR: Empty parentheses '()' at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            last_was_operator = 0;
            last_was_operand = 1;
        }
        else if (isalpha(c)) {
            if (last_was_operand) {
                color_red();
                printf("\n  ERROR: Two consecutive operands at position %d (missing operator)\n", i + 1);
                color_normal();
                return 0;
            }
            last_was_operand = 1;
            last_was_operator = 0;
        }
        else if (is_operator_prefix(c)) {
            if (last_was_operator) {
                color_red();
                printf("\n  ERROR: Two consecutive operators at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            if (i == 0) {
                color_red();
                printf("\n  ERROR: Expression cannot start with an operator\n");
                color_normal();
                return 0;
            }
            if (last_char == '(') {
                color_red();
                printf("\n  ERROR: Operator '%c' after parenthesis '(' at position %d\n", c, i + 1);
                color_normal();
                return 0;
            }
            last_was_operator = 1;
            last_was_operand = 0;
        }

        last_char = c;
    }

    if (parenthesis_balance > 0) {
        color_red();
        printf("\n  ERROR: Missing %d closing parentheses ')'\n", parenthesis_balance);
        color_normal();
        return 0;
    }

    if (last_was_operator) {
        color_red();
        printf("\n  ERROR: Expression cannot end with an operator\n");
        color_normal();
        return 0;
    }

    return 1;
}

void run_prefix_letters() {
    char infix[MAX_EXPR];
    char prefix[MAX_EXPR];
    char final_verification_result[MAX_EXPR]; // To store the final letter or incomplete expression
    char continue_choice;

    do {
        clear_screen();
        printf("\n\n");
        color_green();
        printf("+-------------------------------------------------------------------------------------------------+\n");
        printf("|                                                                                                 |\n");
        printf("|                  INFIX TO PREFIX (LETTERS) & VERIFICATION                                       |\n");
        printf("|                                                                                                 |\n");
        printf("+-------------------------------------------------------------------------------------------------+\n");
        color_normal();

        printf("\n");
        color_yellow();
        printf("  -> Enter the infix expression (letters and operators only): ");
        color_normal();
        fgets(infix, MAX_EXPR, stdin);
        infix[strcspn(infix, "\n")] = '\0';

        if (!validate_syntax_prefix_letters(infix)) {
            printf("\n");
            color_red();
            printf("  The expression contains errors. Please correct the syntax.\n");
            color_normal();
            printf("\n");
            color_yellow();
            printf("  Press Enter to continue...");
            color_normal();
            getchar();
            continue;
        }

        infix_to_prefix_main(infix, prefix); // Reuse conversion from numbers, works the same for letters
        printf("\n  Prefix Expression: %s\n", prefix);

        verify_prefix_letters_main(prefix, final_verification_result); // Pass result string

        // --- SAVE ALL OPERATIONS ---
        save_all_operations_to_file(infix, "Infix", prefix, "Prefix", NULL, 0.0); // No numeric steps, pass 0.0

        printf("\n");
        color_yellow();
        printf("  Press Enter to continue...");
        color_normal();
        getchar();
    } while (0); // Only one iteration for this example
}

// --- NEW CENTRALIZED FUNCTION FOR SAVING ALL OPERATIONS ---
void save_all_operations_to_file(const char *original_expr, const char *type_original, const char *converted_expr, const char *type_converted, Queue *steps_queue, double final_numeric_result) {
    FILE *archivo = fopen("all_operations_log.txt", "a"); // Append mode
    if(archivo == NULL) {
        color_red();
        printf("Error: Could not create/open the file 'all_operations_log.txt'\n");
        color_normal();
        return;
    }

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char buffer[80];
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(archivo, "==============================================\n");
    fprintf(archivo, "OPERATION LOGGED: %s\n", buffer);
    fprintf(archivo, "Original Expression (%s): %s\n", type_original, original_expr);
    fprintf(archivo, "Converted Expression (%s): %s\n", type_converted, converted_expr);

    if (steps_queue != NULL) {
        fprintf(archivo, "----------------------------------------------\n");
        fprintf(archivo, "Evaluation Steps:\n");
        ListNode *actual = list_head(steps_queue);
        int step_num = 1;
        fprintf(archivo, "| %-6s | %-15s | %-10s | %-15s | %-15s |\n",
               "Step", "Operand 1", "Operator", "Operand 2", "Result");
        fprintf(archivo, "+--------+-----------------+------------+-----------------+-----------------+\n");
        while(actual != NULL) {
            StepNumeric *step = (StepNumeric*)list_data(actual);
            if (step != NULL) {
                fprintf(archivo, "| %-6d | %-15.4f | %-10c | %-15.4f | %-15.4f |\n",
                       step_num++, step->op1, step->op, step->op2, step->result);
            }
            actual = list_next(actual);
        }
        fprintf(archivo, "\n");
    }

    fprintf(archivo, "----------------------------------------------\n");
    fprintf(archivo, "Final Numeric Result: %.4f\n", final_numeric_result);
    fprintf(archivo, "==============================================\n\n");

    fclose(archivo);

    // Print confirmation message to console
    printf("\n");
    color_green();
    printf("All operations and results saved to 'all_operations_log.txt'.\n");
    color_normal();
}

// --- Main Menu Function ---
int main() {
    int choice;

    while (1) {
        clear_screen();
        printf("\n\n");
        color_green();
        printf("  *********************************************************\n");
        printf("  *                 CALCULATOR MENU                         *\n");
        printf("  *********************************************************\n");
        printf("  *                                                         *\n");
        printf("  *  1. Infix to Postfix (Numbers) & Evaluation             *\n");
        printf("  *  2. Infix to Prefix (Numbers) & Evaluation              *\n");
        printf("  *  3. Infix to Postfix (Letters) & Verification           *\n");
        printf("  *  4. Infix to Prefix (Letters) & Verification            *\n");
        printf("  *  5. Infix Direct Evaluation (Numbers)                   *\n");
        printf("  *  0. Exit                                                *\n");
        printf("  *                                                         *\n");
        printf("  *********************************************************\n");
        printf("  -> Enter your choice (0-5): ");
        color_normal();
        if (scanf("%d", &choice) != 1) {
            // Handle non-numeric input
            while (getchar() != '\n'); // Clear buffer
            choice = -1; // Assign an invalid value
        }
        getchar(); // Consume the '\n' after the number

        switch (choice) {
            case 1:
                run_postnum();
                break;
            case 2:
                run_prenum();
                break;
            case 3:
                run_postfix_letters();
                break;
            case 4:
                run_prefix_letters();
                break;
            case 5:
                clear_screen();
                printf("\n\n");
                color_green();
                printf("  *********************************************************\n");
                printf("  *  INFIX DIRECT EVALUATION (Numbers) - DEMO             *\n");
                printf("  *********************************************************\n");
                color_normal();
                printf("\n  This function requires the full 'infix.c' logic.\n");
                printf("  It's not fully integrated into this simple menu.\n");
                printf("  However, the 'infix.c' file itself is fully functional.\n");
                printf("\n  Example: 3 + 4 * ( 2 - 1 ) evaluates to 7.0000\n");
                printf("\n");
                color_yellow();
                printf("  Press Enter to continue...");
                color_normal();
                getchar();
                break;
            case 0:
                printf("\n");
                color_green();
                printf("  Thank you for using the calculator. Goodbye!\n");
                color_normal();
                printf("\n");
                return 0;
            default:
                printf("\n");
                color_red();
                printf("  Invalid choice. Please enter a number between 0 and 5.\n");
                color_normal();
                printf("\n");
                color_yellow();
                printf("  Press Enter to continue...");
                color_normal();
                getchar();
                break;
        }
    }

    return 0;
}
