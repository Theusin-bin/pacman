#include "jogo.h"
#include "mapa.h"
#include "entidades.h"
#include "graficos.h"
#include "colisoes.h"
#include "menu.h"
#include <stdio.h>
#include <stdbool.h>

// Declarações externas das variáveis globais de entidades
extern PacMan* pacman_global;
extern Fantasma* fantasmas_global;

// Variáveis globais do jogo
static Mapa *mapaAtual = NULL;
static int nivelAtual = 1;
static EstadoJogo estado = MENU;
static int pontuacaoTotal = 0;
static int vidas = 3;
static int direcao_atual = 0;
static Menu* menuJogo = NULL;

// ================== DECLARAÇÕES ANTECIPADAS ==================

static void processarInputJogando(void);
static void processarInputPausa(void);
static void processarInputGameOver(void);
static void processarInputVitoria(void);
static void processarInputMenuEstado(void);
static void debugEstadoAtual(void);
static bool verificarEstadoJogo(void);
static void desenharMenuSimples(void);
static const char* obterArquivoMapa(int nivel);
bool carregarMapaPorNivel(int nivel);

// ================== FUNÇÕES AUXILIARES ==================

static void debugEstadoAtual(void) {
    printf("=== DEBUG ESTADO ===\n");
    printf("Estado: %d (", estado);
    switch(estado) {
        case MENU: printf("MENU"); break;
        case JOGANDO: printf("JOGANDO"); break;
        case PAUSA: printf("PAUSA"); break;
        case GAME_OVER: printf("GAME_OVER"); break;
        case VITORIA: printf("VITORIA"); break;
    }
    printf(")\n");
    printf("Nível atual: %d\n", nivelAtual);
    printf("Pontuação: %d\n", pontuacaoTotal);
    printf("Vidas: %d\n", vidas);
    printf("Mapa atual: %s\n", mapaAtual ? "CARREGADO" : "NULL");
    printf("Pac-Man: %s\n", pacman_global ? "EXISTE" : "NULL");
    printf("Menu: %s\n", menuJogo ? "EXISTE" : "NULL");
    if (menuJogo) {
        printf("Estado menu: %d\n", get_estado_menu(menuJogo));
    }
    printf("====================\n");
}

static bool verificarEstadoJogo(void) {
    switch (estado) {
        case MENU:
            return (menuJogo != NULL);
        case JOGANDO:
            return (mapaAtual != NULL && pacman_global != NULL);
        case PAUSA:
            return (mapaAtual != NULL && pacman_global != NULL);
        case GAME_OVER:
            return (mapaAtual != NULL);
        case VITORIA:
            return (mapaAtual != NULL);
        default:
            return false;
    }
}

static const char* obterArquivoMapa(int nivel) {
    // APENAS 2 NÍVEIS DISPONÍVEIS
    switch(nivel) {
        case 1: return "assets/mapa1.txt";
        case 2: return "assets/mapa2.txt";
        default: return "assets/mapa1.txt";
    }
}

// ================== FUNÇÕES PRINCIPAIS ==================

bool inicializarJogo(void) {
    printf("Inicializando jogo...\n");
    
    // Inicializar sistema de menu
    menuJogo = criar_menu();
    if (!menuJogo) {
        printf("ERRO: Não foi possível criar menu\n");
        return false;
    }
    inicializar_menu(menuJogo);
    
    // Começar no menu
    estado = MENU;
    nivelAtual = 1;
    pontuacaoTotal = 0;
    vidas = 3;
    
    printf("Jogo inicializado com sucesso\n");
    debugEstadoAtual();
    
    return true;
}

bool carregarMapaPorNivel(int nivel) {
    printf("Carregando mapa para nível %d...\n", nivel);
    
    // Verificar se o nível é válido (apenas 1 ou 2)
    if (nivel < 1 || nivel > 2) {
        printf("AVISO: Nível %d inválido. Usando nível 1.\n", nivel);
        nivel = 1;
        nivelAtual = 1;
    }
    
    // Liberar mapa atual
    if (mapaAtual) {
        liberarMapa(mapaAtual);
        mapaAtual = NULL;
    }
    
    // Finalizar entidades antigas
    finalizarEntidades();
    
    // Obter nome do arquivo para este nível
    const char* arquivoMapa = obterArquivoMapa(nivel);
    printf("Tentando carregar: %s\n", arquivoMapa);
    
    // Carregar o mapa
    mapaAtual = carregarMapa(arquivoMapa);
    
    // Se não carregou, tentar fallback
    if (!mapaAtual) {
        printf("Mapa não encontrado em %s. Tentando fallback...\n", arquivoMapa);
        
        // Tentar sem o caminho assets/
        if (nivel == 1) {
            mapaAtual = carregarMapa("mapa1.txt");
        } else if (nivel == 2) {
            mapaAtual = carregarMapa("mapa2.txt");
        }
    }
    
    if (!mapaAtual) {
        printf("ERRO CRÍTICO: Não foi possível carregar mapa para nível %d!\n", nivel);
        return false;
    }
    
    // Inicializar entidades
    if (!inicializarEntidades(mapaAtual)) {
        printf("Erro ao inicializar entidades\n");
        liberarMapa(mapaAtual);
        mapaAtual = NULL;
        return false;
    }
    
    // Resetar variáveis do jogo para o novo nível
    vidas = pacman_global->vidas;
    pontuacaoTotal = pacman_global->pontos;
    
    printf("=== MAPA CARREGADO ===\n");
    printf("Nível: %d\n", nivel);
    printf("Arquivo: %s\n", arquivoMapa);
    printf("Dimensões: %d x %d\n", mapaAtual->linhas, mapaAtual->colunas);
    printf("Pellets restantes: %d\n", mapaAtual->pontosRestantes);
    printf("Pac-Man posição: (%d, %d)\n", 
           pacman_global->pos.linha, pacman_global->pos.coluna);
    printf("======================\n");
    
    return true;
}

bool carregarProximoMapa(void) {
    return carregarMapaPorNivel(nivelAtual);
}

void atualizarJogo(void) {
    if (estado != JOGANDO || !mapaAtual || !pacman_global) return;
    
    // Processar entrada do usuário
    direcao_atual = tecla_para_direcao();
    if (direcao_atual > 0) {
        int resultado = mover_pacman_com_colisao(pacman_global, mapaAtual, fantasmas_global, direcao_atual);
        
        // Verificar resultado da movimentação
        if (resultado == 6 || resultado == 7) { // Colisão com fantasma
            vidas = pacman_global->vidas;
            if (vidas <= 0) {
                estado = GAME_OVER;
            } else {
                // Reposicionar após perder vida
                reposicionarEntidades(mapaAtual);
            }
        }
        
        pontuacaoTotal = pacman_global->pontos;
        vidas = pacman_global->vidas;
    }
    
    // Atualizar entidades
    atualizarEntidades(mapaAtual);
    
    // VERIFICAÇÃO EXTRA DE COLISÃO EM TEMPO REAL
    verificarTodasColisoes(mapaAtual);
    
    // Atualizar vidas após verificação de colisão
    vidas = pacman_global->vidas;
    
    // Verificar game over
    if (vidas <= 0) {
        estado = GAME_OVER;
    }
    
    // VERIFICAR VITÓRIA - CONTAGEM REAL DE PELLETS
    int pellets_restantes = 0;
    if (mapaAtual && mapaAtual->matriz) {
        for (int i = 0; i < mapaAtual->linhas; i++) {
            for (int j = 0; j < mapaAtual->colunas; j++) {
                char celula = mapaAtual->matriz[i][j];
                if (celula == '.' || celula == 'O') {
                    pellets_restantes++;
                }
            }
        }
    }
    
    // Atualizar contador do mapa
    if (mapaAtual) {
        mapaAtual->pontosRestantes = pellets_restantes;
    }
    
    // Verificar vitória (QUANDO ZERAR TUDO)
    if (pellets_restantes == 0) {
        estado = VITORIA;
        printf("★ ★ ★ VITÓRIA! Todos os pellets foram comidos! ★ ★ ★\n");
    }
}

void desenharJogo(void) {
    // Desenhar fundo
    ClearBackground(BLACK);
    
    // Verificar se o estado é consistente
    if (!verificarEstadoJogo()) {
        printf("AVISO: Estado inconsistente, voltando para menu\n");
        estado = MENU;
    }
    
    switch (estado) {
        case MENU:
            if (menuJogo) {
                atualizar_menu(menuJogo);
                desenhar_menu(menuJogo);
            } else {
                desenharMenuSimples();
            }
            break;
            
        case JOGANDO:
            if (mapaAtual && pacman_global) {
                desenhar_mapa(mapaAtual);
                desenharEntidades();
                desenhar_hud(pacman_global, mapaAtual, nivelAtual);
                
                // Mostrar qual nível está sendo jogado
                if (nivelAtual == 1) {
                    DrawText("NÍVEL 1 - Mapa Básico", 10, 10, 20, YELLOW);
                } else if (nivelAtual == 2) {
                    DrawText("NÍVEL 2 - Mapa com Túneis", 10, 10, 20, YELLOW);
                }
                
                // Instruções durante o jogo
                DrawText("R: Voltar ao menu | S: Salvar jogo | P: Pausar", 
                         GetScreenWidth() - MeasureText("R: Voltar ao menu | S: Salvar jogo | P: Pausar", 18) - 10,
                         GetScreenHeight() - 40, 18, LIGHTGRAY);
            } else {
                printf("ERRO: Elementos de jogo não carregados\n");
            }
            break;
                    
        case PAUSA:
            if (mapaAtual && pacman_global) {
                desenhar_mapa(mapaAtual);
                desenharEntidades();
                desenhar_hud(pacman_global, mapaAtual, nivelAtual);
                desenharPausa();
            }
            break;
            
        case GAME_OVER:
            desenharGameOver();
            break;
            
        case VITORIA:
            desenharVitoria();
            break;
    }
}

// ================== FUNÇÕES DE DESENHO ==================

static void desenharMenuSimples(void) {
    int centroX = GetScreenWidth() / 2;
    int centroY = GetScreenHeight() / 2;
    
    DrawText("PAC-MAN UFRJ", centroX - MeasureText("PAC-MAN UFRJ", 60)/2, centroY - 100, 60, YELLOW);
    DrawText("Pressione ENTER para jogar", centroX - MeasureText("Pressione ENTER para jogar", 30)/2, centroY, 30, WHITE);
    DrawText("R para voltar/sair", centroX - MeasureText("R para voltar/sair", 25)/2, centroY + 50, 25, LIGHTGRAY);
}

void desenharPausa(void) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));
    
    int centroX = GetScreenWidth() / 2;
    int centroY = GetScreenHeight() / 2;
    
    DrawText("PAUSA", centroX - MeasureText("PAUSA", 60)/2, centroY - 50, 60, YELLOW);
    DrawText("Pressione P para continuar", centroX - MeasureText("Pressione P para continuar", 30)/2, centroY + 30, 30, WHITE);
    DrawText("R para voltar ao menu", centroX - MeasureText("R para voltar ao menu", 25)/2, centroY + 70, 25, LIGHTGRAY);
}

void desenharGameOver(void) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(RED, 0.3f));
    
    int centroX = GetScreenWidth() / 2;
    int centroY = GetScreenHeight() / 2;
    
    DrawText("GAME OVER", centroX - MeasureText("GAME OVER", 70)/2, centroY - 100, 70, RED);
    DrawText(TextFormat("Pontuação final: %d", pontuacaoTotal), 
             centroX - MeasureText(TextFormat("Pontuação final: %d", pontuacaoTotal), 30)/2, 
             centroY, 30, YELLOW);
    DrawText("ENTER para tentar novamente", 
             centroX - MeasureText("ENTER para tentar novamente", 25)/2, 
             centroY + 60, 25, WHITE);
    DrawText("R para voltar ao menu", 
             centroX - MeasureText("R para voltar ao menu", 25)/2, 
             centroY + 100, 25, LIGHTGRAY);
}

void desenharVitoria(void) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(GREEN, 0.3f));
    
    int centroX = GetScreenWidth() / 2;
    int centroY = GetScreenHeight() / 2;
    
    DrawText("★ VITÓRIA! ★", centroX - MeasureText("★ VITÓRIA! ★", 70)/2, centroY - 100, 70, GOLD);
    DrawText(TextFormat("Nível %d COMPLETO!", nivelAtual), 
             centroX - MeasureText(TextFormat("Nível %d COMPLETO!", nivelAtual), 30)/2, 
             centroY, 30, WHITE);
    DrawText(TextFormat("Pontuação: %d", pontuacaoTotal), 
             centroX - MeasureText(TextFormat("Pontuação: %d", pontuacaoTotal), 35)/2, 
             centroY + 50, 35, YELLOW);
    
    // Mostrar qual mapa foi completado
    if (nivelAtual == 1) {
        DrawText("Mapa: Básico (mapa1.txt)",
                 centroX - MeasureText("Mapa: Básico (mapa1.txt)", 25)/2,
                 centroY + 80, 25, LIGHTGRAY);
    } else if (nivelAtual == 2) {
        DrawText("Mapa: Com Túneis (mapa2.txt)",
                 centroX - MeasureText("Mapa: Com Túneis (mapa2.txt)", 25)/2,
                 centroY + 80, 25, LIGHTGRAY);
    }
    
    // Opções na tela de vitória
    DrawText("[ENTER] - Jogar novamente", 
             centroX - MeasureText("[ENTER] - Jogar novamente", 25)/2, 
             centroY + 120, 25, WHITE);
    
    // Mostrar "Próximo nível" apenas se houver nível 2 disponível
    if (nivelAtual == 1) {
        DrawText("[N] - Próximo nível (2)", 
                 centroX - MeasureText("[N] - Próximo nível (2)", 25)/2, 
                 centroY + 150, 25, LIGHTGRAY);
    } else {
        DrawText("Último nível concluído!", 
                 centroX - MeasureText("Último nível concluído!", 25)/2, 
                 centroY + 150, 25, YELLOW);
    }
    
    DrawText("[R] - Menu principal", 
             centroX - MeasureText("[R] - Menu principal", 25)/2, 
             centroY + 180, 25, LIGHTGRAY);
}

// ================== PROCESSAMENTO DE INPUT ==================

void processarInput(void) {
    switch (estado) {
        case MENU:
            processarInputMenuEstado();
            break;
        case JOGANDO:
            processarInputJogando();
            break;
        case PAUSA:
            processarInputPausa();
            break;
        case GAME_OVER:
            processarInputGameOver();
            break;
        case VITORIA:
            processarInputVitoria();
            break;
    }
}

static void processarInputMenuEstado(void) {
    if (!menuJogo) return;
    
    // Variável estática para rastrear se o usuário estava na seleção de nível
    static bool estavaNaSelecaoNivel = false;
    
    // Atualizar o estado do menu
    atualizar_menu(menuJogo);
    EstadoMenu estadoMenu = get_estado_menu(menuJogo);
    
    // Atualizar flag de estado anterior
    static EstadoMenu estadoAnterior = MENU_PRINCIPAL;
    
    // Se estava na seleção de nível e agora está no menu principal, usuário escolheu um nível
    if (estadoAnterior == MENU_SELECAO_NIVEL && estadoMenu == MENU_PRINCIPAL) {
        estavaNaSelecaoNivel = true;
    }
    
    // Se está no menu principal e acabou de escolher um nível, iniciar jogo
    if (estadoMenu == MENU_PRINCIPAL && estavaNaSelecaoNivel && IsKeyPressed(KEY_ENTER)) {
        nivelAtual = get_nivel_selecionado(menuJogo);
        
        // Garantir que o nível está entre 1 e 2
        if (nivelAtual < 1) nivelAtual = 1;
        if (nivelAtual > 2) nivelAtual = 2;
        
        printf("Iniciando NOVO JOGO no nível %d\n", nivelAtual);
        
        // Carregar o mapa correspondente
        if (carregarMapaPorNivel(nivelAtual)) {
            estado = JOGANDO;
            if (nivelAtual == 1) {
                printf("Nível 1 - Mapa Básico (assets/mapa1.txt)\n");
            } else {
                printf("Nível 2 - Mapa com Túneis (assets/mapa2.txt)\n");
            }
        }
        
        estavaNaSelecaoNivel = false; // Resetar flag
    }
    
    // Se está na tela de confirmação de saída e pressiona ENTER
    if (estadoMenu == MENU_SAIR_CONFIRMAR && IsKeyPressed(KEY_ENTER)) {
        printf("Saindo do jogo...\n");
        // O main() deve verificar essa condição e fechar a janela
    }
    
    // Se pressionar R em qualquer estado do menu (exceto principal e sair), voltar ao menu principal
    if (IsKeyPressed(KEY_R) && estadoMenu != MENU_PRINCIPAL && estadoMenu != MENU_SAIR_CONFIRMAR) {
        set_estado_menu(menuJogo, MENU_PRINCIPAL);
        printf("Voltando ao menu principal (tecla R)\n");
    }
    
    // Atualizar estado anterior
    estadoAnterior = estadoMenu;
}

static void processarInputJogando(void) {
    // R - Voltar ao menu
    if (IsKeyPressed(KEY_R)) {
        estado = MENU;
        printf("Voltando ao menu (tecla R)\n");
    }
    
    // P - Pausar jogo
    if (IsKeyPressed(KEY_P)) {
        estado = PAUSA;
        printf("Jogo pausado\n");
    }
    
    // S - Salvar jogo
    if (IsKeyPressed(KEY_S)) {
        printf("Salvando jogo... (tecla S)\n");
        if (menuJogo && pacman_global) {
            salvar_jogo(menuJogo, nivelAtual, pontuacaoTotal, vidas);
        }
    }
    
    // T - Reiniciar (opcional, se quiser manter)
    if (IsKeyPressed(KEY_T)) {
        reiniciarJogo();
        printf("Jogo reiniciado (tecla T)\n");
    }
}

static void processarInputPausa(void) {
    // R - Voltar ao menu
    if (IsKeyPressed(KEY_R)) {
        estado = MENU;
        printf("Voltando ao menu da pausa (tecla R)\n");
    }
    
    // P - Continuar jogo
    if (IsKeyPressed(KEY_P)) {
        estado = JOGANDO;
        printf("Jogo retomado\n");
    }
}

static void processarInputGameOver(void) {
    // ENTER - Tentar novamente
    if (IsKeyPressed(KEY_ENTER)) {
        reiniciarJogo();
        printf("Reiniciando jogo após game over\n");
    }
    
    // R - Voltar ao menu
    if (IsKeyPressed(KEY_R)) {
        estado = MENU;
        printf("Voltando ao menu do game over (tecla R)\n");
    }
}

static void processarInputVitoria(void) {
    // ENTER - Rejogar o MESMO nível
    if (IsKeyPressed(KEY_ENTER)) {
        printf("Reiniciando nível %d...\n", nivelAtual);
        
        // Recarregar o mesmo mapa
        if (carregarMapaPorNivel(nivelAtual)) {
            estado = JOGANDO;
            printf("Nível %d reiniciado\n", nivelAtual);
        }
    }
    
    // N - Próximo nível (apenas se for nível 1 e existir nível 2)
    if (IsKeyPressed(KEY_N) && nivelAtual == 1) {
        printf("Carregando próximo nível: 2...\n");
        nivelAtual = 2;
        
        if (carregarMapaPorNivel(nivelAtual)) {
            estado = JOGANDO;
            printf("Nível 2 carregado com sucesso! (assets/mapa2.txt)\n");
        } else {
            // Se não conseguiu carregar nível 2, volta para nível 1
            printf("ERRO: Não foi possível carregar nível 2. Voltando para nível 1.\n");
            nivelAtual = 1;
            carregarMapaPorNivel(nivelAtual);
        }
    }
    
    // R - Voltar ao menu
    if (IsKeyPressed(KEY_R)) {
        estado = MENU;
        printf("Voltando ao menu da vitória (tecla R)\n");
    }
}

// ================== FUNÇÕES DE CONTROLE DO JOGO ==================

void reiniciarJogo(void) {
    printf("Reiniciando jogo...\n");
    
    nivelAtual = 1;
    pontuacaoTotal = 0;
    
    if (mapaAtual) {
        liberarMapa(mapaAtual);
        mapaAtual = NULL;
    }
    
    finalizarEntidades();
    
    if (!carregarMapaPorNivel(nivelAtual)) {
        printf("ERRO: Não foi possível carregar mapa ao reiniciar\n");
        estado = MENU;
        return;
    }
    
    estado = JOGANDO;
    printf("Jogo reiniciado com sucesso! Nível %d\n", nivelAtual);
}

void finalizarJogo(void) {
    printf("Finalizando jogo...\n");
    
    if (menuJogo) {
        liberar_menu(menuJogo);
        menuJogo = NULL;
        printf("Menu liberado\n");
    }
    
    if (mapaAtual) {
        liberarMapa(mapaAtual);
        mapaAtual = NULL;
        printf("Mapa liberado\n");
    }
    
    finalizarEntidades();
    printf("Entidades liberadas\n");
    
    printf("Jogo finalizado com sucesso\n");
}

void adicionarPontos(int pontos) {
    pontuacaoTotal += pontos;
}

void perderVida(void) {
    vidas--;
    if (vidas > 0 && mapaAtual) {
        reposicionarEntidades(mapaAtual);
    }
}

// ================== GETTERS ==================

int getVidas(void) { 
    return vidas; 
}

int getPontuacao(void) { 
    return pontuacaoTotal; 
}

EstadoJogo getEstadoJogo(void) { 
    return estado; 
}

Mapa* getMapaAtual(void) { 
    return mapaAtual; 
}

Menu* getMenuJogo(void) {
    return menuJogo;
}

// ================== SETTERS ==================

void setEstadoJogo(EstadoJogo novoEstado) {
    estado = novoEstado;
}

void setNivelAtual(int nivel) {
    if (nivel >= 1 && nivel <= 2) {
        nivelAtual = nivel;
    }
}

bool existeProximoNivel(void) {
    return (nivelAtual == 1);
}