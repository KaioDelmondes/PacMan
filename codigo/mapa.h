Mapa *criaMapa(int [][PAC_MAPA_LARGURA], int [][PAC_MAPA_LARGURA], Pacman *, Fantasma *);
void mostrarMapa(Mapa *);
void reiniciarMapa(Mapa *, int [][PAC_MAPA_LARGURA]);
Tile *proximoTile(Mapa *, Tile *, int);
Tile *proximoTileEm(int, Mapa *, Tile *, int);
int ePrisao(Tile *);
double distanciaEntre(Tile *, Tile *);
void desenhaQuadrado(float);

// Instancia o mapa
Mapa *criaMapa(int tipos[][PAC_MAPA_LARGURA], int bolinhas[][PAC_MAPA_LARGURA], Pacman *pac, Fantasma *fant)
{
	Mapa *map;

	int i, j, k;
	// l = 0;

	// Instancia o mapa
	map = (Mapa *) malloc(sizeof(Mapa));
	if (!map) return 0;

	// Aloca memória para os tiles do mapa
	map->tiles = (Tile **) malloc(PAC_MAPA_ALTURA * sizeof(Tile *));
	for (k = 0; k < PAC_MAPA_ALTURA; k++)
	{
		map->tiles[k] = (Tile *) malloc(PAC_MAPA_LARGURA * sizeof(Tile));
	}

	// Preenche as informações dos tiles
	for (j = 0; j < PAC_MAPA_ALTURA; j++)
	{
		for (i = 0; i < PAC_MAPA_LARGURA; i++)
		{
			map->tiles[j][i].tipo = tipos[j][i];
			map->tiles[j][i].bolinha = bolinhas[j][i];
			map->tiles[j][i].pos[X] = i;
			map->tiles[j][i].pos[Y] = j;

			
		}
	}

	// Cria a lista de exibição dos tiles
	map->lista = glGenLists(1);
	glNewList(map->lista, GL_COMPILE);
	glTranslated(0.5, -0.5, 0);
	for (j = PAC_MAPA_ALTURA - 1; j >= 0; j--)
	{
		glTranslated(0, 1, 0);
		glPushMatrix();
		for (i = 0; i < PAC_MAPA_LARGURA; i++)
		{
			glBegin(GL_QUADS);
			switch (map->tiles[j][i].tipo)
			{
			case PAC_CAMINHO:
				glColor3f(0, 0, 0);
				desenhaQuadrado(1);
				break;
			case PAC_PAREDE:
				glColor3f(0, 0.1, 0.2);
				desenhaQuadrado(1);
				break;
			
			
				break;
			//A partir daque é dada uma posição inicial para cada fantasma
			case Fantasma1_pac:
				glColor3f(0, 0, 0);
				desenhaQuadrado(1);
				fant[Fantasma1].atual = fant[Fantasma1].inicio = &map->tiles[j][i];
				break;
			case Fantasma2_pac:
				glColor3f(0, 0, 0);
				desenhaQuadrado(1);
				fant[Fantasma2].atual = fant[Fantasma2].inicio = &map->tiles[j][i];
				break;
			case Fantasma3_pac:
				glColor3f(0, 0, 0);
				desenhaQuadrado(1);
				fant[Fantasma3].atual = fant[Fantasma3].inicio = &map->tiles[j][i];
				break;
			case Fantasma4_pac:
				glColor3f(0, 0, 0);
				desenhaQuadrado(1);
				fant[Fantasma4].atual = fant[Fantasma4].inicio = &map->tiles[j][i];
				break;
			//aqui a posição do pacman.
			case PAC_PACMAN:
				glColor3f(0, 0, 0);
				desenhaQuadrado(1);
				pac->atual = pac->inicio = &map->tiles[j][i];
				break;
			}
			//Essas posições foram definidas previamente no cabeçalho matrizes.h
			glEnd();
			glTranslated(1, 0, 0);
		}
		glPopMatrix();
	}
	glEndList();
	/*Até aqui foi criada a lista dos objetos a serem renderizados*/
	
	map->bolinhas = 0;
	for (j = 0; j < PAC_MAPA_ALTURA; j++)
	{
		for (i = 0; i < PAC_MAPA_LARGURA; i++)
		{
			if (bolinhas[j][i] != PAC_BOLINHA_NENHUMA)
				map->bolinhas++;
		}
	}

	return map;
}

// Monta o mapa
void mostrarMapa(Mapa *map)
{
	int i, j;

	// Exibe o mapa
	glPushMatrix();
	glCallList(map->lista);
	glPopMatrix();

	// Exibe as bolinhas
	glPushMatrix();
	glTranslated(1, 0, 0.45);
	for (j = PAC_MAPA_ALTURA - 1; j >= 0; j--)
	{
		glTranslated(0, 1, 0);
		glPushMatrix();
		for (i = 0; i < PAC_MAPA_LARGURA; i++)
		{
			glPushMatrix();
			// Exibe as bolinhas restantes
			switch (map->tiles[j][i].bolinha)
			{
			case PAC_BOLINHA_NORMAL:
				glColor3f(1, 1, 1);
				glutSolidSphere(0.1, 10, 10);
				break;
			case PAC_BOLINHA_ESPECIAL:
				glColor3f(1, 1, 1);
				glutSolidSphere(0.2, 10, 10);
				break;
			//default:
			case PAC_BOLINHA_NENHUMA:
				glColor3f(1, 0, 0);
				//glutWireSphere(0.6, 10, 10);
				glutSolidCube(0.6);
			}
			glPopMatrix();
			glTranslated(1, 0, 0);
		}
		glPopMatrix();
	}
	glPopMatrix();

}

// Reinicia o mapa -- a cada vez que o jogo é iniciado o contador de bolinhas restantes é resetado.
// Essa situação ocorre no início do jogo e quando o pacman captura todas as bolinhas
void reiniciarMapa(Mapa *map, int bolinhas[][PAC_MAPA_LARGURA])
{
	int i, j;

	// Renova as bolinhas
	map->bolinhas = 0;
	for (j = 0; j < PAC_MAPA_ALTURA; j++)
	{
		for (i = 0; i < PAC_MAPA_LARGURA; i++)
		{
			map->tiles[j][i].bolinha = bolinhas[j][i];
			if (bolinhas[j][i] == PAC_BOLINHA_NORMAL || bolinhas[j][i] == PAC_BOLINHA_ESPECIAL)
				map->bolinhas++;
		}
	}
}

// Retorna um ponteiro para o próximo tile
Tile *proximoTile(Mapa *map, Tile *tile, int direcao)
{
	// Verifica se o tile pertence ao mapa
	if (tile != 0
			&& tile->pos[X] > -1
			&& tile->pos[X] < PAC_MAPA_LARGURA
			&& tile->pos[Y] > -1
			&& tile->pos[Y] < PAC_MAPA_ALTURA)
	{
		
		// Procura o próximo
		switch (direcao)
		{
		//para cima
		case 0:
			if (tile->pos[Y] == 0) return 0;
			return &map->tiles[tile->pos[Y] - 1][tile->pos[X]];
		//direita
		case 3:
			if (tile->pos[X] == PAC_MAPA_LARGURA - 1) return 0;
			return &map->tiles[tile->pos[Y]][tile->pos[X] + 1];
		//baixo
		case 2:
			if (tile->pos[Y] == PAC_MAPA_ALTURA - 1) return 0;
			return &map->tiles[tile->pos[Y] + 1][tile->pos[X]];
		//esquerda
		case 1:
			if (tile->pos[X] == 0) return 0;
			return &map->tiles[tile->pos[Y]][tile->pos[X] - 1];
		}
	}
	return 0;
}

// Retorna o n-ésimo tile seguinte
Tile *proximoTileEm(int passos, Mapa *map, Tile *tile, int direcao)
{
	Tile *atual = tile, *proximo = NULL;
	int i = 1;

	proximo = proximoTile(map, atual, direcao);
	while (i < passos && proximo)
	{
		atual = proximo;
		proximo = proximoTile(map, atual, direcao);
		i++;
	}

	return atual;
}

// Verifica se o tile faz parte da prisão
int ePrisao(Tile *tile)
{
	return tile != 0
		   && (tile->tipo == Fantasma1_pac
			   || tile->tipo == Fantasma2_pac
			   || tile->tipo == Fantasma3_pac
			   || tile->tipo == Fantasma4_pac);
}

// Calcula a distância entre 2 tiles
double distanciaEntre(Tile *a, Tile *b)
{
	int deltaX = a->pos[X] - b->pos[X];
	int deltaY = a->pos[Y] - b->pos[Y];

	return sqrt((double) deltaX * deltaX + deltaY * deltaY);
}

// Desenha um quadrado na tela
void desenhaQuadrado(float tamanho)
{
	glBegin(GL_QUADS);
	glVertex2f(0, tamanho);
	glVertex2f(0, 0);
	glVertex2f(tamanho, 0);
	glVertex2f(tamanho, tamanho);
	glEnd();
}
