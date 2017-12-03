Pacman *novo_pac();
void move_pac(Pacman *, Mapa *, Fantasma *);
void show_pac(Pacman *, double);
double speed_pac(Pacman *);
int e_caminho(Tile *);
int nova_direcao(Pacman *, Mapa *);
void is_live(Pacman *, Fantasma *);
void ganha_pontos(Pacman *, int, int *);
void hemisferio(double);

// Listas de exibição
unsigned int cima;
unsigned int baixo;

// Instancia o pac-man
Pacman *novo_pac() {
	Pacman *pac;
	GLUquadricObj *disco;
	
	// Instancia o pac-man
	pac = (Pacman *) malloc(sizeof(Pacman));
	if (!pac) return 0;
	
	pac->vidas = PAC_VIDAS;
	pac->pontos = 0;
	pac->fase = 0;
	pac->morto = 0;
	pac->capturados = 0;
	//direita
	pac->direcao = 3;
	pac->pre = 3;
	pac->boca = 0;
	pac->mov = 0;
	pac->velocidade = 0;

	pac->atual = 0;
	pac->destino = 0;
	pac->inicio = 0;
	
	disco = gluNewQuadric();
	gluQuadricDrawStyle(disco, GLU_FILL);
	
	cima = glGenLists(1);
	glNewList(cima, GL_COMPILE);
	hemisferio(0.6);
	glColor3f(1, 0, 0);
	gluPartialDisk(disco, 0, 0.55, 1, 1, 0, 360);
	glEndList();
	
	baixo = glGenLists(1);
	glNewList(baixo, GL_COMPILE);
	hemisferio(0.6);
	glColor3f(1, 0, 0);
	gluPartialDisk(disco, 0, 0.55, 30, 1, 90, 180);
	glEndList();
	
	return pac;
}

// Realiza um passo de movimento com o pac-man
void move_pac(Pacman *pac, Mapa *map, Fantasma *fant) {
	int i;
	
	// Tenta mudar para a direção desejada
	if (nova_direcao(pac, map))
		pac->direcao = pac->pre;
	
	// Pega o próximo tile
	pac->destino = proximoTile(map, pac->atual, pac->direcao);
	// Verifica se pode mover-se para ele
	if (e_caminho(pac->destino)) {
		// Anda
		pac->velocidade = speed_pac(pac);
		// Verifica se completou o movimento
		if (pac->mov < 1) {//Esse if tem a função também de abrir a boca do pacman na porção certa no momento exato
			/*Quando esse "mov" atingir 1 é por que um quadrado todo foi percorrido pelo PacMan
			e, se esse quadrado estiver preenchido com uma bolinha(comida), o else abaixo fica responsável por
			atualizar a pontuação
			*/
			// Dá um passo
			pac->mov += pac->velocidade;
			pac->boca = PAC_BOCA_MAX * pac->mov;
		} else {
			// Atualiza a pontuação
			if (pac->destino->bolinha == PAC_BOLINHA_NORMAL || pac->destino->bolinha == PAC_BOLINHA_ESPECIAL)
				map->bolinhas--;
			switch (pac->destino->bolinha){
				case PAC_BOLINHA_NORMAL:
					ganha_pontos(pac, 10, &maiorPontuacao);
					break;
				case PAC_BOLINHA_ESPECIAL:
					ganha_pontos(pac, 50, &maiorPontuacao);
					pac->capturados = 0;
					for (i = 0; i < PAC_FANTASMAS; i++)
						if (fant[i].capturado == PAC_CAPTURA_NORMAL)
							fant[i].capturado = PAC_CAPTURA_AZUL;
					if (pac->fase < 11) {/*A cada fase a quantidade de bolinhas especiais do PacMan vai aumentando.
											A partir da 11ª fase essa quantidade fica constante*/
						especiais++;
						glutTimerFunc(PAC_TIMER, terminarEspecial, (11-pac->fase)*1000);
					}
					break;
			}
		
			pac->destino->bolinha = 3;
			
			// Atualiza a posição
			pac->atual = pac->destino;//a posição atual do PacMan é o seu destino
			pac->boca = 0;//Abertura da boca
			pac->mov = 0;
		}
	} else {
		// Pára
		pac->velocidade = 0;
	}
}

// Exibe o Pacman
void show_pac(Pacman *pac, double interpolacao) {
	double posX = pac->atual->pos[X];
	double posY = pac->atual->pos[Y];
	double delta = min(1.0, pac->mov + pac->velocidade * interpolacao);
	
	switch (pac->direcao) {
		case 0:
			posY -= delta;
			break;
		case 1:
			posX -= delta;
			break;
		case 2:
			posY += delta;
			break;
		case 3:
			posX += delta;
			break;
	}
	
	glPushMatrix();
	
	glTranslated(posX + 1, PAC_MAPA_ALTURA - posY, 0.6);
	glRotated(90 * pac->direcao, 0, 0, 1);
	
	glPushMatrix();
	glRotated(pac->boca, 1, 0, 0);
	glCallList(cima);
	glPopMatrix();
	
	glPushMatrix();
	glRotated(-pac->boca - 180, 1, 0, 0);
	glCallList(baixo);
	glPopMatrix();
	
	glPopMatrix();
}

// Retorna a velocidade do pac-man
double speed_pac(Pacman *pac) {
	//a cada fase o fantasma tem novas velocidades
	/*Na primeira fase ele é bem mais lento que o pacman. Na segunda essa diferença cai. Na terceira
	as velocidades de todos se igualam, e a partir da 4 fase o fantasma fica mais rápido que o
	Pacman, aumentando a velocidade. As definições das velocidades podem ser vistas no cabeçalho defines.h
	*/
	if (pac->fase <= 1)
		return PAC_VELOCIDADE_PACMAN_A;
	else if (pac->fase <= 2)
		return PAC_VELOCIDADE_PACMAN_B;
	else if (pac->fase <= 3)
		return PAC_VELOCIDADE_PACMAN_C;
	else
		return PAC_VELOCIDADE_PACMAN_D;
}

// Verifica se é permitido passar sobre este tile
int e_caminho(Tile *tile) {
	return tile != 0
		   && tile->tipo != PAC_PAREDE//Se não for parede
		   && tile->tipo != Fantasma1_pac//Se não for fantasma
		   && tile->tipo != Fantasma2_pac//Se não for fantasma
		   && tile->tipo != Fantasma3_pac//Se não for fantasma
		   && tile->tipo != Fantasma4_pac//Se não for fantasma
		   && tile->tipo != PAC_NADA;//Se a parte for pertencente ao mapa
}

// Verifica se é permitido mudar de direção
int nova_direcao(Pacman *pac, Mapa *map) {
	Tile *destino;
	
	destino = proximoTile(map, pac->atual, pac->pre);
	return pac->pre != pac->direcao
		   && pac->mov == 0 //Se Já se passou um quadrado
		   && destino != 0 
		   && e_caminho(destino);//Se é caminho permitido
		  
}

// Verifica se o pac-man está no mesmo quadrado que um fantasma
void is_live(Pacman *pac, Fantasma *fant) {
	int i;
	for (i = 0; i < PAC_FANTASMAS; i++) {
		if (pac->atual == fant[i].atual) {
			switch (fant[i].capturado) {
				case PAC_CAPTURA_NORMAL:
					// Se o pac-man foi morto
					pac->morto = 1;
					break;
				case PAC_CAPTURA_AZUL:
					// Se o pac-man capturou um fantasma
					fant[i].capturado = PAC_CAPTURA_CAPTURADO;
					ganha_pontos(pac, 200 << pac->capturados, &maiorPontuacao);
					pac->capturados++;
					break;
			}
		}
	}
}

// Aumenta a pontuação
void ganha_pontos(Pacman *pac, int pontos, int *recorde) {
	pac->pontos += pontos;
	if (pac->pontos % 10000 < pontos)
		pac->vidas++;
	if (pac->pontos > *recorde)
		*recorde = pac->pontos;
}

// Desenha um hemisfério
void hemisferio(double raio) {
	double eqn[4] = {0.0, 0.0, 1.0, 0.0};
	glPushMatrix();
	glClipPlane(GL_CLIP_PLANE0, eqn);
	glEnable(GL_CLIP_PLANE0);
	glColor3f(1, 1, 0);
	glutSolidSphere(raio, 30, 30);
	glDisable(GL_CLIP_PLANE0);
	glPopMatrix();
}