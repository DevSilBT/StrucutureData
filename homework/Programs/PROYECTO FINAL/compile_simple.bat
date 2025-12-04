@echo off
echo Compiling all modules...

REM Compila todos los módulos en un solo comando
gcc main\MainCalculator.c -o MainCalculator.exe
gcc main\PRE-LETTERS.c source\list.c source\dlist.c source\stack.c -Iinclude -o PRE-LETTERS.exe -lm
gcc main\Infix.c source\list.c source\dlist.c source\stack.c source\queue.c -Iinclude -o Infix.exe -lm
gcc main\POSTFIX-LETTERS.c source\list.c source\dlist.c source\stack.c -Iinclude -o POSTFIX-LETTERS.exe -lm
gcc main\PRE-NUM.c source\list.c source\dlist.c source\stack.c -Iinclude -o PRE-NUM.exe -lm
gcc main\POST-NUM.c source\list.c source\dlist.c source\stack.c -Iinclude -o POST-NUM.exe -lm

echo Done!
echo.
echo Run: MainCalculator
pause