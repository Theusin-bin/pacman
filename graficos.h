// graficos.h - VERSÃO CORRIGIDA
#ifndef GRAFICOS_H
#define GRAFICOS_H

#include "raylib.h"
#include "mapa.h"
#include "entidades.h"

// Cores do jogo
#define COR_PAREDE BLUE
#define COR_PACMAN YELLOW
#define COR_FANTASMA RED
#define COR_PELLET WHITE
#define COR_POWER_PELLET GREEN
#define COR_PORTA MAGENTA
#define COR_FUNDO BLACK

// Tamanhos (conforme especificação do trabalho)
#define BLOCO_TAMANHO 40
#define HUD_ALTURA 40
#define LARGURA_JANELA 1600    // 40 * BLOCO_TAMANHO (ajustado dinamicamente)
#define ALTURA_JANELA 840      // 20 * BLOCO_TAMANHO + HUD_ALTURA

// Funções de desenho
void inicializar_janela(void);
void fechar_janela(void);
void desenhar_mapa(Mapa* mapa);
void desenhar_pacman(PacMan* pacman);
void desenhar_fantasmas(Fantasma* lista);
void desenhar_hud(PacMan* pacman, Mapa* mapa, int nivel);
int tecla_para_direcao(void);

#endif