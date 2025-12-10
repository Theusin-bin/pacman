# pacman
# PAC-MAN UFRJ 🎮

Um clone do clássico jogo PAC-MAN desenvolvido em C utilizando a biblioteca Raylib, criado como projeto acadêmico para a UFRJ.

## 📋 Índice

- [Sobre o Projeto](#sobre-o-projeto)
- [Características](#características)
- [Requisitos](#requisitos)
- [Instalação](#instalação)
- [Como Jogar](#como-jogar)
- [Estrutura do Projeto](#estrutura-do-projeto)
- [Mapas](#mapas)
- [Controles](#controles)
- [Funcionalidades](#funcionalidades)
- [Compilação](#compilação)
- [Créditos](#créditos)

## 🎯 Sobre o Projeto

PAC-MAN UFRJ é uma implementação completa do clássico jogo arcade PAC-MAN, desenvolvido como trabalho acadêmico. O projeto utiliza programação em C com a biblioteca gráfica Raylib e implementa mecânicas clássicas do jogo original, incluindo:

- Sistema de níveis com mapas customizáveis
- Inteligência artificial dos fantasmas
- Sistema de power pellets
- Portais/túneis para teleporte
- Sistema de pontuação e vidas
- Menu interativo
- Sistema de save/load

## ✨ Características

### Gameplay
- **2 Níveis Jogáveis**: Mapa básico e mapa com túneis
- **4 Fantasmas Inteligentes**: Com comportamento de perseguição
- **Power Pellets**: Tornam os fantasmas vulneráveis temporariamente
- **Sistema de Portais**: Teleporte entre pontos do mapa
- **Sistema de Vidas**: 3 vidas iniciais
- **Pontuação**: 
  - Pellets normais: 10 pontos
  - Power pellets: 50 pontos
  - Fantasmas vulneráveis: 200 pontos

### Interface
- Menu principal com navegação intuitiva
- HUD com informações em tempo real
- Telas de vitória e game over
- Sistema de pausa
- Animações e transições suaves

## 🔧 Requisitos

### Software Necessário
- Compilador C (GCC recomendado)
- Biblioteca Raylib (versão 4.0 ou superior)
- Make (opcional, para usar Makefile)

### Sistema Operacional
- Windows
- Linux
- macOS

## 📦 Instalação

### 1. Clone ou baixe o repositório

```bash
git clone [URL_DO_REPOSITORIO]
cd pacman-ufrj
```

### 2. Instale a Raylib

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get install libraylib-dev
```

#### Windows
Baixe e instale do site oficial: https://www.raylib.com/

#### macOS
```bash
brew install raylib
```

### 3. Prepare os arquivos de mapa

Certifique-se de que os arquivos `mapa1.txt` e `mapa2.txt` estão na pasta `assets/`:

```
assets/
├── mapa1.txt
├── mapa2.txt
├── logo.png (opcional)
├── menu_bg.png (opcional)
└── fonts/
    └── pacfont.ttf (opcional)
```

## 🎮 Como Jogar

### Objetivo
Coma todos os pellets (pontos) do mapa enquanto evita os fantasmas. Use os power pellets (pontos grandes) para tornar os fantasmas vulneráveis e comê-los para ganhar pontos extras!

### Regras
1. Coma todos os pellets para completar o nível
2. Evite os fantasmas vermelhos - eles tiram uma vida
3. Pegue power pellets (pontos verdes grandes) para deixar os fantasmas azuis
4. Fantasmas azuis podem ser comidos por pontos extras
5. Use os portais (magenta) para se teleportar
6. O jogo termina quando você perde todas as vidas

## 🗂️ Estrutura do Projeto

```
pacman-ufrj/
├── main.c              # Ponto de entrada do programa
├── jogo.c/h            # Lógica principal do jogo e estados
├── menu.c/h            # Sistema de menus
├── mapa.c/h            # Carregamento e manipulação de mapas
├── entidades.c/h       # PAC-MAN e fantasmas
├── colisoes.c/h        # Sistema de detecção de colisões
├── graficos.c/h        # Renderização e interface gráfica
├── assets/
│   ├── mapa1.txt       # Mapa básico
│   ├── mapa2.txt       # Mapa com túneis
│   └── ...
└── README.md
```

### Módulos Principais

#### `main.c`
- Inicialização da janela e do jogo
- Loop principal do jogo
- Finalização e limpeza de recursos

#### `jogo.c/h`
- Gerenciamento de estados (MENU, JOGANDO, PAUSA, GAME_OVER, VITÓRIA)
- Controle de fluxo do jogo
- Sistema de níveis
- Processamento de input por estado
- Sistema de save/load

#### `menu.c/h`
- Menu principal interativo
- Seleção de níveis
- Confirmação de saída
- Sistema de navegação
- Detecção de saves

#### `mapa.c/h`
- Carregamento de mapas de arquivo .txt
- Contagem de pellets
- Manipulação de células do mapa
- Validação de limites

#### `entidades.c/h`
- Criação e gerenciamento do PAC-MAN
- Criação e gerenciamento de fantasmas (lista encadeada)
- Movimento e colisões
- Estados (normal, vulnerável, power mode)
- IA dos fantasmas
- Sistema de velocidade configurável

#### `colisoes.c/h`
- Detecção de colisões entre entidades
- Processamento de portais
- Verificação em tempo real
- Tipos de colisão (parede, pellet, power pellet, fantasma, portal)

#### `graficos.c/h`
- Renderização do mapa
- Desenho de entidades
- HUD (vidas, pontos, nível, pellets restantes)
- Telas especiais (pausa, game over, vitória)

## 🗺️ Mapas

### Formato dos Mapas
Os mapas são arquivos de texto (.txt) com os seguintes símbolos:

- `#` - Parede (azul)
- `.` - Pellet normal (branco, 10 pontos)
- `O` - Power pellet (verde, 50 pontos)
- `T` - Portal/Túnel (magenta)
- ` ` - Espaço vazio (preto)

### Exemplo de Mapa
```
############################
#............##............#
#.####.#####.##.#####.####.#
#O####.#####.##.#####.####O#
#..........................#
#.####.##.########.##.####.#
#......##....##....##......#
######.##### ## #####.######
     #.##    T     ##.#     
######.## ######## ##.######
T     .   #      #   .     T
######.## ######## ##.######
     #.##          ##.#     
######.## ######## ##.######
#............##............#
#.####.#####.##.#####.####.#
#O..##.......  .......##..O#
###.##.##.########.##.##.###
#......##....##....##......#
#.##########.##.##########.#
#..........................#
############################
```

### Mapas Disponíveis

#### Nível 1 - Mapa Básico (`mapa1.txt`)
- Ideal para iniciantes
- Layout tradicional
- Dificuldade: Fácil

#### Nível 2 - Mapa com Túneis (`mapa2.txt`)
- Contém portais para teleporte
- Mais desafiador
- Dificuldade: Média

## 🎮 Controles

### Menu
- `↑` / `W` - Navegar para cima
- `↓` / `S` - Navegar para baixo
- `←` / `A` - Selecionar nível anterior
- `→` / `D` - Selecionar próximo nível
- `ENTER` / `ESPAÇO` - Confirmar seleção
- `R` - Voltar ao menu principal

### Durante o Jogo
- `↑` / `W` - Mover para cima
- `↓` / `S` - Mover para baixo
- `←` / `A` - Mover para esquerda
- `→` / `D` - Mover para direita
- `P` - Pausar/Despausar
- `R` - Voltar ao menu
- `S` - Salvar jogo
- `T` - Reiniciar nível (opcional)

### Telas Especiais
- `ENTER` - Reiniciar jogo (Game Over/Vitória)
- `N` - Próximo nível (Vitória - apenas no nível 1)
- `R` - Voltar ao menu

## 🔨 Compilação

### Usando GCC diretamente

```bash
gcc main.c jogo.c menu.c mapa.c entidades.c colisoes.c graficos.c \
    -o pacman -lraylib -lm -lpthread -ldl -lrt -lX11
```

### Usando Makefile (se disponível)

```bash
make
./pacman
```

### Windows (MinGW)

```bash
gcc main.c jogo.c menu.c mapa.c entidades.c colisoes.c graficos.c \
    -o pacman.exe -lraylib -lopengl32 -lgdi32 -lwinmm
```

## 💾 Sistema de Save/Load

O jogo salva automaticamente o progresso em `savegame.dat` quando você pressiona `S` durante o jogo.

**Informações salvas:**
- Nível atual
- Pontuação
- Vidas restantes

**Para carregar:**
- Selecione "CARREGAR JOGO" no menu principal
- O jogo continuará de onde você parou

## 🎨 Configurações Visuais

### Cores Padrão
- **Parede**: Azul
- **PAC-MAN**: Amarelo
- **Fantasma Normal**: Vermelho
- **Fantasma Vulnerável**: Azul
- **Pellet**: Branco
- **Power Pellet**: Verde
- **Portal**: Magenta
- **Fundo**: Preto

### Dimensões
- **Tamanho do bloco**: 40x40 pixels
- **Janela**: 1600x840 pixels (ajustável ao tamanho do mapa)
- **HUD**: 40 pixels de altura
- **FPS**: 60 frames por segundo

## 🐛 Solução de Problemas

### O jogo não inicia
- Verifique se a Raylib está instalada corretamente
- Confirme que todos os arquivos .c e .h estão presentes
- Verifique permissões de execução no Linux/macOS

### Mapas não carregam
- Certifique-se de que `mapa1.txt` e `mapa2.txt` existem na pasta `assets/`
- Verifique se o formato do mapa está correto
- Confirme que não há caracteres especiais inválidos

### Fantasmas não se movem
- Verifique se as entidades foram inicializadas corretamente
- Confirme que o mapa foi carregado

### Colisões não funcionam
- Verifique se todas as funções de colisão estão implementadas
- Confirme que as entidades estão sendo atualizadas a cada frame

## 📝 Notas de Desenvolvimento

### Características Técnicas
- **Linguagem**: C (padrão C99)
- **Biblioteca Gráfica**: Raylib 4.0+
- **Estruturas de Dados**: Listas encadeadas para fantasmas
- **Alocação Dinâmica**: Uso extensivo de malloc/free
- **Sistema de Estados**: Máquina de estados finitos para controle do jogo

### Melhorias Futuras Possíveis
- [ ] Mais níveis
- [ ] Power-ups adicionais
- [ ] Sistema de high scores
- [ ] Música e efeitos sonoros
- [ ] Animações mais elaboradas
- [ ] Diferentes tipos de fantasmas (com comportamentos únicos)
- [ ] Modo multiplayer
- [ ] Editor de mapas

## 👥 Créditos

**Desenvolvido para**: UFRJ (Universidade Federal do Rio de Janeiro)
**Ano**: 2024
**Biblioteca Gráfica**: [Raylib](https://www.raylib.com/)
**Inspirado em**: PAC-MAN original (Namco, 1980)

---

## 📄 Licença

Este projeto é um trabalho acadêmico desenvolvido para fins educacionais.

---

## 🤝 Contribuindo

Este é um projeto acadêmico, mas sugestões e melhorias são bem-vindas!

---

**PAC-MAN UFRJ** - Um projeto de programação em C com paixão por jogos clássicos! 🎮👾
