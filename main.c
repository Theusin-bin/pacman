#include "raylib.h"
#include "jogo.h"
#include "graficos.h"
#include <stdio.h>

int main(void) {
    // Inicializar janela usando as constantes do graficos.h
    inicializar_janela();
    
    // Inicializar jogo
    if (!inicializarJogo()) {
        printf("Falha ao inicializar jogo!\n");
        fechar_janela();
        return 1;
    }
    
    // Loop principal
    while (!WindowShouldClose()) {
        // Processar entrada
        processarInput();
        
        // Atualizar lógica do jogo
        atualizarJogo();
        
        // Renderizar
        BeginDrawing();
        
        desenharJogo();
        
        EndDrawing();
    }
    
    // Finalizar
    finalizarJogo();
    fechar_janela();
    
    return 0;
}