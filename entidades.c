// entidades.c - VERSÃO COMPLETA
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "entidades.h"
#include "mapa.h"
#include "colisoes.h"
#include "graficos.h"

// Variáveis globais
PacMan* pacman_global = NULL;
Fantasma* fantasmas_global = NULL;

// Variáveis de velocidade
static int pacman_speed_factor = 2;
static int pacman_frame_counter = 0;

// ================== FUNÇÕES AUXILIARES ==================

static int contar_fantasmas_lista(Fantasma* lista) {
    int count = 0;
    Fantasma* atual = lista;
    while (atual) {
        count++;
        atual = atual->prox;
    }
    return count;
}

static Fantasma* obter_fantasma_por_indice(Fantasma* lista, int index) {
    Fantasma* atual = lista;
    int contador = 0;
    
    while (atual) {
        if (contador == index) {
            return atual;
        }
        contador++;
        atual = atual->prox;
    }
    
    return NULL;
}

// ================== FUNÇÕES DE CONTROLE DE VELOCIDADE ==================

void set_pacman_speed(int speed) {
    if (speed >= 1 && speed <= 5) {
        pacman_speed_factor = speed;
        printf("Velocidade do Pac-Man ajustada para: %d\n", speed);
    }
}

int get_pacman_speed(void) {
    return pacman_speed_factor;
}

void reset_pacman_speed(void) {
    pacman_speed_factor = 2;
    pacman_frame_counter = 0;
}

void increase_pacman_speed(void) {
    if (pacman_speed_factor > 1) {
        pacman_speed_factor--;
        printf("Pac-Man mais rápido: %d\n", pacman_speed_factor);
    }
}

void decrease_pacman_speed(void) {
    if (pacman_speed_factor < 5) {
        pacman_speed_factor++;
        printf("Pac-Man mais lento: %d\n", pacman_speed_factor);
    }
}

// ================== FUNÇÕES BÁSICAS DE CRIAÇÃO ==================

PacMan* criar_pacman(int linha, int coluna) {
    PacMan* pacman = (PacMan*)malloc(sizeof(PacMan));
    if (!pacman) {
        printf("Erro ao alocar memoria para Pac-Man\n");
        return NULL;
    }
    
    pacman->pos.linha = linha;
    pacman->pos.coluna = coluna;
    pacman->vidas = 3;
    pacman->pontos = 0;
    pacman->power_timer = 0;
    pacman->direcao = 4; // Começa virado para direita
    
    printf("Pac-Man criado em (%d, %d)\n", linha, coluna);
    return pacman;
}

Fantasma* criar_fantasma(int linha, int coluna) {
    Fantasma* fantasma = (Fantasma*)malloc(sizeof(Fantasma));
    if (!fantasma) {
        printf("Erro ao alocar memoria para fantasma\n");
        return NULL;
    }
    
    fantasma->pos.linha = linha;
    fantasma->pos.coluna = coluna;
    fantasma->vulneravel = 0;
    fantasma->timer_vulneravel = 0;
    fantasma->direcao = rand() % 4; // Direção aleatória
    fantasma->prox = NULL;
    
    return fantasma;
}

void liberar_fantasmas(Fantasma* lista) {
    Fantasma* atual = lista;
    Fantasma* proximo;
    
    while (atual) {
        proximo = atual->prox;
        free(atual);
        atual = proximo;
    }
    
    printf("Fantasma(s) liberado(s) da memoria\n");
}

Fantasma* adicionar_fantasma(Fantasma* lista, int linha, int coluna) {
    Fantasma* novo = criar_fantasma(linha, coluna);
    if (!novo) return lista;
    
    novo->prox = lista;
    printf("Fantasma adicionado em (%d, %d)\n", linha, coluna);
    return novo;
}

// ================== FUNÇÕES DE INICIALIZAÇÃO ==================

int inicializarEntidades(Mapa* mapa) {
    if (!mapa) {
        printf("Erro: Mapa nulo ao inicializar entidades\n");
        return 0;
    }
    
    // Inicializar random
    srand((unsigned int)time(NULL));
    
    printf("Inicializando entidades no mapa %s...\n", mapa->nome);
    
    // Encontrar posição para Pac-Man
    int pacman_encontrado = 0;
    int pacman_linha = mapa->linhas / 2;
    int pacman_coluna = mapa->colunas / 2;
    
    // Procurar posição vazia no centro do mapa
    for (int i = pacman_linha - 5; i < pacman_linha + 5 && !pacman_encontrado; i++) {
        for (int j = pacman_coluna - 5; j < pacman_coluna + 5 && !pacman_encontrado; j++) {
            if (i >= 0 && i < mapa->linhas && j >= 0 && j < mapa->colunas) {
                if (mapa->matriz[i][j] == ' ' || mapa->matriz[i][j] == '.') {
                    pacman_linha = i;
                    pacman_coluna = j;
                    pacman_encontrado = 1;
                }
            }
        }
    }
    
    // Criar Pac-Man
    pacman_global = criar_pacman(pacman_linha, pacman_coluna);
    if (!pacman_global) {
        printf("Erro critico: Nao foi possivel criar Pac-Man\n");
        return 0;
    }
    
    // Criar fantasmas
    fantasmas_global = NULL;
    int fantasmas_criados = 0;
    
    // Posições iniciais para fantasmas (cantos do mapa)
    int posicoes_fantasmas[][2] = {
        {1, 1},
        {1, mapa->colunas - 2},
        {mapa->linhas - 2, 1},
        {mapa->linhas - 2, mapa->colunas - 2}
    };
    
    for (int i = 0; i < 4; i++) {
        int linha = posicoes_fantasmas[i][0];
        int coluna = posicoes_fantasmas[i][1];
        
        // Verificar se a posição é válida
        if (linha >= 0 && linha < mapa->linhas && 
            coluna >= 0 && coluna < mapa->colunas &&
            mapa->matriz[linha][coluna] != '#') {
            
            fantasmas_global = adicionar_fantasma(fantasmas_global, linha, coluna);
            fantasmas_criados++;
        }
    }
    
    printf("Entidades inicializadas: 1 Pac-Man, %d fantasmas\n", fantasmas_criados);
    return 1;
}

// ================== FUNÇÕES DE MOVIMENTO ==================

int mover_pacman_com_colisao(PacMan* pacman, Mapa* mapa, Fantasma* lista_fantasmas, int direcao) {
    // Controle de velocidade
    pacman_frame_counter++;
    if (pacman_frame_counter < pacman_speed_factor) {
        return 1; // Ainda não é hora de mover
    }
    pacman_frame_counter = 0;
    
    if (!pacman || !mapa) return 0;
    
    int linha_original = pacman->pos.linha;
    int coluna_original = pacman->pos.coluna;
    
    // Calcular nova posição
    switch(direcao) {
        case 1: // Cima
            pacman->pos.linha--;
            break;
        case 2: // Baixo
            pacman->pos.linha++;
            break;
        case 3: // Esquerda
            pacman->pos.coluna--;
            break;
        case 4: // Direita
            pacman->pos.coluna++;
            break;
        default:
            return 0; // Direção inválida
    }
    
    // Verificar teleporte horizontal (túneis)
    if (pacman->pos.coluna < 0) {
        pacman->pos.coluna = mapa->colunas - 1;
    } else if (pacman->pos.coluna >= mapa->colunas) {
        pacman->pos.coluna = 0;
    }
    
    pacman->direcao = direcao;
    
    // 1. Primeiro verifica colisão com parede/portal
    if (pacman->pos.linha < 0 || pacman->pos.linha >= mapa->linhas) {
        pacman->pos.linha = linha_original;
        pacman->pos.coluna = coluna_original;
        return 0;
    }
    
    char elemento = mapa->matriz[pacman->pos.linha][pacman->pos.coluna];
    
    if (elemento == '#') { // Parede
        pacman->pos.linha = linha_original;
        pacman->pos.coluna = coluna_original;
        return 0;
    }
    
    if (elemento == 'T') { // Portal
        processar_portal(mapa, &pacman->pos.linha, &pacman->pos.coluna, direcao);
        printf("Portal!\n");
        return 3;
    }
    
    // 2. Verificar colisão com pellets
    if (elemento == '.') {
        remover_pellet(mapa, pacman->pos.linha, pacman->pos.coluna);
        pacman->pontos += 10;
        mapa->pontosRestantes--;
        printf("Pellet! +10 pontos\n");
        return 2;
    }
    
    if (elemento == 'O') {
        remover_power_pellet(mapa, pacman->pos.linha, pacman->pos.coluna);
        pacman->pontos += 50;
        pacman->power_timer = 480;
        mapa->pontosRestantes--;
        
        // Tornar todos os fantasmas vulneráveis
        Fantasma* atual = lista_fantasmas;
        while (atual) {
            atual->vulneravel = 1;
            atual->timer_vulneravel = 480;
            atual = atual->prox;
        }
        
        printf("Power Pellet! Fantasmas vulneráveis!\n");
        return 4;
    }
    
    // 3. VERIFICAÇÃO DE COLISÃO COM FANTASMAS (MELHORADA)
    Fantasma* atual = lista_fantasmas;
    int fantasma_idx = 0;
    
    while (atual) {
        if (atual->pos.linha == pacman->pos.linha && 
            atual->pos.coluna == pacman->pos.coluna) {
            
            if (atual->vulneravel) {
                // Pac-Man come o fantasma
                pacman->pontos += 200;
                
                // Reposiciona fantasma
                atual->pos.linha = 1;
                atual->pos.coluna = 1;
                atual->vulneravel = 0;
                atual->timer_vulneravel = 0;
                
                printf("Fantasma comido! +200 pontos\n");
                return 5;
            } else {
                // Colisão com fantasma normal - PERDE VIDA
                pacman->vidas--;
                
                // Volta para posição original
                pacman->pos.linha = linha_original;
                pacman->pos.coluna = coluna_original;
                
                printf("COLISÃO COM FANTASMA! Vidas restantes: %d\n", pacman->vidas);
                
                if (pacman->vidas <= 0) {
                    return 6; // Game Over
                }
                return 7; // Perdeu vida mas ainda tem vidas
            }
        }
        atual = atual->prox;
        fantasma_idx++;
    }
    
    // Movimento bem-sucedido sem colisão
    return 1;
}

void mover_fantasma_com_colisao(Fantasma* fantasma, Mapa* mapa, Fantasma* lista_fantasmas, PacMan* pacman) {
    if (!fantasma || !mapa) return;
    
    static int fantasma_frame_counter = 0;
    fantasma_frame_counter++;
    
    // Se vulnerável, movimento mais lento
    int speed_factor = fantasma->vulneravel ? 3 : 2;
    if (fantasma_frame_counter < speed_factor) {
        return;
    }
    fantasma_frame_counter = 0;
    
    int linha_original = fantasma->pos.linha;
    int coluna_original = fantasma->pos.coluna;
    
    // Tenta mover na direção atual
    switch(fantasma->direcao) {
        case DIRECAO_CIMA:     fantasma->pos.linha--; break;
        case DIRECAO_BAIXO:    fantasma->pos.linha++; break;
        case DIRECAO_ESQUERDA: fantasma->pos.coluna--; break;
        case DIRECAO_DIREITA:  fantasma->pos.coluna++; break;
    }
    
    // Verificar teleporte horizontal (túneis)
    if (fantasma->pos.coluna < 0) {
        fantasma->pos.coluna = mapa->colunas - 1;
    } else if (fantasma->pos.coluna >= mapa->colunas) {
        fantasma->pos.coluna = 0;
    }
    
    // Verificar se a nova posição é válida
    if (fantasma->pos.linha < 0 || fantasma->pos.linha >= mapa->linhas) {
        // Fora dos limites verticais - volta
        fantasma->pos = (Posicao){linha_original, coluna_original};
        fantasma->direcao = rand() % 4;
        return;
    }
    
    char elemento = mapa->matriz[fantasma->pos.linha][fantasma->pos.coluna];
    
    // Verificar parede
    if (elemento == '#') {
        // Colisão com parede - volta e escolhe nova direção
        fantasma->pos = (Posicao){linha_original, coluna_original};
        
        // Tenta encontrar direção livre
        int direcoes[4] = {DIRECAO_CIMA, DIRECAO_BAIXO, DIRECAO_ESQUERDA, DIRECAO_DIREITA};
        
        for (int i = 0; i < 4; i++) {
            int nova_dir = direcoes[i];
            
            // Pula a direção oposta à atual (evita oscilação)
            if ((fantasma->direcao == DIRECAO_CIMA && nova_dir == DIRECAO_BAIXO) ||
                (fantasma->direcao == DIRECAO_BAIXO && nova_dir == DIRECAO_CIMA) ||
                (fantasma->direcao == DIRECAO_ESQUERDA && nova_dir == DIRECAO_DIREITA) ||
                (fantasma->direcao == DIRECAO_DIREITA && nova_dir == DIRECAO_ESQUERDA)) {
                continue;
            }
            
            // Testa a direção
            Posicao teste = (Posicao){linha_original, coluna_original};
            switch(nova_dir) {
                case DIRECAO_CIMA:    teste.linha--; break;
                case DIRECAO_BAIXO:   teste.linha++; break;
                case DIRECAO_ESQUERDA: teste.coluna--; break;
                case DIRECAO_DIREITA:  teste.coluna++; break;
            }
            
            // Verifica teleporte horizontal
            if (teste.coluna < 0) teste.coluna = mapa->colunas - 1;
            if (teste.coluna >= mapa->colunas) teste.coluna = 0;
            
            // Verifica se é válida
            if (teste.linha >= 0 && teste.linha < mapa->linhas &&
                teste.coluna >= 0 && teste.coluna < mapa->colunas &&
                mapa->matriz[teste.linha][teste.coluna] != '#') {
                
                fantasma->direcao = nova_dir;
                fantasma->pos = teste;
                return;
            }
        }
        
        // Se não encontrou direção válida, fica parado
        fantasma->direcao = rand() % 4;
    }
    // Verificar portal
    else if (elemento == 'T') {
        processar_portal(mapa, &fantasma->pos.linha, &fantasma->pos.coluna, fantasma->direcao);
    }
    // Movimento bem sucedido
    else {
        // Verifica colisão com outros fantasmas
        Fantasma* atual = lista_fantasmas;
        while (atual) {
            if (atual != fantasma && 
                atual->pos.linha == fantasma->pos.linha && 
                atual->pos.coluna == fantasma->pos.coluna) {
                
                // Colisão com outro fantasma - volta e muda direção
                fantasma->pos = (Posicao){linha_original, coluna_original};
                fantasma->direcao = rand() % 4;
                return;
            }
            atual = atual->prox;
        }
    }
}

// ================== FUNÇÕES DE ATUALIZAÇÃO ==================

void atualizar_estado_pacman(PacMan* pacman, Mapa* mapa) {
    (void)mapa; // Não usado
    
    if (!pacman) return;
    
    // Atualizar timer do power pellet
    if (pacman->power_timer > 0) {
        pacman->power_timer--;
        
        if (pacman->power_timer == 0) {
            printf("Power pellet acabou!\n");
        }
    }
}

void atualizar_estado_fantasmas(Fantasma* lista, PacMan* pacman) {
    if (!lista) return;
    
    Fantasma* atual = lista;
    while (atual) {
        // Atualizar timer de vulnerabilidade
        if (atual->vulneravel && atual->timer_vulneravel > 0) {
            atual->timer_vulneravel--;
            
            if (atual->timer_vulneravel == 0) {
                atual->vulneravel = 0;
                printf("Fantasma voltou ao normal\n");
            }
        }
        
        // Verificar se power pellet acabou
        if (pacman && pacman->power_timer == 0 && atual->vulneravel) {
            atual->vulneravel = 0;
            atual->timer_vulneravel = 0;
        }
        
        atual = atual->prox;
    }
}

// ================== FUNÇÕES DE DESENHO E ESTADO ==================

void atualizarEntidades(Mapa* mapa) {
    if (!pacman_global || !mapa) return;
    
    // Atualizar estados
    atualizar_estado_pacman(pacman_global, mapa);
    atualizar_estado_fantasmas(fantasmas_global, pacman_global);
    
    // Mover fantasmas
    static int fantasma_update_counter = 0;
    fantasma_update_counter++;
    
    // Mover fantasmas a cada 2 frames
    if (fantasma_update_counter >= 2) {
        fantasma_update_counter = 0;
        
        Fantasma* atual = fantasmas_global;
        while (atual) {
            mover_fantasma_inteligente(atual, mapa, pacman_global);
            atual = atual->prox;
        }
    }
}

void desenharEntidades(void) {
    // Esta função é implementada em graficos.c
    // Chamamos as funções específicas
    if (pacman_global) {
        desenhar_pacman(pacman_global);
    }
    
    if (fantasmas_global) {
        desenhar_fantasmas(fantasmas_global);
    }
}

void finalizarEntidades(void) {
    if (pacman_global) {
        free(pacman_global);
        pacman_global = NULL;
        printf("Pac-Man liberado da memoria\n");
    }
    
    if (fantasmas_global) {
        liberar_fantasmas(fantasmas_global);
        fantasmas_global = NULL;
    }
}

void reposicionarEntidades(Mapa* mapa) {
    if (!mapa || !pacman_global) return;
    
    printf("Reposicionando entidades...\n");
    
    // Reposicionar Pac-Man no centro
    pacman_global->pos.linha = mapa->linhas / 2;
    pacman_global->pos.coluna = mapa->colunas / 2;
    
    // Ajustar se for parede
    while (mapa->matriz[pacman_global->pos.linha][pacman_global->pos.coluna] == '#') {
        pacman_global->pos.linha++;
        if (pacman_global->pos.linha >= mapa->linhas) {
            pacman_global->pos.linha = 1;
            pacman_global->pos.coluna++;
        }
    }
    
    // Reposicionar fantasmas em cantos
    Fantasma* atual = fantasmas_global;
    int idx = 0;
    
    while (atual) {
        switch(idx % 4) {
            case 0: // Canto superior esquerdo
                atual->pos.linha = 1;
                atual->pos.coluna = 1;
                break;
            case 1: // Canto superior direito
                atual->pos.linha = 1;
                atual->pos.coluna = mapa->colunas - 2;
                break;
            case 2: // Canto inferior esquerdo
                atual->pos.linha = mapa->linhas - 2;
                atual->pos.coluna = 1;
                break;
            case 3: // Canto inferior direito
                atual->pos.linha = mapa->linhas - 2;
                atual->pos.coluna = mapa->colunas - 2;
                break;
        }
        
        // Resetar estado do fantasma
        atual->vulneravel = 0;
        atual->timer_vulneravel = 0;
        atual->direcao = rand() % 4;
        
        atual = atual->prox;
        idx++;
    }
}

void imprimir_estado(PacMan* pacman, Fantasma* lista) {
    if (!pacman) {
        printf("Pac-Man nao existe!\n");
        return;
    }
    
    printf("\n=== ESTADO DO JOGO ===\n");
    printf("PAC-MAN:\n");
    printf("  Posicao: (%d, %d)\n", pacman->pos.linha, pacman->pos.coluna);
    printf("  Vidas: %d\n", pacman->vidas);
    printf("  Pontos: %d\n", pacman->pontos);
    printf("  Power Timer: %d\n", pacman->power_timer);
    printf("  Direcao: %d\n", pacman->direcao);
    
    printf("\nFANTASMAS (%d no total):\n", contar_fantasmas_lista(lista));
    
    Fantasma* atual = lista;
    int idx = 0;
    while (atual) {
        printf("  Fantasma %d:\n", idx);
        printf("    Posicao: (%d, %d)\n", atual->pos.linha, atual->pos.coluna);
        printf("    Estado: %s\n", atual->vulneravel ? "VULNERAVEL" : "NORMAL");
        printf("    Timer Vulneravel: %d\n", atual->timer_vulneravel);
        printf("    Direcao: %d\n", atual->direcao);
        
        atual = atual->prox;
        idx++;
    }
    printf("======================\n\n");
}
void mover_fantasma_inteligente(Fantasma* fantasma, Mapa* mapa, PacMan* pacman) {
    if (!fantasma || !mapa || !pacman) return;
    
    // Se vulnerável, usa movimento aleatório simples
    if (fantasma->vulneravel) {
        // Chance de mudar direção aleatoriamente
        if (rand() % 100 < 30) { // 30% de chance por frame
            fantasma->direcao = rand() % 4;
        }
        mover_fantasma_com_colisao(fantasma, mapa, fantasmas_global, pacman);
        return;
    }
    
    // Perseguição inteligente
    int dx = pacman->pos.coluna - fantasma->pos.coluna;
    int dy = pacman->pos.linha - fantasma->pos.linha;
    
    // Decide se persegue na horizontal ou vertical
    int prefer_horizontal = (abs(dx) > abs(dy)) ? 1 : 0;
    
    // 80% chance de seguir perseguição, 20% aleatório
    if (rand() % 100 < 80) {
        if (prefer_horizontal) {
            fantasma->direcao = (dx > 0) ? DIRECAO_DIREITA : DIRECAO_ESQUERDA;
        } else {
            fantasma->direcao = (dy > 0) ? DIRECAO_BAIXO : DIRECAO_CIMA;
        }
    } else {
        fantasma->direcao = rand() % 4;
    }
    
    mover_fantasma_com_colisao(fantasma, mapa, fantasmas_global, pacman);
}
