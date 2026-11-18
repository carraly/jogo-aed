# jogo-aed

Jogo endless runner feito com Raylib, no estilo do dinossauro do Chrome, ambientado em uma rua da Bahia.

## Como executar

1. Compile o jogo com um dos comandos abaixo.
2. Confira se a pasta `assets/` esta ao lado do executavel gerado. Ela ja faz parte do repositorio.
3. Abra o executavel `jogo-aed.exe` ou rode `./jogo-aed.exe` no terminal.

Com o Makefile:

```
make
make run
```

No Windows com MinGW, use `mingw32-make` no lugar de `make`.

Sem o Makefile, compile e execute direto com o gcc:

```
gcc -Wall -std=c99 -O2 -I include src/*.c -o jogo-aed.exe -L lib/windows -lraylib -lopengl32 -lgdi32 -lwinmm
jogo-aed.exe
```

No Linux, o Makefile usa automaticamente a biblioteca em `lib/linux`.

## Controles

- Seta para cima ou Espaco: pular (segure para pular mais alto)
- Seta para baixo: deslizar ou cair mais rapido no ar
- Setas para cima e para baixo: navegar no menu
- Enter ou Espaco: confirmar
- Esc: voltar do ranking
- F11: alternar janela sem bordas e tela cheia

A janela pode ser redimensionada livremente e mantem a proporcao 16:9, com barras pretas quando necessario.
