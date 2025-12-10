#ifndef COLISOES_H
#define COLISOES_H

#include "mapa.h"
#include "entidades.h"

typedef enum {
    COLISAO_NENHUMA,
    COLISAO_PAREDE,
    COLISAO_PELLET,
    COLISAO_POWER_PELLET,
    COLISAO_PORTAL,
    COLISAO_FANTASMA,
    COLISAO_FANTASMA_VULNERAVEL
} TipoColisao;

typedef struct {
    TipoColisao tipo;
    int direcao_saida;
    int linha_saida;
    int coluna_saida;
    int fantasma_index;
} ResultadoColisao;

// Funções principais
ResultadoColisao verificar_colisao_pacman(PacMan* pacman, Mapa* mapa, Fantasma* fantasmas);
int verificar_colisao_fantasma_mapa(Fantasma* fantasma, Mapa* mapa);
int verificar_colisao_fantasma_fantasma(Fantasma* fantasma, Fantasma* lista, int excluir_atual);
void processar_portal(Mapa* mapa, int* linha, int* coluna, int direcao);

// Função de integração
void verificarTodasColisoes(Mapa* mapa); // NOVA LINHA

#endif