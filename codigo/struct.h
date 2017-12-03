typedef struct quadradinho
{
	int tipo; // Tipo
	int bolinha; // Bolinha que est� dentro
	int pos[2]; // Posi��o
} Tile;

typedef struct mapa
{
	struct quadradinho **tiles; // Matriz de tiles
	int bolinhas; // Bolinhas restantes
	unsigned int lista; // Lista de exibi��o
} Mapa;

typedef struct pacman
{
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

	struct quadradinho *atual; // Posi��o atual
	struct quadradinho *destino; // Pr�xima posi��o
	struct quadradinho *inicio; // Posi��o inicial
} Pacman;

typedef struct fantasma
{
	int nome; // Qual fantasma �
	int estado; // Estado
	int capturado; // Se pode ser capturado
	int direcao; // Dire��o de movimento
	double mov; // Porcentagem de movimento
	double velocidade; // Velocidade
	struct quadradinho *atual; // Posi��o atual
	struct quadradinho *destino; // Pr�xima posi��o
	struct quadradinho *inicio; // Posi��o inicial
	struct quadradinho *alvo; // Aonde vai
} Fantasma;
