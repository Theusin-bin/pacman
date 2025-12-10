#ifndef JOGO_H
#define JOGO_H

#include "raylib.h"
#include "mapa.h"
#include "menu.h"
#include <stdbool.h>

// Estados do jogo
typedef enum {
    MENU = 0,
    JOGANDO,
    PAUSA,
    GAME_OVER,
    VITORIA
} EstadoJogo;

// Inicialização e controle
bool inicializarJogo(void);
void finalizarJogo(void);
void atualizarJogo(void);
void desenharJogo(void);
void processarInput(void);

// Controle de jogo
void reiniciarJogo(void);
void adicionarPontos(int pontos);
void perderVida(void);
bool carregarMapaPorNivel(int nivel);
bool carregarProximoMapa(void);

// Funções de desenho (definidas em jogo.c)
void desenharPausa(void);
void desenharGameOver(void);
void desenharVitoria(void);

// Getters
int getVidas(void);
int getPontuacao(void);
EstadoJogo getEstadoJogo(void);
Mapa* getMapaAtual(void);
Menu* getMenuJogo(void);
bool existeProximoNivel(void);

// Setters
void setEstadoJogo(EstadoJogo novoEstado);
void setNivelAtual(int nivel);

#endif // JOGO_H