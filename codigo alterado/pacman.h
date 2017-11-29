Pacman *criaPacman();
void movePacman(Pacman *, Mapa *, Fantasma *);
void mostrarPacman(Pacman *, double);
double velocidadePacman(Pacman *);
int pacmanPodeAndarSobre(Tile *);
int pacmanPodeMudarDirecao(Pacman *, Mapa *);
void estaVivo(Pacman *, Fantasma *);
void ganharPontos(Pacman *, int, int *);
void hemisferio(double);

// Listas de exibi��o
unsigned int cima;
unsigned int baixo;

// Instancia o pac-man
Pacman *criaPacman() {
	Pacman *pac;
	GLUquadricObj *disco;
	
	// Instancia o pac-man
	pac = (Pacman *) malloc(sizeof(Pacman));
	if (!pac) return 0;
	
	pac->vidas = PAC_VIDAS;
	pac->pontos = 0;
	pac->fase = 1;
	pac->morto = 0;
	pac->capturados = 0;
	pac->direcao = PAC_DIRECAO_DIREITA;
	pac->pre = PAC_DIRECAO_DIREITA;
	pac->boca = 0;
	pac->mov = 0;
	pac->velocidade = 0;
	pac->tele = 0;
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
void movePacman(Pacman *pac, Mapa *map, Fantasma *fant) {
	int i;
	
	// Tenta mudar para a dire��o desejada
	if (pacmanPodeMudarDirecao(pac, map))
		pac->direcao = pac->pre;
	
	// Pega o pr�ximo tile
	pac->destino = proximoTile(map, pac->atual, pac->direcao, pac->tele);
	// Verifica se pode mover-se para ele
	if (pacmanPodeAndarSobre(pac->destino)) {
		// Anda
		pac->velocidade = velocidadePacman(pac);
		// Verifica se completou o movimento
		if (pac->mov < 1) {
			// D� um passo
			pac->mov += pac->velocidade;
			pac->boca = PAC_BOCA_MAX * pac->mov;
		} else {
			// Atualiza a pontua��o
			if (pac->destino->bolinha != PAC_BOLINHA_NENHUMA)
				map->bolinhas--;
			switch (pac->destino->bolinha) {
				case PAC_BOLINHA_NORMAL:
					ganharPontos(pac, 10, &maiorPontuacao);
					break;
				case PAC_BOLINHA_ESPECIAL:
					ganharPontos(pac, 50, &maiorPontuacao);
					pac->capturados = 0;
					for (i = 0; i < PAC_FANTASMAS; i++)
						if (fant[i].capturado == PAC_CAPTURA_NORMAL)
							fant[i].capturado = PAC_CAPTURA_AZUL;
					if (pac->fase < 11) {
						especiais++;
						glutTimerFunc(PAC_TIMER, terminarEspecial, (11-pac->fase)*1000);
					}
					break;
			}
		//	pac->destino->bolinha = PAC_BOLINHA_NENHUMA;
			pac->destino->bolinha = 3;
			
			// Atualiza a posi��o
			if (pac->destino->tele) pac->tele = !pac->tele;
			pac->atual = pac->destino;
			pac->boca = 0;
			pac->mov = 0;
		}
		
	
	} else {
		// P�ra
		pac->velocidade = 0;
	}
}

// Exibe o Pacman
void mostrarPacman(Pacman *pac, double interpolacao) {
	double posX = pac->atual->pos[X];
	double posY = pac->atual->pos[Y];
	double delta = min(1.0, pac->mov + pac->velocidade * interpolacao);
	
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
double velocidadePacman(Pacman *pac) {
	if (pac->fase <= 5)
		return PAC_VELOCIDADE_PACMAN_A;
	else if (pac->fase <= 10)
		return PAC_VELOCIDADE_PACMAN_B;
	else if (pac->fase <= 20)
		return PAC_VELOCIDADE_PACMAN_C;
	else
		return PAC_VELOCIDADE_PACMAN_D;
}

// Verifica se � permitido passar sobre este tile
int pacmanPodeAndarSobre(Tile *tile) {
	return tile != 0
		   && tile->tipo != PAC_PAREDE
		   && tile->tipo != PAC_BLINKY
		   && tile->tipo != PAC_PINKY
		   && tile->tipo != PAC_INKY
		   && tile->tipo != PAC_CLYDE
		   && tile->tipo != PAC_NADA;
}

// Verifica se � permitido mudar de dire��o
int pacmanPodeMudarDirecao(Pacman *pac, Mapa *map) {
	Tile *destino;
	
	destino = proximoTile(map, pac->atual, pac->pre, pac->tele);
	return pac->pre != pac->direcao
		   && pac->mov == 0
		   && destino != 0
		   && pacmanPodeAndarSobre(destino)
		   && !destino->tele;
}

// Verifica se o pac-man est� no mesmo quadrado que um fantasma
void estaVivo(Pacman *pac, Fantasma *fant) {
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
					ganharPontos(pac, 200 << pac->capturados, &maiorPontuacao);
					pac->capturados++;
					break;
			}
		}
	}
}

// Aumenta a pontua��o
void ganharPontos(Pacman *pac, int pontos, int *recorde) {
	pac->pontos += pontos;
	if (pac->pontos % 10000 < pontos)
		pac->vidas++;
	if (pac->pontos > *recorde)
		*recorde = pac->pontos;
}

// Desenha um hemisf�rio
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