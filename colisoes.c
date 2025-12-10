// colisoes.c - VERSÃO COMPLETA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "colisoes.h"
#include "graficos.h"

extern PacMan* pacman_global;
extern Fantasma* fantasmas_global;
// Função auxiliar para contar fantasmas
static int contar_fantasmas_lista(Fantasma* lista) {
    int count = 0;
    Fantasma* atual = lista;
    while (atual) {
        count++;
        atual = atual->prox;
    }
    return count;
}

// Verifica colisões do Pac-Man
ResultadoColisao verificar_colisao_pacman(PacMan* pacman, Mapa* mapa, Fantasma* fantasmas) {
    ResultadoColisao resultado = {
        COLISAO_NENHUMA,  // tipo
        0,                // direcao_saida
        0,                // linha_saida
        0,                // coluna_saida
        -1                // fantasma_index
    };
    
    // 1. Verifica limites do mapa
    if (pacman->pos.linha < 0 || pacman->pos.linha >= mapa->linhas ||
        pacman->pos.coluna < 0 || pacman->pos.coluna >= mapa->colunas) {
        resultado.tipo = COLISAO_PAREDE;
        return resultado;
    }
    
    char elemento = mapa->matriz[pacman->pos.linha][pacman->pos.coluna];
    
    // 2. Verifica parede
    if (elemento == '#') {
        resultado.tipo = COLISAO_PAREDE;
        return resultado;
    }
    
    // 3. Verifica portal
    if (elemento == 'T') {
        resultado.tipo = COLISAO_PORTAL;
        resultado.direcao_saida = pacman->direcao;
        processar_portal(mapa, &resultado.linha_saida, &resultado.coluna_saida, pacman->direcao);
        return resultado;
    }
    
    // 4. Verifica pellet
    if (elemento == '.') {
        resultado.tipo = COLISAO_PELLET;
        return resultado;
    }
    
    // 5. Verifica power pellet
    if (elemento == 'O') {
        resultado.tipo = COLISAO_POWER_PELLET;
        return resultado;
    }
    
    // 6. Verifica colisão com fantasmas
    Fantasma* atual = fantasmas;
    int fantasma_idx = 0;
    while (atual) {
        if (atual->pos.linha == pacman->pos.linha && 
            atual->pos.coluna == pacman->pos.coluna) {
            
            if (atual->vulneravel) {
                resultado.tipo = COLISAO_FANTASMA_VULNERAVEL;
                resultado.fantasma_index = fantasma_idx;
            } else {
                resultado.tipo = COLISAO_FANTASMA;
                resultado.fantasma_index = fantasma_idx;
            }
            return resultado;
        }
        atual = atual->prox;
        fantasma_idx++;
    }
    
    return resultado;
}

// Verifica se fantasma pode se mover para posição
int verificar_colisao_fantasma_mapa(Fantasma* fantasma, Mapa* mapa) {
    // Verifica limites
    if (fantasma->pos.linha < 0 || fantasma->pos.linha >= mapa->linhas ||
        fantasma->pos.coluna < 0 || fantasma->pos.coluna >= mapa->colunas) {
        return 0; // Colisão
    }
    
    // Verifica parede
    if (mapa->matriz[fantasma->pos.linha][fantasma->pos.coluna] == '#') {
        return 0; // Colisão
    }
    
    // Verifica portal
    if (mapa->matriz[fantasma->pos.linha][fantasma->pos.coluna] == 'T') {
        return 2; // Portal
    }
    
    return 1; // Movimento permitido
}

// Verifica colisão entre fantasmas
int verificar_colisao_fantasma_fantasma(Fantasma* fantasma, Fantasma* lista, int excluir_atual) {
    Fantasma* atual = lista;
    int contador = 0;
    
    while (atual) {
        if (excluir_atual && contador == excluir_atual) {
            contador++;
            atual = atual->prox;
            continue;
        }
        
        if (atual->pos.linha == fantasma->pos.linha && 
            atual->pos.coluna == fantasma->pos.coluna) {
            return 0; // Colisão
        }
        
        contador++;
        atual = atual->prox;
    }
    
    return 1; // Sem colisão
}

// Processa teleporte por portal
void processar_portal(Mapa* mapa, int* linha, int* coluna, int direcao) {
    // Encontra outro portal no mapa
    for (int i = 0; i < mapa->linhas; i++) {
        for (int j = 0; j < mapa->colunas; j++) {
            if (mapa->matriz[i][j] == 'T' && 
                !(i == *linha && j == *coluna)) { // Não é o mesmo portal
                
                // Teleporta mantendo direção
                switch (direcao) {
                    case 1: // Cima
                        if (i > 0) *linha = i - 1;
                        else *linha = i;
                        *coluna = j;
                        break;
                    case 2: // Baixo
                        if (i < mapa->linhas - 1) *linha = i + 1;
                        else *linha = i;
                        *coluna = j;
                        break;
                    case 3: // Esquerda
                        if (j > 0) *coluna = j - 1;
                        else *coluna = j;
                        *linha = i;
                        break;
                    case 4: // Direita
                        if (j < mapa->colunas - 1) *coluna = j + 1;
                        else *coluna = j;
                        *linha = i;
                        break;
                }
                return;
            }
        }
    }
}

// Função de integração para o sistema de jogo
void verificarTodasColisoes(Mapa* mapa) {
    if (!pacman_global || !fantasmas_global || !mapa) return;
    
    Fantasma* fantasma_atual = fantasmas_global;
    
    // Verifica se algum fantasma está na mesma posição do Pac-Man
    while (fantasma_atual) {
        if (fantasma_atual->pos.linha == pacman_global->pos.linha &&
            fantasma_atual->pos.coluna == pacman_global->pos.coluna) {
            
            if (fantasma_atual->vulneravel) {
                // Come fantasma vulnerável
                pacman_global->pontos += 200;
                
                // Reposiciona fantasma
                fantasma_atual->pos.linha = 1;
                fantasma_atual->pos.coluna = 1;
                fantasma_atual->vulneravel = 0;
                fantasma_atual->timer_vulneravel = 0;
                
                printf("Fantasma comido em tempo real! +200 pontos\n");
            } else if (pacman_global->power_timer == 0) {
                // Colisão com fantasma normal (apenas se não estiver com power)
                pacman_global->vidas--;
                
                printf("COLISÃO EM TEMPO REAL! Vidas: %d\n", pacman_global->vidas);
                
                if (pacman_global->vidas > 0) {
                    // Reposiciona Pac-Man
                    pacman_global->pos.linha = mapa->linhas / 2;
                    pacman_global->pos.coluna = mapa->colunas / 2;
                    
                    // Ajustar posição se for parede
                    while (mapa->matriz[pacman_global->pos.linha][pacman_global->pos.coluna] == '#') {
                        pacman_global->pos.linha++;
                        if (pacman_global->pos.linha >= mapa->linhas) {
                            pacman_global->pos.linha = 1;
                            pacman_global->pos.coluna++;
                        }
                    }
                }
            }
            break; // Só verifica um fantasma por frame
        }
        fantasma_atual = fantasma_atual->prox;
    }
}