#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "global.h"

// Eixos
#define X 0
#define Y 1

// Velocidade do jogo em frames por segundo
#define PAC_FPS 24

// M�ximo de frames pulados
#define PAC_PULOS_MAX 4

// Tempo (em ms) para verificar se um estado do jogo mudou
#define PAC_TIMER 500 

// Tamanho do Mapa
#define PAC_TAMANHO_MAPA 50
#define PAC_TAMANHO_MINIATURA 10
#define PAC_MAPA_ALTURA 21
#define PAC_MAPA_LARGURA 19

// Tipos de Tiles
#define PAC_TILE_CAMINHO 0
#define PAC_TILE_PAREDE 1
#define PAC_TILE_TELETRANSPORTE 2
#define PAC_TILE_BLINKY 3
#define PAC_TILE_PINKY 4
#define PAC_TILE_INKY 5
#define PAC_TILE_CLYDE 6
#define PAC_TILE_PACMAN 7
#define PAC_TILE_NADA 9

// Tipos de bolinhas
#define PAC_BOLINHA_NENHUMA 0
#define PAC_BOLINHA_NORMAL 1
#define PAC_BOLINHA_ESPECIAL 2

// Dire��es de movimento
#define PAC_DIRECAO_CIMA 0
#define PAC_DIRECAO_ESQUERDA 1
#define PAC_DIRECAO_BAIXO 2
#define PAC_DIRECAO_DIREITA 3

// Quantidade de Fantasmas
#define PAC_FANTASMAS 4

// Tipos de fantasmas
#define PAC_FANTASMA_BLINKY 0
#define PAC_FANTASMA_PINKY 1
#define PAC_FANTASMA_INKY 2
#define PAC_FANTASMA_CLYDE 3

// Estados dos fantasmas
#define PAC_ESTADO_ESPALHAR 0
#define PAC_ESTADO_PERSEGUIR 1

// Estados de captura dos fantasmas
#define PAC_CAPTURA_PRISAO -2
#define PAC_CAPTURA_NORMAL -1
#define PAC_CAPTURA_AZUL 0
#define PAC_CAPTURA_CAPTURADO 1

// Velocidade dos personagens
#define PAC_VELOCIDADE_PACMAN_A 0.167 // 1/6 tiles por frame
#define PAC_VELOCIDADE_FANTASMA_A 0.125 // 1/8 tiles por frame
#define PAC_VELOCIDADE_PACMAN_B 0.2 // 1/5 tiles por frame
#define PAC_VELOCIDADE_FANTASMA_B 0.167 // 1/6 tiles por frame
#define PAC_VELOCIDADE_PACMAN_C 0.2 // 1/5 tiles por frame
#define PAC_VELOCIDADE_FANTASMA_C 0.2 // 1/5 tiles por frame
#define PAC_VELOCIDADE_PACMAN_D 0.2 // 1/5 tiles por frame
#define PAC_VELOCIDADE_FANTASMA_D 0.25 // 1/4 tiles por frame

// N�mero inicial de vidas
#define PAC_VIDAS 3

// �ngulo de abertura m�xima da boca do pac-man
#define PAC_BOCA_MAX 30

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

typedef struct tile {
	int tipo; // Tipo
	int bolinha; // Bolinha que est� dentro
	int pos[2]; // Posi��o
	struct tile *tele; // Para onde teletransporta
} Tile;

typedef struct mapa {
	struct tile **tiles; // Matriz de tiles
	int bolinhas; // Bolinhas restantes
	//int paredes;
	unsigned int lista; // Lista de exibi��o
} Mapa;

typedef struct pacman {
	int vidas; // Quantidade de vidas
	int pontos; // Pontua��o
	int fase; // Fase atual
	int morto; // Est� morto
	int capturados; // Fantasmas capturados
	int direcao; // Dire��o de movimento
	int pre; // Dire��o do pr�-move
	int boca; // Abertura da boca
	double mov; // Porcentagem de movimento
	double velocidade; // Velocidade
	int tele; // Se foi teletransportado
	struct tile *atual; // Posi��o atual
	struct tile *destino; // Pr�xima posi��o
	struct tile *inicio; // Posi��o inicial
} Pacman;

typedef struct fantasma {
	int nome; // Qual fantasma �
	int estado; // Estado
	int capturado; // Se pode ser capturado
	int direcao; // Dire��o de movimento
	double mov; // Porcentagem de movimento
	double velocidade; // Velocidade
	int tele; // Se foi teletransportado
	struct tile *atual; // Posi��o atual
	struct tile *destino; // Pr�xima posi��o
	struct tile *inicio; // Posi��o inicial
	struct tile *alvo; // Aonde vai
} Fantasma;

// Tiles do Mapa
int tTipos[][PAC_MAPA_LARGURA] =
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
1,0,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,0,1,
1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1,
1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,
9,9,9,1,0,1,0,0,0,0,0,0,0,1,0,1,9,9,9,
1,1,1,1,0,1,0,1,1,3,1,1,0,1,0,1,1,1,1,
2,0,0,0,0,0,0,1,5,4,6,1,0,0,0,0,0,0,2,
1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,
9,9,9,1,0,1,0,0,0,0,0,0,0,1,0,1,9,9,9,
1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,
1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
1,0,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,0,1,
1,0,0,1,0,0,0,0,0,7,0,0,0,0,0,1,0,0,1,
1,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,1,
1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1,
1,0,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

// Posi��es das Bolinhas
int tBolinhas[][PAC_MAPA_LARGURA] =
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,
0,2,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,2,0,
0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
0,1,0,0,1,0,1,0,0,0,0,0,1,0,1,0,0,1,0,
0,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,0,
0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,
0,1,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,1,0,
0,2,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,2,0,
0,0,1,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,0,
0,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,0,
0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,1,0,
0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

// Vari�veis do jogo
int especiais = 0; // Quantidade de bolinhas especiais ativadas
int estados = 1; // N�mero de vezes nas quais o estado dos fantasmas mudou
int maiorPontuacao = 0; // Recorde atual do jogo

// gui.c
void inicializar();
void desenhar();
void mostrarMenu();
void atualizarJogo();
void mostrarJogo();
void centralizarCamera(Pacman *, double);
void reiniciarJogo();
void reiniciar(Pacman *, Fantasma *);
void mudarEstado(int);
void iniciou(int);
void morreu(int);
void passou(int);
void terminarEspecial(int);
void mover(int, int, int);
void teclado(unsigned char, int, int);
void redimensionar(int, int);
void escrever(char *, int, int, int, int);
void carregarBitmap(char *, unsigned int, int, int);

// mapa.h
Mapa *criaMapa(int [][PAC_MAPA_LARGURA], int [][PAC_MAPA_LARGURA], Pacman *, Fantasma *);
void mostrarMapa(Mapa *);
void reiniciarMapa(Mapa *, int [][PAC_MAPA_LARGURA]);
Tile *proximoTile(Mapa *, Tile *, int, int);
Tile *proximoTileEm(int, Mapa *, Tile *, int);
int ePrisao(Tile *);
double distanciaEntre(Tile *, Tile *);
void desenhaQuadrado(float);
#include "mapa.h"

// pacman.h
Pacman *criaPacman();
void movePacman(Pacman *, Mapa *, Fantasma *);
void mostrarPacman(Pacman *, double);
double velocidadePacman(Pacman *);
int pacmanPodeAndarSobre(Tile *);
int pacmanPodeMudarDirecao(Pacman *, Mapa *);
void estaVivo(Pacman *, Fantasma *);
void ganharPontos(Pacman *, int, int *);
void hemisferio(double);
#include "pacman.h"

// fantasma.h
Fantasma *criaFantasmas();
void moveFantasmas(Fantasma *, Mapa *, Pacman *);
void mostrarFantasmas(Fantasma *, double);
double velocidadeFantasma(Fantasma, Pacman *);
Tile *defineAlvo(Fantasma, Mapa *, Pacman *, Fantasma *);
int fantasmaPodeAndarSobre(Fantasma, Tile *);
int fantasmaPodeMudarDirecao(Fantasma, Mapa *);
int melhorCaminho(Fantasma, Mapa *, Pacman *);
#include "fantasma.h"

// Vari�veis do Game Loop
unsigned int tempoInicial;
unsigned int tempoAtual;
int passos;
int frame = 1000 / PAC_FPS;
double interpolacao;

// Vari�veis da tela
int telaW = 800;
int telaH = 500;

// Vari�veis do jogo
Pacman *pac;
Fantasma *fant;
Mapa *map;
char pontuacao[10];
char vidas[10];
char recorde[10];

// Vari�veis de estado
int menu = 1;
int opcao = 0;
int pause = 0;
int inicio = 1;
int morte = 0;
int fim = 0;

int main() {
	// Configura a janela
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(telaW, telaH);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Pac-Man");

	
	// Inicializa as vari�veis
	inicializar();
	
	// Informa as fun��es de intera��o
	glutDisplayFunc(desenhar);
	glutSpecialFunc(mover);
	glutKeyboardFunc(teclado);
	glutReshapeFunc(redimensionar);
	
	// Inicia o jogo
	glutMainLoop();
}

// Inicializa as vari�veis do jogo
void inicializar() {
	// Cria os objetos do jogo
	pac = criaPacman();
	fant = criaFantasmas();
	map = criaMapa(tTipos, tBolinhas, pac, fant);
	
	// Marca o tempo inicial
	tempoInicial = glutGet(GLUT_ELAPSED_TIME);
	
	// Carrega a imagem de fundo
	//carregarBitmap("pacImg1.bmp", 1, 296, 170);
	carregarBitmap("pacImg2.bmp", 1, 700, 420);
	
	// Habilita o uso de buffer duplo
	glEnable(GL_DEPTH_TEST);
	glDrawBuffer(GL_BACK);
	
	// Inicia o gerador de n�meros
	//rand()%100;
	
	glutTimerFunc(PAC_TIMER, mudarEstado, 5000);
}

// Desenha e exibe a imagem atual na tela
void desenhar() {
	// Limpa a tela
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Determina o tamanho da �rea de visualiza��o
	glMatrixMode(GL_VIEWPORT);
	glLoadIdentity();
	glViewport(0, 0, telaW, telaH);
	
	// Utiliza a proje��o ortogr�fica
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((double) -telaW/2, (double) telaW/2, (double) -telaH/2, (double) telaH/2, -2000, 2000);

	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	if (menu) {
		// Desenha o menu
		mostrarMenu();
	} else {
		// Desenha o jogo
		atualizarJogo();
		mostrarJogo();
	}
	
	glutSwapBuffers();
	glutPostRedisplay();
}

// Monta o menu do jogo
void mostrarMenu() {
	int i;
	char itens[4][21];
	char sobre[4][31];
	char controles[3][34];
	
	// Exibe imagem de fundo
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f((double) -telaW/2, (double) -telaH/2, 0);
		glTexCoord2f(0, 1);
		glVertex3f((double) -telaW/2, (double) telaH/2, 0);
		glTexCoord2f(1, 1);
		glVertex3f((double) telaW/2, (double) telaH/2, 0);
		glTexCoord2f(1, 0);
		glVertex3f((double) telaW/2, (double) -telaH/2, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	
	// Exibe o texto
	switch (menu) {
		case 1: // Tela principal
			if (pause)
				strcpy(itens[0], "Continuar Jogo");
			else{
				strcpy(itens[0], "Iniciar Jogo");
				strcpy(itens[1], "Sobre");
				strcpy(itens[2], "Sair");
			}
			// Desenha os �tens
			for (i = 0; i < 3; i++)
				escrever(itens[i], 0, (double) telaH/2 + 25*i + 40, 0, opcao == i);
			
			// Desenha a pontua��o recorde
			itoa(maiorPontuacao, recorde, 10);
			escrever("Recorde", 20+3*9, 20, 1, 0);
			escrever(recorde, 20, 35, 1, 0);
			break;
		case 2: // Sobre
			strcpy(controles[0], "Autores: Elineide e Kaio");
			strcpy(controles[1], "Setas: Movimentacao do PacMan");
			strcpy(controles[2], "Enter: Continua (ou pausa) o jogo");
			strcpy(controles[3], "Esc: voltar");
		
			for (i = 0; i < 4; i++)
				escrever(controles[i], (double) telaW/2 - 30*8, (double) telaH/2 + 20*(i-0.5), -1, 0);
			break;
		default://Sair
			exit(0);
	}
}

// Atualiza o jogo um passo
void atualizarJogo() {
	int i;
	tempoAtual = glutGet(GLUT_ELAPSED_TIME);
	passos = 0;
	
	if (!pause) {
		// Atualiza o jogo, se n�o estiver pausado
		while ((tempoAtual - tempoInicial) > frame
			   && passos < PAC_PULOS_MAX) {
			// Verifica se ainda restam bolinhas
			if (map->bolinhas) {	
				// Se est� come�ando a fase, espera
				if (inicio) {
					pac->boca = 30;
					glutTimerFunc(PAC_TIMER, iniciou, 2000);
				// Se perdeu uma vida, espera
				} else if (pac->morto && !morte) {
					morte = 1;
					glutTimerFunc(PAC_TIMER, morreu, 2000);
				// Se est� normal, d� um passo
				} else if (!pac->morto && !fim) {
					movePacman(pac, map, fant);
					estaVivo(pac, fant);
				}
				
				// Move os fantasmas
				if (!inicio && !fim)
						moveFantasmas(fant, map, pac);
			} else {
				map->bolinhas = -1;
				fim = 1;
				glutTimerFunc(PAC_TIMER, passou, 2000);
			}
			
			tempoInicial += frame;
			tempoAtual = glutGet(GLUT_ELAPSED_TIME);
			passos++;
		}
		
		if ((tempoAtual - tempoInicial) > frame)
			tempoInicial = tempoAtual - frame;
		
		// Calcula a interpola��o que deve ser usada
		interpolacao = min(1.0, (double) (tempoAtual - tempoInicial) / frame);
	} else {
		interpolacao = 0;
	}
}

// Monta a cena atual do jogo
void mostrarJogo() {
	// Exibe a pontua��o atual e o recorde
	itoa(pac->pontos, pontuacao, 10);
	itoa(pac->vidas, vidas, 10);
	itoa(maiorPontuacao, recorde, 10);
	escrever("Pontos", 20, 20, -1, 0);
	escrever(pontuacao, 20+(10-strlen(pontuacao))*9, 35, -1, 0);
	escrever("Vidas", 20+12*9, 20, -1, 0);
	escrever(vidas, 20+12*9+(10-strlen(vidas))*9, 35, -1, 0);
	escrever("Recorde", 20+3*9, 20, 1, 0);
	escrever(recorde, 20, 35, 1, 0);
	
	// Informa quando est� pausado
    if (pause)
		escrever("Pausado", 0, telaH/2-50, 0, 0);
	
	// Posi��o da c�mera
	glRotated(35, 1, 0, 0);
	glRotated(-45, 0, 1, 0);
	glRotated(-90, 1, 0, 0);
	glScaled(PAC_TAMANHO_MAPA, PAC_TAMANHO_MAPA, PAC_TAMANHO_MAPA);
	
	// Se est� morto, mover apenas os fantasmas
	if (morte || fim) {
		centralizarCamera(pac, 0);
	
	
		mostrarMapa(map);
		mostrarPacman(pac, 0);
		
		// Se passou de fase, n�o move os fantasmas
		if (fim)
			mostrarFantasmas(fant, 0);
		else
			mostrarFantasmas(fant, interpolacao);
	} else {
		centralizarCamera(pac, interpolacao);

		mostrarMapa(map);
		mostrarPacman(pac, interpolacao);
		mostrarFantasmas(fant, interpolacao);
	}
	
	// Miniatura
	glMatrixMode(GL_VIEWPORT);
	glLoadIdentity();
	glViewport(10, 10, (PAC_MAPA_LARGURA+1)*PAC_TAMANHO_MINIATURA, (PAC_MAPA_ALTURA+1)*PAC_TAMANHO_MINIATURA);
	
	// Posi��o da c�mera na miniatura
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, PAC_MAPA_LARGURA+1, 0, PAC_MAPA_ALTURA+1, -600, 600);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0, 0, 400);
	
	// Se est� morto, mover apenas os fantasmas

	mostrarMapa(map);
	if (morte) {
		mostrarPacman(pac, 0);
	} else {
		mostrarPacman(pac, interpolacao);
	}
	mostrarFantasmas(fant, interpolacao);
}

// Atualiza a posi��o da c�mera, centralizando-a no pac-man
void centralizarCamera(Pacman *pac, double interpolacao) {
	double posX = pac->atual->pos[X];
	double posY = pac->atual->pos[Y];
	double delta = min(1.0, pac->mov + pac->velocidade * interpolacao);
	
	// Aplica a interpola��o
	switch (pac->direcao) {
		case PAC_DIRECAO_CIMA:
			posY -= delta;
			break;
		case PAC_DIRECAO_ESQUERDA:
			posX -= delta;
			break;
		case PAC_DIRECAO_BAIXO:
			posY += delta;
			break;
		case PAC_DIRECAO_DIREITA:
			posX += delta;
			break;
	}
	
	// Desloca a c�mera
	glTranslated(-posX, -(PAC_MAPA_ALTURA - posY + 1), 0);
}

// Reinicia o mapa e os personagens
void reiniciarJogo() {
	pac->vidas = PAC_VIDAS;
	pac->pontos = 0;
	pac->fase = 1;
	reiniciarMapa(map, tBolinhas);
	reiniciar(pac, fant);
}

// Reinicia os personagens do jogo
void reiniciar(Pacman *pac, Fantasma *fant) {
	int i;
	
	// Reinicia o gerador de n�meros
	//srand(time(NULL));
	
	// Reinicia os fantasmas
	for (i = 0; i < PAC_FANTASMAS; i++) {
		fant[i].capturado = PAC_CAPTURA_PRISAO;
		fant[i].direcao = PAC_DIRECAO_CIMA;
		fant[i].mov = 0;
		fant[i].velocidade = 0;
		fant[i].tele = 0;
		fant[i].atual = fant[i].inicio;
		fant[i].destino = 0;
		fant[i].alvo = 0;
	}
	
	// Reinicia o pac-man
	pac->direcao = PAC_DIRECAO_DIREITA;
	pac->pre = PAC_DIRECAO_DIREITA;
	pac->boca = 0;
	pac->mov = 0;
	pac->velocidade = 0;
	pac->tele = 0;
	pac->atual = pac->inicio;
	pac->destino = 0;
}

// Altera o estado dos fantasmas
void mudarEstado(int tempo) {
	if (pause) {
		glutTimerFunc(PAC_TIMER, mudarEstado, tempo);
	} else if (tempo <= 0) {
		int i;
		// Espalhar
		if (estados % 2 == 0) {
			for (i = 0; i < PAC_FANTASMAS; i++)
				fant[i].estado = PAC_ESTADO_ESPALHAR;
			estados++;
			glutTimerFunc(PAC_TIMER, mudarEstado, 20000);
		// Perseguir
		} else {
			for (i = 0; i < PAC_FANTASMAS; i++)
				fant[i].estado = PAC_ESTADO_PERSEGUIR;
			if (estados < 6) {
				estados++;
				glutTimerFunc(PAC_TIMER, mudarEstado, 5000);
			}
		}
	} else {
		glutTimerFunc(PAC_TIMER, mudarEstado, tempo - PAC_TIMER);
	}
}

// Faz o jogador esperar antes de come�ar
void iniciou(int tempo) {
	if (pause) {
		glutTimerFunc(PAC_TIMER, iniciou, tempo);
	} else if (tempo <= 0) {
		// Come�a o jogo
		inicio = 0;
	} else {
		glutTimerFunc(PAC_TIMER, iniciou, tempo - PAC_TIMER);
	}
}

// Faz o jogador esperar antes de reiniciar
void morreu(int tempo) {
	if (pause) {
		glutTimerFunc(PAC_TIMER, morreu, tempo);
	} else if (tempo <= 0) {
		// Reinicia o pac-man
		pac->vidas--;
		pac->morto = 0;
		reiniciar(pac, fant);
		morte = 0;
		inicio = 1;
		
		// Se n�o tem mais vidas, volta ao menu
		if (pac->vidas <= 0)
			menu = 1;
	} else {
		glutTimerFunc(PAC_TIMER, morreu, tempo - PAC_TIMER);
	}
}

// Faz o jogador esperar antes de passar de fase
void passou(int tempo) {
	if (pause) {
		glutTimerFunc(PAC_TIMER, passou, tempo);
	} else if (tempo <= 0) {
		// Passa para a pr�xima fase
		pac->fase++;
		reiniciarMapa(map, tBolinhas);
		reiniciar(pac, fant);
		fim = 0;
		inicio = 1;
	} else {
		glutTimerFunc(PAC_TIMER, passou, tempo - PAC_TIMER);
	}
}

// Termina o poder especial obtido pelo pac-man
void terminarEspecial(int tempo) {
	if (pause) {
		glutTimerFunc(PAC_TIMER, terminarEspecial, tempo);
	} else if (tempo <= 0) {
		int i;
		// Se tiver mais de um ao mesmo tempo, continua
		if (especiais > 1) {
			especiais--;
		// Se tiver apenas um, acaba-o
		} else {
			for (i = 0; i < PAC_FANTASMAS; i++)
				if (fant[i].capturado == PAC_CAPTURA_AZUL)
					fant[i].capturado = PAC_CAPTURA_NORMAL;
			especiais = 0;
		}
	} else {
		glutTimerFunc(PAC_TIMER, terminarEspecial, tempo - PAC_TIMER);
	}
}

// Identifica a dire��o de movimento pelas setas do teclado
void mover(int tecla, int x, int y) {
	if (menu) {
		// Move o cursor do menu
		switch (tecla) {
			case GLUT_KEY_UP: // Move para cima
				opcao = (opcao - 1) % 4;
				if (opcao < 0) opcao += 4;
				break;
			case GLUT_KEY_DOWN: // Move para baixo
				opcao = (opcao + 1) % 4;
				break;
		}
	} else if (!pause) {
		// Move o pac-man
		switch (tecla) {
			case GLUT_KEY_UP: // Move para cima
				pac->pre = PAC_DIRECAO_CIMA;
				break;
			case GLUT_KEY_LEFT: // Move para a esquerda
				pac->pre = PAC_DIRECAO_ESQUERDA;
				break;
			case GLUT_KEY_DOWN: // Move para baixo
				pac->pre = PAC_DIRECAO_BAIXO;
				break;
			case GLUT_KEY_RIGHT: // Move para a direita
				pac->pre = PAC_DIRECAO_DIREITA;
				break;
		}
	}
}

// Identifica as teclas apertadas e realiza suas a��es
void teclado(unsigned char tecla, int x, int y) {
	switch (tecla) {
		// Enter
		case 13:
			switch (menu) {
				case 0: // No jogo
					pause = !pause;
					break;
				case 1: // Na tela inicial
					switch (opcao) {
						case 0: // Iniciar Jogo
							if (!pause)
								reiniciarJogo();
							pause = 0;
							menu = 0;
							break;
						case 1: // Sobre
							menu = 2;
							break;
						case 2: // Controles
							menu = 3;
							break;
						case 3: // Sair
							exit(0);
							break;
					}
					break;
				default: // Numa se��o do menu
					menu = 1;
					break;
			}
			break;
		// Esc
		case 27:
			pause = 1;
			menu = 1;
			break;
	}
}

// Captura as dimens�es da tela
void redimensionar(int w, int h) {
	telaW = w;
	telaH = h;
}

// Escreve texto na tela
void escrever(char *texto, int x, int y, int alinhamento, int selecionado) {
	int i;
	double posX;
	
	// Se o texto est� selecionado
	if (selecionado)
		glColor3f(1,0, 0);
	else
		glColor3f(1, 1, 1);
	
	// Define o alinhamento
	if (alinhamento < 0)
		posX = (double) -telaW/2 + x;
	else if (alinhamento > 0)
		posX = (double) telaW/2 - x - 9*strlen(texto);
	else
		posX = (double) -9*strlen(texto)/2 + x;
	glRasterPos3f(posX, (double) telaH/2 - y - 9, 600);
	
	// Escreve o texto
	for (i = 0; i < strlen(texto); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, texto[i]);
	}
}

// Abre um arquivo bitmap e carrega-o como textura
void carregarBitmap(char *arquivo, unsigned int id, int largura, int altura) {
	int i;
	unsigned char dados[(largura*altura+18)*3];
	unsigned char textura[largura*altura*3];
	unsigned char aux;
	FILE *fp;
	
	fp = fopen(arquivo, "r");
	if (!fp) return;
	fread(dados, (largura*altura+18)*3, 1, fp);
	fclose(fp);
	
	for (i = 18*3-1; i < (largura*altura+18)*3; i++)
		textura[i - 18*3] = dados[i];
	
	for (i = 0; i < largura*altura*3; i += 3) {
		aux = textura[i];
		textura[i] = textura[i+2];
		textura[i+2] = aux;
	}
	
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, largura, altura, 0, GL_RGB, GL_UNSIGNED_BYTE, textura);
}