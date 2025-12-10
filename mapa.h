#ifndef MAPA_H
#define MAPA_H

// Definições atualizadas
typedef struct {
    char **matriz;
    int linhas;
    int colunas;
    int pontosRestantes;  // ou pellets_restantes (escolha um)
    char nome[100];       // Adicionado para armazenar nome
} Mapa;

// Funções principais
Mapa* criar_mapa(void);  // Renomeado de carregarMapa
void liberar_mapa(Mapa* mapa);
int carregar_mapa(Mapa* mapa, const char* caminho_arquivo);  // Se precisar manter
void imprimir_mapa(Mapa* mapa);

// Funções utilitárias
char elemento_no(Mapa* mapa, int linha, int coluna);
void remover_pellet(Mapa* mapa, int linha, int coluna);
void remover_power_pellet(Mapa* mapa, int linha, int coluna);

// Novas funções para compatibilidade
Mapa* carregarMapa(const char* arquivo);  // Adicionado
void liberarMapa(Mapa* mapa);             // Adicionado
int contarPontosMapa(Mapa* mapa);         // Adicionado
int verificarCelula(Mapa* mapa, int linha, int coluna, char tipo);  // Adicionado
void definirCelula(Mapa* mapa, int linha, int coluna, char valor);  // Adicionado

#endif