// graficos.c - VERSÃO CORRIGIDA
#include "graficos.h"
#include <stdio.h>

// Declarações externas (variáveis globais de entidades.c)
extern PacMan* pacman_global;
extern Fantasma* fantasmas_global;

void inicializar_janela(void) {
    InitWindow(LARGURA_JANELA, ALTURA_JANELA, "PAC-MAN UFRJ");
    SetTargetFPS(60);
}

void fechar_janela(void) {
    CloseWindow();
}

void desenhar_mapa(Mapa* mapa) {
    if (!mapa) return;
    
    for (int i = 0; i < mapa->linhas; i++) {  // USAR mapa->linhas, não LINHAS constante
        for (int j = 0; j < mapa->colunas; j++) {  // USAR mapa->colunas, não COLUNAS constante
            int x = j * BLOCO_TAMANHO;
            int y = i * BLOCO_TAMANHO;
            
            switch (mapa->matriz[i][j]) {
                case '#': // Parede
                    DrawRectangle(x, y, BLOCO_TAMANHO, BLOCO_TAMANHO, COR_PAREDE);
                    break;
                case '.': // Pellet
                    DrawCircle(x + BLOCO_TAMANHO/2, y + BLOCO_TAMANHO/2, 
                              BLOCO_TAMANHO/8, COR_PELLET);
                    break;
                case 'O': // Power pellet
                    DrawCircle(x + BLOCO_TAMANHO/2, y + BLOCO_TAMANHO/2, 
                              BLOCO_TAMANHO/4, COR_POWER_PELLET);  // Tamanho corrigido
                    break;
                case 'T': // Portal
                    DrawRectangle(x, y, BLOCO_TAMANHO, BLOCO_TAMANHO, COR_PORTA);
                    break;
                // Espaços vazios não precisam ser desenhados
            }
        }
    }
}

void desenhar_pacman(PacMan* pacman) {
    if (!pacman) return;
    
    int x = pacman->pos.coluna * BLOCO_TAMANHO + BLOCO_TAMANHO/2;
    int y = pacman->pos.linha * BLOCO_TAMANHO + BLOCO_TAMANHO/2;
    
    DrawCircle(x, y, BLOCO_TAMANHO/2, COR_PACMAN);
}

void desenhar_fantasmas(Fantasma* lista) {
    Fantasma* atual = lista;
    while (atual) {
        int x = atual->pos.coluna * BLOCO_TAMANHO + BLOCO_TAMANHO/2;
        int y = atual->pos.linha * BLOCO_TAMANHO + BLOCO_TAMANHO/2;
        
        Color cor = atual->vulneravel ? BLUE : COR_FANTASMA;  // Azul quando vulnerável
        DrawCircle(x, y, BLOCO_TAMANHO/2, cor);
        
        atual = atual->prox;
    }
}

void desenhar_hud(PacMan* pacman, Mapa* mapa, int nivel) {
    if (!pacman || !mapa) return;
    
    int hud_y = mapa->linhas * BLOCO_TAMANHO;  // Ajustado dinamicamente
    
    // Fundo do HUD
    DrawRectangle(0, hud_y, LARGURA_JANELA, HUD_ALTURA, DARKGRAY);
    
    // Textos
    DrawText(TextFormat("VIDAS: %d", pacman->vidas), 10, hud_y + 10, 20, WHITE);
    DrawText(TextFormat("PONTOS: %06d", pacman->pontos), 200, hud_y + 10, 20, WHITE);
    DrawText(TextFormat("NIVEL: %d", nivel), 500, hud_y + 10, 20, WHITE);
    DrawText(TextFormat("PELLETS: %d", mapa->pontosRestantes), 700, hud_y + 10, 20, WHITE);
    
    if (pacman->power_timer > 0) {
        DrawText("POWER MODE!", 1000, hud_y + 10, 20, GREEN);
    }
}

int tecla_para_direcao(void) {
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) return 1;    // Cima
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) return 2;  // Baixo
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) return 3;  // Esquerda
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) return 4; // Direita
    return 0; // Nenhuma tecla pressionada
}