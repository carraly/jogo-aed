# jogo-aed
Repositório para o jogo da cadeira Algoritmo e Estrutura de Dados

Jogo endless runner feito com Raylib, no estilo do dinossauro do Chrome, ambientado em uma rua da Bahia.

## Como compilar

Com o Makefile:

```
make
make run
```

Ou diretamente com o gcc:

```
gcc -Wall -std=c99 -O2 -I include src/*.c -o jogo-aed.exe -L lib/windows -lraylib -lopengl32 -lgdi32 -lwinmm
```

As texturas ficam na pasta `assets/`, que precisa estar ao lado do executável.

## Controles

- Seta para cima ou Espaco: pular (segure para pular mais alto)
- Seta para baixo: deslizar ou cair mais rapido no ar
- Setas para cima e para baixo: navegar no menu
- Enter ou Espaco: confirmar
- Esc: voltar do ranking
- F11: alternar janela sem bordas e tela cheia

A janela pode ser redimensionada livremente e mantem a proporcao 16:9, com barras pretas quando necessario.

