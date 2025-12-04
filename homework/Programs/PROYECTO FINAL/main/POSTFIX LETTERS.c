// POSTFIX LETTERS Portable Version
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "stack.h"
#include "dlist.h"
#include "list.h"
#define MAX_EXPR 256

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

/*
    Determine operator precedence
*/
int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;  /* Exponent */
    if (op == 's') return 4;  /* sqrt (square root) */
    return 0;
}

/*
    Check if it's an operator
*/
int is_operator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == 's');
}

/*
    Validate infix expression syntax
*/
int validate_syntax(const char *infix) {
    int i;
    int len = strlen(infix);
    int parenthesis_balance = 0;
    int last_was_operand = 0;
    int last_was_operator = 0;
    char c;
    char last_char = '\0';

    if (len == 0) {
        set_red();
        printf("\n  ERROR: The expression is empty\n");
        reset_color();
        return 0;
    }

    for (i = 0; i < len; i++) {
        c = infix[i];

        if (c == ' ') continue;

        if (!isalnum(c) && !is_operator(c) && c != '(' && c != ')') {
            set_red();
            printf("\n  ERROR: Invalid character '%c' at position %d\n", c, i + 1);
            reset_color();
            return 0;
        }

        if (c == '(') {
            parenthesis_balance++;
            if (last_was_operand) {
                set_red();
                printf("\n  ERROR: Missing operator before parenthesis '(' at position %d\n", i + 1);
                reset_color();
                return 0;
            }
            last_was_operator = 0;
            last_was_operand = 0;
        }
        else if (c == ')') {
            parenthesis_balance--;
            if (parenthesis_balance < 0) {
                set_red();
                printf("\n  ERROR: Closing parenthesis ')' without opening at position %d\n", i + 1);
                reset_color();
                return 0;
            }
            if (last_was_operator) {
                set_red();
                printf("\n  ERROR: Missing operand before closing parenthesis ')' at position %d\n", i + 1);
                reset_color();
                return 0;
            }
            if (last_char == '(') {
                set_red();
                printf("\n  ERROR: Empty parentheses '()' at position %d\n", i + 1);
                reset_color();
                return 0;
            }
            last_was_operator = 0;
            last_was_operand = 1;
        }
        else if (isalnum(c)) {
            if (last_was_operand) {
                set_red();
                printf("\n  ERROR: Two consecutive operands at position %d (missing operator)\n", i + 1);
                reset_color();
                return 0;
            }
            last_was_operand = 1;
            last_was_operator = 0;
        }
        else if (is_operator(c)) {
            if (last_was_operator) {
                set_red();
                printf("\n  ERROR: Two consecutive operators at position %d\n", i + 1);
                reset_color();
                return 0;
            }
            if (i == 0) {
                set_red();
                printf("\n  ERROR: The expression cannot start with an operator\n");
                reset_color();
                return 0;
            }
            if (last_char == '(') {
                set_red();
                printf("\n  ERROR: Operator '%c' after opening parenthesis '(' at position %d\n", c, i + 1);
                reset_color();
                return 0;
            }
            last_was_operator = 1;
            last_was_operand = 0;
        }

        last_char = c;
    }

    if (parenthesis_balance > 0) {
        set_red();
        printf("\n  ERROR: Missing %d closing parentheses ')'\n", parenthesis_balance);
        reset_color();
        return 0;
    }

    if (last_was_operator) {
        set_red();
        printf("\n  ERROR: The expression cannot end with an operator\n");
        reset_color();
        return 0;
    }

    set_green();
    printf("\n  Valid syntax\n");
    reset_color();
    return 1;
}

/*
    Print stack content from RIGHT to LEFT with colors
*/
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

    for (i = 0; i < spaces; i++) {
        printf(" ");
    }

    for (i = 0; i < count; i++) {
        if (highlight && i == 0 && elements[i] == new_element) {
            set_blue();
            printf("%c", elements[i]);
            reset_color();
        } else {
            printf("%c", elements[i]);
        }

        if (i < count - 1) printf(" ");
    }

    int remaining_spaces = 25 - (spaces + total_length);
    for (i = 0; i < remaining_spaces; i++) {
        printf(" ");
    }

    stack_destroy(&temp_stack);
}

/*
    Print expression with last element in blue
*/
void print_colored_operation(const char *operation, int length, int highlight_last) {
    int i;
    int spaces;
    int total_length;

    if (length == 0) {
        for (i = 0; i < 29; i++) printf(" ");
        return;
    }

    total_length = (length * 2) - 1;
    spaces = (29 - total_length) / 2;

    if (spaces < 0) spaces = 0;

    for (i = 0; i < spaces; i++) {
        printf(" ");
    }

    for (i = 0; i < length; i++) {
        if (highlight_last && i == length - 1) {
            set_blue();
            printf("%c", operation[i]);
            reset_color();
        } else {
            printf("%c", operation[i]);
        }
        if (i < length - 1) printf(" ");
    }

    int remaining_spaces = 29 - (spaces + total_length);
    for (i = 0; i < remaining_spaces; i++) {
        printf(" ");
    }
}

/*
    Convert infix to postfix WITH PROFESSIONAL TABLE
    TRAVERSAL FROM LEFT TO RIGHT (first element first)
    STACK is filled from RIGHT to LEFT
    OPERATION is filled from LEFT to RIGHT
*/
void infix_to_postfix(const char *infix, char *postfix) {
    Stack stack;
    int i;
    int j = 0;
    int step = 1;
    char c;
    char *op_ptr;
    char temp_operation[MAX_EXPR];
    int length;

    stack_init(&stack, free);

    printf("\n");
    set_green();
    printf("+-------------------------------------------------------------------------------------------------+\n");
    printf("|                          INFIX TO POSTFIX CONVERSION                                           |\n");
    printf("|                                STEP-BY-STEP ALGORITHM                                          |\n");
    printf("+-------------------------------------------------------------------------------------------------+\n");
    reset_color();

    printf("\n");
    set_yellow();
    printf("  Entered Infix Expression: ");
    set_blue();
    printf("%s\n", infix);
    reset_color();

    printf("  Traversal Method: ");
    set_green();
    printf("LEFT -> RIGHT");
    reset_color();
    printf(" (first element first)\n\n");

    printf("+-----------------------------------------------------------------------------------------------------------------+\n");
    printf("|       |                          |                         |                             |\n");
    printf("|  STEP |         ACTION           |       STACK (R -> L)    |       OPERATION (L -> R)    |\n");
    printf("|       |                          |                         |                             |\n");
    printf("|-------+--------------------------+-------------------------+-----------------------------|\n");

    temp_operation[0] = '\0';
    length = strlen(infix);

    /* TRAVERSE FROM LEFT TO RIGHT (first element first) */
    for (i = 0; i < length; i++) {
        c = infix[i];

        if (c == ' ')
            continue;

        printf("|  %3d  | ", step);

        /* If it's an operand (letter or number) - goes directly to OPERATION */
        if (isalnum(c)) {
            temp_operation[j++] = c;
            temp_operation[j] = '\0';

            printf("ADD [%c]         ", c);
            printf("|    ");
            print_stack(&stack, '\0', 0);
            printf(" | ");
            print_colored_operation(temp_operation, j, 1);
            printf(" |\n");
        }
        /* If it's LEFT parenthesis - PUSH to stack */
        else if (c == '(') {
            op_ptr = (char *)malloc(sizeof(char));
            *op_ptr = c;
            stack_push(&stack, op_ptr);

            printf("PUSH [%c]            ", c);
            printf("|    ");
            print_stack(&stack, c, 1);
            printf(" | ");
            print_colored_operation(temp_operation, j, 0);
            printf(" |\n");
        }
        /* If it's RIGHT parenthesis - POP until finding ( */
        else if (c == ')') {
            printf("FOUND [%c]       ", c);
            printf("|    ");
            print_stack(&stack, '\0', 0);
            printf(" | ");
            print_colored_operation(temp_operation, j, 0);
            printf(" |\n");

            printf("|-------+--------------------------+-------------------------+-----------------------------|\n");
            step++;

            /* POP until finding the opening parenthesis ( */
            while (stack_size(&stack) > 0) {
                stack_pop(&stack, (void **)&op_ptr);

                if (*op_ptr == '(') {
                    free(op_ptr);
                    printf("|  %3d  | POP [(]             ", step);
                    printf("|    ");
                    print_stack(&stack, '\0', 0);
                    printf(" | ");
                    print_colored_operation(temp_operation, j, 0);
                    printf(" |\n");
                    break;
                } else {
                    temp_operation[j++] = *op_ptr;
                    temp_operation[j] = '\0';

                    printf("|  %3d  | POP [%c] (search '(') ", step, *op_ptr);
                    printf("|    ");
                    print_stack(&stack, '\0', 0);
                    printf(" | ");
                    print_colored_operation(temp_operation, j, 1);
                    printf(" |\n");

                    free(op_ptr);
                    step++;
                }
            }
            printf("|-------+--------------------------+-------------------------+-----------------------------|\n");
            step--;
        }
        /* If it's an operator */
        else if (is_operator(c)) {
            /* POP operators with HIGHER OR EQUAL precedence */
            while (stack_size(&stack) > 0) {
                char *top_op;
                top_op = (char *)stack_peek(&stack);

                if (top_op && *top_op != '(' && precedence(*top_op) >= precedence(c)) {
                    stack_pop(&stack, (void **)&op_ptr);
                    temp_operation[j++] = *op_ptr;
                    temp_operation[j] = '\0';

                    printf("POP [%c] (prec %d>=%d) ", *op_ptr, precedence(*op_ptr), precedence(c));
                    printf("|    ");
                    print_stack(&stack, '\0', 0);
                    printf(" | ");
                    print_colored_operation(temp_operation, j, 1);
                    printf(" |\n");
                    printf("|  %3d  | ", step + 1);

                    free(op_ptr);
                    step++;
                } else {
                    break;
                }
            }

            /* PUSH current operator */
            op_ptr = (char *)malloc(sizeof(char));
            *op_ptr = c;
            stack_push(&stack, op_ptr);

            printf("PUSH [%c]            ", c);
            printf("|    ");
            print_stack(&stack, c, 1);
            printf(" | ");
            print_colored_operation(temp_operation, j, 0);
            printf(" |\n");
        }

        if (i < length - 1) {
            printf("|-------+--------------------------+-------------------------+-----------------------------|\n");
        }
        step++;
    }

    /* Separator before emptying the stack */
    if (stack_size(&stack) > 0) {
        printf("|-------+--------------------------+-------------------------+-----------------------------|\n");
        set_yellow();
        printf("|       |     EMPTYING STACK       |                         |                             |\n");
        reset_color();
        printf("|-------+--------------------------+-------------------------+-----------------------------|\n");
    }

    /* POP all remaining operators to empty the STACK */
    while (stack_size(&stack) > 0) {
        stack_pop(&stack, (void **)&op_ptr);
        temp_operation[j++] = *op_ptr;
        temp_operation[j] = '\0';

        printf("|  %3d  | FINAL POP [%c]       ", step, *op_ptr);
        printf("|    ");
        print_stack(&stack, '\0', 0);
        printf(" | ");
        print_colored_operation(temp_operation, j, 1);
        printf(" |\n");

        if (stack_size(&stack) > 0) {
            printf("|-------+--------------------------+-------------------------+-----------------------------|\n");
        }

        free(op_ptr);
        step++;
    }

    printf("+-----------------------------------------------------------------------------------------------------------------+\n");

    /* Copy final result to postfix */
    strcpy(postfix, temp_operation);

    printf("\n");
    printf("+-------------------------------------------------------------------------------------------------+\n");
    set_yellow();
    printf("|                                              FINAL RESULT                                         |\n");
    reset_color();
    printf("|-------------------------------------------------------------------------------------------------|\n");
    printf("|                                                                                                 |\n");
    printf("|   Postfix Expression: ");
    set_blue();
    printf("%-58s", postfix);
    reset_color();
    printf(" |\n");
    printf("|                                                                                                 |\n");
    printf("|                                                                                                 |\n");
    printf("+-------------------------------------------------------------------------------------------------+\n");

    stack_destroy(&stack);
}

/*
    Function to perform postfix expression verification
*/
void verify_postfix(const char *postfix) {
    char expression[MAX_EXPR];
    char result[MAX_EXPR];
    int i, j, k;
    int step = 1;
    char new_letter = 'Z';

    strcpy(expression, postfix);

    printf("\n\n");
    set_green();
    printf("+-------------------------------------------------------------------------------------------------+\n");
    printf("|                            POSTFIX EXPRESSION VERIFICATION                                      |\n");
    printf("|                          SUBSTITUTION STEP-BY-STEP ALGORITHM                                    |\n");
    printf("+-------------------------------------------------------------------------------------------------+\n");
    reset_color();

    printf("\n");
    set_yellow();
    printf("  Original Postfix Expression: ");
    set_blue();
    printf("%s\n", expression);
    reset_color();

    printf("  Traversal Method: ");
    set_green();
    printf("SEARCH SEQUENCE: LETTER, LETTER, OPERATION\n");
    reset_color();

    printf("\n");
    set_green();
    printf("+-------------------------------------------------------------------------------------------------+\n");
    reset_color();
    printf("| STEP |    SEQUENCE FOUND         |   SUBSTITUTION   |        UPDATED EXPRESSION                |\n");
    printf("|------+----------------------------+-----------------+------------------------------------------|\n");

    /* Perform verification while there are operators in the expression */
    while (1) {
        int found = 0;
        int len = strlen(expression);

        /* Search pattern: letter, letter, operation */
        for (i = 0; i < len - 2; i++) {
            if (isalpha(expression[i]) && isalpha(expression[i+1]) && is_operator(expression[i+2])) {
                found = 1;
                break;
            }
        }

        if (!found) {
            /* Check if any unprocessed operator remains */
            for (i = 0; i < len; i++) {
                if (is_operator(expression[i])) {
                    found = 1;
                    /* Search operands for this operator */
                    for (j = i - 1; j >= 0; j--) {
                        if (isalpha(expression[j])) {
                            for (k = j - 1; k >= 0; k--) {
                                if (isalpha(expression[k])) {
                                    i = k; /* Adjust i to use found operands */
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    break;
                }
            }

            if (!found) {
                break; /* No more operators */
            }
        }

        /* Perform substitution */
        if (found) {
            char letter1 = expression[i];
            char letter2 = expression[i+1];
            char op = expression[i+2];

            /* Show current step */
            printf("| %4d |        %c%c%c          |      %c%c%c=%c      |  ",
                   step, letter1, letter2, op, letter1, letter2, op, new_letter);

            /* Build new expression */
            int idx = 0;

            /* Copy part before the sequence */
            for (j = 0; j < i; j++) {
                result[idx++] = expression[j];
            }

            /* Add new letter */
            result[idx++] = new_letter;

            /* Copy part after the sequence */
            for (j = i + 3; j < len; j++) {
                result[idx++] = expression[j];
            }

            result[idx] = '\0';

            /* Show updated expression */
            set_blue();
            printf("%-40s", result);
            reset_color();
            printf(" |\n");

            /* Update expression for next iteration */
            strcpy(expression, result);

            /* Decrement letter for next substitution */
            new_letter--;

            /* If uppercase letters run out, restart */
            if (new_letter < 'A') {
                new_letter = 'Z';
            }
            
            step++;
        }
    }

    printf("+-------------------------------------------------------------------------------------------------+\n");

    printf("\n");
    set_yellow();
    printf("  FINAL VERIFICATION RESULT:\n");
    reset_color();
    printf("  Completely reduced expression: ");
    set_green();
    printf("%s\n", expression);
    reset_color();

    if (strlen(expression) == 1 && isalpha(expression[0])) {
        set_green();
        printf("  The postfix expression is correct and has been reduced to a single variable!\n");
        reset_color();
    } else {
        set_red();
        printf("  WARNING! The expression could not be completely reduced.\n");
        reset_color();
    }
}

/*
    Main
*/
int main(void) {
    char infix[MAX_EXPR];
    char postfix[MAX_EXPR];
    char continue_char;

    do {
        clear_screen();

        printf("\n\n");
        set_green();
        printf("+-------------------------------------------------------------------------------------------------+\n");
        printf("|                                                                                                 |\n");
        printf("|                       INFIX TO POSTFIX CALCULATOR                                               |\n");
        printf("|                         STEP-BY-STEP CONVERSION                                                 |\n");
        printf("|                                                                                                 |\n");
        printf("+-------------------------------------------------------------------------------------------------+\n");
        reset_color();

        printf("\n");
        printf("+-------------------------------------------------------------------------------------------------+\n");
        printf("| OPERATOR HIERARCHY (from highest to lowest precedence):                                         |\n");
        printf("+-------------------------------------------------------------------------------------------------|\n");
        printf("|  1. ( )         Parentheses                                                                     |\n");
        printf("|  2. s           Square Roots                                                                    |\n");
        printf("|  3. ^           Exponents                                                                       |\n");
        printf("|  4. * /         Multiplication and Division                                                     |\n");
        printf("|  5. + -         Addition and Subtraction                                                        |\n");
        printf("+-------------------------------------------------------------------------------------------------+\n");

        printf("\n");
        printf("+-------------------------------------------------------------------------------------------------+\n");
        printf("| ALGORITHM FOR CONVERTING INFIX TO POSTFIX:                                                     |\n");
        printf("+-------------------------------------------------------------------------------------------------|\n");
        printf("|                                                                                                 |\n");
        printf("|   Read from LEFT TO RIGHT (first element first)                                                |\n");
        printf("|   The infix operation remains original                                                         |\n");
        printf("|   STACK COLUMN: Filled from RIGHT TO LEFT                                                      |\n");
        printf("|   OPERATION COLUMN: Filled from LEFT TO RIGHT                                                  |\n");
        printf("|                                                                                                 |\n");
        printf("|  POP IS DONE WHEN:                                                                              |\n");
        printf("|     Closing parentheses: )                                                                     |\n");
        printf("|     About to PUSH operation of LOWER OR EQUAL hierarchy                                        |\n");
        printf("|     No more elements to add (empty the STACK)                                                  |\n");
        printf("|                                                                                                 |\n");
        printf("|  It's NOT NECESSARY to invert the final result                                                  |\n");
        printf("|                                                                                                 |\n");
        printf("|  NOTE: New elements appear in ");
        set_blue();
        printf("BLUE COLOR");
        reset_color();
        printf("                                                          |\n");
        printf("|                                                                                                 |\n");
        printf("+-------------------------------------------------------------------------------------------------+\n");

        printf("\n");
        printf("+-------------------------------------------------------------------------------------------------+\n");
        printf("| CONVERSION EXAMPLES:                                                                           |\n");
        printf("+-------------------------------------------------------------------------------------------------|\n");
        printf("|   a*b+(c^2-d)     ->    ab*c2d-+^                                                            |\n");
        printf("|   (a+b)*c         ->    ab+c*                                                                 |\n");
        printf("|   a+b*c           ->    abc*+                                                                 |\n");
        printf("+-------------------------------------------------------------------------------------------------+\n");

        printf("\n");
        set_yellow();
        printf("  -> Enter the infix expression (letters only, no spaces): ");
        reset_color();
        fgets(infix, MAX_EXPR, stdin);

        infix[strcspn(infix, "\n")] = '\0';

        /* Validate it only contains letters and valid operators */
        int valid = 1;
        for (int i = 0; infix[i] != '\0'; i++) {
            if (infix[i] != ' ' && !isalpha(infix[i]) && !is_operator(infix[i]) &&
                infix[i] != '(' && infix[i] != ')') {
                valid = 0;
                break;
            }
        }

        if (!valid) {
            set_red();
            printf("\n  ERROR: Only letters (A-Z, a-z) and operators are allowed\n");
            reset_color();
            printf("\n");
            set_yellow();
            printf("  Try with another expression? (y/n): ");
            reset_color();
            continue_char = getchar();
            while (getchar() != '\n');

            if (continue_char != 'y' && continue_char != 'Y') {
                break;
            }
            continue;
        }

        if (!validate_syntax(infix)) {
            printf("\n");
            set_red();
            printf("  The expression contains errors. Please correct the syntax.\n");
            reset_color();
            printf("\n");
            set_yellow();
            printf("  Try with another expression? (y/n): ");
            reset_color();
            continue_char = getchar();
            while (getchar() != '\n');

            if (continue_char != 'y' && continue_char != 'Y') {
                break;
            }
            continue;
        }

        infix_to_postfix(infix, postfix);

        /* AUTOMATIC VERIFICATION - Always executed */
        verify_postfix(postfix);

        printf("\n");
        set_green();
        printf("+-------------------------------------------------------------------------------------------------+\n");
        printf("|                                FINAL SUMMARY                                                    |\n");
        printf("|-------------------------------------------------------------------------------------------------|\n");
        reset_color();
        printf("|                                                                                                 |\n");
        printf("|   Infix Expression:   ");
        set_yellow();
        printf("%-60s", infix);
        reset_color();
        printf(" |\n");
        printf("|                                                                                                 |\n");
        printf("|   Postfix Expression:  ");
        set_blue();
        printf("%-60s", postfix);
        reset_color();
        printf(" |\n");
        printf("|                                                                                                 |\n");
        set_green();
        printf("+-------------------------------------------------------------------------------------------------+\n");
        reset_color();

        printf("\n");
        set_green();
        printf("  Conversion and verification completed successfully\n");
        reset_color();

        printf("\n");
        set_yellow();
        printf("  Convert another expression? (y/n): ");
        reset_color();
        continue_char = getchar();
        while (getchar() != '\n');

    } while (continue_char == 'y' || continue_char == 'Y');

    printf("\n");
    set_green();
    printf("  Thank you for using the calculator. Goodbye!\n");
    reset_color();
    printf("\n");

    return 0;
}
