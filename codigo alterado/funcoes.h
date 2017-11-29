// Variáveis jogo
int especiais = 0; // Quantidade de bolinhas especiais ativadas
int estados = 1; // Número de vezes nas quais o estado dos fantasmas mudou
int maiorPontuacao = 0; // Recorde atual do jogo

void inicializacao();
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
void escreveTela(char *, int, int, int, int);
void carregarBitmap(char *, unsigned int, int, int);