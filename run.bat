:: main and all of src
gcc main.c src/*.c -o sf-editor.exe -O1 -Wall -std=c99 -Wno-missing-braces -I include/ -I src/include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm

:: only main
:: gcc main.c -o template.exe -O1 -Wall -std=c99 -Wno-missing-braces -I include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm