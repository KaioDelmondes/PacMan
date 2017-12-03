typedef struct quadradinho
{
	int tipo; // Tipo
	int bolinha; // Bolinha que está dentro
	int pos[2]; // Posição
} Tile;

typedef struct mapa
{
	struct quadradinho **tiles; // Matriz de tiles
	int bolinhas; // Bolinhas restantes
	unsigned int lista; // Lista de exibição
} Mapa;

typedef struct pacman
{
	int vidas; // Quantidade de vidas
	int pontos; // Pontuação
	int fase; // Fase atual
	int morto; // Está morto
	int capturados; // Fantasmas capturados
	int direcao; // Direção de movimento
	int pre; // Direção do pré-move
	int boca; // Abertura da boca
	double mov; // Porcentagem de movimento
	double velocidade; // Velocidade

	struct quadradinho *atual; // Posição atual
	struct quadradinho *destino; // Próxima posição
	struct quadradinho *inicio; // Posição inicial
} Pacman;

typedef struct fantasma
{
	int nome; // Qual fantasma é
	int estado; // Estado
	int capturado; // Se pode ser capturado
	int direcao; // Direção de movimento
	double mov; // Porcentagem de movimento
	double velocidade; // Velocidade
	struct quadradinho *atual; // Posição atual
	struct quadradinho *destino; // Próxima posição
	struct quadradinho *inicio; // Posição inicial
	struct quadradinho *alvo; // Aonde vai
} Fantasma;
