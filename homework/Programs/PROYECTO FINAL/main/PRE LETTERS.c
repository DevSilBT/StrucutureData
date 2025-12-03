//PRE-LETTERS Portable Version with Prefix Verification (English)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
// #include <windows.h> // Eliminado
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
// Opcional: Puedes definir más si lo deseas
// #define COLOR_MAGENTA "\033[1;35m"
// #define COLOR_CYAN "\033[1;36m"
// #define COLOR_WHITE "\033[1;37m"

// --- Función para limpiar pantalla portable ---
void clear_screen() {
    printf("\033[2J\033[H");
    fflush(stdout); // Asegura que la limpieza se aplique inmediatamente
}

/*
    Restore original color
*/
void color_normal() {
    printf(RESET_COLOR);
}

/*
    Set blue color
*/
void color_blue() {
    printf(COLOR_BLUE);
}

/*
    Set green color
*/
void color_green() {
    printf(COLOR_GREEN);
}

void color_yellow() {
    printf(COLOR_YELLOW);
}

/*
    Set red color
*/
void color_red() {
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
    NEW FUNCTION: Validate infix expression syntax
    Returns 1 if expression is valid, 0 if it has errors
*/
int validate_syntax(const char *infix) {
    int i;
    int len = strlen(infix);
    int parenthesis_balance = 0;
    int last_was_operand = 0;
    int last_was_operator = 0;
    char c;
    char last_char = '\0';
    
    /* Check for empty expression */
    if (len == 0) {
        color_red();
        printf("\n  ERROR: The expression is empty\n");
        color_normal();
        return 0;
    }
    
    for (i = 0; i < len; i++) {
        c = infix[i];
        
        /* Ignore spaces */
        if (c == ' ') continue;
        
        /* Check for invalid characters */
        if (!isalnum(c) && !is_operator(c) && c != '(' && c != ')') {
            color_red();
            printf("\n  ERROR: Invalid character '%c' at position %d\n", c, i + 1);
            color_normal();
            return 0;
        }
        
        /* Check parentheses */
        if (c == '(') {
            parenthesis_balance++;
            /* Cannot have operand before ( */
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
            /* Check for negative balance */
            if (parenthesis_balance < 0) {
                color_red();
                printf("\n  ERROR: Closing parenthesis ')' without opening at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            /* Cannot have operator before ) */
            if (last_was_operator) {
                color_red();
                printf("\n  ERROR: Missing operand before parenthesis ')' at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            /* Cannot have ( followed by ) */
            if (last_char == '(') {
                color_red();
                printf("\n  ERROR: Empty parentheses '()' at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            last_was_operator = 0;
            last_was_operand = 1;
        }
        /* If it's an operand */
        else if (isalnum(c)) {
            /* Cannot have two consecutive operands */
            if (last_was_operand) {
                color_red();
                printf("\n  ERROR: Two consecutive operands at position %d (missing operator)\n", i + 1);
                color_normal();
                return 0;
            }
            last_was_operand = 1;
            last_was_operator = 0;
        }
        /* If it's an operator */
        else if (is_operator(c)) {
            /* Cannot have two consecutive operators */
            if (last_was_operator) {
                color_red();
                printf("\n  ERROR: Two consecutive operators at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            /* Cannot start with operator (except - for negatives, but simplified here) */
            if (i == 0) {
                color_red();
                printf("\n  ERROR: Expression cannot start with an operator\n");
                color_normal();
                return 0;
            }
            /* Cannot have operator after ( */
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
    
    /* Check final parentheses balance */
    if (parenthesis_balance > 0) {
        color_red();
        printf("\n  ERROR: Missing %d closing parentheses ')'\n", parenthesis_balance);
        color_normal();
        return 0;
    }
    
    /* Cannot end with operator */
    if (last_was_operator) {
        color_red();
        printf("\n  ERROR: Expression cannot end with an operator\n");
        color_normal();
        return 0;
    }
    
    /* If we got here, syntax is valid */
    color_green();
    printf("\n  Valid syntax\n");
    color_normal();
    return 1;
}

/*
    Print stack content from RIGHT TO LEFT with colors
    Improved version: centered elements and ordered from right to left
    New elements are inserted to the LEFT of the first element
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
        /* When stack is empty, print centered spaces */
        for (i = 0; i < 25; i++) printf(" ");
        return;
    }
    
    /* Copy elements to temporary stack to preserve original order */
    stack_init(&temp_stack, NULL);
    
    /* Extract elements from main stack */
    while (stack_size(stack) > 0) {
        stack_pop(stack, (void **)&op_ptr);
        elements[count++] = *op_ptr;  /* Save the character */
        stack_push(&temp_stack, op_ptr);  /* Preserve to restore later */
    }
    
    /* Restore original stack */
    while (stack_size(&temp_stack) > 0) {
        stack_pop(&temp_stack, (void **)&op_ptr);
        stack_push(stack, op_ptr);
    }
    
    /* CALCULATE SPACES FOR CENTERING */
    /* Each element takes 2 spaces (character + space) except the last one */
    total_length = (count * 2) - 1;
    spaces = (25 - total_length) / 2;
    
    /* Ensure spaces is not negative */
    if (spaces < 0) spaces = 0;
    
    /* Print initial spaces for centering */
    for (i = 0; i < spaces; i++) {
        printf(" ");
    }
    
    /* Print elements from LEFT TO RIGHT (new elements to the left) */
    /* The last entered element (top) is shown on the LEFT */
    for (i = 0; i < count; i++) {
        if (highlight && i == 0 && elements[i] == new_element) {
            color_blue();
            printf("%c", elements[i]);
            color_normal();
        } else {
            printf("%c", elements[i]);
        }
        
        /* Add space between elements, except after the last one */
        if (i < count - 1) printf(" ");
    }
    
    /* Complete with spaces if needed to maintain alignment */
    int remaining_spaces = 25 - (spaces + total_length);
    for (i = 0; i < remaining_spaces; i++) {
        printf(" ");
    }
    
    stack_destroy(&temp_stack);
}

/*
    Print expression with last element in blue (from LEFT TO RIGHT)
    Improved version: centered elements
*/
void print_colored_operation(const char *operation, int length, int highlight_last) {
    int i;
    int spaces;
    int total_length;
    
    if (length == 0) {
        for (i = 0; i < 29; i++) printf(" ");
        return;
    }
    
    /* CALCULATE SPACES FOR CENTERING */
    total_length = (length * 2) - 1;
    spaces = (29 - total_length) / 2;
    
    /* Ensure spaces is not negative */
    if (spaces < 0) spaces = 0;
    
    /* Print initial spaces for centering */
    for (i = 0; i < spaces; i++) {
        printf(" ");
    }
    
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
    
    /* Complete with spaces if needed to maintain alignment */
    int remaining_spaces = 29 - (spaces + total_length);
    for (i = 0; i < remaining_spaces; i++) {
        printf(" ");
    }
}

/*
    Convert infix to prefix WITH PROFESSIONAL TABLE
    TRAVERSAL FROM RIGHT TO LEFT (last element first)
    STACK is filled from RIGHT TO LEFT
    OPERATION is filled from LEFT TO RIGHT
*/
void infix_to_prefix(const char *infix, char *prefix) {
    Stack stack;
    int i;
    int j = 0;
    int step = 1;
    char c;
    char *op_ptr;
    char temp_operation[MAX_EXPR];
    int length;
    
    /* Initialize stack */
    stack_init(&stack, free);
    
    printf("\n");
    color_green();
    printf("+-------------------------------------------------------------------------------------------------+\n");
    printf("|                          CONVERSION FROM INFIX TO PREFIX                                        |\n");
    printf("|                                STEP BY STEP ALGORITHM                                           |\n");
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
    printf("RIGHT -> LEFT");
    color_normal();
    printf(" (last element first)\n\n");
    
    printf("+-----------------------------------------------------------------------------------------------------------------+\n");
    printf("|       |                          |                         |                             |\n");
    printf("|  STEP |         ACTION           |       STACK (R -> L)    |       OPERATION (L -> R)    |\n");
    printf("|       |                          |                         |                             |\n");
    printf("|-------+--------------------------+-------------------------+-----------------------------|\n");
    
    temp_operation[0] = '\0';
    length = strlen(infix);
    
    /* TRAVERSE FROM RIGHT TO LEFT (last element first) */
    for (i = length - 1; i >= 0; i--) {
        c = infix[i];
        
        /* Ignore spaces */
        if (c == ' ')
            continue;
        
        printf("|  %3d  | ", step);
        
        /* If it's an operand (letter or number) - goes directly to OPERATION */
        if (isalnum(c)) {
            temp_operation[j++] = c;
            temp_operation[j] = '\0';
            
            printf("ADD [%c]          ", c);
            printf("|    ");
            print_stack(&stack, '\0', 0);
            printf(" | ");
            print_colored_operation(temp_operation, j, 1);
            printf(" |\n");
        }
        /* If it's a RIGHT parenthesis - PUSH to stack */
        else if (c == ')') {
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
        /* If it's a LEFT parenthesis - POP until finding ) */
        else if (c == '(') {
            printf("FOUND [%c]       ", c);
            printf("|    ");
            print_stack(&stack, '\0', 0);
            printf(" | ");
            print_colored_operation(temp_operation, j, 0);
            printf(" |\n");
            
            printf("|-------+--------------------------+-------------------------+-----------------------------|\n");
            step++;
            
            /* POP until finding the right parenthesis ) */
            while (stack_size(&stack) > 0) {
                stack_pop(&stack, (void **)&op_ptr);
                
                if (*op_ptr == ')') {
                    free(op_ptr);
                    printf("|  %3d  | POP [)]             ", step);
                    printf("|    ");
                    print_stack(&stack, '\0', 0);
                    printf(" | ");
                    print_colored_operation(temp_operation, j, 0);
                    printf(" |\n");
                    break;
                } else {
                    temp_operation[j++] = *op_ptr;
                    temp_operation[j] = '\0';
                    
                    printf("|  %3d  | POP [%c] (find '(') ", step, *op_ptr);
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
            /* POP operators of HIGHER OR EQUAL hierarchy */
            while (stack_size(&stack) > 0) {
                char *top_op;
                top_op = (char *)stack_peek(&stack);
                
                if (top_op && *top_op != ')' && precedence(*top_op) >= precedence(c)) {
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
        
        if (i > 0) {
            printf("|-------+--------------------------+-------------------------+-----------------------------|\n");
        }
        step++;
    }
    
    /* Separator before emptying stack */
    if (stack_size(&stack) > 0) {
        printf("|-------+--------------------------+-------------------------+-----------------------------|\n");
        color_yellow();
        printf("|       |     EMPTYING STACK       |                         |                             |\n");
        color_normal();
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
    
    temp_operation[j] = '\0';
    
    /* INVERT the result using DLIST (read from tail to head) */
    DList list;
    dlist_init(&list, NULL);
    
    /* Insert each character at the end of the list */
    for (i = 0; i < j; i++) {
        char *character = (char *)malloc(sizeof(char));
        *character = temp_operation[i];
        dlist_ins_next(&list, dlist_tail(&list), character);
    }
    
    /* Read from tail to head to invert */
    int k = 0;
    DListNode *node = dlist_tail(&list);
    
    while (node != NULL) {
        prefix[k++] = *(char *)dlist_data(node);
        node = dlist_prev(node);
    }
    prefix[k] = '\0';
    
    /* Destroy list and free memory */
    dlist_destroy(&list);
    
    printf("\n");
    printf("+-------------------------------------------------------------------------------------------------+\n");
    color_yellow();
    printf("|                           FINAL STEP: INVERT RESULT                                            |\n");
    color_normal();
    printf("|-------------------------------------------------------------------------------------------------|\n");
    printf("|                                                                                                 |\n");
    printf("|   Before inversion:   %-60s |\n", temp_operation);
    printf("|                                                                                                 |\n");
    printf("|   After inversion:    ");
    color_blue();
    printf("%-58s", prefix);
    color_normal();
    printf(" |\n");
    printf("|                                                                                                 |\n");
    printf("|   Method used: DLIST (reading from TAIL to HEAD)                                               |\n");
    printf("|                                                                                                 |\n");
    printf("+-------------------------------------------------------------------------------------------------+\n");
    
    /* Destroy stack */
    stack_destroy(&stack);
}

/*
    Function to evaluate prefix expression through traversals
    Identifies binary operations of the form: operator letter letter
    For example: +ab is evaluated and replaced with a new variable
    MODIFIED: Uses VT100 colors and improved consistency
*/
void evaluate_prefix_letters(const char *prefix) {
    char expression[MAX_EXPR];
    char result[MAX_EXPR];
    int i, j, k;
    int step = 1;
    char new_var = 'Z';  /* Variable to be used to replace operations (Z, Y, X, ...) */
    int changes = 1;
    int length;
    
    strcpy(expression, prefix); // Copia la expresión original para trabajar con ella
    
    printf("\n");
    color_green();
    printf("+-------------------------------------------------------------------------------------------------+\n");
    printf("|                          PREFIX EXPRESSION VERIFICATION                                        |\n");
    printf("|                              STEP BY STEP EVALUATION                                           |\n");
    printf("+-------------------------------------------------------------------------------------------------+\n");
    color_normal();
    
    printf("\n");
    color_yellow();
    printf("  Prefix Expression to Verify: ");
    color_blue();
    printf("%s\n", expression);
    color_normal();
    
    printf("  Method: Search for binary operations of the form: ");
    color_green();
    printf("operator letter letter");
    color_normal();
    printf("\n\n");
    
    printf("+-------------------------------------------------------------------------------------------------+\n");
    printf("|  STEP |         OPERATION FOUND              |         RESULTING EXPRESSION                    |\n");
    printf("|-------------------------------------------------------------------------------------------------|\n");
    
    /* Iterate until no more changes can be made */
    while (changes) {
        changes = 0;
        length = strlen(expression);
        
        /* Search for binary operations: operator followed by two operands */
        for (i = 0; i < length - 2; i++) { // Buscamos patrones de 3 caracteres
            /* If we find an operator followed by two letters */
            if (is_operator(expression[i]) &&
                isalpha(expression[i+1]) &&
                isalpha(expression[i+2])) {
                
                /* Print the found operation */
                printf("|  %3d  |   ", step);
                color_blue();
                printf("%c%c%c", expression[i], expression[i+1], expression[i+2]);
                color_normal();
                printf(" = ");
                color_green();
                printf("%c", new_var);
                color_normal();
                
                /* Spaces for alignment */
                int op_spaces = 30 - 3 - 1 - 1; // 3 chars for op, 1 for '=', 1 for var
                for (k = 0; k < op_spaces; k++) printf(" ");
                
                printf("|   ");
                
                /* Build the new expression */
                j = 0;
                /* Copy until before the operation */
                for (k = 0; k < i; k++) {
                    result[j++] = expression[k];
                }
                /* Insert the new variable */
                result[j++] = new_var;
                /* Copy the rest of the expression (skipping the 3 characters) */
                for (k = i + 3; k < length; k++) {
                    result[j++] = expression[k];
                }
                result[j] = '\0';
                
                /* Print the resulting expression with the new variable highlighted */
                for (k = 0; k < strlen(result); k++) {
                    if (k == i) { // Highlight the new variable at its position
                        color_green();
                        printf("%c", result[k]);
                        color_normal();
                    } else {
                        printf("%c", result[k]);
                    }
                    if (k < strlen(result) - 1) printf(" ");
                }
                
                /* Complete spaces */
                int spaces = 45 - (strlen(result) * 2 - 1);
                for (k = 0; k < spaces; k++) printf(" ");
                
                printf(" |\n");
                
                /* Update the expression */
                strcpy(expression, result);
                new_var--;  /* Next variable (Z, Y, X, ...) */
                changes = 1;  /* Mark that a change was made */
                step++;       /* Increment step counter */
                
                /* Add separator if there are still operators */
                int has_operators = 0;
                for (k = 0; k < strlen(expression); k++) {
                    if (is_operator(expression[k])) {
                        has_operators = 1;
                        break;
                    }
                }
                if (has_operators) {
                    printf("|-------------------------------------------------------------------------------------------------|\n");
                }
                
                break;  /* Restart search from the beginning after a change */
            }
        }
    }
    
    printf("+-------------------------------------------------------------------------------------------------+\n");
    
    printf("\n");
    color_green();
    printf("+-------------------------------------------------------------------------------------------------+\n");
    printf("|                                FINAL VERIFICATION RESULT                                        |\n");
    printf("|-------------------------------------------------------------------------------------------------|\n");
    color_normal();
    printf("|                                                                                                 |\n");
    printf("|   Original Expression:  ");
    color_yellow();
    printf("%-60s", prefix);
    color_normal();
    printf(" |\n");
    printf("|                                                                                                 |\n");
    printf("|   Final Result:         ");
    color_blue();
    printf("%-60s", expression);
    color_normal();
    printf(" |\n");
    printf("|                                                                                                 |\n");
    
    if (strlen(expression) == 1 && isalpha(expression[0])) {
        color_green();
        printf("|   Status: SUCCESSFUL VERIFICATION - Expression reduced to a single variable                    |\n");
        color_normal();
    } else {
        color_red();
        printf("|   Status: INCONCLUSIVE VERIFICATION - Expression not completely reduced                        |\n");
        color_normal();
    }
    
    printf("|                                                                                                 |\n");
    color_green();
    printf("+-------------------------------------------------------------------------------------------------+\n");
    color_normal();
}

/*
    Main
*/
int main(void) {
    char infix[MAX_EXPR];
    char prefix[MAX_EXPR];
    char continue_char;
    
    do {
        clear_screen();  /* Clear screen on each iteration - Portable version */
        // system("cls");  /* Clear screen on each iteration - Quitamos dependencia de windows */
        
        printf("\n\n");
        color_green();
        printf("+-------------------------------------------------------------------------------------------------+\n");
        printf("|                                                                                                 |\n");
        printf("|                       INFIX TO PREFIX CALCULATOR                                                |\n");
        printf("|                         STEP BY STEP CONVERSION                                                 |\n");
        printf("|                                                                                                 |\n");
        printf("+-------------------------------------------------------------------------------------------------+\n");
        color_normal();
        
        printf("\n");
        printf("+-------------------------------------------------------------------------------------------------+\n");
        printf("| OPERATOR HIERARCHY (from highest to lowest precedence):                                         |\n");
        printf("+-------------------------------------------------------------------------------------------------|\n");
        printf("|  1. ( )         Parentheses                                                                     |\n");
        printf("|  2. s           Roots                                                                           |\n");
        printf("|  3. ^           Exponents                                                                       |\n");
        printf("|  4. * /         Multiplication and Division                                                     |\n");
        printf("|  5. + -         Addition and Subtraction                                                        |\n");
        printf("+-------------------------------------------------------------------------------------------------+\n");
        
        printf("\n");
        printf("+-------------------------------------------------------------------------------------------------+\n");
        printf("| ALGORITHM TO CONVERT FROM INFIX TO PREFIX:                                                      |\n");
        printf("+-------------------------------------------------------------------------------------------------|\n");
        printf("|                                                                                                 |\n");
        printf("|   Read from RIGHT TO LEFT (last element first)                                                 |\n");
        printf("|   The infix operation is NOT inverted (remains original)                                       |\n");
        printf("|   STACK COLUMN: Filled from RIGHT TO LEFT                                                      |\n");
        printf("|   OPERATION COLUMN: Filled from LEFT TO RIGHT                                                  |\n");
        printf("|                                                                                                 |\n");
        printf("|  POP IS DONE WHEN:                                                                              |\n");
        printf("|     Parentheses close: ( )                                                                    |\n");
        printf("|     PUSH to operation of LOWER OR EQUAL hierarchy                                             |\n");
        printf("|     No more elements to add (empty the STACK)                                                 |\n");
        printf("|                                                                                                 |\n");
        printf("|  FINAL STEP: INVERT the complete result                                                         |\n");
        printf("|                                                                                                 |\n");
        printf("|  NOTE: New elements appear in ");
        color_blue();
        printf("BLUE COLOR");
        color_normal();
        printf("                                                          |\n");
        printf("|                                                                                                 |\n");
        printf("+-------------------------------------------------------------------------------------------------+\n");
        
        printf("\n");
        printf("+-------------------------------------------------------------------------------------------------+\n");
        printf("| CONVERSION EXAMPLES:                                                                            |\n");
        printf("+-------------------------------------------------------------------------------------------------|\n");
        printf("|   a*b+(c^2-d)     ->    +*ab-^c2d                                                             |\n");
        printf("|   (a+b)*c         ->    *+abc                                                                 |\n");
        printf("|   a+b*c           ->    +a*bc                                                                 |\n");
        printf("+-------------------------------------------------------------------------------------------------+\n");
        
        printf("\n");
        color_yellow();
        printf("  -> Enter the infix expression (without spaces): ");
        color_normal();
        fgets(infix, MAX_EXPR, stdin);
        
        /* Remove newline */
        infix[strcspn(infix, "\n")] = '\0';
        
        /* VALIDATE SYNTAX BEFORE CONVERTING */
        if (!validate_syntax(infix)) {
            printf("\n");
            color_red();
            printf("  The expression contains errors. Please correct the syntax.\n");
            color_normal();
            printf("\n");
            color_yellow();
            printf("  Want to try another expression? (y/n): ");
            color_normal();
            continue_char = getchar();
            while (getchar() != '\n');  /* Clear buffer */
            
            if (continue_char != 'y' && continue_char != 'Y') {
                break;  /* Exit loop if doesn't want to continue */
            }
            continue;  /* Ask for expression again */
        }
        
        /* Convert to prefix */
        infix_to_prefix(infix, prefix);
        
        printf("\n");
        color_green();
        printf("+-------------------------------------------------------------------------------------------------+\n");
        printf("|                                FINAL RESULT                                                     |\n");
        printf("|-------------------------------------------------------------------------------------------------|\n");
        color_normal();
        printf("|                                                                                                 |\n");
        printf("|   Infix Expression:   ");
        color_yellow();
        printf("%-60s", infix);
        color_normal();
        printf(" |\n");
        printf("|                                                                                                 |\n");
        printf("|   Prefix Expression:  ");
        color_blue();
        printf("%-60s", prefix);
        color_normal();
        printf(" |\n");
        printf("|                                                                                                 |\n");
        color_green();
        printf("+-------------------------------------------------------------------------------------------------+\n");
        color_normal();
        
        printf("\n");
        color_green();
        printf("  Conversion successfully completed\n");
        color_normal();
        
        /* Perform verification of the prefix expression */
        evaluate_prefix_letters(prefix); // <-- Llamada a la función de verificación (renombrada)
        
        printf("\n");
        color_yellow();
        printf("  Want to convert another expression? (y/n): ");
        color_normal();
        continue_char = getchar();
        while (getchar() != '\n');  /* Clear buffer */
        
    } while (continue_char == 'y' || continue_char == 'Y');
    
    printf("\n");
    color_green();
    printf("  Thank you for using the calculator. Goodbye!\n");
    color_normal();
    printf("\n");
    
    return 0;
}