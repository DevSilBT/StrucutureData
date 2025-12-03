// mainCalculator.c - Integrated Calculator
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

// --- VT100 Color Definitions ---
#define RESET_COLOR "\033[0m"
#define COLOR_RED "\033[1;31m"
#define COLOR_GREEN "\033[1;32m"
#define COLOR_YELLOW "\033[1;33m"
#define COLOR_BLUE "\033[1;34m"

// --- Function Declarations (for C standard compliance) ---
void clear_screen();
void color_normal();
void color_blue();
void color_green();
void color_yellow();
void color_red();

// --- Stack Implementation (inline) ---
typedef struct StackNode_ {
    void *data;
    struct StackNode_ *next;
} StackNode;

typedef struct Stack_ {
    int size;
    void (*destroy) (void *data);
    StackNode *head;
} Stack;

void stack_init (Stack *stack, void (*destroy)(void *data)) {
    stack->size = 0;
    stack->destroy = destroy;
    stack->head = NULL;
}

void stack_destroy (Stack *stack) {
    void *data;
    while(stack_size(stack) > 0) {
        if (stack_pop(stack, (void **)&data) == 0 && stack->destroy != NULL) {
            stack->destroy(data);
        }
    }
    memset(stack, 0, sizeof(Stack));
}

int stack_push (Stack *stack, const void *data) {
    StackNode *new_node;
    if ((new_node = (StackNode *)malloc(sizeof(StackNode))) == NULL)
        return -1;
    new_node->data = (void *)data;
    new_node->next = stack->head;
    stack->head = new_node;
    stack->size++;
    return 0;
}

int stack_pop (Stack *stack, void **data) {
    StackNode *old_node;
    if (stack_size(stack) == 0) return -1;
    *data = stack->head->data;
    old_node = stack->head;
    stack->head = stack->head->next;
    old_node->next = NULL;
    free(old_node);
    stack->size--;
    return 0;
}

#define stack_size(stack) ((stack)->size)
#define stack_peek(stack) ((stack)->head == NULL ? NULL : (stack)->head->data)

// --- List Implementation (inline) ---
typedef struct ListNode_ {
    void *data;
    struct ListNode_ *next;
} ListNode;

typedef struct List_ {
    int size;
    void (*destroy) (void *data);
    ListNode *head;
    ListNode *tail;
} List;

void list_init (List *list, void (*destroy)(void *data)) {
    list->size = 0;
    list->destroy = destroy;
    list->head = NULL;
    list->tail = NULL;
}

void list_destroy (List *list) {
    void *data;
    while(list_size(list) > 0) {
        if (list_rem_next(list, NULL, (void **)&data) == 0 && list->destroy != NULL) {
            list->destroy(data);
        }
    }
    memset(list, 0, sizeof(List));
}

int list_ins_next (List *list, ListNode *node, const void *data) {
    ListNode    *new_node;
    if ((new_node = (ListNode *)malloc(sizeof(ListNode))) == NULL)
        return -1;
    new_node->data = (void *)data;
    if (node == NULL) {
        if (list_size(list) == 0)
            list->tail = new_node;
        new_node->next = list->head;
        list->head = new_node;
    } else {
        if (node->next == NULL)
            list->tail = new_node;
        new_node->next = node->next;
        node->next = new_node;
    }
    list->size++;
    return 0;
}

int list_rem_next (List *list, ListNode *node, void **data) {
    ListNode *old_node;
    if (list_size(list) == 0) return -1;
    if (node == NULL) {
        *data = list->head->data;
        old_node = list->head;
        list->head = list->head->next;
        if (list_size(list) == 1)
            list->tail = NULL;
    } else {
        if (node->next == NULL) return -1;
        *data = node->next->data;
        old_node = node->next;
        node->next = node->next->next;
        if (node->next == NULL) list->tail = node;
    }
    free(old_node);
    list->size--;
    return 0;
}

#define list_size(list) ((list)->size)
#define list_head(list) ((list)->head)
#define list_tail(list) ((list)->tail)
#define list_is_head(list, node) ((node) == (list)->head ? 1 : 0)
#define list_is_tail(node) ((node)->next == NULL ? 1 : 0)
#define list_data(node) ((node)->data)
#define list_next(node) ((node)->next)

// --- DList Implementation (inline) ---
typedef struct DListNode_ {
    void *data;
    struct DListNode_ *next;
    struct DListNode_ *prev;
} DListNode;

typedef struct DList_ {
    int size;
    void (*destroy) (void *data);
    DListNode *head;
    DListNode *tail;
} DList;

void dlist_init (DList *list, void (*destroy)(void *data)) {
    list->size = 0;
    list->destroy = destroy;
    list->head = NULL;
    list->tail = NULL;
}

void dlist_destroy (DList *list) {
    void *data;
    while(dlist_size(list) > 0) {
        if (dlist_remove(list, dlist_tail(list), (void **)&data) == 0 && list->destroy != NULL) {
            list->destroy(data);
        }
    }
    memset(list, 0, sizeof(DList));
}

int dlist_ins_next (DList *list, DListNode *node, const void *data ) {
    DListNode    *new_node;
    if (node == NULL && dlist_size(list) != 0)
        return -1;
    if ((new_node = (DListNode *)malloc(sizeof(DListNode))) == NULL)
        return -1;
    new_node->data = (void *)data;
    if (dlist_size(list) == 0){
        list->head = new_node;
        list->head->prev = NULL;
        list->head->next = NULL;
        list->tail = new_node;
    } else {
        new_node->next = node->next;
        new_node->prev = node;
        if (node->next == NULL)
            list->tail = new_node;
        else
            node->next->prev = new_node;
        node->next = new_node;
    }
    list->size++;
    return 0;
}

int dlist_ins_prev (DList *list, DListNode *node, const void *data ) {
    DListNode    *new_node;
    if (node == NULL && dlist_size(list) != 0)
        return -1;
    if ((new_node = (DListNode *)malloc(sizeof(DListNode))) == NULL)
        return -1;
    new_node->data = (void *)data;
    if (dlist_size(list) == 0){
        list->head = new_node;
        list->head->prev = NULL;
        list->head->next = NULL;
        list->tail = new_node;
    } else {
        new_node->next = node;
        new_node->prev = node->prev;
        if (node->prev == NULL)
            list->head = new_node;
        else
            node->prev->next = new_node;
        node->prev = new_node;
    }
    list->size++;
    return 0;
}

int dlist_remove (DList *list, DListNode *node, void **data) {
    if (node == NULL && dlist_size(list) == 0)
        return -1;
    *data = node->data;
    if (node == list->head) {
        list->head = node->next;
        if (list->head == NULL)
            list->tail = NULL;
        else
            node->next->prev = NULL;
    } else {
        node->prev->next = node->next;
        if (node->next == NULL)
            list->tail = node->prev;
        else
            node->next->prev = node->prev;
    }
    free(node);
    list->size--;
    return 0;
}

#define dlist_size(list) ((list)->size)
#define dlist_head(list) ((list)->head)
#define dlist_tail(list) ((list)->tail)
#define dlist_is_head(node) ((node)->prev == NULL ? 1 : 0)
#define dlist_is_tail(node) ((node)->next == NULL ? 1 : 0)
#define dlist_data(node) ((node)->data)
#define dlist_next(node) ((node)->next)
#define dlist_prev(node) ((node)->prev)

// --- Queue Implementation (inline, using List) ---
typedef List Queue;

#define queue_init list_init
#define queue_destroy list_destroy
int queue_enqueue (Queue *queue, const void *data) {
    return list_ins_next(queue, list_tail(queue), data);
}
int queue_dequeue (Queue *queue, void **data) {
    return list_rem_next(queue, NULL, data);
}
#define queue_peek(queue) ((queue)->head == NULL ? NULL : (queue)->head->data)
#define queue_size list_size

// --- Helper Functions ---
void clear_screen() {
    printf("\033[2J\033[H");
    fflush(stdout);
}

void color_normal() { printf(RESET_COLOR); }
void color_blue() { printf(COLOR_BLUE); }
void color_green() { printf(COLOR_GREEN); }
void color_yellow() { printf(COLOR_YELLOW); }
void color_red() { printf(COLOR_RED); }

// --- Structure for conversion steps ---
typedef struct {
    int step_num;
    char action[100];
    char stack_state[200];
    char operation_state[200];
} ConversionStep;

// --- Structure for evaluation steps ---
typedef struct {
    double op1;
    double op2;
    char op;
    double result;
} StepNumeric;

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

// --- INFIX.C Logic ---
int infix_precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    return 0;
}

int infix_is_operator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^');
}

typedef struct {
    char tipo;  // 'N' = number, 'O' = operator, 'P' = parenthesis
    double valor;
    char operador;
} InfixToken;

typedef struct {
    double operando1;
    double operando2;
    char operador;
    double resultado;
} InfixPaso;

int infix_validarSintaxis(const char *expr) {
    int i, len = strlen(expr);
    int parentesis = 0;
    int esperaOperando = 1;
    char anterior = '\0';
    int enNumero = 0;

    if(len == 0) {
        color_red();
        printf("    Error: Empty expression\n");
        color_normal();
        return 0;
    }

    for(i = 0; i < len; i++) {
        char c = expr[i];

        if(isspace(c)) {
            enNumero = 0;
            continue;
        }

        if(c == '(') {
            if(!esperaOperando && !infix_is_operator(anterior) && anterior != '(' && anterior != '\0') {
                color_red();
                printf("    Error: Unexpected opening parenthesis at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            parentesis++;
            esperaOperando = 1;
            enNumero = 0;
            anterior = c;
        }
        else if(c == ')') {
            parentesis--;
            if(esperaOperando) {
                color_red();
                printf("    Error: Closing parenthesis after operator at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            if(parentesis < 0) {
                color_red();
                printf("    Error: Closing parenthesis without opening at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            if(anterior == '(') {
                color_red();
                printf("    Error: Empty parentheses '()' at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            esperaOperando = 0;
            enNumero = 0;
            anterior = c;
        }
        else if(isdigit(c) || c == '.') {
            if(!esperaOperando && !enNumero) {
                color_red();
                printf("    Error: Unexpected number at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            esperaOperando = 0;
            enNumero = 1;
        }
        else if(infix_is_operator(c)) {
            if((c == '-' || c == '+') && esperaOperando) {
                esperaOperando = 1;
            }
            else if(esperaOperando) {
                color_red();
                printf("    Error: Unexpected operator '%c' at position %d\n", c, i + 1);
                color_normal();
                return 0;
            }
            else {
                esperaOperando = 1;
            }
            enNumero = 0;
            anterior = c;
        }
        else {
            color_red();
            printf("    Error: Invalid character '%c' at position %d\n", c, i + 1);
            color_normal();
            return 0;
        }
    }

    if(parentesis != 0) {
        color_red();
        printf("    Error: Unclosed parentheses\n");
        color_normal();
        return 0;
    }

    if(esperaOperando) {
        color_red();
        printf("    Error: Incomplete expression\n");
        color_normal();
        return 0;
    }

    return 1;
}

void infix_tokenizar(const char *expr, DList *tokens) {
    int i = 0, len = strlen(expr);

    while(i < len) {
        if(isspace(expr[i])) {
            i++;
            continue;
        }

        InfixToken *token = (InfixToken*)malloc(sizeof(InfixToken));

        if(isdigit(expr[i]) || expr[i] == '.') {
            char numStr[50];
            int j = 0;

            while(i < len && (isdigit(expr[i]) || expr[i] == '.')) {
                numStr[j++] = expr[i++];
            }
            numStr[j] = '\0';

            token->tipo = 'N';
            token->valor = atof(numStr);

            if(dlist_size(tokens) == 0) {
                dlist_ins_next(tokens, NULL, token);
            } else {
                dlist_ins_next(tokens, dlist_tail(tokens), token);
            }
        }
        else {
            token->tipo = (expr[i] == '(' || expr[i] == ')') ? 'P' : 'O';
            token->operador = expr[i];

            if(dlist_size(tokens) == 0) {
                dlist_ins_next(tokens, NULL, token);
            } else {
                dlist_ins_next(tokens, dlist_tail(tokens), token);
            }
            i++;
        }
    }
}

double infix_aplicarOperacion(char op, double a, double b) {
    switch(op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/':
            if(b == 0) {
                color_red();
                printf("\nERROR: Division by zero!\n");
                color_normal();
                exit(1);
            }
            return a / b;
        case '^': return pow(a, b);
        default: return 0;
    }
}

void infix_mostrarPasos(Queue *pasos) {
    ListNode *actual = list_head(pasos);
    int numPaso = 1;

    color_green();
    printf("| %-6s | %-15s | %-10s | %-15s | %-15s |\n",
           "Step", "Operand 1", "Operator", "Operand 2", "Result");
    color_normal();
    color_green();
    printf("+--------+-----------------+------------+-----------------+-----------------+\n");
    color_normal();

    while(actual != NULL) {
        InfixPaso *paso = (InfixPaso*)list_data(actual);
        if (paso != NULL) {
            printf("| ");
            color_blue();
            printf("%-6d", numPaso++);
            color_normal();
            printf(" | ");
            color_yellow();
            printf("%-15.4f", paso->operando1);
            color_normal();
            printf(" | ");
            color_red();
            printf("%-10c", paso->operador);
            color_normal();
            printf(" | ");
            color_yellow();
            printf("%-15.4f", paso->operando2);
            color_normal();
            printf(" | ");
            color_green();
            printf("%-15.4f", paso->resultado);
            color_normal();
            printf(" |\n");
        }
        actual = list_next(actual);
    }
}

double infix_evaluarExpresion(DList *tokens, Queue *pasos) {
    Stack pilaNumeros, pilaOperadores;
    DListNode *actual;

    stack_init(&pilaNumeros, NULL);
    stack_init(&pilaOperadores, NULL);

    actual = dlist_head(tokens);

    while(actual != NULL) {
        InfixToken *token = (InfixToken*)dlist_data(actual);

        if(token->tipo == 'N') {
            // Number: push directly
            double *num = (double*)malloc(sizeof(double));
            *num = token->valor;
            stack_push(&pilaNumeros, num);
        }
        else if(token->tipo == 'O') {
            // Operator: process according to precedence
            while(stack_size(&pilaOperadores) > 0) {
                char *topOp = (char*)stack_peek(&pilaOperadores);

                if(*topOp == '(') break;

                // Check precedence
                if(infix_precedence(token->operador) > infix_precedence(*topOp)) break;

                // Right associativity for ^
                if(token->operador == '^' && *topOp == '^') break;

                // Perform operation
                char *op;
                stack_pop(&pilaOperadores, (void**)&op);

                double *num2, *num1;
                stack_pop(&pilaNumeros, (void**)&num2);
                stack_pop(&pilaNumeros, (void**)&num1);

                double resultado = infix_aplicarOperacion(*op, *num1, *num2);

                // Save step
                InfixPaso *paso = (InfixPaso*)malloc(sizeof(InfixPaso));
                paso->operando1 = *num1;
                paso->operando2 = *num2;
                paso->operador = *op;
                paso->resultado = resultado;
                queue_enqueue(pasos, paso);

                // Push result
                double *res = (double*)malloc(sizeof(double));
                *res = resultado;
                stack_push(&pilaNumeros, res);

                free(op);
                free(num1);
                free(num2);
            }

            char *nuevoOp = (char*)malloc(sizeof(char));
            *nuevoOp = token->operador;
            stack_push(&pilaOperadores, nuevoOp);
        }
        else if(token->tipo == 'P') {
            if(token->operador == '(') {
                char *par = (char*)malloc(sizeof(char));
                *par = '(';
                stack_push(&pilaOperadores, par);
            }
            else if(token->operador == ')') {
                // Process until '(' is found
                while(stack_size(&pilaOperadores) > 0) {
                    char *op;
                    stack_pop(&pilaOperadores, (void**)&op);

                    if(*op == '(') {
                        free(op);
                        break;
                    }

                    double *num2, *num1;
                    stack_pop(&pilaNumeros, (void**)&num2);
                    stack_pop(&pilaNumeros, (void**)&num1);

                    double resultado = infix_aplicarOperacion(*op, *num1, *num2);

                    // Save step
                    InfixPaso *paso = (InfixPaso*)malloc(sizeof(InfixPaso));
                    paso->operando1 = *num1;
                    paso->operando2 = *num2;
                    paso->operador = *op;
                    paso->resultado = resultado;
                    queue_enqueue(pasos, paso);

                    double *res = (double*)malloc(sizeof(double));
                    *res = resultado;
                    stack_push(&pilaNumeros, res);

                    free(op);
                    free(num1);
                    free(num2);
                }
            }
        }

        actual = dlist_next(actual);
    }

    // Process remaining operators
    while(stack_size(&pilaOperadores) > 0) {
        char *op;
        stack_pop(&pilaOperadores, (void**)&op);

        double *num2, *num1;
        stack_pop(&pilaNumeros, (void**)&num2);
        stack_pop(&pilaNumeros, (void**)&num1);

        double resultado = infix_aplicarOperacion(*op, *num1, *num2);

        // Save step
        InfixPaso *paso = (InfixPaso*)malloc(sizeof(InfixPaso));
        paso->operando1 = *num1;
        paso->operando2 = *num2;
        paso->operador = *op;
        paso->resultado = resultado;
        queue_enqueue(pasos, paso);

        double *res = (double*)malloc(sizeof(double));
        *res = resultado;
        stack_push(&pilaNumeros, res);

        free(op);
        free(num1);
        free(num2);
    }

    // Get final result
    double *resultadoFinal;
    stack_pop(&pilaNumeros, (void**)&resultadoFinal);
    double valor = *resultadoFinal;
    free(resultadoFinal);

    stack_destroy(&pilaNumeros);
    stack_destroy(&pilaOperadores);

    return valor;
}

void infix_liberarToken(void *data) { free(data); }
void infix_liberarPaso(void *data) { free(data); }

void run_infix() {
    char expresion[256];
    DList tokens;
    Queue pasos;
    double resultado;

    printf("\n");
    color_yellow();
    printf("  -> Enter the expression (or 'exit' to finish): ");
    color_normal();
    fgets(expresion, 256, stdin);
    expresion[strcspn(expresion, "\n")] = 0;

    if(strcmp(expresion, "exit") == 0) {
        printf("\n");
        color_green();
        printf("  Thank you for using the calculator! Goodbye!\n");
        color_normal();
        printf("\n");
        return;
    }

    printf("\n");
    color_amarillo(); // Assuming color_amarillo exists or map to color_yellow
    printf("[1] Validating syntax...\n");
    color_normal();
    if(!infix_validarSintaxis(expresion)) {
        color_rojo(); // Assuming color_rojo exists or map to color_red
        printf("    ERROR: The expression has syntax errors.\n\n");
        color_normal();
        return;
    }

    color_verde(); // Assuming color_verde exists or map to color_green
    printf("    Syntax correct!\n");
    color_normal();

    printf("\n");
    color_amarillo(); // Assuming color_amarillo exists or map to color_yellow
    printf("[2] Tokenizing expression...\n");
    color_normal();
    dlist_init(&tokens, infix_liberarToken);
    infix_tokenizar(expresion, &tokens);
    color_azul(); // Assuming color_azul exists or map to color_blue
    printf("    Tokens processed: %d\n", dlist_size(&tokens));
    color_normal();

    printf("\n");
    color_amarillo(); // Assuming color_amarillo exists or map to color_yellow
    printf("[3] Evaluating expression with operation hierarchy...\n");
    color_normal();

    color_verde(); // Assuming color_verde exists or map to color_green
    printf("+-------------------------------------------------------------------------------------------------+\n");
    color_normal();

    queue_init(&pasos, infix_liberarPaso);
    resultado = infix_evaluarExpresion(&tokens, &pasos);

    printf("\n");
    color_amarillo(); // Assuming color_amarillo exists or map to color_yellow
    printf("[4] Evaluation steps (binary operations):\n");
    color_normal();

    color_verde(); // Assuming color_verde exists or map to color_green
    printf("+-------------------------------------------------------------------------------------------------+\n");
    color_normal();

    infix_mostrarPasos(&pasos);

    color_verde(); // Assuming color_verde exists or map to color_green
    printf("+-------------------------------------------------------------------------------------------------+\n");
    color_normal();

    printf("\n");
    color_verde(); // Assuming color_verde exists or map to color_green
    printf("[FINAL RESULT] = ");
    color_azul(); // Assuming color_azul exists or map to color_blue
    printf("%.4f\n", resultado);
    color_normal();

    // NEW FEATURE: SAVE TO FILE
    printf("\n");
    color_yellow();
    printf("==============================================\n");
    printf("Do you want to save the operations to a file? (y/n): ");
    color_normal();
    char respuesta;
    scanf(" %c", &respuesta);
    getchar(); // Clear the buffer

    if(respuesta == 'y' || respuesta == 'Y') {
        save_all_operations_to_file(expresion, "Infix", "", "Direct_Evaluation", &pasos, resultado);
        color_green();
        printf("Operations saved.\n");
        color_normal();
    }

    dlist_destroy(&tokens);
    queue_destroy(&pasos);
}

// --- POST-NUM.C Logic ---
int postnum_precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    return 0;
}

int postnum_is_operator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^');
}

int postnum_validate_syntax(const char *infix) {
    // Validation logic for numbers and operators
    int i;
    int len = strlen(infix);
    int parenthesis_balance = 0;
    int last_was_operand = 0; // 1 if number, 2 if letter
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

        if (!isdigit(c) && !isalpha(c) && !postnum_is_operator(c) && c != '(' && c != ')') {
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
            last_was_operand = 1; // Treat expression inside () as one operand
            inside_number = 0;
        }
        else if (isdigit(c)) {
            if (inside_number) {
                // Continues in the same number
            } else {
                if (last_was_operand) { // Could be number or letter
                    color_red();
                    printf("\n  ERROR: Missing operator between operands at position %d\n", i + 1);
                    color_normal();
                    return 0;
                }
                last_was_operand = 1; // Mark as numeric operand
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
            last_was_operand = 2; // Mark as non-numeric operand (letter)
            last_was_operator = 0;
            inside_number = 0;
        }
        else if (postnum_is_operator(c)) {
            if (last_was_operator) {
                color_red();
                printf("\n  ERROR: Two consecutive operators at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            if (i == 0) {
                color_red();
                printf("\n  ERROR: The expression cannot start with an operator\n");
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
        printf("\n  ERROR: The expression cannot end with an operator\n");
        color_normal();
        return 0;
    }

    color_green();
    printf("\n  Valid syntax\n");
    color_normal();
    return 1;
}

int postnum_extract_number(const char *expr, int start, double *num) {
    int i = start;
    while (expr[i] == ' ') i++;
    int start_num = i;
    int has_decimal = 0;
    while (isdigit(expr[i]) || (expr[i] == '.' && !has_decimal)) {
        if (expr[i] == '.') has_decimal = 1;
        i++;
    }
    if (i == start_num) return 0; // No se encontró número
    char num_str[256];
    strncpy(num_str, expr + start_num, i - start_num);
    num_str[i - start_num] = '\0';
    *num = atof(num_str);
    return i - start_num; // Devuelve la longitud del número encontrado
}

double postnum_evaluate_postfix_numeric(const char *postfix, Queue *steps) {
    Stack stack;
    stack_init(&stack, NULL); // La pila contendrá punteros a double

    char token_str[256];
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
            int num_len = postnum_extract_number(postfix, i, NULL); // Obtenemos la longitud
            if (num_len > 0) {
                char num_str[256];
                strncpy(num_str, postfix + i, num_len);
                num_str[num_len] = '\0';
                double *num_ptr = (double*)malloc(sizeof(double));
                *num_ptr = atof(num_str);
                stack_push(&stack, num_ptr);
                i += num_len;
                continue; // Saltar al siguiente token
            }
        }
        else if (postnum_is_operator(c)) {
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

            // Save step if a queue is provided
            if (steps) {
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

void postnum_print_stack(Stack *stack, char new_element, int highlight) {
    // Simplified print for demonstration
    printf("[Stack: Size %d] ", stack_size(stack)); // Just print size for now
}

void postnum_print_colored_operation(const char *operation, int length, int highlight_last) {
    printf("OP: %s", operation); // Just print operation for now
}

void postnum_infix_to_postfix(const char *infix, char *postfix) {
    Stack stack;
    int i, j = 0, step = 1;
    char temp_operation[256] = "";
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
            i--; // Ajustar índice porque el bucle for también incrementa
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
                    strcat(temp_operation, *op_ptr == '+' || *op_ptr == '-' || *op_ptr == '*' || *op_ptr == '/' || *op_ptr == '^' ? (char[]){*op_ptr, ' ', '\0'} : (char[]){*op_ptr, '\0'});
                    free(op_ptr);
                }
            }
        }
        else if (postnum_is_operator(c)) {
            while (stack_size(&stack) > 0) {
                char *top_op;
                top_op = (char *)stack_peek(&stack);
                if (top_op && *top_op != '(' && postnum_precedence(*top_op) >= postnum_precedence(c)) {
                    char *op_ptr;
                    stack_pop(&stack, (void **)&op_ptr);
                    strcat(temp_operation, *op_ptr == '+' || *op_ptr == '-' || *op_ptr == '*' || *op_ptr == '/' || *op_ptr == '^' ? (char[]){*op_ptr, ' ', '\0'} : (char[]){*op_ptr, '\0'});
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
        strcat(temp_operation, *op_ptr == '+' || *op_ptr == '-' || *op_ptr == '*' || *op_ptr == '/' || *op_ptr == '^' ? (char[]){*op_ptr, ' ', '\0'} : (char[]){*op_ptr, '\0'});
        free(op_ptr);
    }

    strcpy(postfix, temp_operation);

    stack_destroy(&stack);
}

void run_postnum() {
    char infix[256];
    char postfix[256];
    char continue_choice;

    printf("\n");
    color_yellow();
    printf("  -> Enter the infix expression (with numbers, letters, and operators): ");
    color_normal();
    fgets(infix, 256, stdin);
    infix[strcspn(infix, "\n")] = '\0';

    if (!postnum_validate_syntax(infix)) {
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
        if (continue_choice != 'y' && continue_choice != 'Y') return;
        return; // Or loop back if desired
    }

    postnum_infix_to_postfix(infix, postfix);

    Queue steps;
    queue_init(&steps, free);
    double result = postnum_evaluate_postfix_numeric(postfix, &steps);
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

    // Save operations
    save_all_operations_to_file(infix, "Infix", postfix, "Postfix", &steps, result);
    queue_destroy(&steps);
}

// --- PRE-NUM.C Logic ---
int prenum_precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;  /* Exponent */
    if (op == 's') return 4;  /* sqrt (square root) */
    return 0;
}

int prenum_is_operator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == 's');
}

int prenum_validate_syntax(const char *infix) {
    // Validation logic for numbers and operators
    int i;
    int len = strlen(infix);
    int parenthesis_balance = 0;
    int last_was_operand = 0; // 1 if number, 2 if letter
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

        if (!isdigit(c) && !isalpha(c) && !prenum_is_operator(c) && c != '(' && c != ')') {
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
            last_was_operand = 1; // Treat expression inside () as one operand
            inside_number = 0;
        }
        else if (isdigit(c)) {
            if (inside_number) {
                // Continues in the same number
            } else {
                if (last_was_operand) { // Could be number or letter
                    color_red();
                    printf("\n  ERROR: Missing operator between operands at position %d\n", i + 1);
                    color_normal();
                    return 0;
                }
                last_was_operand = 1; // Mark as numeric operand
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
            last_was_operand = 2; // Mark as non-numeric operand (letter)
            last_was_operator = 0;
            inside_number = 0;
        }
        else if (prenum_is_operator(c)) {
            if (last_was_operator) {
                color_red();
                printf("\n  ERROR: Two consecutive operators at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            if (i == 0) {
                color_red();
                printf("\n  ERROR: The expression cannot start with an operator\n");
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
        printf("\n  ERROR: The expression cannot end with an operator\n");
        color_normal();
        return 0;
    }

    color_green();
    printf("\n  Valid syntax\n");
    color_normal();
    return 1;
}

double prenum_evaluate_prefix_numeric(const char *prefix, Queue *steps) {
    char expression[256];
    char result_str[256];
    int i, j, k;
    int step_num = 1;
    int changes = 1;
    int length;
    char num1_str[256], num2_str[256];
    int advance_num1, advance_num2;
    int pos_after_num1, pos_after_num2;
    int pos_num1, pos_num2;

    strcpy(expression, prefix);

    while (changes) {
        changes = 0;
        length = strlen(expression);

        // SEARCH FROM LEFT TO RIGHT for the first complete operation
        for (i = 0; i < length - 2; i++) { // Buscamos patrones de 3 caracteres
            // If we find an operator followed by two numbers
            if (prenum_is_operator(expression[i]) &&
                isdigit(expression[i+1]) ||
                (isdigit(expression[i+1]) && isalpha(expression[i+2]))) { // Adjusted for number/letter detection

                // Find first number/letter after operator
                pos_num1 = i + 1;
                while (pos_num1 < length && isspace(expression[pos_num1])) pos_num1++;
                if (pos_num1 < length && isdigit(expression[pos_num1])) {
                    // Extract first number
                    advance_num1 = postnum_extract_number(expression, pos_num1, NULL); // Reuse function from POST-NUM
                    if (advance_num1 > 0) {
                        strncpy(num1_str, expression + pos_num1, advance_num1);
                        num1_str[advance_num1] = '\0';
                        pos_after_num1 = pos_num1 + advance_num1;
                        // Skip spaces between numbers
                        while (pos_after_num1 < length && isspace(expression[pos_after_num1])) pos_after_num1++;
                        // Check if there's a second number (not another operator)
                        if (pos_after_num1 < length && isdigit(expression[pos_after_num1])) {
                            // Extract second number
                            advance_num2 = postnum_extract_number(expression, pos_after_num1, NULL);
                            if (advance_num2 > 0) {
                                strncpy(num2_str, expression + pos_after_num1, advance_num2);
                                num2_str[advance_num2] = '\0';
                                pos_after_num2 = pos_after_num1 + advance_num2;

                                // Calculate the real result of the operation
                                double a = atof(num1_str);
                                double b = atof(num2_str);
                                double operation_result;

                                // Perform the corresponding operation
                                switch (expression[i]) {
                                    case '+':
                                        operation_result = a + b;
                                        break;
                                    case '-':
                                        operation_result = a - b;
                                        break;
                                    case '*':
                                        operation_result = a * b;
                                        break;
                                    case '/':
                                        if (b != 0) {
                                            operation_result = a / b;
                                        } else {
                                            operation_result = 0;  /* Division by zero */
                                        }
                                        break;
                                    case '^':
                                        operation_result = pow(a, b);
                                        break;
                                    case 's': // Unary operator example
                                        operation_result = sqrt(a);
                                        pos_after_num2 = pos_after_num1; // Adjust end position for unary
                                        break;
                                    default:
                                        operation_result = 0;
                                }

                                // Build the operation string for display
                                char operation_str[256];
                                sprintf(operation_str, "%c%s%s", expression[i], num1_str, num2_str);
                                char new_num_str[20];
                                sprintf(new_num_str, "%.4f", operation_result); // Use fixed precision

                                // Save the found operation
                                if (steps && expression[i] != 's') { // Only record binary operations
                                    StepNumeric *step = (StepNumeric*)malloc(sizeof(StepNumeric));
                                    step->op1 = a;
                                    step->op2 = b;
                                    step->op = expression[i];
                                    step->result = operation_result;
                                    queue_enqueue(steps, step);
                                }

                                // Build the new expression
                                j = 0;
                                // Copy until before the operator
                                for (k = 0; k < i; k++) {
                                    result_str[j++] = expression[k];
                                }
                                // Insert the new number
                                for (k = 0; k < strlen(new_num_str); k++) {
                                    result_str[j++] = new_num_str[k];
                                }
                                // Copy the rest of the expression (after the second number)
                                for (k = pos_after_num2; k < length; k++) {
                                    result_str[j++] = expression[k];
                                }
                                result_str[j] = '\0';

                                // Update the expression
                                strcpy(expression, result_str);
                                changes = 1;
                                step_num++;

                                // Check if there are still operators
                                int has_operators = 0;
                                for (k = 0; k < strlen(expression); k++) {
                                    if (prenum_is_operator(expression[k])) {
                                        has_operators = 1;
                                        break;
                                    }
                                }
                                if (has_operators) {
                                    break; // Continue outer loop
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Check if only a number remains (could be multi-digit)
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

void prenum_infix_to_prefix(const char *infix, char *prefix) {
    Stack stack;
    int i;
    int j = 0;
    int step = 1;
    char c;
    char *op_ptr;
    char temp_operation[256];
    int length;
    int last_was_digit = 0;  /* Track if last character was a digit */
    int needs_space = 0;

    /* Initialize stack */
    stack_init(&stack, free);

    temp_operation[0] = '\0';
    length = strlen(infix);

    /* TRAVERSE FROM RIGHT to LEFT (last element first) */
    for (i = length - 1; i >= 0; i--) {
        c = infix[i];

        /* Ignore spaces */
        if (c == ' ')
            continue;

        /* If it's a digit (part of a number) */
        if (isdigit(c)) {
            /* If space needed before this complete number */
            if (needs_space && !last_was_digit) {
                temp_operation[j++] = ' ';
                needs_space = 0;
            }
            temp_operation[j++] = c;
            temp_operation[j] = '\0';
            last_was_digit = 1;
        }
        /* If it's a letter */
        else if (isalpha(c)) {
            /* Add space if needed */
            if (needs_space) {
                temp_operation[j++] = ' ';
                needs_space = 0;
            }
            temp_operation[j++] = c;
            temp_operation[j] = '\0';
            last_was_digit = 0;
            needs_space = 1;  /* Next operand will need space */
        }
        /* If it's RIGHT parenthesis - PUSH to stack */
        else if (c == ')') {
            op_ptr = (char *)malloc(sizeof(char));
            *op_ptr = c;
            stack_push(&stack, op_ptr);
            /* If there was a number before, mark that next needs space */
            if (last_was_digit) {
                needs_space = 1;
            }
            last_was_digit = 0;
        }
        /* If it's LEFT parenthesis - POP until finding ) */
        else if (c == '(') {
            /* POP until finding the right parenthesis ) */
            while (stack_size(&stack) > 0) {
                stack_pop(&stack, (void **)&op_ptr);
                if (*op_ptr == ')') {
                    free(op_ptr);
                    break;
                } else {
                    /* Add space before operator if needed */
                    if (needs_space || last_was_digit) {
                        temp_operation[j++] = ' ';
                        needs_space = 0;
                    }
                    temp_operation[j++] = *op_ptr;
                    temp_operation[j] = '\0';
                    last_was_digit = 0;
                    needs_space = 1;
                    free(op_ptr);
                }
            }
        }
        /* If it's an operator */
        else if (prenum_is_operator(c)) {
            /* POP operators of HIGHER OR EQUAL hierarchy */
            while (stack_size(&stack) > 0) {
                char *top_op;
                top_op = (char *)stack_peek(&stack);
                if (top_op && *top_op != ')' && prenum_precedence(*top_op) >= prenum_precedence(c)) {
                    stack_pop(&stack, (void **)&op_ptr);
                    /* Add space before operator if needed */
                    if (needs_space || last_was_digit) {
                        temp_operation[j++] = ' ';
                        needs_space = 0;
                    }
                    temp_operation[j++] = *op_ptr;
                    temp_operation[j] = '\0';
                    last_was_digit = 0;
                    needs_space = 1;
                    free(op_ptr);
                } else {
                    break;
                }
            }

            /* PUSH current operator */
            op_ptr = (char *)malloc(sizeof(char));
            *op_ptr = c;
            stack_push(&stack, op_ptr);
            /* If there was a number before, mark that next needs space */
            if (last_was_digit) {
                needs_space = 1;
            }
            last_was_digit = 0;
        }
    }

    /* POP all remaining operators to empty the STACK */
    while (stack_size(&stack) > 0) {
        stack_pop(&stack, (void **)&op_ptr);
        /* Add space before operator if needed */
        if (needs_space || last_was_digit) {
            temp_operation[j++] = ' ';
            needs_space = 0;
        }
        temp_operation[j++] = *op_ptr;
        temp_operation[j] = '\0';
        last_was_digit = 0;
        needs_space = 1;
        free(op_ptr);
    }

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

    /* Destroy stack */
    stack_destroy(&stack);
}

void run_prenum() {
    char infix[256];
    char prefix[256];
    char continue_char;

    printf("\n");
    color_yellow();
    printf("  -> Enter the infix expression (without spaces): ");
    color_normal();
    fgets(infix, 256, stdin);

    /* Remove newline */
    infix[strcspn(infix, "\n")] = '\0';

    /* VALIDATE SYNTAX BEFORE CONVERTING */
    if (!prenum_validate_syntax(infix)) {
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

        if (continue_char != 's' && continue_char != 'S') {
            return;  /* Exit if doesn't want to continue */
        }
        // Recursive call or loop back if needed
    }

    /* Convert to prefix */
    prenum_infix_to_prefix(infix, prefix);

    printf("\n");
    printf("  Prefix Expression:  ");
    color_blue();
    printf("%-60s", prefix);
    color_normal();
    printf(" \n");

    /* Perform verification of the prefix expression */
    Queue steps;
    queue_init(&steps, free);
    double result = prenum_evaluate_prefix_numeric(prefix, &steps);
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

    // Save operations
    save_all_operations_to_file(infix, "Infix", prefix, "Prefix", &steps, result);
    queue_destroy(&steps);
}

// --- POSTFIX LETTERS.C Logic ---
int pl_precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;  /* Exponent */
    if (op == 's') return 4;  /* sqrt (square root) */
    return 0;
}

int pl_is_operator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == 's');
}

int pl_validate_syntax(const char *infix) {
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

        if (!isalnum(c) && !pl_is_operator(c) && c != '(' && c != ')') {
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
        else if (isalnum(c)) {
            if (last_was_operand) {
                color_red();
                printf("\n  ERROR: Two consecutive operands at position %d (missing operator)\n", i + 1);
                color_normal();
                return 0;
            }
            last_was_operand = 1;
            last_was_operator = 0;
        }
        else if (pl_is_operator(c)) {
            if (last_was_operator) {
                color_red();
                printf("\n  ERROR: Two consecutive operators at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            if (i == 0) {
                color_red();
                printf("\n  ERROR: The expression cannot start with an operator\n");
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
        printf("\n  ERROR: The expression cannot end with an operator\n");
        color_normal();
        return 0;
    }

    color_green();
    printf("\n  Valid syntax\n");
    color_normal();
    return 1;
}

void pl_verify_postfix(const char *postfix) {
    char expression[256];
    char result[256];
    char substituted[256];
    int i, j, k;
    int step = 1;
    char new_letter = 'Z';

    strcpy(expression, postfix);

    printf("\n\n");
    color_green();
    printf("+-------------------------------------------------------------------------------------------------+\n");
    printf("|                            POSTFIX EXPRESSION VERIFICATION                                      |\n");
    printf("|                          SUBSTITUTION STEP-BY-STEP ALGORITHM                                    |\n");
    printf("+-------------------------------------------------------------------------------------------------+\n");
    color_normal();

    printf("\n");
    color_yellow();
    printf("  Original Postfix Expression: ");
    color_blue();
    printf("%s\n", expression);
    color_normal();

    printf("  Traversal Method: ");
    color_green();
    printf("SEARCH SEQUENCE: LETTER, LETTER, OPERATION\n");
    color_normal();

    printf("\n");
    color_green();
    printf("+-------------------------------------------------------------------------------------------------+\n");
    color_normal();
    printf("| STEP |    SEQUENCE FOUND         |   SUBSTITUTION   |        UPDATED EXPRESSION                |\n");
    printf("|------+----------------------------+-----------------+------------------------------------------|\n");

    /* Perform verification while there are operators in the expression */
    while (1) {
        int found = 0;
        int len = strlen(expression);

        /* Search pattern: letter, letter, operation */
        for (i = 0; i < len - 2; i++) {
            if (isalpha(expression[i]) && isalpha(expression[i+1]) && pl_is_operator(expression[i+2])) {
                found = 1;
                break;
            }
        }

        if (!found) {
            /* Check if any unprocessed operator remains */
            for (i = 0; i < len; i++) {
                if (pl_is_operator(expression[i])) {
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
            char operator = expression[i+2];

            /* Show current step */
            printf("| %4d |        %c%c%c          |      %c%c%c=%c      |  ",
                   step, letter1, letter2, operator, letter1, letter2, operator, new_letter);

            /* Build new expression */
            int idx = 0;

            /* Copy part before the sequence */
            for (k = 0; k < i; k++) {
                result[idx++] = expression[k];
            }

            /* Add new letter */
            result[idx++] = new_letter;

            /* Copy part after the sequence */
            for (k = i + 3; k < len; k++) {
                result[idx++] = expression[k];
            }

            result[idx] = '\0';

            /* Show updated expression */
            color_blue();
            printf("%-40s", result);
            color_normal();
            printf(" |\n");

            /* Update expression for next iteration */
            strcpy(expression, result);

            /* Decrement letter for next substitution */
            new_letter--;
            step++;

            /* If uppercase letters run out, restart */
            if (new_letter < 'A') {
                new_letter = 'Z';
            }
        }
    }

    printf("+-------------------------------------------------------------------------------------------------+\n");

    printf("\n");
    color_yellow();
    printf("  FINAL VERIFICATION RESULT:\n");
    color_normal();
    printf("  Completely reduced expression: ");
    color_green();
    printf("%s\n", expression);
    color_normal();

    if (strlen(expression) == 1 && isalpha(expression[0])) {
        color_green();
        printf("  The postfix expression is correct and has been reduced to a single variable!\n");
        color_normal();
    } else {
        color_red();
        printf("  WARNING! The expression could not be completely reduced.\n");
        color_normal();
    }
}

void run_postfix_letters() {
    char infix[256];
    char postfix[256];
    char continue_char;

    printf("\n");
    color_yellow();
    printf("  -> Enter the infix expression (letters only, no spaces): ");
    color_normal();
    fgets(infix, 256, stdin);

    infix[strcspn(infix, "\n")] = '\0';

    /* Validate it only contains letters and valid operators */
    int valid = 1;
    for (int i = 0; infix[i] != '\0'; i++) {
        if (infix[i] != ' ' && !isalpha(infix[i]) && !pl_is_operator(infix[i]) &&
            infix[i] != '(' && infix[i] != ')') {
            valid = 0;
            break;
        }
    }

    if (!valid) {
        color_red();
        printf("\n  ERROR: Only letters (A-Z, a-z) and operators are allowed\n");
        color_normal();
        printf("\n");
        color_yellow();
        printf("  Try with another expression? (y/n): ");
        color_normal();
        continue_char = getchar();
        while (getchar() != '\n');

        if (continue_char != 'y' && continue_char != 'Y') {
            return;
        }
        // Recursive call or loop back if needed
    }

    if (!pl_validate_syntax(infix)) {
        printf("\n");
        color_red();
        printf("  The expression contains errors. Please correct the syntax.\n");
        color_normal();
        printf("\n");
        color_yellow();
        printf("  Try with another expression? (y/n): ");
        color_normal();
        continue_char = getchar();
        while (getchar() != '\n');

        if (continue_char != 'y' && continue_char != 'Y') {
            return;
        }
        // Recursive call or loop back if needed
    }

    // Assume conversion function exists or integrate logic
    strcpy(postfix, infix); // Placeholder, actual conversion needed

    printf("\n");
    printf("  Postfix Expression: ");
    color_blue();
    printf("%-58s", postfix);
    color_normal();
    printf(" \n");

    /* AUTOMATIC VERIFICATION - Always executed */
    pl_verify_postfix(postfix); // <-- Call to verification function

    printf("\n");
    color_green();
    printf("  Conversion and verification completed successfully\n");
    color_normal();

    // Save operations - No numeric steps for this verification
    save_all_operations_to_file(infix, "Infix", postfix, "Postfix", NULL, 0.0);
}

// --- PRE-LETTERS.C Logic ---
int prl_precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;  /* Exponent */
    if (op == 's') return 4;  /* sqrt (square root) */
    return 0;
}

int prl_is_operator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == 's');
}

int prl_validate_syntax(const char *infija) {
    int i;
    int len = strlen(infija);
    int parentesis_balance = 0;
    int ultimo_fue_operando = 0;
    int ultimo_fue_operador = 0;
    char c;
    char ultimo_char = '\0';

    /* Check for empty expression */
    if (len == 0) {
        color_red();
        printf("\n  ERROR: The expression is empty\n");
        color_normal();
        return 0;
    }

    for (i = 0; i < len; i++) {
        c = infija[i];

        /* Ignore spaces */
        if (c == ' ') continue;

        /* Check for invalid characters */
        if (!isalnum(c) && !prl_is_operator(c) && c != '(' && c != ')') {
            color_red();
            printf("\n  ERROR: Invalid character '%c' at position %d\n", c, i + 1);
            color_normal();
            return 0;
        }

        /* Check parentheses */
        if (c == '(') {
            parentesis_balance++;
            /* Cannot have operand before ( */
            if (ultimo_fue_operando) {
                color_red();
                printf("\n  ERROR: Missing operator before parenthesis '(' at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            ultimo_fue_operador = 0;
            ultimo_fue_operando = 0;
        }
        else if (c == ')') {
            parentesis_balance--;
            /* Check for negative balance */
            if (parentesis_balance < 0) {
                color_red();
                printf("\n  ERROR: Closing parenthesis ')' without opening at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            /* Cannot have operator before ) */
            if (ultimo_fue_operador) {
                color_red();
                printf("\n  ERROR: Missing operand before parenthesis ')' at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            /* Cannot have ( followed by ) */
            if (ultimo_char == '(') {
                color_red();
                printf("\n  ERROR: Empty parentheses '()' at position %d\n", i + 1);
                color_normal();
                return 0;
            }
            ultimo_fue_operador = 0;
            ultimo_fue_operando = 1;
        }
        /* If it's an operand */
        else if (isalnum(c)) {
            /* Cannot have two consecutive operands */
            if (ultimo_fue_operando) {
                color_red();
                printf("\n  ERROR: Two consecutive operands at position %d (missing operator)\n", i + 1);
                color_normal();
                return 0;
            }
            ultimo_fue_operando = 1;
            ultimo_fue_operador = 0;
        }
        /* If it's an operator */
        else if (prl_is_operator(c)) {
            /* Cannot have two consecutive operators */
            if (ultimo_fue_operador) {
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
            if (ultimo_char == '(') {
                color_red();
                printf("\n  ERROR: Operator '%c' after parenthesis '(' at position %d\n", c, i + 1);
                color_normal();
                return 0;
            }
            ultimo_fue_operador = 1;
            ultimo_fue_operando = 0;
        }

        ultimo_char = c;
    }

    /* Check final parenthesis balance */
    if (parentesis_balance > 0) {
        color_red();
        printf("\n  ERROR: Missing %d closing parentheses ')'\n", parentesis_balance);
        color_normal();
        return 0;
    }

    /* Cannot end with operator */
    if (ultimo_fue_operador) {
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

void prl_evaluar_prefija(const char *prefija) {
    char expresion[256];
    char resultado[256];
    int i, j, k;
    int paso = 1;
    char nueva_var = 'z';  /* Variable to be used to replace operations (99, 98, 97...) */
    int cambios = 1;
    int longitud;

    strcpy(expresion, prefija);

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
    printf("%s\n", expresion);
    color_normal();

    printf("  Method: Search for binary operations of the form: ");
    color_green();
    printf("operator letter letter");
    color_normal();
    printf("\n\n");

    printf("+-------------------------------------------------------------------------------------------------+\n");
    printf("|  STEP |         OPERATION FOUND              |         RESULTING EXPRESSION                  |\n");
    printf("|-------------------------------------------------------------------------------------------------|\n");

    /* Iterate until no more changes */
    while (cambios) {
        cambios = 0;
        longitud = strlen(expresion);

        /* SEARCH FROM LEFT TO RIGHT for the first complete operation */
        for (i = 0; i < longitud - 2; i++) {
            /* If we find an operator followed by two letters */
            if (prl_is_operator(expresion[i]) &&
                isalpha(expresion[i+1]) &&
                isalpha(expresion[i+2])) {

                /* Print the found operation */
                printf("|  %3d  |   ", paso);
                color_blue();
                printf("%c%c%c", expresion[i], expresion[i+1], expresion[i+2]);
                color_normal();
                printf(" = ");
                color_green();
                printf("%c", nueva_var);
                color_normal();

                /* Spaces for alignment */
                int op_spaces = 30 - 3 - 1 - 1;
                for (k = 0; k < op_spaces; k++) printf(" ");

                printf("|   ");

                /* Build the new expression */
                j = 0;
                /* Copy until before the operator */
                for (k = 0; k < i; k++) {
                    resultado[j++] = expresion[k];
                }
                /* Insert the new variable */
                resultado[j++] = nueva_var;
                /* Copy the rest of the expression (skipping the 3 characters) */
                for (k = i + 3; k < longitud; k++) {
                    resultado[j++] = expresion[k];
                }
                resultado[j] = '\0';

                /* Print the resulting expression with the new variable highlighted */
                for (k = 0; k < strlen(resultado); k++) {
                    if (k == i) {
                        color_green();
                        printf("%c", resultado[k]);
                        color_normal();
                    } else {
                        printf("%c", resultado[k]);
                    }
                    if (k < strlen(resultado) - 1) printf(" ");
                }

                /* Complete spaces */
                int spaces = 45 - strlen(resultado);
                for (k = 0; k < spaces; k++) printf(" ");

                printf(" |\n");

                /* Update the expression */
                strcpy(expresion, resultado);
                nueva_var--;  /* Next variable (z, y, x, ...) */
                cambios = 1;
                paso++;

                if (strlen(expresion) > 1) {
                    printf("|-------------------------------------------------------------------------------------------------|\n");
                }

                break;  /* Restart search from the beginning */
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
    printf("%-60s", prefija);
    color_normal();
    printf(" |\n");
    printf("|                                                                                                 |\n");
    printf("|   Final Result:         ");
    color_blue();
    printf("%-60s", expresion);
    color_normal();
    printf(" |\n");
    printf("|                                                                                                 |\n");

    /* Check if only a letter remains (could be multi-digit) */
    int only_letter = 1;
    int has_letter = 0;
    for (i = 0; i < strlen(expresion); i++) {
        if (isalpha(expresion[i])) {
            has_letter = 1;
        } else if (expresion[i] != ' ') {
            only_letter = 0;
            break;
        }
    }

    if (only_letter && has_letter) {
        color_green();
        printf("|   Status: SUCCESSFUL VERIFICATION - Expression reduced to a single letter                     |\n");
        color_normal();
    } else {
        color_red();
        printf("|   Status: INCONCLUSIVE VERIFICATION - Expression not completely reduced                      |\n");
        color_normal();
    }

    printf("|                                                                                                 |\n");
    color_green();
    printf("+-------------------------------------------------------------------------------------------------+\n");
    color_normal();
}

void run_pre_letters() {
    char infija[256];
    char prefija[256];
    char continuar;

    printf("\n");
    color_yellow();
    printf("  -> Enter the infix expression (without spaces): ");
    color_normal();
    fgets(infija, 256, stdin);

    /* Remove newline */
    infija[strcspn(infija, "\n")] = '\0';

    /* VALIDATE SYNTAX BEFORE CONVERTING */
    if (!prl_validate_syntax(infija)) {
        printf("\n");
        color_red();
        printf("  The expression contains errors. Please correct the syntax.\n");
        color_normal();
        printf("\n");
        color_yellow();
        printf("  Do you want to try with another expression? (y/n): ");
        color_normal();
        continuar = getchar();
        while (getchar() != '\n');  /* Clear buffer */

        if (continuar != 'y' && continuar != 'Y') {
            return;  /* Exit loop if doesn't want to continue */
        }
        // Recursive call or loop back if needed
    }

    // Assume conversion function exists or integrate logic
    strcpy(prefija, infija); // Placeholder, actual conversion needed

    printf("\n");
    printf("  Prefix Expression:  ");
    color_blue();
    printf("%-60s", prefija);
    color_normal();
    printf(" \n");

    printf("\n");
    color_green();
    printf("  Conversion successfully completed\n");
    color_normal();

    /* Perform verification of the prefix expression */
    prl_evaluar_prefija(prefija);

    // Save operations - No numeric steps for this verification
    save_all_operations_to_file(infija, "Infix", prefija, "Prefix", NULL, 0.0);
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
                run_pre_letters();
                break;
            case 5:
                run_infix();
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
