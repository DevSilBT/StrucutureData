@echo off
echo ===============================================
echo      COMPILING EXPRESSION CALCULATOR
echo ===============================================
echo.

echo [1] Compiling data structures...
gcc -c source\list.c -Iinclude -o list.o
gcc -c source\dlist.c -Iinclude -o dlist.o
gcc -c source\stack.c -Iinclude -o stack.o
gcc -c source\queue.c -Iinclude -o queue.o

echo.
echo [2] Compiling main modules...
echo.

echo  2.1 PRE-LETTERS...
gcc -c main\PRE-LETTERS.c -Iinclude -o PRE-LETTERS.o
gcc PRE-LETTERS.o list.o dlist.o stack.o -o PRE-LETTERS.exe -lm

echo  2.2 Infix...
gcc -c main\Infix.c -Iinclude -o Infix.o
gcc Infix.o list.o dlist.o stack.o queue.o -o Infix.exe -lm

echo  2.3 POSTFIX-LETTERS...
gcc -c main\POSTFIX-LETTERS.c -Iinclude -o POSTFIX-LETTERS.o
gcc POSTFIX-LETTERS.o list.o dlist.o stack.o -o POSTFIX-LETTERS.exe -lm

echo  2.4 PRE-NUM...
gcc -c main\PRE-NUM.c -Iinclude -o PRE-NUM.o
gcc PRE-NUM.o list.o dlist.o stack.o -o PRE-NUM.exe -lm

echo  2.5 POST-NUM...
gcc -c main\POST-NUM.c -Iinclude -o POST-NUM.o
gcc POST-NUM.o list.o dlist.o stack.o -o POST-NUM.exe -lm

echo  2.6 MainCalculator...
gcc main\MainCalculator.c -o MainCalculator.exe

echo.
echo ===============================================
echo      COMPILATION COMPLETE!
echo ===============================================
echo.
echo Generated executables:
echo   MainCalculator.exe    - Main menu
echo   PRE-LETTERS.exe       - Prefix with letters
echo   Infix.exe             - Infix evaluator
echo   POSTFIX-LETTERS.exe   - Postfix with letters
echo   PRE-NUM.exe           - Prefix with numbers
echo   POST-NUM.exe          - Postfix with numbers
echo.
echo To run the program: MainCalculator
echo.
pause