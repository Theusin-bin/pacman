#include "mapa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "raylib.h"

#define MAX_LINHAS 100
#define MAX_COLUNAS 100

static const char* extrairNomeArquivo(const char *caminho) {
    const char *nome = strrchr(caminho, '/');
    if (!nome) nome = strrchr(caminho, '\\');
    return nome ? nome + 1 : caminho;
}

Mapa *carregarMapa(const char *arquivo) {
    if (!arquivo) {
        fprintf(stderr, "Erro: Nome de arquivo nulo\n");
        return NULL;
    }
    
    FILE *f = fopen(arquivo, "r");
    if (!f) {
        fprintf(stderr, "Erro ao abrir arquivo: %s\n", arquivo);
        return NULL;
    }

    Mapa *m = (Mapa*)malloc(sizeof(Mapa));
    if (!m) {
        fclose(f);
        fprintf(stderr, "Erro de alocação de memória\n");
        return NULL;
    }

    // Inicializar valores
    m->linhas = 0;
    m->colunas = 0;
    m->pontosRestantes = 0;
    strcpy(m->nome, extrairNomeArquivo(arquivo));

    // Ler dimensões
    char buffer[MAX_COLUNAS + 2];
    int maxColunas = 0;
    
    // Contar linhas e largura
    while (fgets(buffer, sizeof(buffer), f)) {
        m->linhas++;
        int len = strlen(buffer);
        while (len > 0 && (buffer[len-1] == '\n' || buffer[len-1] == '\r')) {
            buffer[--len] = '\0';
        }
        if (len > maxColunas) maxColunas = len;
    }
    
    m->colunas = maxColunas;
    
    if (m->linhas == 0 || m->colunas == 0) {
        fprintf(stderr, "Mapa vazio ou inválido: %s\n", arquivo);
        fclose(f);
        free(m);
        return NULL;
    }
    
    // Voltar ao início
    rewind(f);
    
    // Alocar matriz
    m->matriz = (char**)malloc(sizeof(char*) * m->linhas);
    if (!m->matriz) {
        fclose(f);
        free(m);
        return NULL;
    }
    
    for (int i = 0; i < m->linhas; i++) {
        m->matriz[i] = (char*)malloc(sizeof(char) * (m->colunas + 1));
        if (!m->matriz[i]) {
            for (int j = 0; j < i; j++) free(m->matriz[j]);
            free(m->matriz);
            fclose(f);
            free(m);
            return NULL;
        }
        
        // Inicializar com espaços
        for (int j = 0; j < m->colunas; j++) {
            m->matriz[i][j] = ' ';
        }
        m->matriz[i][m->colunas] = '\0';
        
        // Ler linha
        if (fgets(buffer, sizeof(buffer), f)) {
            int len = strlen(buffer);
            while (len > 0 && (buffer[len-1] == '\n' || buffer[len-1] == '\r')) {
                buffer[--len] = '\0';
            }
            
            int copyLen = (len < m->colunas) ? len : m->colunas;
            strncpy(m->matriz[i], buffer, copyLen);
        }
    }
    
    fclose(f);
    
    // Contar pellets
    m->pontosRestantes = 0;
    for (int i = 0; i < m->linhas; i++) {
        for (int j = 0; j < m->colunas; j++) {
            if (m->matriz[i][j] == '.' || m->matriz[i][j] == 'O') {
                m->pontosRestantes++;
            }
        }
    }
    
    printf("Mapa carregado: %s (%dx%d), pellets: %d\n", 
           m->nome, m->colunas, m->linhas, m->pontosRestantes);
    
    return m;
}

void liberarMapa(Mapa *mapa) {
    if (!mapa) return;
    
    if (mapa->matriz) {
        for (int i = 0; i < mapa->linhas; i++) {
            free(mapa->matriz[i]);
        }
        free(mapa->matriz);
    }
    free(mapa);
}

int contarPontosMapa(Mapa *mapa) {
    if (!mapa) return 0;
    return mapa->pontosRestantes;
}

int verificarCelula(Mapa *mapa, int linha, int coluna, char tipo) {
    if (!mapa || linha < 0 || linha >= mapa->linhas || 
        coluna < 0 || coluna >= mapa->colunas) {
        return 0;
    }
    return (mapa->matriz[linha][coluna] == tipo);
}

void definirCelula(Mapa *mapa, int linha, int coluna, char valor) {
    if (!mapa || linha < 0 || linha >= mapa->linhas || 
        coluna < 0 || coluna >= mapa->colunas) {
        return;
    }
    
    char antigo = mapa->matriz[linha][coluna];
    mapa->matriz[linha][coluna] = valor;
    
    // Atualizar contagem
    if ((antigo == '.' || antigo == 'O') && (valor == ' ')) {
        mapa->pontosRestantes--;
    }
}

void remover_pellet(Mapa *mapa, int linha, int coluna) {
    if (verificarCelula(mapa, linha, coluna, '.')) {
        definirCelula(mapa, linha, coluna, ' ');
    }
}

void remover_power_pellet(Mapa *mapa, int linha, int coluna) {
    if (verificarCelula(mapa, linha, coluna, 'O')) {
        definirCelula(mapa, linha, coluna, ' ');
    }
}