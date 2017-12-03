// Variáveis jogo
int especiais = 0; // Quantidade de bolinhas especiais ativadas
int estados = 1; // Número de vezes nas quais o estado dos fantasmas mudou
int maiorPontuacao = 0; // Recorde atual do jogo
int niveis=0;

void inicializacao();
void constroiTela();
void montaFundo();
void Menu();
void atualizarJogo();
void mostrarJogo();
void posicionarCamera(Pacman *, double);
void reiniciaJogo();
void reiniciar(Pacman *, Fantasma *);
void mudarEstadoFantasma(int);
void iniciou(int);
void morreu(int);
void passarFase(int);
void terminarEspecial(int);
void moveCursor(int, int, int);
void teclado(unsigned char, int, int);
void redimensionar(int, int);
void carregarBitmap(char *, unsigned int, int, int);
void escreveTela(char *, int, int, int, int);
