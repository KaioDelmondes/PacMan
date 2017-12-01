//Bibliotecas utilizadas no programa
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mmsystem.h>
#include <windows.h>


//Cabeçalhos importados
#include "defines.h"
#include "struct.h"
#include "matrizes.h"
#include "funcoes.h"

#include "mapa.h"
#include "pacman.h"
#include "fantasma.h"


//unsigned é usado para retornar valores sempre positivos
unsigned int tempoInicial;
unsigned int tempoAtual;
int passos;
int frame = 1000 / PAC_FPS;
double interpolacao;

// Variáveis do tamanho da tela
int Largura = 900;
int Altura = 600;

// Variáveis do jogo
Pacman *pac; //declara uma variavel pacman
Fantasma *ghost;//declara uma variavel fantasma
Mapa *map; // declara uma variavel fantasma
char pontuacao[8]; //variaveis adicionais
char vidas[8]; //variaveis adicionais
char nivel[8];//variaveis adicionais
char pontuacaoRecorde[8]; //variaveis adicionais

int menu = 1, op = 0, pause = 0, inicio = 1, morto = 0, fim = 0;

int main()
{
	// Configura a janela
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(Largura, Altura);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Jogo do Pac-Man 3D");

	// Inicializa as variáveis
	inicializacao();

	//Funções ativadas no decorrer da execução
	glutDisplayFunc(constroiTela);
	glutSpecialFunc(moveCursor);
	glutKeyboardFunc(teclado);
	glutReshapeFunc(redimensionar);//muda tamanho da tela

	glutMainLoop();
}

void inicializacao()
{
	// Cria os objetos do jogo
	pac = novo_pac();
	ghost = novo_ghost();
	//no mapa são carregadas duas matrizes uma com a posição dos elementos e a outa com a
	//opcao das bolinhas
	map = criaMapa(MPosicao, PBolinhas, pac, ghost);

	// Marca o tempo inicial em milisegundos
	tempoInicial = glutGet(GLUT_ELAPSED_TIME);

	// Carrega fundo da tela de apresentação
	//carregarBitmap("pacImg1.bmp", 1, 296, 170); //imgem 2
	carregarBitmap("pacImg2.bmp", 1, 700, 420);

	//uso de buffer
	glEnable(GL_DEPTH_TEST);
	glDrawBuffer(GL_BACK);

	// Inicia o gerador de números
	//srand(tempoInicial);
	srand(tempoInicial);
	PlaySound("Pacman2.wav", NULL, SND_ASYNC|SND_FILENAME|SND_LOOP);
	glutTimerFunc(PAC_TIMER, mudarEstadoFantasma, 5000);
}

// Exibe a imagem na tela
void constroiTela()
{
	// Limpa a tela e todos os bits
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Determina o tamanho da área de visualização
	glMatrixMode(GL_VIEWPORT);
	glLoadIdentity();
	glViewport(0, 0, Largura, Altura);

	//posiciona tela
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((double) - Largura / 2, (double) Largura / 2, (double) - Altura / 2, (double) Altura / 2, -2000, 2000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (menu)
		// Constroi menu principal
		Menu();
	else
	{
		// Constroi tela do jogo
		atualizarJogo();
		mostrarJogo();
	}

	glutSwapBuffers();
	glutPostRedisplay();
}

// Exibe imagem de fundo
void montaFundo()
{
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f((double) - Largura / 2, (double) - Altura / 2, 0);
	glTexCoord2f(0, 1);
	glVertex3f((double) - Largura / 2, (double) Altura / 2, 0);
	glTexCoord2f(1, 1);
	glVertex3f((double) Largura / 2, (double) Altura / 2, 0);
	glTexCoord2f(1, 0);
	glVertex3f((double) Largura / 2, (double) - Altura / 2, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}
// Monta o menu do jogo
void Menu()
{
	char item[4][20];
	char sobre[3][30];
	montaFundo();
	// Imprimi o texto
	switch (menu)
	{
	case 1:
		if (pause)
		{
			//Submenu
			strcpy(item[0], "Continuar Jogo");
			strcpy(item[1], "Sobre");
			strcpy(item[2], "Sair");
			strcpy(item[3], "Menu inicial");
			for (int i = 0; i < 4; i++)
			{
				double yy = (double) Altura / 2 + 25 * i + 40;
				escreveTela(item[i], 0, yy, 0, op == i);
			}
		}
		else
		{
			//Menu principal
			strcpy(item[0], "Iniciar Jogo");
			strcpy(item[1], "Sobre");
			strcpy(item[2], "Sair");
			for (int i = 0; i < 3; i++)
			{
				double yy = (double) Altura / 2 + 25 * i + 40;
				escreveTela(item[i], 0, yy, 0, op == i);
			}
		}
		break;

	case 2: // Sobre o jogo
		strcpy(sobre[0], "Autores: Elineide e Kaio");
		strcpy(sobre[1], "Setas: Movimentacao do PacMan");
		strcpy(sobre[2], "Enter: Continua (ou pausa) o jogo");
		strcpy(sobre[3], "Esc: voltar");

		for (int i = 0; i < 4; i++)
		{
			double xx = (double) Largura / 2 - 30 * 8;
			double yy = (double) Altura / 2 + 20 * (i - 0.5);
			escreveTela(sobre[i], xx , yy, -1, 0);
		}
		break;
	}
}

// Atualiza o jogo a cada passo
void atualizarJogo()
{
	tempoAtual = glutGet(GLUT_ELAPSED_TIME);
	passos = 0;
	if (!pause)
	{
		// Atualiza o jogo, se não estiver pausado

		while ((tempoAtual - tempoInicial) > frame && (passos < PAC_PULOS_MAX))
		{
			// Verifica se ainda restam bolinhas
			if (map->bolinhas)
			{
				// Se está começando a fase, espera
				if (inicio)
				{
					pac->boca = 30;
					glutTimerFunc(PAC_TIMER, iniciou, 2000);
				}
				//se continua o jogo, verifica se o pac já morreu
				else if (pac->morto && !morto)
				{
					morto = 1;
					glutTimerFunc(PAC_TIMER, morreu, 2000);

				}
				//se o pac não está morto e nem terminou então move pacman
				else if (!pac->morto && !fim)
				{
					move_pac(pac, map, ghost);
					is_live(pac, ghost);
				}

				// Move os fantasmas
				if (!inicio && !fim)
					move_ghost(ghost, map, pac);
			}
			else
			{
				map->bolinhas = -1;
				fim = 1;
				niveis = pac->fase;
				glutTimerFunc(PAC_TIMER, passarFase, 2000);
			}

			tempoInicial += frame;
			tempoAtual = glutGet(GLUT_ELAPSED_TIME);
			passos++;
		}
		if ((tempoAtual - tempoInicial) > frame)
			tempoInicial = tempoAtual - frame;

		// Calcula a interpolação que deve ser usada
		interpolacao = min(1.0, (double) (tempoAtual - tempoInicial) / frame);
	}
	else
	{
		interpolacao = 0;
	}
}

// Monta a cena do jogo de forma atualizada
void mostrarJogo()
{


	// Exibe a pontuação atual e o recorde
	itoa(pac->pontos, pontuacao, 10);
	itoa(pac->vidas, vidas, 10);
	itoa(maiorPontuacao, pontuacaoRecorde, 10);
	itoa(niveis, nivel, 10);
	escreveTela("Pontos", 20, 20, -1, 0);
	escreveTela(pontuacao, 20 + (10 - strlen(pontuacao)) * 9, 35, -1, 0);
	escreveTela("Vidas", 20 + 12 * 9, 20, -1, 0);
	escreveTela(vidas, 20 + 12 * 9 + (10 - strlen(vidas)) * 9, 35, -1, 0);
	itoa(niveis, nivel, 10);
	escreveTela("Nivel", 0 + 3 * 9, 15, 0, 0);
	escreveTela(nivel, 10, 35, 0, 0);
	escreveTela("pontuacaoRecorde", 20 + 3 * 9, 20, 1, 0);
	escreveTela(pontuacaoRecorde, 20, 35, 1, 0);

	// Exibe mensagem quando pausado
	if (pause)
		escreveTela("Pausado", 0, Altura / 2 - 50, 0, 0);

	// Posição câmera
	glRotated(35, 1, 0, 0);
	glRotated(-45, 0, 1, 0);
	glRotated(-90, 1, 0, 0);
	glScaled(PAC_TAMANHO_MAPA, PAC_TAMANHO_MAPA, PAC_TAMANHO_MAPA);

	// Se está morto, mover apenas os fantasmas
	if (morto || fim)
	{
		posicionarCamera(pac, 0);
		mostrarMapa(map);
		show_pac(pac, 0);

		// Se passou de fase, não move os fantasmas
		if (fim)
			show_ghost(ghost, 0);
		else
			show_ghost(ghost, interpolacao);
	}
	else
	{
		posicionarCamera(pac, interpolacao);
		mostrarMapa(map);
		show_pac(pac, interpolacao);
		show_ghost(ghost, interpolacao);
	}
//-------------------------- Monta miniatura-------------------------------
	// Miniatura
	glMatrixMode(GL_VIEWPORT);
	glLoadIdentity();
	glViewport(10, 10, (PAC_MAPA_LARGURA + 1)*PAC_TAMANHO_MINIATURA, (PAC_MAPA_ALTURA + 1)*PAC_TAMANHO_MINIATURA);

	// Posição da câmera em miniatura
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, PAC_MAPA_LARGURA + 1, 0, PAC_MAPA_ALTURA + 1, -600, 600);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0, 0, 400);
//------------------------------------------------------------------------

	mostrarMapa(map);
	if (morto)
		show_pac(pac, 0); //sem mover pac
	else
		show_pac(pac, interpolacao); //mostra pac com movmentação

	show_ghost(ghost, interpolacao); //fantasmas sempre se movem
}

// Atualiza a posição da câmera, centralizando-a no pac-man
void posicionarCamera(Pacman *pac, double interpolacao)
{
	double posX = pac->atual->pos[X];
	double posY = pac->atual->pos[Y];
	double delta = min(1.0, pac->mov + pac->velocidade * interpolacao);

	// Aplica a interpolação
	switch (pac->direcao)
	{
		//para cima
	case 0:
		posY -= delta;
		break;
		//para esquerda
	case 1:
		posX -= delta;
		break;
		//para baixo
	case 2:
		posY += delta;
		break;
		//para direita
	case 3:
		posX += delta;
		break;
	}

	// Desloca a câmera
	glTranslated(-posX, -(PAC_MAPA_ALTURA - posY + 1), 0);
}

// Reinicia o mapa e os personagens
void reiniciaJogo()
{
	pac->vidas = PAC_VIDAS;
	pac->pontos = 0;
	pac->fase = 1;
	reiniciarMapa(map, PBolinhas);
	reiniciar(pac, ghost);
}

// Reinicia os personagens do jogo
void reiniciar(Pacman *pac, Fantasma *ghost)
{
	int i;

	// Reinicia o gerador de números
	//srand(time(NULL));

	// Reinicia os fantasmas
	for (i = 0; i < PAC_FANTASMAS; i++)
	{
		ghost[i].capturado = PAC_CAPTURA_NORMAL;
		ghost[i].direcao = 0;
		ghost[i].mov = 0;
		ghost[i].velocidade = 0;
		ghost[i].tele = 0;
		ghost[i].atual = ghost[i].inicio;
		ghost[i].destino = 0;
		ghost[i].alvo = 0;
	}

	// Reinicia o pac-man
	pac->direcao = 3;
	pac->pre = 3;
	pac->boca = 0;
	pac->mov = 0;
	pac->velocidade = 0;
	pac->tele = 0;
	pac->atual = pac->inicio;
	pac->destino = 0;
}

// Altera o estado dos fantasmas
void mudarEstadoFantasma(int tempo)
{
	if (pause)
	{
		glutTimerFunc(PAC_TIMER, mudarEstadoFantasma, tempo);
	}
	else if (tempo <= 0)
	{
		int i;
		// Espalhar
		if (estados % 2 == 0)
		{
			for (i = 0; i < PAC_FANTASMAS; i++)
				ghost[i].estado = PAC_ESTADO_ESPALHAR;
			estados++;
			glutTimerFunc(PAC_TIMER, mudarEstadoFantasma, 20000);
			// Perseguir
		}
		else
		{
			for (i = 0; i < PAC_FANTASMAS; i++)
				ghost[i].estado = PAC_ESTADO_PERSEGUIR;
			if (estados < 6)
			{
				estados++;
				glutTimerFunc(PAC_TIMER, mudarEstadoFantasma, 5000);
			}
		}
	}
	else
	{
		glutTimerFunc(PAC_TIMER, mudarEstadoFantasma, tempo - PAC_TIMER);
	}
}

// Faz o jogador esperar antes de começar
void iniciou(int tempo)
{
	if (pause)
	{
		glutTimerFunc(PAC_TIMER, iniciou, tempo);
	}
	else if (tempo <= 0)
	{
		// Começa o jogo
		inicio = 0;
	}
	else
	{
		glutTimerFunc(PAC_TIMER, iniciou, tempo - PAC_TIMER);
	}
}

// Faz o jogador esperar antes de reiniciar
void morreu(int tempo)
{
	if (pause)
	{
		glutTimerFunc(PAC_TIMER, morreu, tempo);
	}
	else if (tempo <= 0)
	{
		// Reinicia o pac-man
		pac->vidas--;
		pac->morto = 0;
		reiniciar(pac, ghost);
		morto = 0;
		inicio = 1;

		// Se não tem mais vidas, volta ao menu
		if (pac->vidas <= 0)
			menu = 1;
	}
	else
	{
		glutTimerFunc(PAC_TIMER, morreu, tempo - PAC_TIMER);
	}
}

// Faz o jogador esperar antes de passar de fase
void passarFase(int tempo)
{
	if (pause)
	{
		glutTimerFunc(PAC_TIMER, passarFase, tempo);
	}
	else if (tempo <= 0)
	{
		// Passa para a próxima fase
		pac->fase++;
		reiniciarMapa(map, PBolinhas);
		reiniciar(pac, ghost);
		fim = 0;
		inicio = 1;
	}
	else
	{
		glutTimerFunc(PAC_TIMER, passarFase, tempo - PAC_TIMER);
	}
}

// Termina o poder especial obtido pelo pac-man
void terminarEspecial(int tempo)
{
	if (pause)
	{
		glutTimerFunc(PAC_TIMER, terminarEspecial, tempo);
	}
	else if (tempo <= 0)
	{
		int i;
		// Se tiver mais de um ao mesmo tempo, continua
		if (especiais > 1)
		{
			especiais--;
			// Se tiver apenas um, acaba-o
		}
		else
		{
			for (i = 0; i < PAC_FANTASMAS; i++)
				if (ghost[i].capturado == PAC_CAPTURA_AZUL)
					ghost[i].capturado = PAC_CAPTURA_NORMAL;
			especiais = 0;
		}
	}
	else
	{
		glutTimerFunc(PAC_TIMER, terminarEspecial, tempo - PAC_TIMER);
	}
}

// Coloca ações em algumas teclas do teclado
void teclado(unsigned char tecla, int x, int y)
{
	switch (tecla)
	{
		// Enter
	case 13:
		switch (menu)
		{
		case 0: // No jogo
			pause = !pause;
			break;
		case 1: // Na tela inicial
			switch (op)
			{
			case 0: // Iniciar Jogo
				if (!pause)
					reiniciaJogo();
				pause = 0;
				menu = 0;
				break;
			case 1: // Sobre
				menu = 2;
				break;
			case 2: // Sair
				exit(0);
				break;

			case 3: // Voltar ao menu Inicial
				menu = 1;
				pause = 0;
				break;
			}
			break;
		default:
			menu = 1;
			break;
		}
		break;
		//  função do Esc
	case 27:
		//pause = 1;
		menu = 1;
		break;
	}
}

// movimentação das teclas do teclado
void moveCursor(int tecla, int x, int y)
{
	//se estiver na tela menuas opcoes são para cima e para baixo
	if (menu)
	{
		switch (tecla)
		{
		case GLUT_KEY_UP: // Move para cima
			op = (op - 1) % 4;
			if (op < 0) op += 4;
			break;
		case GLUT_KEY_DOWN: // Move para baixo
			op = (op + 1) % 4;
			break;
		}
	}
	//se não vai está na tela do jogo e diferente de parado
	else if (!pause)
	{
		// Movimentação do pac-man
		switch (tecla)
		{
		case GLUT_KEY_UP: // Move para cima
			pac->pre = 0;
			break;
		case GLUT_KEY_DOWN: // Move para baixo
			pac->pre = 2;
			break;
		case GLUT_KEY_LEFT: // Move para a esquerda
			pac->pre = 1;
			break;
		case GLUT_KEY_RIGHT: // Move para a direita
			pac->pre = 3;
			break;
		}
	}
}
//Dimensões da tela
void redimensionar(int w, int h)
{
	Largura = w;
	Altura = h;
}

// Escreve texto na tela
void escreveTela(char *texto, int x, int y, int alinhamento, int selecionado)
{
	int i;
	double posX;

	// Se o texto está selecionado
	if (selecionado)
		glColor3f(1, 0, 0);
	else
		glColor3f(1, 1, 1);

	// Define o alinhamento
	if (alinhamento < 0)
		posX = (double) - Largura / 2 + x;
	else if (alinhamento > 0)
		posX = (double) Largura / 2 - x - 9 * strlen(texto);
	else
		posX = (double) - 9 * strlen(texto) / 2 + x;
	glRasterPos3f(posX, (double) Altura / 2 - y - 9, 600);

	//Define as posiçoes x,y,z na tela
	glRasterPos3f(posX, (double) Altura / 2 - y - 9, 600);

	// Escreve o texto
	int size = strlen(texto);
	for (i = 0; i < size; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, texto[i]);
	}
}

// Abre um arquivo bitmap e carrega como uma  textura
void carregarBitmap(char *arquivo, unsigned int id, int largura, int altura)
{
	//unsigned utilizado para retornar apenas valores positivos
	unsigned char dados[(largura * altura + 18) * 3];
	unsigned char textura[largura * altura * 3];
	unsigned char aux;
	//criando arquivo
	FILE *arquAux;

	arquAux = fopen(arquivo, "r");
	if (!arquAux) return;
	fread(dados, (largura * altura + 18) * 3, 1, arquAux);
	fclose(arquAux);

	//aplicação de mais detalhes a imagem
	for (int i = 18 * 3 - 1; i < (largura * altura + 18) * 3; i++)
		textura[i - 18 * 3] = dados[i];

	for (int i = 0; i < largura * altura * 3; i += 3)
	{
		aux = textura[i];
		textura[i] = textura[i + 2];
		textura[i + 2] = aux;
	}

	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, largura, altura, 0, GL_RGB, GL_UNSIGNED_BYTE, textura);
}
