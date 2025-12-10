#ifndef ENTIDADES_H
#define ENTIDADES_H
#define DIRECAO_CIMA 0
#define DIRECAO_BAIXO 1
#define DIRECAO_ESQUERDA 2
#define DIRECAO_DIREITA 3

#include "mapa.h"

typedef struct {
    int linha, coluna;
} Posicao;

typedef struct {
    Posicao pos;
    int vidas;
    int pontos;
    int power_timer;
    int direcao;
} PacMan;

typedef struct Fantasma {
    Posicao pos;
    int vulneravel;
    int timer_vulneravel;
    int direcao;
    struct Fantasma* prox;
} Fantasma;

// Funções básicas
PacMan* criar_pacman(int linha, int coluna);
Fantasma* criar_fantasma(int linha, int coluna);
void liberar_fantasmas(Fantasma* lista);
Fantasma* adicionar_fantasma(Fantasma* lista, int linha, int coluna);

// Funções de movimento
int mover_pacman_com_colisao(PacMan* pacman, Mapa* mapa, Fantasma* lista_fantasmas, int direcao);
void mover_fantasma_com_colisao(Fantasma* fantasma, Mapa* mapa, Fantasma* lista_fantasmas, PacMan* pacman);
void mover_fantasma_inteligente(Fantasma* fantasma, Mapa* mapa, PacMan* pacman); // NOVA LINHA

// Funções de atualização
void atualizar_estado_pacman(PacMan* pacman, Mapa* mapa);
void atualizar_estado_fantasmas(Fantasma* lista, PacMan* pacman);

// Controle de velocidade
void set_pacman_speed(int speed);
int get_pacman_speed(void);
void reset_pacman_speed(void);
void increase_pacman_speed(void);
void decrease_pacman_speed(void);

// Funções de integração
int inicializarEntidades(Mapa* mapa);
void atualizarEntidades(Mapa* mapa);
void desenharEntidades(void);
void finalizarEntidades(void);
void reposicionarEntidades(Mapa* mapa);
void imprimir_estado(PacMan* pacman, Fantasma* lista);

// Variáveis globais
extern PacMan* pacman_global;
extern Fantasma* fantasmas_global;
void reposicionarFantasma(Fantasma* fantasma, Mapa* mapa);
#endif