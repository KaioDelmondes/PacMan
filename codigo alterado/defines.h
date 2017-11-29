// Eixos
#define X 0
#define Y 1

// Velocidade do jogo em frames por segundo
#define PAC_FPS 20

// Máximo de frames pulados
#define PAC_PULOS_MAX 4

// Tempo (em ms) para verificar se um estado do jogo mudou
#define PAC_TIMER 500

// Tamanho do Mapa
#define PAC_TAMANHO_MAPA 50
#define PAC_TAMANHO_MINIATURA 8
#define PAC_MAPA_ALTURA 21
#define PAC_MAPA_LARGURA 19

// posições dos elementos do pacman
#define PAC_CAMINHO 0
#define PAC_PAREDE 1
#define PAC_TELETRANSPORTE 2
#define PAC_BLINKY 3
#define PAC_PINKY 4
#define PAC_INKY 5
#define PAC_CLYDE 6
#define PAC_PACMAN 7
#define PAC_NADA 9

// Tipos de bolinhas
#define PAC_BOLINHA_NENHUMA 0
#define PAC_BOLINHA_NORMAL 1
#define PAC_BOLINHA_ESPECIAL 2

// Direções de movimento
#define PAC_DIRECAO_CIMA 0
#define PAC_DIRECAO_ESQUERDA 1
#define PAC_DIRECAO_BAIXO 2
#define PAC_DIRECAO_DIREITA 3

// Quantidade de Fantasmas
#define PAC_FANTASMAS 4

// Tipos de fantasmas
#define Fantasma1 0
#define Fantasma2 1
#define Fantasma3 2
#define Fantasma4 3

// Estados dos fantasmas
#define PAC_ESTADO_ESPALHAR 0
#define PAC_ESTADO_PERSEGUIR 1

// Estados de captura dos fantasmas
#define PAC_CAPTURA_PRISAO -2
#define PAC_CAPTURA_NORMAL -1
#define PAC_CAPTURA_AZUL 0
#define PAC_CAPTURA_CAPTURADO 1

// Velocidade dos personagens
#define PAC_VELOCIDADE_PACMAN_A 0.167 // 1/6 quadradinhos por frame
#define PAC_VELOCIDADE_FANTASMA_A 0.125 // 1/8 quadradinhos por frame
#define PAC_VELOCIDADE_PACMAN_B 0.2 // 1/5 quadradinhos por frame
#define PAC_VELOCIDADE_FANTASMA_B 0.167 // 1/6 quadradinhos por frame
#define PAC_VELOCIDADE_PACMAN_C 0.2 // 1/5 quadradinhos por frame
#define PAC_VELOCIDADE_FANTASMA_C 0.2 // 1/5 quadradinhos por frame
#define PAC_VELOCIDADE_PACMAN_D 0.2 // 1/5 quadradinhos por frame
#define PAC_VELOCIDADE_FANTASMA_D 0.25 // 1/4 quadradinhos por frame

// Número inicial de vidas
#define PAC_VIDAS 5

// Ângulo de abertura máxima da boca do pac-man
#define PAC_BOCA_MAX 30
