Fantasma *novo_ghost();
void move_ghost(Fantasma *, Mapa *, Pacman *);
void show_ghost(Fantasma *, double);
double speed_ghost(Fantasma, Pacman *);
Tile *novo_alvo(Fantasma, Mapa *, Pacman *, Fantasma *);
int e_caminho(Fantasma, Tile *);
int fantasmaPodeMudarDirecao(Fantasma, Mapa *);
int mehor_escolha(Fantasma, Mapa *, Pacman *);

// Listas de Exibi��o
unsigned int corpo;
unsigned int olhos;

// Instancia os fantasmas
Fantasma *novo_ghost() {
	int i;
	Fantasma *fant;
	GLUquadricObj *cilindro;
	
	fant = (Fantasma *) malloc(PAC_FANTASMAS * sizeof(Fantasma));
	if (!fant) return 0;
	
	for (i = 0; i < PAC_FANTASMAS; i++) {
		fant[i].nome = i;
		fant[i].estado = PAC_ESTADO_ESPALHAR;
		fant[i].capturado = PAC_CAPTURA_PRISAO;
		fant[i].direcao = 0;
		fant[i].mov = 0;
		fant[i].velocidade = 0;
		fant[i].tele = 0;
		fant[i].atual = 0;
		fant[i].destino = 0;
		fant[i].inicio = 0;
		fant[i].alvo = 0;
	}
	
	cilindro = gluNewQuadric();
	gluQuadricDrawStyle(cilindro, GLU_FILL);
	
	corpo = glGenLists(1);
	glNewList(corpo, GL_COMPILE);
	gluCylinder(cilindro, 0.6, 0.6, 0.6, 30, 1);
	glTranslated(0, 0, 0.6);
	glutSolidSphere(0.6, 30, 30);
	glEndList();
	
	olhos = glGenLists(1);
	glNewList(olhos, GL_COMPILE);
	glColor3f(1, 1, 1);
	glTranslated(0.275, 0.5, 0.65);
	glScaled(1, 0.5, 1.75);
	glutSolidSphere(0.15, 30, 30);
	glColor3f(0, 0, 1);
	glTranslated(0, 0.1, 0);
	glutSolidSphere(0.08, 30, 30);
	glPopMatrix();
	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslated(-0.275, 0.5, 0.65);
	glScaled(1, 0.5, 1.75);
	glutSolidSphere(0.15, 30, 30);
	glColor3f(0, 0, 1);
	glTranslated(0, 0.1, 0);
	glutSolidSphere(0.08, 30, 30);
	glEndList();
	
	return fant;
}

// Realiza um passo de movimento com o fantasma
void move_ghost(Fantasma *fant, Mapa *map, Pacman *pac) {
	int i;
	
	for (i = 0; i < PAC_FANTASMAS; i++) {//Para cada Fantasma
		// Atualiza o alvo -- Pode ir pra pris�o, ca�ar o pacman ou fugir dele
		fant[i].alvo = novo_alvo(fant[i], map, pac, fant);
		// Pega o pr�ximo tile -- se � o quadrado acima, abaixo, a direita ou a esquerda
		fant[i].destino = proximoTile(map, fant[i].atual, fant[i].direcao, fant[i].tele);
		// Verifica se pode mover-se para ele -- Pode ser uma parede
		if (e_caminho(fant[i], fant[i].destino)) { // Se � permitido ir para o novo destino
			// Anda
			fant[i].velocidade = speed_ghost(fant[i], pac);
			// Verifica se completou o movimento
			if (fant[i].mov < 1) {
				// D� um passo
				fant[i].mov += fant[i].velocidade;
			} else {//se n�o completou o movimento
				// Permite ser capturado quando sai da pris�o
				if (fant[i].capturado == PAC_CAPTURA_PRISAO
					&& !ePrisao(fant[i].destino))
					fant[i].capturado = PAC_CAPTURA_NORMAL;
				// Sai da pris�o quando volta da captura
				else if (fant[i].capturado == PAC_CAPTURA_CAPTURADO
						 && fant[i].destino == fant[i].inicio)
					fant[i].capturado = PAC_CAPTURA_PRISAO;
				
				// Atualiza a posi��o -- A cada passo que o fantasma d�, � parado o seu movimento e calcu
				//lada a melhor dire��o para ele mover-se para que na pr�xima itera��o ele possa mudar de
				//dire��o
				if (fant[i].destino->tele)
					fant[i].tele = !fant[i].tele;
				fant[i].atual = fant[i].destino; //atualiza a posi��o do fantasma
				fant[i].mov = 0; //Para o movimento para que na proxima itera��o seja recalculado
				fant[i].direcao = mehor_escolha(fant[i], map, pac);//calcula a melhor dire��o do prox. passo
			}
		} else {
			// P�ra -- se n�o puder passar sobre, o fantasma p�ra
			fant[i].velocidade = 0;
			fant[i].direcao = mehor_escolha(fant[i], map, pac);//calcula a melhor dire��o para o movimento
																//da proxima itera��o
		}
	}
}

// Exibe os fantasmas
void show_ghost(Fantasma *fant, double interpolacao) {
	int i;
	
	for (i = 0; i < PAC_FANTASMAS; i++) {//Posi��o de cada fantasma no mapa
		double posX = fant[i].atual->pos[X];
		double posY = fant[i].atual->pos[Y];
		double delta = min(1.0, fant[i].mov + fant[i].velocidade * interpolacao);
		
		switch (fant[i].direcao) {
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
		
		switch (fant[i].nome) {
			case Fantasma1:
				glColor3f(1, 0, 0);
				break;
			case Fantasma2:
				glColor3f(1, 0.75, 1);
				break;
			case Fantasma3:
				glColor3f(0, 1, 1);
				break;
			case Fantasma4:
				glColor3f(1, 0.75, 0.3);
				break;
		}
		
		glPushMatrix();
		
		if (fant[i].capturado == PAC_CAPTURA_AZUL)
			glColor3f(0, 0, 1);
		
		glTranslated(posX + 1, PAC_MAPA_ALTURA - posY, 0);
		glRotated(90 * fant[i].direcao, 0, 0, 1);
		
		if (fant[i].capturado != PAC_CAPTURA_CAPTURADO) {
			glPushMatrix();
			glCallList(corpo);
			glPopMatrix();
		}
		
		glPushMatrix();
		glCallList(olhos);
		glPopMatrix();
		
		glPopMatrix();
	}
}

// Retorna a velocidade do fantasma
double speed_ghost(Fantasma fant, Pacman *pac) {//a cada fase o fantasma tem novas velocidades
	/*Na primeira fase ele � bem mais lento que o pacman. Na segunda essa diferen�a cai. Na terceira
	as velocidades de todos se igualam, e a partir da 4 fase o fantasma fica mais r�pido que o
	Pacman, aumentando a velocidade. As defini��es das velocidades podem ser vistas no cabe�alho defines.h
	*/
	if (pac->fase <= 1)
		return PAC_VELOCIDADE_FANTASMA_A;
	else if (pac->fase <= 2)
		return PAC_VELOCIDADE_FANTASMA_B;
	else if (pac->fase <= 3)
		return PAC_VELOCIDADE_FANTASMA_C;
	else
		return PAC_VELOCIDADE_FANTASMA_D;
}

// Calcula o alvo do fantasma
Tile *novo_alvo(Fantasma fant, Mapa *map, Pacman *pac, Fantasma *blinky) {
	// Sair da pris�o 
	if (fant.capturado == PAC_CAPTURA_PRISAO) {
		return &map->tiles[8][7];
	// Fugir
	} else if (fant.capturado == PAC_CAPTURA_AZUL) {
		return &map->tiles[rand() % PAC_MAPA_ALTURA]
						  [rand() % PAC_MAPA_LARGURA];
	// Voltar � pris�o
	} else if (fant.capturado == PAC_CAPTURA_CAPTURADO) {
		return fant.inicio;
	// Espalhar
	} else if (fant.estado == PAC_ESTADO_ESPALHAR) {
		return &map->tiles[(fant.nome/2)*(PAC_MAPA_ALTURA-1)]
						  [((fant.nome+1)%2)*(PAC_MAPA_LARGURA-1)];
	// Perseguir
	} else if (fant.estado == PAC_ESTADO_PERSEGUIR) {
		Tile *referencia;
		int delta[2];
		switch (fant.nome) {
			//O fantasma 1 � mais objetivo e tem como alvo sempre a posi��o do pacman
			case Fantasma1:
				return pac->atual;
			//O fantasma 2 pega o tile que vai deix�-lo 4 tiles mais pr�ximo ao PacMan
			case Fantasma2:
				return proximoTileEm(4, map, pac->atual, pac->direcao);
			case Fantasma3:
				referencia = proximoTileEm(2, map, pac->atual, pac->direcao);
				delta[X] = referencia->pos[X] - blinky->atual->pos[X];
				delta[Y] = referencia->pos[Y] - blinky->atual->pos[Y];
				
				if (delta[X] > 0)
					referencia = proximoTileEm(2 * delta[X], map, blinky->atual, 3);
				else
					referencia = proximoTileEm(-2 * delta[X], map, blinky->atual, 1);
				
				if (delta[Y] > 0)
					referencia = proximoTileEm(2 * delta[Y], map, referencia, 2);
				else
					referencia = proximoTileEm(-2 * delta[Y], map, referencia, 0);
				
				return referencia;
			//Se a dist�ncia entre o Fantasma 4 e o PacMan for maior que 8, o alvo se torna o pacman,
			//caso contr�rio � dada a posi��o do canto inferior esquedo do mapa
			case Fantasma4:
				if (distanciaEntre(fant.atual, pac->atual) > 8)
					return pac->atual;
				else
					return &map->tiles[PAC_MAPA_ALTURA-1][0];
			
		}
	}
	return NULL;
}

// Verifica se � permitido passar sobre este tile
int e_caminho(Fantasma fant, Tile *tile) {
	return tile != 0
		   && tile->tipo != PAC_PAREDE
		   && (!ePrisao(tile)
			   || fant.capturado == PAC_CAPTURA_PRISAO//Se ta preso, pode passar por cima
			   || fant.capturado == PAC_CAPTURA_CAPTURADO)//Se ele foi capturado, pode passar por cima
		   && tile->tipo != PAC_NADA; //N�o for parte preta fora do mapa
}

int fantasmaPodeMudarDirecao(Fantasma fant, Mapa *map) {
	Tile *destino;
	
	destino = proximoTile(map, fant.atual, fant.direcao, fant.tele);
	return fant.mov == 0//Se o fantasma nao esbarrou em alguma parede
		   && destino != 0 //Se pertencer �s delimita��es da matriz
		   && e_caminho(fant, destino)//se for um caminho possivel
		   && !destino->tele;//Se nao for teletransporte
}

// Indica a dire��o que o fantasma deve seguir
int mehor_escolha(Fantasma fant, Mapa *map, Pacman *pac) {
	int i;
	int podeMudar[4];
	double distancia[4];
	int menorDistancia = PAC_MAPA_LARGURA + PAC_MAPA_ALTURA; // N�mero maior que o poss�vel
	int melhorDirecao = 4;
	int direcaoOposta = (fant.direcao + 2) % 4;
	Tile *destino;
	
	/*Verifica as dire��es possiveis para a movimenta��o do fantasma. Depois disso
	calcula a dist�ncia entre o destino calculado e o alvo do fantasma*/
	for (i = 0; i < 4; i++) {
		fant.direcao = i;
		destino = proximoTile(map, fant.atual, fant.direcao, fant.tele);
		if (i != direcaoOposta && destino)
			podeMudar[i] = fantasmaPodeMudarDirecao(fant, map);
		else
			podeMudar[i] = 0;
		distancia[i] = distanciaEntre(destino, fant.alvo);
	}
	
	/*Com as dist�ncias calculadas acima salvas no vetor distancia[], pega a dire�o de menor dist�ncia ao alvo*/
	for (i = 0; i < 4; i++) {
		if (podeMudar[i] && distancia[i] < menorDistancia) {
			melhorDirecao = i;
			menorDistancia = distancia[i];
		}
	}
	
	// Se foi encontrada uma dire��o, segue-a; se n�o, d� a volta
	if (melhorDirecao < 4)
		return melhorDirecao;
	else
		return direcaoOposta;
}