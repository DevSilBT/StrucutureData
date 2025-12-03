//POST-NUM Portable Version with Postfix Evaluation (Numbers, English)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
// #include <windows.h> // Eliminado
#include <math.h> // Para pow
#include "stack.h"
#include "dlist.h"
#include "list.h"
#define MAX_EXPR 256

// --- Definiciones de Secuencias VT100 ---
#define RESET_COLOR "\033[0m"
#define COLOR_RED "\033[1;31m"
#define COLOR_GREEN "\033[1;32m"
#define COLOR_YELLOW "\033[1;33m"
#define COLOR_BLUE "\033[1;34m"

// --- Función para limpiar pantalla portable ---
void clear_screen() {
    printf("\033[2J\033[H");
    fflush(stdout);
}

void color_normal() { printf(RESET_COLOR); }
void color_blue() { printf(COLOR_BLUE); }
void color_green() { printf(COLOR_GREEN); }
void color_yellow() { printf(COLOR_YELLOW); }
void color_red() { printf(COLOR_RED); }

int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    return 0;
}

int is_operator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^');
}

// Función para extraer un número (entero o decimal) desde una posición
int extract_number(const char *expr, int start, double *num) {
    int i = start;
    // Saltar espacios iniciales (aunque infix_to_postfix debería haberlos eliminado)
    while (expr[i] == ' ') i++;
    int start_num = i;
    // Extraer dígitos y un punto decimal opcional
    int has_decimal = 0;
    while (isdigit(expr[i]) || (expr[i] == '.' && !has_decimal)) {
        if (expr[i] == '.') has_decimal = 1;
        i++;
    }
    if (i == start_num) return 0; // No se encontró número

    char num_str[MAX_EXPR];
    strncpy(num_str, expr + start_num, i - start_num);
    num_str[i - start_num] = '\0';
    *num = atof(num_str);
    return i - start_num; // Devuelve la longitud del número encontrado
}

// Valida expresión infija que puede contener números
int validate_syntax(const char *infix) {
    int i;
    int len = strlen(infix);
    int parenthesis_balance = 0;
    int last_was_operand = 0; // 1 si es número, 2 si es letra
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

        if (!isdigit(c) && !isalpha(c) && !is_operator(c) && c != '(' && c != ')') {
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
            last_was_operand = 1; // Un número (o expresión) se considera como un operando
            inside_number = 0;
        }
        else if (isdigit(c)) {
            if (inside_number) {
                // Continúa en el mismo número
            } else {
                if (last_was_operand) { // Podría ser un número o una letra
                    color_red();
                    printf("\n  ERROR: Missing operator between operands at position %d\n", i + 1);
                    color_normal();
                    return 0;
                }
                last_was_operand = 1; // Marca como operando numérico
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
            last_was_operand = 1; // Marca como operando no numérico (letra)
            last_was_operator = 0;
            inside_number = 0;
        }
        else if (is_operator(c)) {
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

    color_green();
    printf("\n  Valid syntax\n");
    color_normal();
    return 1;
}

// --- Nueva Función: Evaluación de Postfix con Números ---
double evaluate_postfix_numeric(const char *postfix) {
    Stack stack;
    stack_init(&stack, NULL); // La pila contendrá punteros a double

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
            // Extraer número completo
            int num_len = extract_number(postfix, i, NULL); // Obtenemos la longitud
            if (num_len > 0) {
                char num_str[MAX_EXPR];
                strncpy(num_str, postfix + i, num_len);
                num_str[num_len] = '\0';
                double *num_ptr = (double*)malloc(sizeof(double));
                *num_ptr = atof(num_str);
                stack_push(&stack, num_ptr);
                i += num_len;
                continue; // Saltar al siguiente token
            }
        }
        else if (is_operator(c)) {
            if (stack_size(&stack) < 2) {
                color_red();
                printf("\nERROR: Not enough operands for operator '%c'\n", c);
                color_normal();
                // Liberar memoria de la pila antes de salir
                while (stack_size(&stack) > 0) {
                    double *val;
                    stack_pop(&stack, (void**)&val);
                    free(val);
                }
                stack_destroy(&stack);
                return 0.0 / 0.0; // NaN para indicar error
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
                        // Liberar memoria de la pila antes de salir
                        while (stack_size(&stack) > 0) {
                            double *val;
                            stack_pop(&stack, (void**)&val);
                            free(val);
                        }
                        stack_destroy(&stack);
                        return 0.0 / 0.0; // NaN
                    }
                    result = *op1_ptr / *op2_ptr;
                    break;
                case '^': result = pow(*op1_ptr, *op2_ptr); break;
                default: result = 0.0; break; // Esto no debería pasar
            }

            free(op1_ptr);
            free(op2_ptr);

            double *res_ptr = (double*)malloc(sizeof(double));
            *res_ptr = result;
            stack_push(&stack, res_ptr);
        }
        else {
            // Carácter no válido
            color_red();
            printf("\nERROR: Invalid character '%c' in postfix expression\n", c);
            color_normal();
            // Liberar memoria de la pila antes de salir
            while (stack_size(&stack) > 0) {
                double *val;
                stack_pop(&stack, (void**)&val);
                free(val);
            }
            stack_destroy(&stack);
            return 0.0 / 0.0; // NaN
        }
        i++;
    }

    if (stack_size(&stack) != 1) {
        color_red();
        printf("\nERROR: Invalid postfix expression structure\n");
        color_normal();
        // Liberar memoria de la pila antes de salir
        while (stack_size(&stack) > 0) {
            double *val;
            stack_pop(&stack, (void**)&val);
            free(val);
        }
        stack_destroy(&stack);
        return 0.0 / 0.0; // NaN
    }

    double *final_result_ptr;
    stack_pop(&stack, (void**)&final_result_ptr);
    double final_result = *final_result_ptr;
    free(final_result_ptr);

    stack_destroy(&stack);
    return final_result;
}

// Funciones auxiliares para infix_to_postfix (sin cambios significativos)
void print_stack(Stack *stack, char new_element, int highlight) {
    Stack temp_stack;
    char *op_ptr;
    char elements[MAX_EXPR];
    int count = 0;
    int i;
    int spaces;
    int total_length;

    if (stack_size(stack) == 0) {
        for (i = 0; i < 25; i++) printf(" ");
        return;
    }

    stack_init(&temp_stack, NULL);
    while (stack_size(stack) > 0) {
        stack_pop(stack, (void **)&op_ptr);
        elements[count++] = *op_ptr;
        stack_push(&temp_stack, op_ptr);
    }
    while (stack_size(&temp_stack) > 0) {
        stack_pop(&temp_stack, (void **)&op_ptr);
        stack_push(stack, op_ptr);
    }

    total_length = (count * 2) - 1;
    spaces = (25 - total_length) / 2;
    if (spaces < 0) spaces = 0;
    for (i = 0; i < spaces; i++) printf(" ");
    for (i = 0; i < count; i++) {
        if (highlight && i == 0 && elements[i] == new_element) {
            color_blue();
            printf("%c", elements[i]);
            color_normal();
        } else {
            printf("%c", elements[i]);
        }
        if (i < count - 1) printf(" ");
    }
    int remaining_spaces = 25 - (spaces + total_length);
    for (i = 0; i < remaining_spaces; i++) printf(" ");

    stack_destroy(&temp_stack);
}

void print_colored_operation(const char *operation, int length, int highlight_last) {
    int i;
    int spaces;
    int total_length;
    if (length == 0) { for (i = 0; i < 29; i++) printf(" "); return; }
    total_length = (length * 2) - 1;
    spaces = (29 - total_length) / 2;
    if (spaces < 0) spaces = 0;
    for (i = 0; i < spaces; i++) printf(" ");
    for (i = 0; i < length; i++) {
        if (highlight_last && i == length - 1) {
            color_blue();
            printf("%c", operation[i]);
            color_normal();
        } else {
            printf("%c", operation[i]);
        }
        if (i < length - 1) printf(" ");
    }
    int remaining_spaces = 29 - (spaces + total_length);
    for (i = 0; i < remaining_spaces; i++) printf(" ");
}

void infix_to_postfix(const char *infix, char *postfix) {
    Stack stack;
    int i, j = 0, step = 1;
    char temp_operation[MAX_EXPR] = "";
    int length = strlen(infix);

    stack_init(&stack, free);

    printf("\n");
    color_green();
    printf("+-------------------------------------------------------------------------------------------------+\n");
    printf("|                          INFIX TO POSTFIX CONVERSION                                           |\n");
    printf("|                                STEP-BY-STEP ALGORITHM                                          |\n");
    printf("+-------------------------------------------------------------------------------------------------+\n");
    color_normal();

    printf("\n");
    color_yellow();
    printf("  Entered Infix Expression: ");
    color_blue();
    printf("%s\n", infix);
    color_normal();

    printf("  Traversal Method: ");
    color_green();
    printf("LEFT -> RIGHT");
    color_normal();
    printf(" (first element first)\n\n");

    printf("+-----------------------------------------------------------------------------------------------------------------+\n");
    printf("|       |                          |                         |                             |\n");
    printf("|  STEP |         ACTION           |       STACK (D -> I)    |       OPERATION (I -> D)    |\n");
    printf("|       |                          |                         |                             |\n");
    printf("|-------+--------------------------+-------------------------+-----------------------------|\n");

    temp_operation[0] = '\0';
    length = strlen(infix);

    for (i = 0; i < length; i++) {
        char c = infix[i];
        if (c == ' ') continue;

        printf("|  %3d  | ", step);

        if (isdigit(c)) {
            char number[50];
            int num_idx = 0;
            while (i < length && (isdigit(infix[i]) || infix[i] == '.')) {
                number[num_idx++] = infix[i++];
            }
            number[num_idx] = '\0';
            i--; // Ajustar índice porque el bucle for también incrementa

            strcat(temp_operation, number);
            strcat(temp_operation, " ");
            printf("ADD [%s]       ", number);
            printf("|    ");
            print_stack(&stack, '\0', 0);
            printf(" | ");
            print_colored_operation(temp_operation, strlen(temp_operation), 1);
            printf(" |\n");
        }
        else if (isalpha(c)) { // Acepta letras también
            temp_operation[j++] = c;
            temp_operation[j++] = ' ';
            temp_operation[j] = '\0';
            printf("ADD [%c]       ", c);
            printf("|    ");
            print_stack(&stack, '\0', 0);
            printf(" | ");
            print_colored_operation(temp_operation, j, 1);
            printf(" |\n");
        }
        else if (c == '(') {
            char *op_ptr = (char *)malloc(sizeof(char));
            *op_ptr = c;
            stack_push(&stack, op_ptr);
            printf("PUSH [%c]            ", c);
            printf("|    ");
            print_stack(&stack, c, 1);
            printf(" | ");
            print_colored_operation(temp_operation, strlen(temp_operation), 0);
            printf(" |\n");
        }
        else if (c == ')') {
            printf("FOUND [%c]       ", c);
            printf("|    ");
            print_stack(&stack, '\0', 0);
            printf(" | ");
            print_colored_operation(temp_operation, strlen(temp_operation), 0);
            printf(" |\n");

            printf("|-------+--------------------------+-------------------------+-----------------------------|\n");
            step++;

            char *op_ptr;
            while (stack_size(&stack) > 0) {
                char *top = (char *)stack_peek(&stack);
                if (top && *top == '(') {
                    stack_pop(&stack, (void **)&op_ptr);
                    free(op_ptr);
                    printf("|  %3d  | POP [(]             ", step);
                    printf("|    ");
                    print_stack(&stack, '\0', 0);
                    printf(" | ");
                    print_colored_operation(temp_operation, strlen(temp_operation), 0);
                    printf(" |\n");
                    break;
                } else {
                    stack_pop(&stack, (void **)&op_ptr);
                    strcat(temp_operation, *op_ptr == '+' || *op_ptr == '-' || *op_ptr == '*' || *op_ptr == '/' || *op_ptr == '^' ? (char[]){*op_ptr, ' ', '\0'} : (char[]){*op_ptr, '\0'});
                    printf("|  %3d  | POP [%c] (find '(') ", step, *op_ptr);
                    printf("|    ");
                    print_stack(&stack, '\0', 0);
                    printf(" | ");
                    print_colored_operation(temp_operation, strlen(temp_operation), 1);
                    printf(" |\n");
                    free(op_ptr);
                    step++;
                }
            }
            printf("|-------+--------------------------+-------------------------+-----------------------------|\n");
            step--;
        }
        else if (is_operator(c)) {
            while (stack_size(&stack) > 0) {
                char *top_op = (char *)stack_peek(&stack);
                if (top_op && *top_op != '(' && precedence(*top_op) >= precedence(c)) {
                    char *op_ptr;
                    stack_pop(&stack, (void **)&op_ptr);
                    strcat(temp_operation, *op_ptr == '+' || *op_ptr == '-' || *op_ptr == '*' || *op_ptr == '/' || *op_ptr == '^' ? (char[]){*op_ptr, ' ', '\0'} : (char[]){*op_ptr, '\0'});
                    printf("POP [%c] (prec %d>=%d) ", *op_ptr, precedence(*op_ptr), precedence(c));
                    printf("|    ");
                    print_stack(&stack, '\0', 0);
                    printf(" | ");
                    print_colored_operation(temp_operation, strlen(temp_operation), 1);
                    printf(" |\n");
                    printf("|  %3d  | ", step + 1);
                    free(op_ptr);
                    step++;
                } else {
                    break;
                }
            }

            char *op_ptr = (char *)malloc(sizeof(char));
            *op_ptr = c;
            stack_push(&stack, op_ptr);
            printf("PUSH [%c]            ", c);
            printf("|    ");
            print_stack(&stack, c, 1);
            printf(" | ");
            print_colored_operation(temp_operation, strlen(temp_operation), 0);
            printf(" |\n");
        }

        if (i < length - 1) {
            printf("|-------+--------------------------+-------------------------+-----------------------------|\n");
        }
        step++;
    }

    if (stack_size(&stack) > 0) {
        printf("|-------+--------------------------+-------------------------+-----------------------------|\n");
        color_yellow();
        printf("|       |     EMPTYING STACK       |                         |                             |\n");
        color_normal();
        printf("|-------+--------------------------+-------------------------+-----------------------------|\n");
    }

    while (stack_size(&stack) > 0) {
        char *op_ptr;
        stack_pop(&stack, (void **)&op_ptr);
        strcat(temp_operation, *op_ptr == '+' || *op_ptr == '-' || *op_ptr == '*' || *op_ptr == '/' || *op_ptr == '^' ? (char[]){*op_ptr, ' ', '\0'} : (char[]){*op_ptr, '\0'});
        printf("|  %3d  | FINAL POP [%c]       ", step, *op_ptr);
        printf("|    ");
        print_stack(&stack, '\0', 0);
        printf(" | ");
        print_colored_operation(temp_operation, strlen(temp_operation), 1);
        printf(" |\n");

        if (stack_size(&stack) > 0) {
            printf("|-------+--------------------------+-------------------------+-----------------------------|\n");
        }

        free(op_ptr);
        step++;
    }

    printf("+-----------------------------------------------------------------------------------------------------------------+\n");

    // Copiar resultado final
    strcpy(postfix, temp_operation);

    printf("\n");
    color_green();
    printf("+-------------------------------------------------------------------------------------------------+\n");
    printf("|                                FINAL RESULT                                                    |\n");
    printf("|-------------------------------------------------------------------------------------------------|\n");
    color_normal();
    printf("|                                                                                                 |\n");
    printf("|   Postfix Expression: ");
    color_blue();
    printf("%-58s", postfix);
    color_normal();
    printf(" |\n");
    printf("|                                                                                                 |\n");
    printf("|   NOTE: The ^ operator has right associativity                                                |\n");
    printf("|                                                                                                 |\n");
    printf("+-------------------------------------------------------------------------------------------------+\n");

    stack_destroy(&stack);
}

int main(void) {
    char infix[MAX_EXPR];
    char postfix[MAX_EXPR];
    char continue_choice;

    do {
        clear_screen();

        printf("\n\n");
        color_green();
        printf("+-------------------------------------------------------------------------------------------------+\n");
        printf("|                                                                                                 |\n");
        printf("|                       INFIX TO POSTFIX CALCULATOR                                               |\n");
        printf("|                         STEP-BY-STEP CONVERSION AND NUMERIC EVALUATION                        |\n");
        printf("|                                                                                                 |\n");
        printf("+-------------------------------------------------------------------------------------------------+\n");
        color_normal();

        printf("\n");
        printf("+-------------------------------------------------------------------------------------------------+\n");
        printf("| OPERATOR HIERARCHY (from highest to lowest precedence):                                        |\n");
        printf("+-------------------------------------------------------------------------------------------------|\n");
        printf("|  1. ( )         Parentheses                                                                   |\n");
        printf("|  2. ^           Exponents (right-associative)                                                 |\n");
        printf("|  3. * /         Multiplication and Division (left-associative)                                |\n");
        printf("|  4. + -         Addition and Subtraction (left-associative)                                   |\n");
        printf("+-------------------------------------------------------------------------------------------------+\n");

        printf("\n");
        color_yellow();
        printf("  -> Enter the infix expression (with numbers, letters, and operators): ");
        color_normal();
        fgets(infix, MAX_EXPR, stdin);
        infix[strcspn(infix, "\n")] = '\0';

        if (!validate_syntax(infix)) {
            printf("\n");
            color_red();
            printf("  The expression contains errors. Please correct the syntax.\n");
            color_normal();
            printf("\n");
            color_yellow();
            printf("  Do you want to try another expression? (y/n): ");
            color_normal();
            continue_choice = getchar();
            while (getchar() != '\n');
            if (continue_choice != 'y' && continue_choice != 'Y') break;
            continue;
        }

        infix_to_postfix(infix, postfix);

        // Evaluación numérica
        double result = evaluate_postfix_numeric(postfix);
        if (isnan(result)) {
            printf("\n");
            color_red();
            printf("  Evaluation failed due to an error in the expression.\n");
            color_normal();
        } else {
            printf("\n");
            color_green();
            printf("+-------------------------------------------------------------------------------------------------+\n");
            printf("|                                NUMERIC EVALUATION RESULT                                      |\n");
            printf("|-------------------------------------------------------------------------------------------------|\n");
            color_normal();
            printf("|                                                                                                 |\n");
            printf("|   Postfix Expression:  ");
            color_yellow();
            printf("%-60s", postfix);
            color_normal();
            printf(" |\n");
            printf("|                                                                                                 |\n");
            printf("|   Numeric Result:      ");
            color_blue();
            printf("%-60.4f", result); // Mostrar con 4 decimales
            color_normal();
            printf(" |\n");
            printf("|                                                                                                 |\n");
            color_green();
            printf("+-------------------------------------------------------------------------------------------------+\n");
            color_normal();
        }

        printf("\n");
        color_yellow();
        printf("  Do you want to convert another expression? (y/n): ");
        color_normal();
        continue_choice = getchar();
        while (getchar() != '\n');

    } while (continue_choice == 'y' || continue_choice == 'Y');

    printf("\n");
    color_green();
    printf("  Thank you for using the calculator. Goodbye!\n");
    color_normal();
    printf("\n");

    return 0;
}