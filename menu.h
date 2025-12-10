#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include <stdbool.h>

// Definir NUM_OPCOES aqui no header para consistência
#define NUM_OPCOES 3

// Enum para opções do menu principal
typedef enum {
    OPCAO_NOVO_JOGO = 0,
    OPCAO_CARREGAR,
    OPCAO_SAIR
} OpcaoMenu;

// Enum para estados do menu (com compatibilidade)
typedef enum {
    MENU_PRINCIPAL = 0,
    MENU_SELECAO_NIVEL,
    MENU_SAIR_CONFIRMAR,
    // Aliases para compatibilidade com código antigo
    MENU_SAIR = MENU_SAIR_CONFIRMAR,      // Compatibilidade
    MENU_JOGAR = MENU_SELECAO_NIVEL,      // Compatibilidade
    MENU_OPCOES = -1,                     // Não usado mais, mas definido
    MENU_CREDITOS = -2                    // Não usado mais, mas definido
} EstadoMenu;

// Estrutura do Menu (simplificada sem sons)
typedef struct Menu {
    EstadoMenu estado;
    OpcaoMenu opcaoSelecionada;
    int nivelSelecionado;
    float tempoAnimacao;
    bool transicaoAtiva;
    float transicaoTempo;
    
    // Sistema de save/load
    bool temSave;
    char saveFileName[100];
    
    // Recursos gráficos
    Texture2D logo;
    Texture2D background;
    Font fonteTitulo;
    Font fonteMenu;
} Menu;

// Criação / inicialização / atualização / liberação
Menu* criar_menu(void);
void inicializar_menu(Menu* menu);
void atualizar_menu(Menu* menu);
void processar_input_menu(Menu* menu);
void liberar_menu(Menu* menu);

// Desenho
void desenhar_menu(Menu* menu);
void desenhar_menu_principal(Menu* menu);
void desenhar_menu_selecao_nivel(Menu* menu);
void desenhar_botao(const char* texto, int x, int y, int largura, int altura,
                    bool selecionado, Color corNormal, Color corSelecionado);
void animar_transicao(Menu* menu);

// Getters / setters
EstadoMenu get_estado_menu(Menu* menu);
int get_nivel_selecionado(Menu* menu);
bool get_tem_save(Menu* menu);
void set_estado_menu(Menu* menu, EstadoMenu novoEstado);
void set_nivel_selecionado(Menu* menu, int nivel);

// Sistema de save/load
void salvar_jogo(Menu* menu, int nivel, int pontos, int vidas);
bool carregar_jogo(Menu* menu, int* nivel, int* pontos, int* vidas);
void verificar_save(Menu* menu);

#endif // MENU_H