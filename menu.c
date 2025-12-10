#include "menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

// Cores personalizadas
#define COR_FUNDO_MENU CLITERAL(Color){10, 10, 40, 255}
#define COR_TITULO CLITERAL(Color){255, 203, 0, 255}
#define COR_SUBTITULO CLITERAL(Color){200, 200, 255, 255}
#define COR_TEXTO_NORMAL CLITERAL(Color){255, 255, 255, 255}
#define COR_TEXTO_SELECIONADO CLITERAL(Color){255, 203, 0, 255}
#define COR_TEXTO_DESABILITADO CLITERAL(Color){150, 150, 150, 255}
#define COR_BOTAO_NORMAL CLITERAL(Color){40, 40, 80, 200}
#define COR_BOTAO_SELECIONADO CLITERAL(Color){60, 60, 120, 255}

// Textos do menu simplificado
static const char* textosOpcoes[] = {
    "NOVO JOGO",
    "CARREGAR JOGO",
    "SAIR"
};

// ================== FUNÇÕES BÁSICAS ==================

Menu* criar_menu(void) {
    Menu* menu = (Menu*)malloc(sizeof(Menu));
    if (!menu) {
        printf("Erro ao alocar memória para menu\n");
        return NULL;
    }
    
    // Inicializar valores padrão
    menu->estado = MENU_PRINCIPAL;
    menu->opcaoSelecionada = OPCAO_NOVO_JOGO;
    menu->nivelSelecionado = 1; // SEMPRE COMEÇA NO NÍVEL 1
    menu->tempoAnimacao = 0.0f;
    menu->transicaoAtiva = false;
    menu->transicaoTempo = 0.0f;
    
    // Sistema de save
    menu->temSave = false;
    strcpy(menu->saveFileName, "savegame.dat");
    
    // Inicializar recursos gráficos como NULL
    menu->logo = (Texture2D){0};
    menu->background = (Texture2D){0};
    menu->fonteTitulo = GetFontDefault();
    menu->fonteMenu = GetFontDefault();
    
    return menu;
}

void inicializar_menu(Menu* menu) {
    if (!menu) return;
    
    printf("Inicializando menu...\n");
    
    // Verificar save game
    verificar_save(menu);
    
    // Tentar carregar texturas
    if (FileExists("assets/logo.png")) {
        menu->logo = LoadTexture("assets/logo.png");
        printf("Logo carregada\n");
    }
    
    if (FileExists("assets/menu_bg.png")) {
        menu->background = LoadTexture("assets/menu_bg.png");
        printf("Background carregado\n");
    }
    
    // Tentar carregar fontes
    if (FileExists("assets/fonts/pacfont.ttf")) {
        menu->fonteTitulo = LoadFont("assets/fonts/pacfont.ttf");
        menu->fonteMenu = LoadFont("assets/fonts/pacfont.ttf");
        printf("Fonte personalizada carregada\n");
    } else {
        menu->fonteTitulo = GetFontDefault();
        menu->fonteMenu = GetFontDefault();
    }
    
    printf("Menu inicializado com sucesso\n");
}

void atualizar_menu(Menu* menu) {
    if (!menu) return;
    
    // Atualizar animações
    menu->tempoAnimacao += GetFrameTime();
    
    // Atualizar transição
    if (menu->transicaoAtiva) {
        menu->transicaoTempo += GetFrameTime() * 2.0f;
        if (menu->transicaoTempo >= 1.0f) {
            menu->transicaoAtiva = false;
            menu->transicaoTempo = 0.0f;
        }
    }
    
    // Processar input
    processar_input_menu(menu);
}

void processar_input_menu(Menu* menu) {
    if (!menu) return;
    if (menu->transicaoAtiva) return;
    
    switch (menu->estado) {
        case MENU_PRINCIPAL:
            // Navegação vertical
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                menu->opcaoSelecionada--;
                if (menu->opcaoSelecionada < 0) {
                    menu->opcaoSelecionada = NUM_OPCOES - 1;
                }
            }
            
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                menu->opcaoSelecionada++;
                if (menu->opcaoSelecionada >= NUM_OPCOES) {
                    menu->opcaoSelecionada = 0;
                }
            }
            
            // Selecionar opção
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                switch (menu->opcaoSelecionada) {
                    case OPCAO_NOVO_JOGO:
                        menu->estado = MENU_SELECAO_NIVEL;
                        menu->transicaoAtiva = true;
                        break;
                        
                    case OPCAO_CARREGAR:
                        if (menu->temSave) {
                            printf("Carregando jogo salvo...\n");
                            // TODO: Implementar carregamento do jogo
                        } else {
                            printf("Nenhum jogo salvo encontrado!\n");
                        }
                        break;
                        
                    case OPCAO_SAIR:
                        menu->estado = MENU_SAIR_CONFIRMAR;
                        menu->transicaoAtiva = true;
                        break;
                }
            }
            
            // Tecla R para voltar (se estiver em outro lugar)
            if (IsKeyPressed(KEY_R)) {
                // Voltar para menu principal (já está, mas para consistência)
                menu->estado = MENU_PRINCIPAL;
            }
            break;
            
        case MENU_SELECAO_NIVEL:
            // Navegação nos níveis - APENAS 2 NÍVEIS (1 e 2)
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
                menu->nivelSelecionado--;
                if (menu->nivelSelecionado < 1) menu->nivelSelecionado = 2;
            }
            
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
                menu->nivelSelecionado++;
                if (menu->nivelSelecionado > 2) menu->nivelSelecionado = 1;
            }
            
            // Iniciar novo jogo no nível selecionado
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                printf("Iniciando NOVO JOGO no nível %d\n", menu->nivelSelecionado);
                // ALTERAÇÃO: Voltar ao estado principal
                menu->estado = MENU_PRINCIPAL;
                menu->transicaoAtiva = true;
                // O jogo.c detectará a mudança e iniciará o nível
            }
            
            // Voltar ao menu principal com R
            if (IsKeyPressed(KEY_R)) {
                menu->estado = MENU_PRINCIPAL;
                menu->transicaoAtiva = true;
            }
            break;
            
        case MENU_SAIR_CONFIRMAR:
            // Confirmar saída
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                printf("Saindo do jogo...\n");
                // TODO: Fechar a janela
                // CloseWindow();
            }
            
            // Cancelar saída com R
            if (IsKeyPressed(KEY_R)) {
                menu->estado = MENU_PRINCIPAL;
                menu->transicaoAtiva = true;
            }
            break;
    }
}

// ================== FUNÇÕES DE DESENHO ==================

void desenhar_menu(Menu* menu) {
    if (!menu) return;
    
    ClearBackground(COR_FUNDO_MENU);
    
    if (menu->background.id > 0) {
        DrawTexture(menu->background, 0, 0, Fade(WHITE, 0.3f));
    }
    
    if (menu->transicaoAtiva) {
        animar_transicao(menu);
    }
    
    switch (menu->estado) {
        case MENU_PRINCIPAL:
            desenhar_menu_principal(menu);
            break;
        case MENU_SELECAO_NIVEL:
            desenhar_menu_selecao_nivel(menu);
            break;
        case MENU_SAIR_CONFIRMAR:
            desenhar_menu_principal(menu);
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));
            DrawText("Deseja realmente sair?",
                     GetScreenWidth()/2 - MeasureText("Deseja realmente sair?", 40)/2,
                     GetScreenHeight()/2 - 50, 40, YELLOW);
            DrawText("ENTER para sair, R para cancelar",
                     GetScreenWidth()/2 - MeasureText("ENTER para sair, R para cancelar", 20)/2,
                     GetScreenHeight()/2 + 30, 20, WHITE);
            break;
    }
    
    // Rodapé com instruções atualizadas
    DrawText("PAC-MAN UFRJ - 2024",
             10, GetScreenHeight() - 25, 15, COR_SUBTITULO);
    
    char instrucoes[100];
    if (menu->temSave) {
        sprintf(instrucoes, "S: Salvar jogo | R: Voltar ao menu");
    } else {
        sprintf(instrucoes, "WASD/Setas: Navegar | ENTER: Selecionar | R: Voltar");
    }
    
    DrawText(instrucoes,
             GetScreenWidth() - MeasureText(instrucoes, 15) - 10,
             GetScreenHeight() - 25, 15, COR_SUBTITULO);
}

void desenhar_menu_principal(Menu* menu) {
    int centroX = GetScreenWidth() / 2;
    int centroY = GetScreenHeight() / 2;
    
    // Desenhar logo
    if (menu->logo.id > 0) {
        DrawTexture(menu->logo, centroX - menu->logo.width/2, 50, WHITE);
    } else {
        DrawText("PAC-MAN", centroX - MeasureText("PAC-MAN", 80)/2, 60, 80, COR_TITULO);
        DrawText("UFRJ", centroX - MeasureText("UFRJ", 60)/2, 150, 60, COR_TITULO);
    }
    
    // Desenhar opções do menu
    int espacamento = 70;
    int inicioY = centroY;
    
    for (int i = 0; i < NUM_OPCOES; i++) {
        bool selecionado = (i == menu->opcaoSelecionada);
        int y = inicioY + i * espacamento;
        
        // Efeito de animação para item selecionado
        float escala = 1.0f;
        Color cor = COR_TEXTO_NORMAL;
        
        if (selecionado) {
            escala = 1.1f + sinf(menu->tempoAnimacao * 5.0f) * 0.05f;
            cor = COR_TEXTO_SELECIONADO;
        }
        
        // Desenhar botão
        int larguraTexto = MeasureText(textosOpcoes[i], 40);
        int alturaTexto = 40;
        int larguraBotao = larguraTexto + 60;
        int alturaBotao = alturaTexto + 20;
        
        // Se for CARREGAR e não tiver save, mostrar desabilitado
        if (i == OPCAO_CARREGAR && !menu->temSave) {
            desenhar_botao(textosOpcoes[i], 
                         centroX - larguraBotao/2, 
                         y - alturaBotao/2,
                         larguraBotao, alturaBotao,
                         false,
                         COR_TEXTO_DESABILITADO,
                         COR_TEXTO_DESABILITADO);
            DrawText("(Nenhum save)", centroX - MeasureText("(Nenhum save)", 20)/2,
                    y + 30, 20, COR_TEXTO_DESABILITADO);
        } else {
            desenhar_botao(textosOpcoes[i], 
                         centroX - larguraBotao/2, 
                         y - alturaBotao/2,
                         larguraBotao, alturaBotao,
                         selecionado,
                         COR_BOTAO_NORMAL,
                         COR_BOTAO_SELECIONADO);
        }
        
        // Indicador de seleção (setas)
        if (selecionado && !(i == OPCAO_CARREGAR && !menu->temSave)) {
            DrawText(">", centroX - larguraBotao/2 - 30, y - alturaTexto/2, 40, COR_TEXTO_SELECIONADO);
            DrawText("<", centroX + larguraBotao/2 + 10, y - alturaTexto/2, 40, COR_TEXTO_SELECIONADO);
        }
    }
    
    // Mostrar status do save
    if (menu->temSave) {
        DrawText("✓ Jogo salvo disponível", 
                centroX - MeasureText("✓ Jogo salvo disponível", 25)/2,
                GetScreenHeight() - 100, 25, GREEN);
    }
}

void desenhar_menu_selecao_nivel(Menu* menu) {
    int centroX = GetScreenWidth() / 2;
    int centroY = GetScreenHeight() / 2;
    
    // Título
    DrawText("SELECIONE O NÍVEL", centroX - MeasureText("SELECIONE O NÍVEL", 50)/2, 80, 50, COR_TITULO);
    
    // Garantir que o nível está entre 1 e 2
    int nivel_display = menu->nivelSelecionado;
    if (nivel_display < 1) nivel_display = 1;
    if (nivel_display > 2) nivel_display = 2;
    
    // Nível selecionado
    char nivelStr[50];
    sprintf(nivelStr, "NÍVEL %d", nivel_display);
    
    DrawText(nivelStr, centroX - MeasureText(nivelStr, 70)/2, centroY - 50, 70, COR_TEXTO_SELECIONADO);
    
    // Setas de navegação (apenas 2 níveis disponíveis)
    DrawText("<", centroX - 150, centroY - 25, 60, 
            (nivel_display > 1) ? COR_TEXTO_NORMAL : COR_TEXTO_DESABILITADO);
    
    DrawText(">", centroX + 120, centroY - 25, 60, 
            (nivel_display < 2) ? COR_TEXTO_NORMAL : COR_TEXTO_DESABILITADO);
    
    // Descrição do nível
    const char* descricao = "";
    const char* mapaArquivo = "";
    
    switch (nivel_display) {
        case 1: 
            descricao = "Mapa Básico - Ideal para iniciantes";
            mapaArquivo = "assets/mapa1.txt";
            break;
        case 2: 
            descricao = "Mapa com Túneis - Mais desafiador";
            mapaArquivo = "assets/mapa2.txt";
            break;
    }
    
    DrawText(descricao, centroX - MeasureText(descricao, 25)/2, centroY + 50, 25, COR_SUBTITULO);
    DrawText(TextFormat("Arquivo: %s", mapaArquivo), 
             centroX - MeasureText(TextFormat("Arquivo: %s", mapaArquivo), 20)/2,
             centroY + 85, 20, COR_SUBTITULO);
    
    // Instruções atualizadas
    DrawText("ENTER para iniciar novo jogo, R para voltar",
             centroX - MeasureText("ENTER para iniciar novo jogo, R para voltar", 20)/2,
             GetScreenHeight() - 100, 20, COR_TEXTO_NORMAL);
}

// ================== FUNÇÕES AUXILIARES ==================

void desenhar_botao(const char* texto, int x, int y, int largura, int altura, 
                   bool selecionado, Color corNormal, Color corSelecionado) {
    
    Color corFundo = selecionado ? corSelecionado : corNormal;
    Color corTexto = selecionado ? COR_TEXTO_SELECIONADO : COR_TEXTO_NORMAL;
    
    // Fundo do botão
    DrawRectangleRounded((Rectangle){x, y, largura, altura}, 0.3f, 10, corFundo);
    
    // Borda
    DrawRectangleRoundedLinesEx((Rectangle){x, y, largura, altura}, 0.3f, 10, 2.0f,
                               selecionado ? YELLOW : GRAY);
    
    // Texto centralizado
    int tamanhoFonte = 30;
    int textoX = x + (largura - MeasureText(texto, tamanhoFonte)) / 2;
    int textoY = y + (altura - tamanhoFonte) / 2;
    
    DrawText(texto, textoX, textoY, tamanhoFonte, corTexto);
}

void animar_transicao(Menu* menu) {
    if (!menu || !menu->transicaoAtiva) return;
    
    float alpha = menu->transicaoTempo;
    if (alpha > 1.0f) alpha = 1.0f;
    
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), 
                  Fade(BLACK, alpha * 0.5f));
}

// ================== SISTEMA DE SAVE/LOAD ==================

void verificar_save(Menu* menu) {
    if (!menu) return;
    
    menu->temSave = FileExists(menu->saveFileName);
    if (menu->temSave) {
        printf("Save game encontrado: %s\n", menu->saveFileName);
    } else {
        printf("Nenhum save game encontrado\n");
    }
}

void salvar_jogo(Menu* menu, int nivel, int pontos, int vidas) {
    if (!menu) return;
    
    FILE* arquivo = fopen(menu->saveFileName, "wb");
    if (arquivo) {
        // Salvar dados básicos
        fwrite(&nivel, sizeof(int), 1, arquivo);
        fwrite(&pontos, sizeof(int), 1, arquivo);
        fwrite(&vidas, sizeof(int), 1, arquivo);
        fclose(arquivo);
        
        menu->temSave = true;
        printf("Jogo salvo com sucesso! Nível: %d, Pontos: %d, Vidas: %d\n", nivel, pontos, vidas);
    } else {
        printf("Erro ao salvar jogo!\n");
    }
}

bool carregar_jogo(Menu* menu, int* nivel, int* pontos, int* vidas) {
    if (!menu || !menu->temSave) return false;
    
    FILE* arquivo = fopen(menu->saveFileName, "rb");
    if (arquivo) {
        fread(nivel, sizeof(int), 1, arquivo);
        fread(pontos, sizeof(int), 1, arquivo);
        fread(vidas, sizeof(int), 1, arquivo);
        fclose(arquivo);
        
        printf("Jogo carregado: Nível %d, %d pontos, %d vidas\n", *nivel, *pontos, *vidas);
        return true;
    }
    return false;
}

// ================== GETTERS E SETTERS ==================

EstadoMenu get_estado_menu(Menu* menu) {
    return menu ? menu->estado : MENU_PRINCIPAL;
}

int get_nivel_selecionado(Menu* menu) {
    if (!menu) return 1;
    
    // Garantir que o nível está entre 1 e 2
    if (menu->nivelSelecionado < 1) return 1;
    if (menu->nivelSelecionado > 2) return 2;
    
    return menu->nivelSelecionado;
}

bool get_tem_save(Menu* menu) {
    return menu ? menu->temSave : false;
}

void set_estado_menu(Menu* menu, EstadoMenu novoEstado) {
    if (menu) {
        menu->estado = novoEstado;
        menu->transicaoAtiva = true;
        menu->transicaoTempo = 0.0f;
    }
}

void set_nivel_selecionado(Menu* menu, int nivel) {
    if (menu && nivel >= 1 && nivel <= 2) {
        menu->nivelSelecionado = nivel;
    }
}

void liberar_menu(Menu* menu) {
    if (!menu) return;
    
    printf("Liberando recursos do menu...\n");
    
    // Liberar texturas
    if (menu->logo.id > 0) UnloadTexture(menu->logo);
    if (menu->background.id > 0) UnloadTexture(menu->background);
    
    // Liberar fontes personalizadas
    if (menu->fonteTitulo.texture.id > 0 &&
        menu->fonteTitulo.texture.id != GetFontDefault().texture.id) {
        UnloadFont(menu->fonteTitulo);
        printf("Fonte personalizada liberada\n");
    }
    
    free(menu);
    printf("Menu liberado da memória\n");
}