// MainCalculator.c - Expression Calculator System
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- VT100 Sequence Definitions ---
#define RESET_COLOR "\033[0m"
#define COLOR_RED "\033[1;31m"
#define COLOR_GREEN "\033[1;32m"
#define COLOR_YELLOW "\033[1;33m"
#define COLOR_BLUE "\033[1;34m"
#define COLOR_CYAN "\033[1;36m"

// --- Portable screen clear function ---
void clear_screen() {
    printf("\033[2J\033[H");
    fflush(stdout);
}

// Color function prototypes
void reset_color() { printf(RESET_COLOR); }
void set_green() { printf(COLOR_GREEN); }
void set_blue() { printf(COLOR_BLUE); }
void set_yellow() { printf(COLOR_YELLOW); }
void set_red() { printf(COLOR_RED); }
void set_cyan() { printf(COLOR_CYAN); }

// Function to show the main banner
void show_banner() {
    set_cyan();
    printf("\n");
    printf("  ================================================================================\n");
    printf("  ||                                                                            ||\n");
    printf("  ||                   EXPRESSION CALCULATOR SYSTEM                             ||\n");
    printf("  ||              Infix, Prefix and Postfix Conversions                         ||\n");
    printf("  ||                                                                            ||\n");
    printf("  ================================================================================\n");
    reset_color();
}

// Function to show the main menu
void show_menu() {
    set_green();
    printf("\n  ================================================================================\n");
    printf("  ||                            MAIN MENU                                       ||\n");
    printf("  ================================================================================\n");
    reset_color();
    
    printf("  ||                                                                            ||\n");
    printf("  ||  ");
    set_yellow();
    printf("[1]");
    reset_color();
    printf(" PREFIX with LETTERS    - Convert infix to prefix (symbolic)           ||\n");
    
    printf("  ||  ");
    set_yellow();
    printf("[2]");
    reset_color();
    printf(" INFIX EVALUATOR        - Evaluate infix expressions with numbers      ||\n");
    
    printf("  ||  ");
    set_yellow();
    printf("[3]");
    reset_color();
    printf(" POSTFIX with LETTERS   - Convert infix to postfix (symbolic)          ||\n");
    
    printf("  ||  ");
    set_yellow();
    printf("[4]");
    reset_color();
    printf(" PREFIX with NUMBERS    - Convert infix to prefix with evaluation      ||\n");
    
    printf("  ||  ");
    set_yellow();
    printf("[5]");
    reset_color();
    printf(" POSTFIX with NUMBERS   - Convert infix to postfix with evaluation     ||\n");
    
    printf("  ||                                                                            ||\n");
    
    printf("  ||  ");
    set_cyan();
    printf("[0]");
    reset_color();
    printf(" HELP                   - User guide and examples                       ||\n");
    
    printf("  ||  ");
    set_red();
    printf("[6]");
    reset_color();
    printf(" EXIT                   - Close the program                             ||\n");
    
    printf("  ||                                                                            ||\n");
    set_green();
    printf("  ================================================================================\n");
    reset_color();
}

// Function to show help
void show_help() {
    clear_screen();
    set_cyan();
    printf("\n  ================================================================================\n");
    printf("  ||                            USER GUIDE                                      ||\n");
    printf("  ================================================================================\n");
    reset_color();
    
    printf("\n");
    set_green();
    printf("  1. WHAT IS THIS CALCULATOR?\n");
    reset_color();
    printf("     This system allows you to work with mathematical expressions:\n");
    printf("     - INFIX   : Standard notation (e.g., a + b * c)\n");
    printf("     - PREFIX  : Operators before operands (e.g., + a * b c)\n");
    printf("     - POSTFIX : Operators after operands (e.g., a b c * +)\n");
    
    printf("\n");
    set_green();
    printf("  2. OPERATOR PRECEDENCE\n");
    reset_color();
    printf("     From highest to lowest priority:\n");
    printf("     1. ( )    - Parentheses\n");
    printf("     2. ^      - Exponentiation\n");
    printf("     3. * /    - Multiplication and Division\n");
    printf("     4. + -    - Addition and Subtraction\n");
    
    printf("\n");
    set_green();
    printf("  3. MODULE DESCRIPTIONS\n");
    reset_color();
    
    printf("\n     [1] PREFIX with LETTERS\n");
    printf("         - Converts infix to prefix using variables (a, b, c...)\n");
    printf("         - Example: a+b*c -> +a*bc\n");
    
    printf("\n     [2] INFIX EVALUATOR\n");
    printf("         - Evaluates infix expressions with numbers\n");
    printf("         - Example: 3+4*5 -> 23\n");
    printf("         - Can save results to a file\n");
    
    printf("\n     [3] POSTFIX with LETTERS\n");
    printf("         - Converts infix to postfix using variables\n");
    printf("         - Example: a+b*c -> abc*+\n");
    
    printf("\n     [4] PREFIX with NUMBERS\n");
    printf("         - Converts and evaluates infix to prefix\n");
    printf("         - Example: 18/(3^2) -> /18^32 -> 2\n");
    
    printf("\n     [5] POSTFIX with NUMBERS\n");
    printf("         - Converts and evaluates infix to postfix\n");
    printf("         - Example: 3^2^3 -> 3 2 3 ^ ^ -> 6561\n");
    
    printf("\n");
    set_green();
    printf("  4. USAGE TIPS\n");
    reset_color();
    printf("     - Always use parentheses when in doubt\n");
    printf("     - Spaces are optional\n");
    printf("     - For letters: use single lowercase letters (a-z)\n");
    printf("     - For numbers: multi-digit numbers are supported\n");
    
    printf("\n");
    set_green();
    printf("  5. CONVERSION EXAMPLES\n");
    reset_color();
    printf("     Infix:    a*b+(c^2-d)\n");
    printf("     Prefix:   +*ab-^c2d\n");
    printf("     Postfix:  ab*c2^d-+\n");
    
    printf("\n     Infix:    (a+b)*c\n");
    printf("     Prefix:   *+abc\n");
    printf("     Postfix:  ab+c*\n");
    
    printf("\n");
    set_yellow();
    printf("  Press ENTER to return to main menu...");
    reset_color();
    getchar();
}

// Function to pause execution
void pause_execution() {
    printf("\n");
    set_yellow();
    printf("  Press ENTER to return to main menu...");
    reset_color();
    while(getchar() != '\n');
    getchar();
}

// Function to execute an external program
void execute_module(const char* executable_name, const char* module_name) {
    clear_screen();
    set_green();
    printf("\n  Starting: %s...\n", module_name);
    reset_color();
    printf("  ==============================================================\n\n");
    
    int result = system(executable_name);
    
    if (result != 0) {
        printf("\n  ");
        set_red();
        printf("ERROR: Could not execute %s\n", executable_name);
        printf("       Make sure the file exists in the same directory.\n");
        reset_color();
    }
    
    pause_execution();
}

// Main
int main(void) {
    int option;
    char input_buffer[10];
    
    while(1) {
        clear_screen();
        show_banner();
        show_menu();
        
        printf("\n  ");
        set_yellow();
        printf(">> Select an option: ");
        set_blue();
        fflush(stdout);
        
        if(fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
            continue;
        }
        
        option = atoi(input_buffer);
        reset_color();
        
        switch(option) {
            case 1:
                execute_module("PRE-LETTERS.exe", "PREFIX with LETTERS");
                break;
                
            case 2:
                execute_module("Infix.exe", "INFIX EVALUATOR");
                break;
                
            case 3:
                execute_module("POSTFIX-LETTERS.exe", "POSTFIX with LETTERS");
                break;
                
            case 4:
                execute_module("PRE-NUM.exe", "PREFIX with NUMBERS");
                break;
                
            case 5:
                execute_module("POST-NUM.exe", "POSTFIX with NUMBERS");
                break;
                
            case 0:
                show_help();
                break;
                
            case 6:
                clear_screen();
                printf("\n");
                set_cyan();
                printf("  ================================================================================\n");
                printf("  ||                                                                            ||\n");
                printf("  ||            ");
                set_green();
                printf("Thank you for using the calculator!");
                set_cyan();
                printf("                             ||\n");
                printf("  ||                                                                            ||\n");
                printf("  ||                         ");
                set_yellow();
                printf("Goodbye!");
                set_cyan();
                printf("                                          ||\n");
                printf("  ||                                                                            ||\n");
                printf("  ================================================================================\n");
                reset_color();
                printf("\n");
                return 0;
                
            default:
                set_red();
                printf("\n  ERROR: Invalid option. Please select a number from 0 to 6.\n");
                reset_color();
                pause_execution();
                break;
        }
    }
    
    return 0;
}
