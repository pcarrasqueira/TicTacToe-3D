#include "Tic_Tac_Toe_3D.h"

#define ESCAPE 27

#define MAX_NO_TEXTURES 11



/*****************************************************************/
/*			VARIAVEIS											*/
/*****************************************************************/

face_cubo face_frontal, face_direita, face_esquerda, face_tras, face_baixo, face_cima, face_aux;

struct pos
{
	int posX;
	int posY;
	int posZ;
};

double razao_aspeto = 1;		   // razão de aspecto da janela de visualização

double limite_z_anterior = 1;			            // plano de recorte anterior
double limite_z_posterior = 10000;		           // plano de recorte posterior
double campo_visao_y = 50;

double camara_x = -15;					                // posição da câmara (x)
double camara_y = 10;					                // posição da câmara (y)
double camara_z = 200;					                // posição da câmara (z)

double mira_x = 0;				        // direção de visualização da câmara (x)
double mira_y = 0;				        // direção de visualização da câmara (y)
double mira_z = 2;				        // direção de visualização da câmara (z)

double teta_x = 0;				                         // orientação da câmara
double teta_z = 0;				                         // orientação da câmara

GLint zoom_z = 100;

int largura = 1024;					  // largura da nossa janela de visualização
int altura = 768;					   // altura da nossa janela de visualização


//////////////////////////////////
/* VARIAVEIS DAS FACES DO CUBO */
//////////////////////////////////

/*
Posicoes da grelha:

[ 0 | 3 | 6 ]
[ 1 | 4 | 7 ]
[ 2 | 5 | 8 ]

*/



int roda_dir = 0;
int roda_esq = 0;
int roda_cima = 0;
int roda_baixo = 0;
GLint face; // id da face: 0-frontal | 1-direita | 2-esquerda | 3-tras | 4-baixo | 5-cima
int face_activa; // variaveis para saber qual a face que esta em frente a camara
int face_activa2;
int guarda_face_activa; //Quando é iniciado o modo auto (demo)

GLfloat yRotated = 0;
GLfloat xRotated = 0;

/* Detecta limites */
float xmax = 0;
float ymax = 0;

///////////////////////////////
/* FIM DE VARIAVEIS DO CUBO */
////////////////////////////////


int margem_grelha = 1; // E' melhor adicionar uma margem para evitar carregar fora da posicao correcta
int game_over = 0;
//define quem ganhou em cada face
int jogos[6] = { -1, -1, -1, -1, -1, -1 }; //0-empate 1- jogador X, 2 - jogador O  |||  id da face: 0-frontal | 1-direita | 2-esquerda | 3-tras | 4-baixo | 5-cima  
int jogo_final = -1; // 0 - empate | 1- jogador X | 2- Jogador O
int conta_x = 0, conta_o = 0, conta_empate = 0;
int start_face;
int start_face_aux; // salva estado de start face quando visualizamos os controlos novamente
int restart = 0;
int escolha = 0;
int aux = 0;
int modo_jogo = 9;		//	0-> 2 players  |  1-> VS PC  | 2-> free view | 3 - Modo Auto | 9->Menu Inicial | 

// vetor com os números das texturas
GLuint texture_id[MAX_NO_TEXTURES];

/////////////////////
/* MENUS-- BOTOES */
////////////////////
int botao1_x = 50, botao1_y = 15;				 // Largura (x) e Altura (y) do botao
int botao1_pos_x = 35, botao1_pos_y = -40;		// Ponto superior esquerdo do botao

int botao2_x = 50, botao2_y = 15;
int botao2_pos_x = 35, botao2_pos_y = -10;

int botao3_x = 50, botao3_y = 15;
int botao3_pos_x = 35, botao3_pos_y = -70;

int botao4_x = 20, botao4_y = 10;
int botao4_pos_x = 100, botao4_pos_y = -90;

int botaosair_x = 7, botaosair_y = 7;
int botaosair_pos_x = 113, botaosair_pos_y = 82;

int botaook_x = 50, botaook_y = 15;
int botaook_pos_x = 0 - 25, botaook_pos_y = -75;

int botaook2_x = 50, botaook2_y = 15;
int botaook2_pos_x = 0 - 25, botaook2_pos_y = -90;
//////////////
//FIM BOTOES//
//////////////


int verifica_resultado_actual = 0;
char* contax;
char* contay;
int jogos0;
int menu1, menu2;
int play = 0;
bool PC_turno = 0; // Determina se é o turno do PC ou do jogador, no modo <VS PC>

/*****************************************************************/
/*			FUNÇÔES												*/
/*****************************************************************/

void Desenha_Circulo_face_frontal_tras(GLfloat x, GLfloat y, GLfloat z, GLfloat raio) {

	glPushMatrix();
	glTranslatef(x, y, z);
	glColor3f(0, 0.2, 0.8);
	glutSolidTorus(raio / 4, raio, 5, 5);
	glPopMatrix();
}

void Desenha_Circulo_face_direita_esquerda(GLfloat x, GLfloat y, GLfloat z, GLfloat raio) {

	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(90, 0, 1, 0);
	glColor3f(0, 0.2, 0.8);
	glutSolidTorus(raio / 4, raio, 5, 5);
	glPopMatrix();
}

void Desenha_Circulo_face_cima_baixo(GLfloat x, GLfloat y, GLfloat z, GLfloat raio) {

	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(90, 1, 0, 0);
	glColor3f(0, 0.2, 0.8);
	glutSolidTorus(raio / 4, raio, 5, 5);
	glPopMatrix();
}

void Desenha_X_face_frontal(GLint x_esq_cima, GLint y_esq_cima, GLint z_esq_cima, GLint largura) {

	glColor3f(0.8, 0.2, 0.2);
	int espessura = 10;
	glLineWidth(espessura);
	glBegin(GL_LINES);

	glVertex3f(x_esq_cima, y_esq_cima, z_esq_cima);
	glVertex3f(x_esq_cima + 2 * largura, y_esq_cima - 2 * largura, z_esq_cima);
	glVertex3f(x_esq_cima + 2 * largura, y_esq_cima, z_esq_cima);
	glVertex3f(x_esq_cima, y_esq_cima - largura * 2, z_esq_cima);

	glEnd();
	glLineWidth(1);
}

void Desenha_X_face_tras(GLint x_esq_cima, GLint y_esq_cima, GLint z_esq_cima, GLint largura) {

	glColor3f(0.8, 0.2, 0.2);
	int espessura = 10;
	glLineWidth(espessura);
	glBegin(GL_LINES);

	glVertex3f(x_esq_cima, y_esq_cima, z_esq_cima);
	glVertex3f(x_esq_cima - 2 * largura, y_esq_cima - 2 * largura, z_esq_cima);
	glVertex3f(x_esq_cima - 2 * largura, y_esq_cima, z_esq_cima);
	glVertex3f(x_esq_cima, y_esq_cima - largura * 2, z_esq_cima);

	glEnd();
	glLineWidth(1);
}

void Desenha_X_face_direita(GLint x_esq_cima, GLint y_esq_cima, GLint z_esq_cima, GLint largura) {

	glColor3f(0.8, 0.2, 0.2);
	int espessura = 10;
	glLineWidth(espessura);
	glBegin(GL_LINES);

	glVertex3f(x_esq_cima, y_esq_cima, z_esq_cima);
	glVertex3f(x_esq_cima, y_esq_cima - 2 * largura, z_esq_cima - 2 * largura);
	glVertex3f(x_esq_cima, y_esq_cima, z_esq_cima - 2 * largura);
	glVertex3f(x_esq_cima, y_esq_cima - largura * 2, z_esq_cima);

	glEnd();
	glLineWidth(1);
}

void Desenha_X_face_esquerda(GLint x_esq_cima, GLint y_esq_cima, GLint z_esq_cima, GLint largura) {

	glColor3f(0.8, 0.2, 0.2);
	int espessura = 10;
	glLineWidth(espessura);
	glBegin(GL_LINES);

	glVertex3f(x_esq_cima, y_esq_cima, z_esq_cima);
	glVertex3f(x_esq_cima, y_esq_cima - 2 * largura, z_esq_cima + 2 * largura);
	glVertex3f(x_esq_cima, y_esq_cima, z_esq_cima + 2 * largura);
	glVertex3f(x_esq_cima, y_esq_cima - largura * 2, z_esq_cima);

	glEnd();
	glLineWidth(1);
}

void Desenha_X_face_cima(GLint x_esq_cima, GLint y_esq_cima, GLint z_esq_cima, GLint largura) {

	glColor3f(0.8, 0.2, 0.2);
	int espessura = 10;
	glLineWidth(espessura);
	glBegin(GL_LINES);

	glVertex3f(x_esq_cima, y_esq_cima, z_esq_cima);
	glVertex3f(x_esq_cima + 2 * largura, y_esq_cima, z_esq_cima + 2 * largura);
	glVertex3f(x_esq_cima + 2 * largura, y_esq_cima, z_esq_cima);
	glVertex3f(x_esq_cima, y_esq_cima, z_esq_cima + 2 * largura);

	glEnd();
	glLineWidth(1);
}

void Desenha_X_face_baixo(GLint x_esq_cima, GLint y_esq_cima, GLint z_esq_cima, GLint largura) {

	glColor3f(0.8, 0.2, 0.2);
	int espessura = 10;
	glLineWidth(espessura);
	glBegin(GL_LINES);

	glVertex3f(x_esq_cima, y_esq_cima, z_esq_cima);
	glVertex3f(x_esq_cima + 2 * largura, y_esq_cima, z_esq_cima - 2 * largura);
	glVertex3f(x_esq_cima + 2 * largura, y_esq_cima, z_esq_cima);
	glVertex3f(x_esq_cima, y_esq_cima, z_esq_cima - 2 * largura);

	glEnd();
	glLineWidth(1);
}

void isGameOver()
{
	/* ------------------------------------------------------------ */
	/*							FACE DA FRENTE                     */
	/* ---------------------------------------------------------- */

	// Verticais com simbolo BOLA
	if (face_frontal.simbolo[0] == 0 && face_frontal.simbolo[1] == 0 && face_frontal.simbolo[2] == 0
		|| face_frontal.simbolo[3] == 0 && face_frontal.simbolo[4] == 0 && face_frontal.simbolo[5] == 0
		|| face_frontal.simbolo[6] == 0 && face_frontal.simbolo[7] == 0 && face_frontal.simbolo[8] == 0

		// Horizontais com simbolo BOLA
		|| face_frontal.simbolo[0] == 0 && face_frontal.simbolo[3] == 0 && face_frontal.simbolo[6] == 0
		|| face_frontal.simbolo[1] == 0 && face_frontal.simbolo[4] == 0 && face_frontal.simbolo[7] == 0
		|| face_frontal.simbolo[2] == 0 && face_frontal.simbolo[5] == 0 && face_frontal.simbolo[8] == 0

		// Diagonais com simbolo BOLA
		|| face_frontal.simbolo[0] == 0 && face_frontal.simbolo[4] == 0 && face_frontal.simbolo[8] == 0
		|| face_frontal.simbolo[2] == 0 && face_frontal.simbolo[4] == 0 && face_frontal.simbolo[6] == 0) {


		if (jogos[0] == -1){
			start_face = 0;
			sndPlaySoundA("done.wav", SND_ASYNC | SND_NOSTOP);
		}

		jogos[0] = 2; // 1- jogador X, 2 - jogador O
		//id da face : 0 - frontal | 1 - direita | 2 - esquerda | 3 - tras | 4 - baixo | 5 - cima



	}
	// Verticais com simbolo CRUZ
	else if (face_frontal.simbolo[0] == 1 && face_frontal.simbolo[1] == 1 && face_frontal.simbolo[2] == 1
		|| face_frontal.simbolo[3] == 1 && face_frontal.simbolo[4] == 1 && face_frontal.simbolo[5] == 1
		|| face_frontal.simbolo[6] == 1 && face_frontal.simbolo[7] == 1 && face_frontal.simbolo[8] == 1


		// Horizontais com simbolo CRUZ
		|| face_frontal.simbolo[0] == 1 && face_frontal.simbolo[3] == 1 && face_frontal.simbolo[6] == 1
		|| face_frontal.simbolo[1] == 1 && face_frontal.simbolo[4] == 1 && face_frontal.simbolo[7] == 1
		|| face_frontal.simbolo[2] == 1 && face_frontal.simbolo[5] == 1 && face_frontal.simbolo[8] == 1


		// Diagonais com simbolo CRUZ
		|| face_frontal.simbolo[0] == 1 && face_frontal.simbolo[4] == 1 && face_frontal.simbolo[8] == 1
		|| face_frontal.simbolo[2] == 1 && face_frontal.simbolo[4] == 1 && face_frontal.simbolo[6] == 1) {

		if (jogos[0] == -1){
			start_face = 0;
			sndPlaySoundA("done.wav", SND_ASYNC | SND_NOSTOP);
		}

		jogos[0] = 1; // 1- jogador X, 2 - jogador O
		//id da face : 0 - frontal | 1 - direita | 2 - esquerda | 3 - tras | 4 - baixo | 5 - cima



	}

	else if (face_frontal.num_jogada == 9 && jogos[0] == -1) {

		if (jogos[0] == -1){
			start_face = 0;
			sndPlaySoundA("done.wav", SND_ASYNC | SND_NOSTOP);
		}
		jogos[0] = 0; // empate


	}

	/* ------------------------------------------------------------ */
	/*							FACE DA DIREITA                    */
	/* ---------------------------------------------------------- */


	// Verticais com simbolo BOLA
	if (face_direita.simbolo[0] == 0 && face_direita.simbolo[1] == 0 && face_direita.simbolo[2] == 0
		|| face_direita.simbolo[3] == 0 && face_direita.simbolo[4] == 0 && face_direita.simbolo[5] == 0
		|| face_direita.simbolo[6] == 0 && face_direita.simbolo[7] == 0 && face_direita.simbolo[8] == 0

		// Horizontais com simbolo BOLA
		|| face_direita.simbolo[0] == 0 && face_direita.simbolo[3] == 0 && face_direita.simbolo[6] == 0
		|| face_direita.simbolo[1] == 0 && face_direita.simbolo[4] == 0 && face_direita.simbolo[7] == 0
		|| face_direita.simbolo[2] == 0 && face_direita.simbolo[5] == 0 && face_direita.simbolo[8] == 0

		// Diagonais com simbolo BOLA
		|| face_direita.simbolo[0] == 0 && face_direita.simbolo[4] == 0 && face_direita.simbolo[8] == 0
		|| face_direita.simbolo[2] == 0 && face_direita.simbolo[4] == 0 && face_direita.simbolo[6] == 0) {

		if (jogos[1] == -1){
			start_face = 0;
			sndPlaySoundA("done.wav", SND_ASYNC | SND_NOSTOP);
		}

		jogos[1] = 2; // 1- jogador X, 2 - jogador O
		//id da face : 0 - frontal | 1 - direita | 2 - esquerda | 3 - tras | 4 - baixo | 5 - cima

	}

	// Verticais com simbolo CRUZ
	else if (face_direita.simbolo[0] == 1 && face_direita.simbolo[1] == 1 && face_direita.simbolo[2] == 1
		|| face_direita.simbolo[3] == 1 && face_direita.simbolo[4] == 1 && face_direita.simbolo[5] == 1
		|| face_direita.simbolo[6] == 1 && face_direita.simbolo[7] == 1 && face_direita.simbolo[8] == 1


		// Horizontais com simbolo CRUZ
		|| face_direita.simbolo[0] == 1 && face_direita.simbolo[3] == 1 && face_direita.simbolo[6] == 1
		|| face_direita.simbolo[1] == 1 && face_direita.simbolo[4] == 1 && face_direita.simbolo[7] == 1
		|| face_direita.simbolo[2] == 1 && face_direita.simbolo[5] == 1 && face_direita.simbolo[8] == 1


		// Diagonais com simbolo CRUZ
		|| face_direita.simbolo[0] == 1 && face_direita.simbolo[4] == 1 && face_direita.simbolo[8] == 1
		|| face_direita.simbolo[2] == 1 && face_direita.simbolo[4] == 1 && face_direita.simbolo[6] == 1) {

		if (jogos[1] == -1){
			start_face = 0;
			sndPlaySoundA("done.wav", SND_ASYNC | SND_NOSTOP);
		}


		jogos[1] = 1; // 1- jogador X, 2 - jogador O
		//id da face : 0 - frontal | 1 - direita | 2 - esquerda | 3 - tras | 4 - baixo | 5 - cima

	}

	else if (face_direita.num_jogada == 9) {

		if (jogos[1] == -1){
			start_face = 0;
			sndPlaySoundA("done.wav", SND_ASYNC | SND_NOSTOP);
		}


		jogos[1] = 0; // empate

	}

	/* ------------------------------------------------------------- */
	/*							FACE DA ESQUERDA                    */
	/* ----------------------------------------------------------- */

	// Verticais com simbolo BOLA
	if (face_esquerda.simbolo[0] == 0 && face_esquerda.simbolo[1] == 0 && face_esquerda.simbolo[2] == 0
		|| face_esquerda.simbolo[3] == 0 && face_esquerda.simbolo[4] == 0 && face_esquerda.simbolo[5] == 0
		|| face_esquerda.simbolo[6] == 0 && face_esquerda.simbolo[7] == 0 && face_esquerda.simbolo[8] == 0

		// Horizontais com simbolo BOLA
		|| face_esquerda.simbolo[0] == 0 && face_esquerda.simbolo[3] == 0 && face_esquerda.simbolo[6] == 0
		|| face_esquerda.simbolo[1] == 0 && face_esquerda.simbolo[4] == 0 && face_esquerda.simbolo[7] == 0
		|| face_esquerda.simbolo[2] == 0 && face_esquerda.simbolo[5] == 0 && face_esquerda.simbolo[8] == 0

		// Diagonais com simbolo BOLA
		|| face_esquerda.simbolo[0] == 0 && face_esquerda.simbolo[4] == 0 && face_esquerda.simbolo[8] == 0
		|| face_esquerda.simbolo[2] == 0 && face_esquerda.simbolo[4] == 0 && face_esquerda.simbolo[6] == 0) {

		if (jogos[2] == -1){
			start_face = 0;
			sndPlaySoundA("done.wav", SND_ASYNC | SND_NOSTOP);
		}


		jogos[2] = 2; // 1- jogador X, 2 - jogador O
		//id da face : 0 - frontal | 1 - esquerda | 2 - esquerda | 3 - tras | 4 - baixo | 5 - cima

	}

	// Verticais com simbolo CRUZ
	else if (face_esquerda.simbolo[0] == 1 && face_esquerda.simbolo[1] == 1 && face_esquerda.simbolo[2] == 1
		|| face_esquerda.simbolo[3] == 1 && face_esquerda.simbolo[4] == 1 && face_esquerda.simbolo[5] == 1
		|| face_esquerda.simbolo[6] == 1 && face_esquerda.simbolo[7] == 1 && face_esquerda.simbolo[8] == 1


		// Horizontais com simbolo CRUZ
		|| face_esquerda.simbolo[0] == 1 && face_esquerda.simbolo[3] == 1 && face_esquerda.simbolo[6] == 1
		|| face_esquerda.simbolo[1] == 1 && face_esquerda.simbolo[4] == 1 && face_esquerda.simbolo[7] == 1
		|| face_esquerda.simbolo[2] == 1 && face_esquerda.simbolo[5] == 1 && face_esquerda.simbolo[8] == 1


		// Diagonais com simbolo CRUZ
		|| face_esquerda.simbolo[0] == 1 && face_esquerda.simbolo[4] == 1 && face_esquerda.simbolo[8] == 1
		|| face_esquerda.simbolo[2] == 1 && face_esquerda.simbolo[4] == 1 && face_esquerda.simbolo[6] == 1) {

		if (jogos[2] == -1){
			start_face = 0;
			sndPlaySoundA("done.wav", SND_ASYNC | SND_NOSTOP);
		}


		jogos[2] = 1; // 1- jogador X, 2 - jogador O

	}


	else if (face_esquerda.num_jogada == 9) {

		if (jogos[2] == -1){
			start_face = 0;
			sndPlaySoundA("done.wav", SND_ASYNC | SND_NOSTOP);
		}


		jogos[2] = 0; // empate

	}


	/* ------------------------------------------------------------ */
	/*							FACE DE TRAS                       */
	/* ---------------------------------------------------------- */

	// Verticais com simbolo BOLA
	if (face_tras.simbolo[0] == 0 && face_tras.simbolo[1] == 0 && face_tras.simbolo[2] == 0
		|| face_tras.simbolo[3] == 0 && face_tras.simbolo[4] == 0 && face_tras.simbolo[5] == 0
		|| face_tras.simbolo[6] == 0 && face_tras.simbolo[7] == 0 && face_tras.simbolo[8] == 0

		// Horizontais com simbolo BOLA
		|| face_tras.simbolo[0] == 0 && face_tras.simbolo[3] == 0 && face_tras.simbolo[6] == 0
		|| face_tras.simbolo[1] == 0 && face_tras.simbolo[4] == 0 && face_tras.simbolo[7] == 0
		|| face_tras.simbolo[2] == 0 && face_tras.simbolo[5] == 0 && face_tras.simbolo[8] == 0

		// Diagonais com simbolo BOLA
		|| face_tras.simbolo[0] == 0 && face_tras.simbolo[4] == 0 && face_tras.simbolo[8] == 0
		|| face_tras.simbolo[2] == 0 && face_tras.simbolo[4] == 0 && face_tras.simbolo[6] == 0) {

		if (jogos[3] == -1){
			start_face = 0;
			sndPlaySoundA("done.wav", SND_ASYNC | SND_NOSTOP);
		}

		jogos[3] = 2; // 1- jogador X, 2 - jogador O
	}

	// Verticais com simbolo CRUZ
	else if (face_tras.simbolo[0] == 1 && face_tras.simbolo[1] == 1 && face_tras.simbolo[2] == 1
		|| face_tras.simbolo[3] == 1 && face_tras.simbolo[4] == 1 && face_tras.simbolo[5] == 1
		|| face_tras.simbolo[6] == 1 && face_tras.simbolo[7] == 1 && face_tras.simbolo[8] == 1


		// Horizontais com simbolo CRUZ
		|| face_tras.simbolo[0] == 1 && face_tras.simbolo[3] == 1 && face_tras.simbolo[6] == 1
		|| face_tras.simbolo[1] == 1 && face_tras.simbolo[4] == 1 && face_tras.simbolo[7] == 1
		|| face_tras.simbolo[2] == 1 && face_tras.simbolo[5] == 1 && face_tras.simbolo[8] == 1


		// Diagonais com simbolo CRUZ
		|| face_tras.simbolo[0] == 1 && face_tras.simbolo[4] == 1 && face_tras.simbolo[8] == 1
		|| face_tras.simbolo[2] == 1 && face_tras.simbolo[4] == 1 && face_tras.simbolo[6] == 1) {


		if (jogos[3] == -1){
			start_face = 0;
			sndPlaySoundA("done.wav", SND_ASYNC | SND_NOSTOP);
		}

		jogos[3] = 1; // 1- jogador X, 2 - jogador O
	}

	else if (face_tras.num_jogada == 9) {


		if (jogos[3] == -1){
			start_face = 0;
			sndPlaySoundA("done.wav", SND_ASYNC | SND_NOSTOP);
		}

		jogos[3] = 0; // empate

	}



	/* ------------------------------------------------------------ */
	/*							FACE DE BAIXO                      */
	/* ---------------------------------------------------------- */

	// Verticais com simbolo BOLA
	if (face_baixo.simbolo[0] == 0 && face_baixo.simbolo[1] == 0 && face_baixo.simbolo[2] == 0
		|| face_baixo.simbolo[3] == 0 && face_baixo.simbolo[4] == 0 && face_baixo.simbolo[5] == 0
		|| face_baixo.simbolo[6] == 0 && face_baixo.simbolo[7] == 0 && face_baixo.simbolo[8] == 0

		// Horizontais com simbolo BOLA
		|| face_baixo.simbolo[0] == 0 && face_baixo.simbolo[3] == 0 && face_baixo.simbolo[6] == 0
		|| face_baixo.simbolo[1] == 0 && face_baixo.simbolo[4] == 0 && face_baixo.simbolo[7] == 0
		|| face_baixo.simbolo[2] == 0 && face_baixo.simbolo[5] == 0 && face_baixo.simbolo[8] == 0

		// Diagonais com simbolo BOLA
		|| face_baixo.simbolo[0] == 0 && face_baixo.simbolo[4] == 0 && face_baixo.simbolo[8] == 0
		|| face_baixo.simbolo[2] == 0 && face_baixo.simbolo[4] == 0 && face_baixo.simbolo[6] == 0) {


		if (jogos[4] == -1){
			start_face = 0;
			sndPlaySoundA("done.wav", SND_ASYNC | SND_NOSTOP);
		}

		jogos[4] = 2; // 1- jogador X, 2 - jogador O
		//id da face : 0 - frontal | 1 - baixo | 2 - baixo | 3 - baixo | 4 - baixo | 5 - cima

	}
	// Verticais com simbolo CRUZ
	else if (face_baixo.simbolo[0] == 1 && face_baixo.simbolo[1] == 1 && face_baixo.simbolo[2] == 1
		|| face_baixo.simbolo[3] == 1 && face_baixo.simbolo[4] == 1 && face_baixo.simbolo[5] == 1
		|| face_baixo.simbolo[6] == 1 && face_baixo.simbolo[7] == 1 && face_baixo.simbolo[8] == 1


		// Horizontais com simbolo CRUZ
		|| face_baixo.simbolo[0] == 1 && face_baixo.simbolo[3] == 1 && face_baixo.simbolo[6] == 1
		|| face_baixo.simbolo[1] == 1 && face_baixo.simbolo[4] == 1 && face_baixo.simbolo[7] == 1
		|| face_baixo.simbolo[2] == 1 && face_baixo.simbolo[5] == 1 && face_baixo.simbolo[8] == 1


		// Diagonais com simbolo CRUZ
		|| face_baixo.simbolo[0] == 1 && face_baixo.simbolo[4] == 1 && face_baixo.simbolo[8] == 1
		|| face_baixo.simbolo[2] == 1 && face_baixo.simbolo[4] == 1 && face_baixo.simbolo[6] == 1) {

		if (jogos[4] == -1){
			start_face = 0;
			sndPlaySoundA("done.wav", SND_ASYNC | SND_NOSTOP);
		}

		jogos[4] = 1; // 1- jogador X, 2 - jogador O
		//id da face : 0 - frontal | 1 - baixo | 2 - baixo | 3 - baixo | 4 - baixo | 5 - cima

	}
	else if (face_baixo.num_jogada == 9) {

		if (jogos[4] == -1){
			start_face = 0;
			sndPlaySoundA("done.wav", SND_ASYNC | SND_NOSTOP);
		}

		jogos[4] = 0; // empate

	}


	/* ------------------------------------------------------------ */
	/*							FACE DE CIMA                       */
	/* ---------------------------------------------------------- */

	// Verticais com simbolo BOLA
	if (face_cima.simbolo[0] == 0 && face_cima.simbolo[1] == 0 && face_cima.simbolo[2] == 0
		|| face_cima.simbolo[3] == 0 && face_cima.simbolo[4] == 0 && face_cima.simbolo[5] == 0
		|| face_cima.simbolo[6] == 0 && face_cima.simbolo[7] == 0 && face_cima.simbolo[8] == 0

		// Horizontais com simbolo BOLA
		|| face_cima.simbolo[0] == 0 && face_cima.simbolo[3] == 0 && face_cima.simbolo[6] == 0
		|| face_cima.simbolo[1] == 0 && face_cima.simbolo[4] == 0 && face_cima.simbolo[7] == 0
		|| face_cima.simbolo[2] == 0 && face_cima.simbolo[5] == 0 && face_cima.simbolo[8] == 0

		// Diagonais com simbolo BOLA
		|| face_cima.simbolo[0] == 0 && face_cima.simbolo[4] == 0 && face_cima.simbolo[8] == 0
		|| face_cima.simbolo[2] == 0 && face_cima.simbolo[4] == 0 && face_cima.simbolo[6] == 0) {

		if (jogos[5] == -1){
			start_face = 0;
			sndPlaySoundA("done.wav", SND_ASYNC | SND_NOSTOP);
		}

		jogos[5] = 2; // 1- jogador X, 2 - jogador O
		//id da face : 0 - frontal | 1 - cima | 2 - cima | 3 - cima | 4 - cima | 5 - cima

	}

	// Verticais com simbolo CRUZ
	else if (face_cima.simbolo[0] == 1 && face_cima.simbolo[1] == 1 && face_cima.simbolo[2] == 1
		|| face_cima.simbolo[3] == 1 && face_cima.simbolo[4] == 1 && face_cima.simbolo[5] == 1
		|| face_cima.simbolo[6] == 1 && face_cima.simbolo[7] == 1 && face_cima.simbolo[8] == 1


		// Horizontais com simbolo CRUZ
		|| face_cima.simbolo[0] == 1 && face_cima.simbolo[3] == 1 && face_cima.simbolo[6] == 1
		|| face_cima.simbolo[1] == 1 && face_cima.simbolo[4] == 1 && face_cima.simbolo[7] == 1
		|| face_cima.simbolo[2] == 1 && face_cima.simbolo[5] == 1 && face_cima.simbolo[8] == 1


		// Diagonais com simbolo CRUZ
		|| face_cima.simbolo[0] == 1 && face_cima.simbolo[4] == 1 && face_cima.simbolo[8] == 1
		|| face_cima.simbolo[2] == 1 && face_cima.simbolo[4] == 1 && face_cima.simbolo[6] == 1) {

		if (jogos[5] == -1){
			start_face = 0;
			sndPlaySoundA("done.wav", SND_ASYNC | SND_NOSTOP);
		}

		jogos[5] = 1; // 1- jogador X, 2 - jogador O
		//id da face : 0 - frontal | 1 - cima | 2 - cima | 3 - cima | 4 - cima | 5 - cima

	}

	else if (face_cima.num_jogada == 9) {

		if (jogos[5] == -1){
			start_face = 0;
			sndPlaySoundA("done.wav", SND_ASYNC | SND_NOSTOP);
		}

		jogos[5] = 0; // empate

	}


	if (jogos[0] != -1 && jogos[1] != -1 && jogos[2] != -1 && jogos[3] != -1 && jogos[4] != -1 && jogos[5] != -1 || verifica_resultado_actual == 1) {

		if (game_over == 0) {


			for (int i = 0; i < 6; i++){

				if (jogos[i] == 1) {
					conta_x++;
				}
				else if (jogos[i] == 2) {
					conta_o++;

				}
				else if (jogos[i] == 0){
					conta_empate++;
				}
			}

			if (verifica_resultado_actual != 1) {
				if (conta_x > conta_o)
					jogo_final = 1;
				else if (conta_x < conta_o)
					jogo_final = 2;
				else
					jogo_final = 0;
				game_over = 1;
			}

		}
	}


}

void Detecta_Jogada()
{

	/* Verifica se algum jogador ganhou */
	isGameOver();

	/* ------------------------------------------------------------ */
	/*							FACE DA FRENTE                     */
	/* ---------------------------------------------------------- */

	// Posicao esquerda cima
	if (face_frontal.pos_click[0] > 0)
	{
		if (face_frontal.pos_click[0] == 1){
			face_frontal.simbolo[0] = face_frontal.num_jogada % 2;
			face_frontal.pos_click[0]++;
		}
		if (face_frontal.simbolo[0] == 0) { // Bolas 
			Desenha_Circulo_face_frontal_tras(-25 + 50 / 6, 25 - 50 / 6, 26, 4);
		}
		else // Cruzes
			Desenha_X_face_frontal(-25 + 50 / 12, 25 - 50 / 12, 26, 4);
	}

	// Posicao esquerda meio
	if (face_frontal.pos_click[1] > 0)
	{
		if (face_frontal.pos_click[1] == 1){
			face_frontal.simbolo[1] = face_frontal.num_jogada % 2;
			face_frontal.pos_click[1]++;
		}
		if (face_frontal.simbolo[1] == 0) // Bolas
			Desenha_Circulo_face_frontal_tras(-25 + 50 / 6, 0, 26, 4);
		else // Cruzes
			Desenha_X_face_frontal(-25 + 50 / 12, 50 / 12, 26, 4);
	}

	// Posicao esquerda baixo
	if (face_frontal.pos_click[2] > 0)
	{
		if (face_frontal.pos_click[2] == 1){
			face_frontal.simbolo[2] = face_frontal.num_jogada % 2;
			face_frontal.pos_click[2]++;
		}

		if (face_frontal.simbolo[2] == 0) // Bolas
			Desenha_Circulo_face_frontal_tras(-25 + 50 / 6, -25 + 50 / 3 - 50 / 6, 26, 4);
		else // Cruzes
			Desenha_X_face_frontal(-25 + 50 / 12, -25 + 50 / 3 - 50 / 12, 26, 4);
	}

	// Posicao meio cima
	if (face_frontal.pos_click[3] > 0)
	{
		if (face_frontal.pos_click[3] == 1){
			face_frontal.simbolo[3] = face_frontal.num_jogada % 2;
			face_frontal.pos_click[3]++;
		}

		if (face_frontal.simbolo[3] == 0) // Bolas
			Desenha_Circulo_face_frontal_tras(0, 25 - 50 / 6, 26, 4);
		else // Cruzes
			Desenha_X_face_frontal(-25 + 50 / 3 + 50 / 12, 25 - 50 / 12, 26, 4);
	}

	// Posicao meio
	if (face_frontal.pos_click[4] > 0)
	{
		if (face_frontal.pos_click[4] == 1){
			face_frontal.simbolo[4] = face_frontal.num_jogada % 2;
			face_frontal.pos_click[4]++;
		}

		if (face_frontal.simbolo[4] == 0) // Bolas
			Desenha_Circulo_face_frontal_tras(0, 0, 26, 4);
		else // Cruzes
			Desenha_X_face_frontal(-25 + 50 / 3 + 50 / 12, 25 - 50 / 3 - 50 / 12, 26, 4);
	}

	// Posicao meio baixo
	if (face_frontal.pos_click[5] > 0)
	{
		if (face_frontal.pos_click[5] == 1){
			face_frontal.simbolo[5] = face_frontal.num_jogada % 2;
			face_frontal.pos_click[5]++;
		}

		if (face_frontal.simbolo[5] == 0) // Bolas
			Desenha_Circulo_face_frontal_tras(0, -25 + 50 / 3 - 50 / 6, 26, 4);
		else // Cruzes
			Desenha_X_face_frontal(-25 + 50 / 3 + 50 / 12, -25 + 50 / 3 - 50 / 12, 26, 4);
	}

	// Posicao direita cima
	if (face_frontal.pos_click[6] > 0)
	{
		if (face_frontal.pos_click[6] == 1){
			face_frontal.simbolo[6] = face_frontal.num_jogada % 2;
			face_frontal.pos_click[6]++;
		}

		if (face_frontal.simbolo[6] == 0) // Bolas
			Desenha_Circulo_face_frontal_tras(25 - 50 / 6, 25 - 50 / 6, 26, 4);
		else // Cruzes
			Desenha_X_face_frontal(25 - 50 / 3 + 50 / 12, 25 - 50 / 12, 26, 4);
	}

	// Posicao direita meio
	if (face_frontal.pos_click[7] > 0)
	{
		if (face_frontal.pos_click[7] == 1){
			face_frontal.simbolo[7] = face_frontal.num_jogada % 2;
			face_frontal.pos_click[7]++;
		}

		if (face_frontal.simbolo[7] == 0) // Bolas
			Desenha_Circulo_face_frontal_tras(25 - 50 / 6, 0, 26, 4);
		else // Cruzes
			Desenha_X_face_frontal(25 - 50 / 3 + 50 / 12, 25 - 50 / 3 - 50 / 12, 26, 4);
	}

	// Posicao direita baixo
	if (face_frontal.pos_click[8] > 0)
	{
		if (face_frontal.pos_click[8] == 1){
			face_frontal.simbolo[8] = face_frontal.num_jogada % 2;
			face_frontal.pos_click[8]++;
		}

		if (face_frontal.simbolo[8] == 0) // Bolas
			Desenha_Circulo_face_frontal_tras(25 - 50 / 6, -25 + 50 / 6, 26, 4);
		else // Cruzes
			Desenha_X_face_frontal(25 - 50 / 3 + 50 / 12, -25 + 50 / 3 - 50 / 12, 26, 4);
	}


	/* ------------------------------------------------------------ */
	/*							FACE DA DIREITA                    */
	/* ---------------------------------------------------------- */

	// Posicao esquerda cima
	if (face_direita.pos_click[0] > 0)
	{
		/* Esta condicao serve para guardar o simbolo associado a esta posicao */
		if (face_direita.pos_click[0] == 1){
			face_direita.simbolo[0] = face_direita.num_jogada % 2;
			face_direita.pos_click[0]++;
		}

		if (face_direita.simbolo[0] == 0) // Bolas
			Desenha_Circulo_face_direita_esquerda(26, 25 - 50 / 6, 25 - 50 / 6, 4);
		else // Cruzes
			Desenha_X_face_direita(26, 25 - 50 / 12, 25 - 50 / 12, 4);
	}

	// Posicao esquerda meio
	if (face_direita.pos_click[1] > 0)
	{
		if (face_direita.pos_click[1] == 1){
			face_direita.simbolo[1] = face_direita.num_jogada % 2;
			face_direita.pos_click[1]++;
		}

		if (face_direita.simbolo[1] == 0) // Bolas
			Desenha_Circulo_face_direita_esquerda(26, 0, 25 - 50 / 6, 4);
		else // Cruzes
			Desenha_X_face_direita(26, 50 / 12, 25 - 50 / 12, 4);
	}

	// Posicao esquerda baixo
	if (face_direita.pos_click[2] > 0)
	{
		if (face_direita.pos_click[2] == 1){
			face_direita.simbolo[2] = face_direita.num_jogada % 2;
			face_direita.pos_click[2]++;
		}

		if (face_direita.simbolo[2] == 0) // Bolas
			Desenha_Circulo_face_direita_esquerda(26, -25 + 50 / 6, 25 - 50 / 6, 4);
		else // Cruzes
			Desenha_X_face_direita(26, -50 / 6 - 50 / 12, 25 - 50 / 12, 4);
	}

	// Posicao meio cima
	if (face_direita.pos_click[3] > 0)
	{
		if (face_direita.pos_click[3] == 1){
			face_direita.simbolo[3] = face_direita.num_jogada % 2;
			face_direita.pos_click[3]++;
		}

		if (face_direita.simbolo[3] == 0) // Bolas
			Desenha_Circulo_face_direita_esquerda(26, 50 / 3, 0, 4);
		else // Cruzes
			Desenha_X_face_direita(26, 25 - 50 / 12, 50 / 12, 4);
	}

	// Posicao meio
	if (face_direita.pos_click[4] > 0)
	{
		if (face_direita.pos_click[4] == 1){
			face_direita.simbolo[4] = face_direita.num_jogada % 2;
			face_direita.pos_click[4]++;
		}

		if (face_direita.simbolo[4] == 0) // Bolas
			Desenha_Circulo_face_direita_esquerda(26, 0, 0, 4);
		else // Cruzes
			Desenha_X_face_direita(26, 50 / 12, 50 / 12, 4);
	}

	// Posicao meio baixo
	if (face_direita.pos_click[5] > 0)
	{
		if (face_direita.pos_click[5] == 1){
			face_direita.simbolo[5] = face_direita.num_jogada % 2;
			face_direita.pos_click[5]++;
		}

		if (face_direita.simbolo[5] == 0) // Bolas
			Desenha_Circulo_face_direita_esquerda(26, -50 / 3, 0, 4);
		else // Cruzes
			Desenha_X_face_direita(26, -50 / 6 - 50 / 12, 50 / 12, 4);
	}

	// Posicao direita cima
	if (face_direita.pos_click[6] > 0)
	{
		if (face_direita.pos_click[6] == 1){
			face_direita.simbolo[6] = face_direita.num_jogada % 2;
			face_direita.pos_click[6]++;
		}

		if (face_direita.simbolo[6] == 0) // Bolas
			Desenha_Circulo_face_direita_esquerda(26, 50 / 3, -25 + 50 / 6, 4);
		else // Cruzes
			Desenha_X_face_direita(26, 25 - 50 / 12, -50 / 6 - 50 / 12, 4);
	}

	// Posicao direita meio
	if (face_direita.pos_click[7] > 0)
	{
		if (face_direita.pos_click[7] == 1){
			face_direita.simbolo[7] = face_direita.num_jogada % 2;
			face_direita.pos_click[7]++;
		}

		if (face_direita.simbolo[7] == 0) // Bolas
			Desenha_Circulo_face_direita_esquerda(26, 0, -50 / 3, 4);
		else // Cruzes
			Desenha_X_face_direita(26, 50 / 12, -50 / 6 - 50 / 12, 4);
	}

	// Posicao direita baixo
	if (face_direita.pos_click[8] > 0)
	{
		if (face_direita.pos_click[8] == 1){
			face_direita.simbolo[8] = face_direita.num_jogada % 2;
			face_direita.pos_click[8]++;
		}

		if (face_direita.simbolo[8] == 0) // Bolas
			Desenha_Circulo_face_direita_esquerda(26, -25 + 50 / 6, -25 + 50 / 6, 4);
		else // Cruzes
			Desenha_X_face_direita(26, -50 / 6 - 50 / 12, -50 / 6 - 50 / 12, 4);
	}

	/* ------------------------------------------------------------ */
	/*							FACE DA ESQUERDA                   */
	/* ---------------------------------------------------------- */

	// Posicao esquerda cima
	if (face_esquerda.pos_click[0] > 0)
	{
		/* Esta condicao serve para guardar o simbolo associado a esta posicao */
		if (face_esquerda.pos_click[0] == 1){
			face_esquerda.simbolo[0] = face_esquerda.num_jogada % 2;
			face_esquerda.pos_click[0]++;
		}

		if (face_esquerda.simbolo[0] == 0) // Bolas
			Desenha_Circulo_face_direita_esquerda(-26, 25 - 50 / 6, -25 + 50 / 6, 4);
		else // Cruzes
			Desenha_X_face_esquerda(-26, 25 - 50 / 12, -25 + 50 / 12, 4);
	}

	// Posicao esquerda meio
	if (face_esquerda.pos_click[1] > 0)
	{
		if (face_esquerda.pos_click[1] == 1){
			face_esquerda.simbolo[1] = face_esquerda.num_jogada % 2;
			face_esquerda.pos_click[1]++;
		}

		if (face_esquerda.simbolo[1] == 0) // Bolas
			Desenha_Circulo_face_direita_esquerda(-26, 0, -25 + 50 / 6, 4);
		else // Cruzes
			Desenha_X_face_esquerda(-26, 50 / 12, -25 + 50 / 12, 4);
	}

	// Posicao esquerda baixo
	if (face_esquerda.pos_click[2] > 0)
	{
		if (face_esquerda.pos_click[2] == 1){
			face_esquerda.simbolo[2] = face_esquerda.num_jogada % 2;
			face_esquerda.pos_click[2]++;
		}

		if (face_esquerda.simbolo[2] == 0) // Bolas
			Desenha_Circulo_face_direita_esquerda(-26, -25 + 50 / 6, -25 + 50 / 6, 4);
		else // Cruzes
			Desenha_X_face_esquerda(-26, -50 / 6 - 50 / 12, -25 + 50 / 12, 4);
	}

	// Posicao meio cima
	if (face_esquerda.pos_click[3] > 0)
	{
		if (face_esquerda.pos_click[3] == 1){
			face_esquerda.simbolo[3] = face_esquerda.num_jogada % 2;
			face_esquerda.pos_click[3]++;
		}

		if (face_esquerda.simbolo[3] == 0) // Bolas
			Desenha_Circulo_face_direita_esquerda(-26, 50 / 3, 0, 4);
		else // Cruzes
			Desenha_X_face_esquerda(-26, 25 - 50 / 12, -50 / 12, 4);
	}

	// Posicao meio
	if (face_esquerda.pos_click[4] > 0)
	{
		if (face_esquerda.pos_click[4] == 1){
			face_esquerda.simbolo[4] = face_esquerda.num_jogada % 2;
			face_esquerda.pos_click[4]++;
		}

		if (face_esquerda.simbolo[4] == 0) // Bolas
			Desenha_Circulo_face_direita_esquerda(-26, 0, 0, 4);
		else // Cruzes
			Desenha_X_face_esquerda(-26, 50 / 12, -50 / 12, 4);
	}

	// Posicao meio baixo
	if (face_esquerda.pos_click[5] > 0)
	{
		if (face_esquerda.pos_click[5] == 1){
			face_esquerda.simbolo[5] = face_esquerda.num_jogada % 2;
			face_esquerda.pos_click[5]++;
		}

		if (face_esquerda.simbolo[5] == 0) // Bolas
			Desenha_Circulo_face_direita_esquerda(-26, -50 / 3, 0, 4);
		else // Cruzes
			Desenha_X_face_esquerda(-26, -50 / 6 - 50 / 12, -50 / 12, 4);
	}

	// Posicao direita cima
	if (face_esquerda.pos_click[6] > 0)
	{
		if (face_esquerda.pos_click[6] == 1){
			face_esquerda.simbolo[6] = face_esquerda.num_jogada % 2;
			face_esquerda.pos_click[6]++;
		}

		if (face_esquerda.simbolo[6] == 0) // Bolas
			Desenha_Circulo_face_direita_esquerda(-26, 50 / 3, +25 - 50 / 6, 4);
		else // Cruzes
			Desenha_X_face_esquerda(-26, 25 - 50 / 12, +50 / 6 + 50 / 12, 4);
	}

	// Posicao direita meio
	if (face_esquerda.pos_click[7] > 0)
	{
		if (face_esquerda.pos_click[7] == 1){
			face_esquerda.simbolo[7] = face_esquerda.num_jogada % 2;
			face_esquerda.pos_click[7]++;
		}

		if (face_esquerda.simbolo[7] == 0) // Bolas
			Desenha_Circulo_face_direita_esquerda(-26, 0, +50 / 3, 4);
		else // Cruzes
			Desenha_X_face_esquerda(-26, 50 / 12, +50 / 6 + 50 / 12, 4);
	}

	// Posicao direita baixo
	if (face_esquerda.pos_click[8] > 0)
	{
		if (face_esquerda.pos_click[8] == 1){
			face_esquerda.simbolo[8] = face_esquerda.num_jogada % 2;
			face_esquerda.pos_click[8]++;
		}

		if (face_esquerda.simbolo[8] == 0) // Bolas
			Desenha_Circulo_face_direita_esquerda(-26, -25 + 50 / 6, +25 - 50 / 6, 4);
		else // Cruzes
			Desenha_X_face_esquerda(-26, -50 / 6 - 50 / 12, +50 / 6 + 50 / 12, 4);
	}



	/* ------------------------------------------------------------ */
	/*							FACE DE TRAS                       */
	/* ---------------------------------------------------------- */

	// Posicao esquerda cima
	if (face_tras.pos_click[0] > 0)
	{
		/* Esta condicao serve para guardar o simbolo associado a esta posicao */
		if (face_tras.pos_click[0] == 1){
			face_tras.simbolo[0] = face_tras.num_jogada % 2;
			face_tras.pos_click[0]++;
		}

		if (face_tras.simbolo[0] == 0) // Bolas
			Desenha_Circulo_face_frontal_tras(25 - 50 / 6, 25 - 50 / 6, -26, 4);
		else // Cruzes
			Desenha_X_face_tras(+25 - 50 / 12, 25 - 50 / 12, -26, 4);
	}

	// Posicao esquerda meio
	if (face_tras.pos_click[1] > 0)
	{
		if (face_tras.pos_click[1] == 1){
			face_tras.simbolo[1] = face_tras.num_jogada % 2;
			face_tras.pos_click[1]++;
		}
		if (face_tras.simbolo[1] == 0) // Bolas
			Desenha_Circulo_face_frontal_tras(25 - 50 / 6, 0, -26, 4);
		else // Cruzes
			Desenha_X_face_tras(+25 - 50 / 12, 50 / 12, -26, 4);
	}

	// Posicao esquerda baixo
	if (face_tras.pos_click[2] > 0)
	{
		if (face_tras.pos_click[2] == 1){
			face_tras.simbolo[2] = face_tras.num_jogada % 2;
			face_tras.pos_click[2]++;
		}

		if (face_tras.simbolo[2] == 0) // Bolas
			Desenha_Circulo_face_frontal_tras(25 - 50 / 6, -25 + 50 / 3 - 50 / 6, -26, 4);
		else // Cruzes
			Desenha_X_face_tras(25 - 50 / 12, -50 / 6 - 50 / 12, -26, 4);
	}

	// Posicao meio cima
	if (face_tras.pos_click[3] > 0)
	{
		if (face_tras.pos_click[3] == 1){
			face_tras.simbolo[3] = face_tras.num_jogada % 2;
			face_tras.pos_click[3]++;
		}

		if (face_tras.simbolo[3] == 0) // Bolas
			Desenha_Circulo_face_frontal_tras(0, 25 - 50 / 6, -26, 4);
		else // Cruzes
			Desenha_X_face_tras(25 - 50 / 3 - 50 / 12, 25 - 50 / 12, -26, 4);
	}

	// Posicao meio
	if (face_tras.pos_click[4] > 0)
	{
		if (face_tras.pos_click[4] == 1){
			face_tras.simbolo[4] = face_tras.num_jogada % 2;
			face_tras.pos_click[4]++;
		}

		if (face_tras.simbolo[4] == 0) // Bolas
			Desenha_Circulo_face_frontal_tras(0, 0, -26, 4);
		else // Cruzes
			Desenha_X_face_tras(25 - 50 / 3 - 50 / 12, 25 - 50 / 3 - 50 / 12, -26, 4);
	}

	// Posicao meio baixo
	if (face_tras.pos_click[5] > 0)
	{
		if (face_tras.pos_click[5] == 1){
			face_tras.simbolo[5] = face_tras.num_jogada % 2;
			face_tras.pos_click[5]++;
		}

		if (face_tras.simbolo[5] == 0) // Bolas
			Desenha_Circulo_face_frontal_tras(0, -25 + 50 / 3 - 50 / 6, -26, 4);
		else // Cruzes
			Desenha_X_face_tras(25 - 50 / 3 - 50 / 12, -25 + 50 / 3 - 50 / 12, -26, 4);
	}

	// Posicao direita cima
	if (face_tras.pos_click[6] > 0)
	{
		if (face_tras.pos_click[6] == 1){
			face_tras.simbolo[6] = face_tras.num_jogada % 2;
			face_tras.pos_click[6]++;
		}

		if (face_tras.simbolo[6] == 0) // Bolas
			Desenha_Circulo_face_frontal_tras(-25 + 50 / 6, 25 - 50 / 6, -26, 4);
		else // Cruzes
			Desenha_X_face_tras(-25 + 50 / 3 - 50 / 12, 25 - 50 / 12, -26, 4);
	}

	// Posicao direita meio
	if (face_tras.pos_click[7] > 0)
	{
		if (face_tras.pos_click[7] == 1){
			face_tras.simbolo[7] = face_tras.num_jogada % 2;
			face_tras.pos_click[7]++;
		}

		if (face_tras.simbolo[7] == 0) // Bolas
			Desenha_Circulo_face_frontal_tras(-25 + 50 / 6, 0, -26, 4);
		else // Cruzes
			Desenha_X_face_tras(-25 + 50 / 3 - 50 / 12, 25 - 50 / 3 - 50 / 12, -26, 4);
	}

	// Posicao direita baixo
	if (face_tras.pos_click[8] > 0)
	{
		if (face_tras.pos_click[8] == 1){
			face_tras.simbolo[8] = face_tras.num_jogada % 2;
			face_tras.pos_click[8]++;
		}

		if (face_tras.simbolo[8] == 0) // Bolas
			Desenha_Circulo_face_frontal_tras(-25 + 50 / 6, -25 + 50 / 6, -26, 4);
		else // Cruzes
			Desenha_X_face_tras(-25 + 50 / 3 - 50 / 12, -25 + 50 / 3 - 50 / 12, -26, 4);
	}


	/* ------------------------------------------------------------ */
	/*							FACE DE CIMA                       */
	/* ---------------------------------------------------------- */

	// Posicao esquerda cima
	if (face_cima.pos_click[0] > 0)
	{
		/* Esta condicao serve para guardar o simbolo associado a esta posicao */
		if (face_cima.pos_click[0] == 1){
			face_cima.simbolo[0] = face_cima.num_jogada % 2;
			face_cima.pos_click[0]++;
		}

		if (face_cima.simbolo[0] == 0) // Bolas
			Desenha_Circulo_face_cima_baixo(-25 + 50 / 6, 26, -25 + 50 / 6, 4);
		else // Cruzes
			Desenha_X_face_cima(-25 + 50 / 12, 26, -25 + 50 / 12, 4);
	}

	// Posicao esquerda meio
	if (face_cima.pos_click[1] > 0)
	{
		if (face_cima.pos_click[1] == 1){
			face_cima.simbolo[1] = face_cima.num_jogada % 2;
			face_cima.pos_click[1]++;
		}
		if (face_cima.simbolo[1] == 0) // Bolas
			Desenha_Circulo_face_cima_baixo(-25 + 50 / 6, 26, 0, 4);
		else // Cruzes
			Desenha_X_face_cima(-25 + 50 / 12, 26, -50 / 12, 4);
	}

	// Posicao esquerda baixo
	if (face_cima.pos_click[2] > 0)
	{
		if (face_cima.pos_click[2] == 1){
			face_cima.simbolo[2] = face_cima.num_jogada % 2;
			face_cima.pos_click[2]++;
		}

		if (face_cima.simbolo[2] == 0) // Bolas
			Desenha_Circulo_face_cima_baixo(-25 + 50 / 6, 26, 25 - 50 / 3 + 50 / 6, 4);
		else // Cruzes
			Desenha_X_face_cima(-25 + 50 / 12, 26, 25 - 50 / 3 + 50 / 12, 4);
	}

	// Posicao meio cima
	if (face_cima.pos_click[3] > 0)
	{
		if (face_cima.pos_click[3] == 1){
			face_cima.simbolo[3] = face_cima.num_jogada % 2;
			face_cima.pos_click[3]++;
		}

		if (face_cima.simbolo[3] == 0) // Bolas
			Desenha_Circulo_face_cima_baixo(0, 26, -25 + 50 / 6, 4);
		else // Cruzes
			Desenha_X_face_cima(-25 + 50 / 3 + 50 / 12, 26, -25 + 50 / 12, 4);
	}

	// Posicao meio
	if (face_cima.pos_click[4] > 0)
	{
		if (face_cima.pos_click[4] == 1){
			face_cima.simbolo[4] = face_cima.num_jogada % 2;
			face_cima.pos_click[4]++;
		}

		if (face_cima.simbolo[4] == 0) // Bolas
			Desenha_Circulo_face_cima_baixo(0, 26, 0, 4);
		else // Cruzes
			Desenha_X_face_cima(-25 + 50 / 3 + 50 / 12, 26, -25 + 50 / 3 + 50 / 12, 4);
	}

	// Posicao meio baixo
	if (face_cima.pos_click[5] > 0)
	{
		if (face_cima.pos_click[5] == 1){
			face_cima.simbolo[5] = face_cima.num_jogada % 2;
			face_cima.pos_click[5]++;
		}

		if (face_cima.simbolo[5] == 0) // Bolas
			Desenha_Circulo_face_cima_baixo(0, 26, 25 - 50 / 3 + 50 / 6, 4);
		else // Cruzes
			Desenha_X_face_cima(-25 + 50 / 3 + 50 / 12, 26, 25 - 50 / 3 + 50 / 12, 4);
	}

	// Posicao direita cima
	if (face_cima.pos_click[6] > 0)
	{
		if (face_cima.pos_click[6] == 1){
			face_cima.simbolo[6] = face_cima.num_jogada % 2;
			face_cima.pos_click[6]++;
		}

		if (face_cima.simbolo[6] == 0) // Bolas
			Desenha_Circulo_face_cima_baixo(25 - 50 / 6, 26, -25 + 50 / 6, 4);
		else // Cruzes
			Desenha_X_face_cima(25 - 50 / 3 + 50 / 12, 26, -25 + 50 / 12, 4);
	}

	// Posicao direita meio
	if (face_cima.pos_click[7] > 0)
	{
		if (face_cima.pos_click[7] == 1){
			face_cima.simbolo[7] = face_cima.num_jogada % 2;
			face_cima.pos_click[7]++;
		}

		if (face_cima.simbolo[7] == 0) // Bolas
			Desenha_Circulo_face_cima_baixo(25 - 50 / 6, 26, 0, 4);
		else // Cruzes
			Desenha_X_face_cima(25 - 50 / 3 + 50 / 12, 26, -25 + 50 / 3 + 50 / 12, 4);
	}

	// Posicao direita baixo
	if (face_cima.pos_click[8] > 0)
	{
		if (face_cima.pos_click[8] == 1){
			face_cima.simbolo[8] = face_cima.num_jogada % 2;
			face_cima.pos_click[8]++;
		}

		if (face_cima.simbolo[8] == 0) // Bolas
			Desenha_Circulo_face_cima_baixo(25 - 50 / 6, 26, 25 - 50 / 6, 4);
		else // Cruzes
			Desenha_X_face_cima(25 - 50 / 3 + 50 / 12, 26, 25 - 50 / 3 + 50 / 12, 4);
	}


	/* ------------------------------------------------------------- */
	/*							FACE DE BAIXO                       */
	/* ----------------------------------------------------------- */

	// Posicao esquerda cima
	if (face_baixo.pos_click[0] > 0)
	{
		/* Esta condicao serve para guardar o simbolo associado a esta posicao */
		if (face_baixo.pos_click[0] == 1){
			face_baixo.simbolo[0] = face_baixo.num_jogada % 2;
			face_baixo.pos_click[0]++;
		}

		if (face_baixo.simbolo[0] == 0) // Bolas
			Desenha_Circulo_face_cima_baixo(-25 + 50 / 6, -26, 25 - 50 / 6, 4);
		else // Cruzes
			Desenha_X_face_baixo(-25 + 50 / 12, -26, +25 - 50 / 12, 4);
	}

	// Posicao esquerda meio
	if (face_baixo.pos_click[1] > 0)
	{
		if (face_baixo.pos_click[1] == 1){
			face_baixo.simbolo[1] = face_baixo.num_jogada % 2;
			face_baixo.pos_click[1]++;
		}
		if (face_baixo.simbolo[1] == 0) // Bolas
			Desenha_Circulo_face_cima_baixo(-25 + 50 / 6, -26, 0, 4);
		else // Cruzes
			Desenha_X_face_baixo(-25 + 50 / 12, -26, 50 / 12, 4);
	}

	// Posicao esquerda baixo
	if (face_baixo.pos_click[2] > 0)
	{
		if (face_baixo.pos_click[2] == 1){
			face_baixo.simbolo[2] = face_baixo.num_jogada % 2;
			face_baixo.pos_click[2]++;
		}

		if (face_baixo.simbolo[2] == 0) // Bolas
			Desenha_Circulo_face_cima_baixo(-25 + 50 / 6, -26, -25 + 50 / 3 - 50 / 6, 4);
		else // Cruzes
			Desenha_X_face_baixo(-25 + 50 / 12, -26, -25 + 50 / 3 - 50 / 12, 4);
	}

	// Posicao meio cima
	if (face_baixo.pos_click[3] > 0)
	{
		if (face_baixo.pos_click[3] == 1){
			face_baixo.simbolo[3] = face_baixo.num_jogada % 2;
			face_baixo.pos_click[3]++;
		}

		if (face_baixo.simbolo[3] == 0) // Bolas
			Desenha_Circulo_face_cima_baixo(0, -26, 25 - 50 / 6, 4);
		else // Cruzes
			Desenha_X_face_baixo(-25 + 50 / 3 + 50 / 12, -26, 25 - 50 / 12, 4);
	}

	// Posicao meio
	if (face_baixo.pos_click[4] > 0)
	{
		if (face_baixo.pos_click[4] == 1){
			face_baixo.simbolo[4] = face_baixo.num_jogada % 2;
			face_baixo.pos_click[4]++;
		}

		if (face_baixo.simbolo[4] == 0) // Bolas
			Desenha_Circulo_face_cima_baixo(0, -26, 0, 4);
		else // Cruzes
			Desenha_X_face_baixo(-25 + 50 / 3 + 50 / 12, -26, 25 - 50 / 3 - 50 / 12, 4);
	}

	// Posicao meio baixo
	if (face_baixo.pos_click[5] > 0)
	{
		if (face_baixo.pos_click[5] == 1){
			face_baixo.simbolo[5] = face_baixo.num_jogada % 2;
			face_baixo.pos_click[5]++;
		}

		if (face_baixo.simbolo[5] == 0) // Bolas
			Desenha_Circulo_face_cima_baixo(0, -26, -25 + 50 / 3 - 50 / 6, 4);
		else // Cruzes
			Desenha_X_face_baixo(-25 + 50 / 3 + 50 / 12, -26, -25 + 50 / 3 - 50 / 12, 4);
	}

	// Posicao direita cima
	if (face_baixo.pos_click[6] > 0)
	{
		if (face_baixo.pos_click[6] == 1){
			face_baixo.simbolo[6] = face_baixo.num_jogada % 2;
			face_baixo.pos_click[6]++;
		}

		if (face_baixo.simbolo[6] == 0) // Bolas
			Desenha_Circulo_face_cima_baixo(25 - 50 / 6, -26, 25 - 50 / 6, 4);
		else // Cruzes
			Desenha_X_face_baixo(25 - 50 / 3 + 50 / 12, -26, 25 - 50 / 12, 4);
	}

	// Posicao direita meio
	if (face_baixo.pos_click[7] > 0)
	{
		if (face_baixo.pos_click[7] == 1){
			face_baixo.simbolo[7] = face_baixo.num_jogada % 2;
			face_baixo.pos_click[7]++;
		}

		if (face_baixo.simbolo[7] == 0) // Bolas
			Desenha_Circulo_face_cima_baixo(25 - 50 / 6, -26, 0, 4);
		else // Cruzes
			Desenha_X_face_baixo(25 - 50 / 3 + 50 / 12, -26, 25 - 50 / 3 - 50 / 12, 4);
	}

	// Posicao direita baixo
	if (face_baixo.pos_click[8] > 0)
	{
		if (face_baixo.pos_click[8] == 1){
			face_baixo.simbolo[8] = face_baixo.num_jogada % 2;
			face_baixo.pos_click[8]++;
		}

		if (face_baixo.simbolo[8] == 0) // Bolas
			Desenha_Circulo_face_cima_baixo(25 - 50 / 6, -26, -25 + 50 / 6, 4);
		else // Cruzes
			Desenha_X_face_baixo(25 - 50 / 3 + 50 / 12, -26, -25 + 50 / 3 - 50 / 12, 4);
	}

}

void redimensiona_janela(int Larg, int Alt) {


	largura = Larg;
	altura = Alt;

	if (altura < 768 || largura < 1024) {
		altura = 768;
		largura = 1024;
		glutReshapeWindow(largura, altura);
	}

	glViewport(0, 0, Larg, Alt);
	razao_aspeto = (double)Larg / (double)Alt;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(campo_visao_y, razao_aspeto, limite_z_anterior, limite_z_posterior);
	glMatrixMode(GL_MODELVIEW);
}

/* Traduz coordenadas do mundo para as da cena */
struct pos GetOGLPos(int x, int y)
{
	struct pos temp_pos;

	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	winX = (float)x;
	winY = (float)viewport[3] - (float)y;
	glReadPixels(x, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

	gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

	if (modo_jogo != 9 && modo_jogo != 3 && PC_turno==0)
	{
		/* ------------------------------------------------------------ */
		/*							FACE DA FRENTE                     */
		/* ---------------------------------------------------------- */

		// Posicao esquerda cima
		if (posZ < 25 + margem_grelha && posZ > 25 - margem_grelha
			&& posX < -50 / 6 - margem_grelha && posX > -25 + margem_grelha
			&& posY > 50 / 6 + margem_grelha && posY < 25 - margem_grelha)
		{
			if (face_frontal.pos_click[0] == 0)
			{
				face_frontal.num_jogada++;
				face_frontal.pos_click[0]++;
			}
			if (modo_jogo == 1)
				PC_turno = 1;
			face = 0;
		}


		// Posicao esquerda meio
		else if (posZ < 25 + margem_grelha && posZ > 25 - margem_grelha
			&&   posX < -50 / 6 - margem_grelha && posX > -25 + margem_grelha
			&&   posY < 50 / 6 - margem_grelha && posY > -50 / 6 + margem_grelha)
		{
			if (face_frontal.pos_click[1] == 0)
			{
				face_frontal.num_jogada++;
				face_frontal.pos_click[1]++;
			}
			if (modo_jogo == 1)
				PC_turno = 1;

			face = 0;
		}

		// Posicao esquerda baixo
		else if (posZ < 25 + margem_grelha && posZ > 25 - margem_grelha
			&&   posX < -50 / 6 - margem_grelha && posX > -25 + margem_grelha
			&&   posY < -50 / 6 - margem_grelha && posY > -25 + margem_grelha)
		{
			if (face_frontal.pos_click[2] == 0)
			{
				face_frontal.num_jogada++;
				face_frontal.pos_click[2]++;
			}
			face = 0;
			if (modo_jogo == 1)
				PC_turno = 1;

		}

		// Posicao meio cima
		else if (posZ < 25 + margem_grelha && posZ > 25 - margem_grelha
			&&   posX > -50 / 6 + margem_grelha && posX < 50 / 6 - margem_grelha
			&&   posY > 50 / 6 + margem_grelha && posY < 25 - margem_grelha)
		{
			if (face_frontal.pos_click[3] == 0)
			{
				face_frontal.num_jogada++;
				face_frontal.pos_click[3]++;
			}
			face = 0;
			if (modo_jogo == 1)
				PC_turno = 1;

		}

		// Posicao do meio
		else if (posZ < 25 + margem_grelha && posZ > 25 - margem_grelha
			&&   posX < 50 / 6 - margem_grelha && posX > -50 / 6 + margem_grelha
			&&   posY > -50 / 6 + margem_grelha && posY < 50 / 6 - margem_grelha)
		{
			if (face_frontal.pos_click[4] == 0)
			{
				face_frontal.num_jogada++;
				face_frontal.pos_click[4]++;
			}
			face = 0;
			if (modo_jogo == 1)
				PC_turno = 1;

		}

		// Posicao meio baixo
		else if (posZ < 25 + margem_grelha && posZ > 25 - margem_grelha
			&&   posX > -50 / 6 + margem_grelha && posX < 50 / 6 - margem_grelha
			&&   posY < -50 / 6 - margem_grelha && posY > -25 + margem_grelha)
		{
			if (face_frontal.pos_click[5] == 0)
			{
				face_frontal.num_jogada++;
				face_frontal.pos_click[5]++;
			}
			face = 0;
			if (modo_jogo == 1)
				PC_turno = 1;

		}

		// Posicao direita cima
		else if (posZ < 25 + margem_grelha && posZ > 25 - margem_grelha
			&&   posX < 25 - margem_grelha && posX > 50 / 6 + margem_grelha
			&&   posY > 50 / 6 + margem_grelha && posY < 25 - margem_grelha)
		{
			if (face_frontal.pos_click[6] == 0)
			{
				face_frontal.num_jogada++;
				face_frontal.pos_click[6]++;
			}
			face = 0;
			if (modo_jogo == 1)
				PC_turno = 1;

		}

		// Posicao direita meio
		else if (posZ < 25 + margem_grelha && posZ > 25 - margem_grelha
			&&   posX < 25 - margem_grelha && posX > 50 / 6 + margem_grelha
			&&   posY > -50 / 6 + margem_grelha && posY < 50 / 6 - margem_grelha)
		{
			if (face_frontal.pos_click[7] == 0)
			{
				face_frontal.num_jogada++;
				face_frontal.pos_click[7]++;
			}
			face = 0;
			if (modo_jogo == 1)
				PC_turno = 1;

		}

		// Posicao direita baixo
		else if (posZ < 25 + margem_grelha && posZ > 25 - margem_grelha
			&&   posX < 25 - margem_grelha && posX > 50 / 6 + margem_grelha
			&&   posY < -50 / 6 - margem_grelha && posY > -25 + margem_grelha)
		{
			if (face_frontal.pos_click[8] == 0)
			{
				face_frontal.num_jogada++;
				face_frontal.pos_click[8]++;
			}
			face = 0;
			if (modo_jogo == 1)
				PC_turno = 1;

		}


		/* ------------------------------------------------------------- */
		/*							FACE DA DIREITA                     */
		/* ----------------------------------------------------------- */

		// Posicao esquerda cima
		if (posX < 25 + face_direita.margem_face && posX > 25 - face_direita.margem_face
			&& posY > 50 / 6 && posY < 25
			&& posZ > 50 / 6 && posZ < 25){
			if (face_direita.pos_click[0] == 0)
			{
				face_direita.num_jogada++;
				face_direita.pos_click[0]++;
			}
			face = 1;
			if (modo_jogo == 1)
				PC_turno = 1;

		}

		// Posicao esquerda meio
		if (posX < 25 + face_direita.margem_face && posX > 25 - face_direita.margem_face
			&& posY > -50 / 6 && posY < 50 / 6
			&& posZ > 50 / 6 && posZ < 25){
			if (face_direita.pos_click[1] == 0)
			{
				face_direita.num_jogada++;
				face_direita.pos_click[1]++;
			}
			face = 1;
			if (modo_jogo == 1)
				PC_turno = 1;

		}

		// Posicao esquerda baixo
		if (posX < 25 + face_direita.margem_face && posX > 25 - face_direita.margem_face
			&& posY < -50 / 6 && posY > -25
			&& posZ > 50 / 6 && posZ < 25){
			if (face_direita.pos_click[2] == 0)
			{
				face_direita.num_jogada++;
				face_direita.pos_click[2]++;
			}
			face = 1;
			if (modo_jogo == 1)
				PC_turno = 1;

		}

		// Posicao meio cima
		if (posX < 25 + face_direita.margem_face && posX > 25 - face_direita.margem_face
			&& posY > 50 / 6 && posY < 25
			&& posZ > -50 / 6 && posZ < 50 / 6){
			if (face_direita.pos_click[3] == 0)
			{
				face_direita.num_jogada++;
				face_direita.pos_click[3]++;
			}
			face = 1;
			if (modo_jogo == 1)
				PC_turno = 1;

		}

		// Posicao meio
		if (posX < 25 + face_direita.margem_face && posX > 25 - face_direita.margem_face
			&& posY > -50 / 6 && posY < 50 / 6
			&& posZ > -50 / 6 && posZ < 50 / 6){
			if (face_direita.pos_click[4] == 0)
			{
				face_direita.num_jogada++;
				face_direita.pos_click[4]++;
			}
			face = 1;
			if (modo_jogo == 1)
				PC_turno = 1;

		}

		// Posicao meio baixo
		if (posX < 25 + face_direita.margem_face && posX > 25 - face_direita.margem_face
			&& posY > -25 && posY < -50 / 6
			&& posZ > -50 / 6 && posZ < 50 / 6){
			if (face_direita.pos_click[5] == 0)
			{
				face_direita.num_jogada++;
				face_direita.pos_click[5]++;
			}
			face = 1;
			if (modo_jogo == 1)
				PC_turno = 1;

		}

		// Posicao direita cima
		if (posX < 25 + face_direita.margem_face && posX > 25 - face_direita.margem_face
			&& posY > 50 / 6 && posY < 25
			&& posZ > -25 && posZ < -50 / 6){
			if (face_direita.pos_click[6] == 0)
			{
				face_direita.num_jogada++;
				face_direita.pos_click[6]++;
			}
			face = 1;
			if (modo_jogo == 1)
				PC_turno = 1;

		}

		// Posicao direita meio
		if (posX < 25 + face_direita.margem_face && posX > 25 - face_direita.margem_face
			&& posY > -50 / 6 && posY < 50 / 6
			&& posZ > -25 && posZ < -50 / 6){
			if (face_direita.pos_click[7] == 0)
			{
				face_direita.num_jogada++;
				face_direita.pos_click[7]++;
			}
			face = 1;
			if (modo_jogo == 1)
				PC_turno = 1;

		}

		// Posicao direita baixo
		if (posX < 25 + face_direita.margem_face && posX > 25 - face_direita.margem_face
			&& posY < -50 / 6 && posY > -25
			&& posZ > -25 && posZ < -50 / 6){
			if (face_direita.pos_click[8] == 0)
			{
				face_direita.num_jogada++;
				face_direita.pos_click[8]++;
			}
			face = 1;
			if (modo_jogo == 1)
				PC_turno = 1;

		}

		/* ------------------------------------------------------------- */
		/*							FACE DA ESQUERDA                    */
		/* ----------------------------------------------------------- */

		// Posicao esquerda cima
		if (posX < -25 + face_esquerda.margem_face && posX > -25 - face_esquerda.margem_face
			&& posY > 50 / 6 && posY < 25
			&& posZ < -50 / 6 && posZ > -25){
			if (face_esquerda.pos_click[0] == 0)
			{
				face_esquerda.num_jogada++;
				face_esquerda.pos_click[0]++;
			}
			face = 2;
			if (modo_jogo == 1)
				PC_turno = 1;

		}

		// Posicao esquerda meio
		if (posX < -25 + face_esquerda.margem_face && posX > -25 - face_esquerda.margem_face
			&& posY > -50 / 6 && posY < 50 / 6
			&& posZ < -50 / 6 && posZ > -25){
			if (face_esquerda.pos_click[1] == 0)
			{
				face_esquerda.num_jogada++;
				face_esquerda.pos_click[1]++;
			}
			face = 2;
			if (modo_jogo == 1)
				PC_turno = 1;

		}

		// Posicao esquerda baixo
		if (posX < -25 + face_esquerda.margem_face && posX > -25 - face_esquerda.margem_face
			&& posY < -50 / 6 && posY > -25
			&& posZ < -50 / 6 && posZ > -25){
			if (face_esquerda.pos_click[2] == 0)
			{
				face_esquerda.num_jogada++;
				face_esquerda.pos_click[2]++;
			}
			face = 2;
			if (modo_jogo == 1)
				PC_turno = 1;

		}

		// Posicao meio cima
		if (posX < -25 + face_esquerda.margem_face && posX > -25 - face_esquerda.margem_face
			&& posY > 50 / 6 && posY < 25
			&& posZ < 50 / 6 && posZ > -50 / 6){
			if (face_esquerda.pos_click[3] == 0)
			{
				face_esquerda.num_jogada++;
				face_esquerda.pos_click[3]++;
			}
			face = 2;
			if (modo_jogo == 1)
				PC_turno = 1;

		}

		// Posicao meio
		if (posX < -25 + face_esquerda.margem_face && posX > -25 - face_esquerda.margem_face
			&& posY > -50 / 6 && posY < 50 / 6
			&& posZ < 50 / 6 && posZ > -50 / 6){
			if (face_esquerda.pos_click[4] == 0)
			{
				face_esquerda.num_jogada++;
				face_esquerda.pos_click[4]++;
			}
			face = 2;
			if (modo_jogo == 1)
				PC_turno = 1;

		}

		// Posicao meio baixo
		if (posX < -25 + face_esquerda.margem_face && posX > -25 - face_esquerda.margem_face
			&& posY > -25 && posY < -50 / 6
			&& posZ < 50 / 6 && posZ > -50 / 6){
			if (face_esquerda.pos_click[5] == 0)
			{
				face_esquerda.num_jogada++;
				face_esquerda.pos_click[5]++;
			}
			face = 2;
			if (modo_jogo == 1)
				PC_turno = 1;

		}

		// Posicao direita cima
		if (posX < -25 + face_esquerda.margem_face && posX > -25 - face_esquerda.margem_face
			&& posY > 50 / 6 && posY < 25
			&& posZ < 25 && posZ > +50 / 6){
			if (face_esquerda.pos_click[6] == 0)
			{
				face_esquerda.num_jogada++;
				face_esquerda.pos_click[6]++;
			}
			face = 2;
			if (modo_jogo == 1)
				PC_turno = 1;

		}

		// Posicao direita meio
		if (posX < -25 + face_esquerda.margem_face && posX > -25 - face_esquerda.margem_face
			&& posY > -50 / 6 && posY < 50 / 6
			&& posZ < 25 && posZ > 50 / 6){
			if (face_esquerda.pos_click[7] == 0)
			{
				face_esquerda.num_jogada++;
				face_esquerda.pos_click[7]++;
			}
			face = 2;
			if (modo_jogo == 1)
				PC_turno = 1;

		}

		// Posicao direita baixo
		if (posX < -25 + face_esquerda.margem_face && posX > -25 - face_esquerda.margem_face
			&& posY < -50 / 6 && posY > -25
			&& posZ < 25 && posZ > 50 / 6){
			if (face_esquerda.pos_click[8] == 0)
			{
				face_esquerda.num_jogada++;
				face_esquerda.pos_click[8]++;
			}
			face = 2;
			if (modo_jogo == 1)
				PC_turno = 1;

		}


		/* ------------------------------------------------------------ */
		/*							FACE DE TRAS                       */
		/* ---------------------------------------------------------- */

		// Posicao esquerda cima
		if (posZ < -25 + margem_grelha && posZ > -25 - margem_grelha
			&& posX > +50 / 6 + margem_grelha && posX < +25 - margem_grelha
			&& posY > 50 / 6 + margem_grelha && posY < 25 - margem_grelha)
		{
			if (face_tras.pos_click[0] == 0)
			{
				face_tras.num_jogada++;
				face_tras.pos_click[0]++;
			}
			face = 3;
			if (modo_jogo == 1)
				PC_turno = 1;

		}


		// Posicao esquerda meio
		else if (posZ < -25 + margem_grelha && posZ > -25 - margem_grelha
			&&   posX > +50 / 6 + margem_grelha && posX < +25 - margem_grelha
			&&   posY < 50 / 6 - margem_grelha && posY > -50 / 6 + margem_grelha)
		{
			if (face_tras.pos_click[1] == 0)
			{
				face_tras.num_jogada++;
				face_tras.pos_click[1]++;
			}
			face = 3;
			if (modo_jogo == 1)
				PC_turno = 1;

		}

		// Posicao esquerda baixo
		else if (posZ < -25 + margem_grelha && posZ > -25 - margem_grelha
			&&   posX > +50 / 6 + margem_grelha && posX < 25 - margem_grelha
			&&   posY < -50 / 6 - margem_grelha && posY > -25 + margem_grelha)
		{
			if (face_tras.pos_click[2] == 0)
			{
				face_tras.num_jogada++;
				face_tras.pos_click[2]++;
			}
			face = 3;
			if (modo_jogo == 1)
				PC_turno = 1;
		}

		// Posicao meio cima
		else if (posZ < -25 + margem_grelha && posZ > -25 - margem_grelha
			&&   posX < +50 / 6 - margem_grelha && posX > -50 / 6 + margem_grelha
			&&   posY > 50 / 6 + margem_grelha && posY < 25 - margem_grelha)
		{
			if (face_tras.pos_click[3] == 0)
			{
				face_tras.num_jogada++;
				face_tras.pos_click[3]++;
			}
			face = 3;
			if (modo_jogo == 1)
				PC_turno = 1;
		}

		// Posicao do meio
		else if (posZ < -25 + margem_grelha && posZ > -25 - margem_grelha
			&&   posX > -50 / 6 + margem_grelha && posX < 50 / 6 - margem_grelha
			&&   posY > -50 / 6 + margem_grelha && posY < 50 / 6 - margem_grelha)
		{
			if (face_tras.pos_click[4] == 0)
			{
				face_tras.num_jogada++;
				face_tras.pos_click[4]++;
			}
			face = 3;
			if (modo_jogo == 1)
				PC_turno = 1;
		}

		// Posicao meio baixo
		else if (posZ < -25 + margem_grelha && posZ > -25 - margem_grelha
			&&   posX < 50 / 6 - margem_grelha && posX > -50 / 6 + margem_grelha
			&&   posY < -50 / 6 - margem_grelha && posY > -25 + margem_grelha)
		{
			if (face_tras.pos_click[5] == 0)
			{
				face_tras.num_jogada++;
				face_tras.pos_click[5]++;
			}
			face = 3;
			if (modo_jogo == 1)
				PC_turno = 1;
		}

		// Posicao direita cima
		else if (posZ < -25 + margem_grelha && posZ > -25 - margem_grelha
			&&   posX > -25 + margem_grelha && posX < -50 / 6 - margem_grelha
			&&   posY > 50 / 6 + margem_grelha && posY < 25 - margem_grelha)
		{
			if (face_tras.pos_click[6] == 0)
			{
				face_tras.num_jogada++;
				face_tras.pos_click[6]++;
			}
			face = 3;
			if (modo_jogo == 1)
				PC_turno = 1;
		}

		// Posicao direita meio
		else if (posZ < -25 + margem_grelha && posZ > -25 - margem_grelha
			&&   posX > -25 + margem_grelha && posX < -50 / 6 - margem_grelha
			&&   posY > -50 / 6 + margem_grelha && posY < 50 / 6 - margem_grelha)
		{
			if (face_tras.pos_click[7] == 0)
			{
				face_tras.num_jogada++;
				face_tras.pos_click[7]++;
			}
			face = 3;
			if (modo_jogo == 1)
				PC_turno = 1;
		}

		// Posicao direita baixo
		else if (posZ < -25 + margem_grelha && posZ > -25 - margem_grelha
			&&   posX > -25 + margem_grelha && posX < -50 / 6 - margem_grelha
			&&   posY < -50 / 6 - margem_grelha && posY > -25 + margem_grelha)
		{
			if (face_tras.pos_click[8] == 0)
			{
				face_tras.num_jogada++;
				face_tras.pos_click[8]++;
			}
			face = 3;
			if (modo_jogo == 1)
				PC_turno = 1;
		}


		/* ------------------------------------------------------------ */
		/*							FACE DE CIMA                       */
		/* ---------------------------------------------------------- */

		// Posicao esquerda cima
		if (posY < 25 + margem_grelha && posY > 25 - margem_grelha
			&& posX < -50 / 6 - margem_grelha && posX > -25 + margem_grelha
			&& posZ < -50 / 6 - margem_grelha && posZ > -25 + margem_grelha)
		{
			if (face_cima.pos_click[0] == 0)
			{
				face_cima.num_jogada++;
				face_cima.pos_click[0]++;
			}
			face = 4;
			if (modo_jogo == 1)
				PC_turno = 1;
		}


		// Posicao esquerda meio
		else if (posY < 25 + margem_grelha && posY > 25 - margem_grelha
			&&   posX < -50 / 6 - margem_grelha && posX > -25 + margem_grelha
			&&   posZ > -50 / 6 + margem_grelha && posZ < 50 / 6 - margem_grelha)
		{
			if (face_cima.pos_click[1] == 0)
			{
				face_cima.num_jogada++;
				face_cima.pos_click[1]++;
			}
			face = 4;
			if (modo_jogo == 1)
				PC_turno = 1;
		}

		// Posicao esquerda baixo
		else if (posY < 25 + margem_grelha && posY > 25 - margem_grelha
			&&   posX < -50 / 6 - margem_grelha && posX > -25 + margem_grelha
			&&   posZ > 50 / 6 + margem_grelha && posZ < 25 - margem_grelha)
		{
			if (face_cima.pos_click[2] == 0)
			{
				face_cima.num_jogada++;
				face_cima.pos_click[2]++;
			}
			face = 4;
			if (modo_jogo == 1)
				PC_turno = 1;
		}

		// Posicao meio cima
		else if (posY < 25 + margem_grelha && posY > 25 - margem_grelha
			&&   posX > -50 / 6 + margem_grelha && posX < 50 / 6 - margem_grelha
			&&   posZ < -50 / 6 - margem_grelha && posZ > -25 + margem_grelha)
		{
			if (face_cima.pos_click[3] == 0)
			{
				face_cima.num_jogada++;
				face_cima.pos_click[3]++;
			}
			face = 4;
			if (modo_jogo == 1)
				PC_turno = 1;
		}

		// Posicao do meio
		else if (posY < 25 + margem_grelha && posY > 25 - margem_grelha
			&&   posX < 50 / 6 - margem_grelha && posX > -50 / 6 + margem_grelha
			&&   posZ < 50 / 6 - margem_grelha && posZ > -50 / 6 + margem_grelha)
		{
			if (face_cima.pos_click[4] == 0)
			{
				face_cima.num_jogada++;
				face_cima.pos_click[4]++;
			}
			face = 4;
			if (modo_jogo == 1)
				PC_turno = 1;
		}

		// Posicao meio baixo
		else if (posY < 25 + margem_grelha && posY > 25 - margem_grelha
			&&   posX > -50 / 6 + margem_grelha && posX < 50 / 6 - margem_grelha
			&&   posZ > 50 / 6 + margem_grelha && posZ < 25 - margem_grelha)
		{
			if (face_cima.pos_click[5] == 0)
			{
				face_cima.num_jogada++;
				face_cima.pos_click[5]++;
			}
			face = 4;
			if (modo_jogo == 1)
				PC_turno = 1;
		}

		// Posicao direita cima
		else if (posY < 25 + margem_grelha && posY > 25 - margem_grelha
			&&   posX < 25 - margem_grelha && posX > 50 / 6 + margem_grelha
			&&   posZ < -50 / 6 - margem_grelha && posZ > -25 + margem_grelha)
		{
			if (face_cima.pos_click[6] == 0)
			{
				face_cima.num_jogada++;
				face_cima.pos_click[6]++;
			}
			face = 4;
			if (modo_jogo == 1)
				PC_turno = 1;
		}

		// Posicao direita meio
		else if (posY < 25 + margem_grelha && posY > 25 - margem_grelha
			&&   posX < 25 - margem_grelha && posX > 50 / 6 + margem_grelha
			&&   posZ < 50 / 6 - margem_grelha && posZ > -50 / 6 + margem_grelha)
		{
			if (face_cima.pos_click[7] == 0)
			{
				face_cima.num_jogada++;
				face_cima.pos_click[7]++;
			}
			face = 4;
			if (modo_jogo == 1)
				PC_turno = 1;
		}

		// Posicao direita baixo
		else if (posY < 25 + margem_grelha && posY > 25 - margem_grelha
			&&   posX < 25 - margem_grelha && posX > 50 / 6 + margem_grelha
			&&   posZ > 50 / 6 + margem_grelha && posZ < 25 - margem_grelha)
		{
			if (face_cima.pos_click[8] == 0)
			{
				face_cima.num_jogada++;
				face_cima.pos_click[8]++;
			}
			face = 4;
			if (modo_jogo == 1)
				PC_turno = 1;
		}


		/* ------------------------------------------------------------- */
		/*							FACE DE BAIXO                       */
		/* ----------------------------------------------------------- */

		// Posicao esquerda cima
		if (posY > -25 - margem_grelha && posY < -25 + margem_grelha
			&& posX < -50 / 6 - margem_grelha && posX > -25 + margem_grelha
			&& posZ > +50 / 6 + margem_grelha && posZ < +25 - margem_grelha)
		{
			if (face_baixo.pos_click[0] == 0)
			{
				face_baixo.num_jogada++;
				face_baixo.pos_click[0]++;
			}
			face = 5;
			if (modo_jogo == 1)
				PC_turno = 1;
		}


		// Posicao esquerda meio
		else if (posY > -25 - margem_grelha && posY < -25 + margem_grelha
			&&   posX < -50 / 6 - margem_grelha && posX > -25 + margem_grelha
			&&   posZ < +50 / 6 - margem_grelha && posZ > -50 / 6 + margem_grelha)
		{
			if (face_baixo.pos_click[1] == 0)
			{
				face_baixo.num_jogada++;
				face_baixo.pos_click[1]++;
			}
			face = 5;
			if (modo_jogo == 1)
				PC_turno = 1;
		}

		// Posicao esquerda baixo
		else if (posY > -25 - margem_grelha && posY < -25 + margem_grelha
			&&   posX < -50 / 6 - margem_grelha && posX > -25 + margem_grelha
			&&   posZ < -50 / 6 - margem_grelha && posZ > -25 + margem_grelha)
		{
			if (face_baixo.pos_click[2] == 0)
			{
				face_baixo.num_jogada++;
				face_baixo.pos_click[2]++;
			}
			face = 5;
			if (modo_jogo == 1)
				PC_turno = 1;
		}

		// Posicao meio cima
		else if (posY > -25 - margem_grelha && posY < -25 + margem_grelha
			&&   posX > -50 / 6 + margem_grelha && posX < 50 / 6 - margem_grelha
			&&   posZ > 50 / 6 + margem_grelha && posZ < 25 - margem_grelha)
		{
			if (face_baixo.pos_click[3] == 0)
			{
				face_baixo.num_jogada++;
				face_baixo.pos_click[3]++;
			}
			face = 5;
			if (modo_jogo == 1)
				PC_turno = 1;
		}

		// Posicao do meio
		else if (posY > -25 - margem_grelha && posY < -25 + margem_grelha
			&&   posX < 50 / 6 - margem_grelha && posX > -50 / 6 + margem_grelha
			&&   posZ > -50 / 6 + margem_grelha && posZ < 50 / 6 - margem_grelha)
		{
			if (face_baixo.pos_click[4] == 0)
			{
				face_baixo.num_jogada++;
				face_baixo.pos_click[4]++;
			}
			face = 5;
			if (modo_jogo == 1)
				PC_turno = 1;
		}

		// Posicao meio baixo
		else if (posY > -25 - margem_grelha && posY < -25 + margem_grelha
			&&   posX > -50 / 6 + margem_grelha && posX < 50 / 6 - margem_grelha
			&&   posZ < -50 / 6 - margem_grelha && posZ > -25 + margem_grelha)
		{
			if (face_baixo.pos_click[5] == 0)
			{
				face_baixo.num_jogada++;
				face_baixo.pos_click[5]++;
			}
			face = 5;
			if (modo_jogo == 1)
				PC_turno = 1;
		}

		// Posicao direita cima
		else if (posY > -25 - margem_grelha && posY < -25 + margem_grelha
			&&   posX < 25 - margem_grelha && posX > 50 / 6 + margem_grelha
			&&   posZ > +50 / 6 + margem_grelha && posZ < 25 - margem_grelha)
		{
			if (face_baixo.pos_click[6] == 0)
			{
				face_baixo.num_jogada++;
				face_baixo.pos_click[6]++;
			}
			face = 5;
			if (modo_jogo == 1)
				PC_turno = 1;
		}

		// Posicao direita meio
		else if (posY > -25 - margem_grelha && posY < -25 + margem_grelha
			&&   posX < 25 - margem_grelha && posX > 50 / 6 + margem_grelha
			&&   posZ > -50 / 6 + margem_grelha && posZ < 50 / 6 - margem_grelha)
		{
			if (face_baixo.pos_click[7] == 0)
			{
				face_baixo.num_jogada++;
				face_baixo.pos_click[7]++;
			}
			face = 5;
			if (modo_jogo == 1)
				PC_turno = 1;
		}

		// Posicao direita baixo
		else if (posY > -25 - margem_grelha && posY < -25 + margem_grelha
			&&   posX < 25 - margem_grelha && posX > 50 / 6 + margem_grelha
			&&   posZ < -50 / 6 - margem_grelha && posZ > -25 + margem_grelha)
		{
			if (face_baixo.pos_click[8] == 0)
			{
				face_baixo.num_jogada++;
				face_baixo.pos_click[8]++;
			}
			face = 5;
			if (modo_jogo == 1)
				PC_turno = 1;
		}
	}

	/* Detectar clicks no menu inicial */
	if (modo_jogo == 9)
	{

		if (posX > botao1_pos_x && posX < botao1_pos_x + botao1_x && posY > botao1_pos_y && posY < botao1_pos_y + botao1_y)
			escolha = 1;

		else if (posX > botao2_pos_x && posX < botao2_pos_x + botao2_x && posY > botao2_pos_y && posY < botao2_pos_y + botao2_y)
			escolha = 2;

		else if (posX > botaook_pos_x && posX < botaook_pos_x + botaook_x && posY > botaook_pos_y && posY < botaook_pos_y + botaook_y && escolha == 1) {
			modo_jogo = 0;
			sndPlaySoundA(NULL, SND_ASYNC);
			if (start_face_aux == 0)
				start_face = 0;
		}

		else if (posX > botaook_pos_x && posX < botaook_pos_x + botaook_x && posY > botaook_pos_y && posY < botaook_pos_y + botaook_y && escolha == 2) {
			modo_jogo = 1;
			sndPlaySoundA(NULL, SND_ASYNC);
			if (start_face_aux == 0)
				start_face = 0;
		}

		else if (posX > botao2_pos_x && posX < botao2_pos_x + botao2_x && posY > botao2_pos_y && posY < botao2_pos_y + botao2_y)
		{
			modo_jogo = 1;
			sndPlaySoundA(NULL, SND_ASYNC);
			if (start_face_aux == 0)
				start_face = 0;
		}
		else if (posX > botao3_pos_x && posX < botao3_pos_x + botao3_x && posY > botao3_pos_y && posY < botao3_pos_y + botao3_y)
			escolha = 3;

		else if (posX > botaook2_pos_x && posX < botaook2_pos_x + botaook2_x && posY > botaook2_pos_y && posY < botaook2_pos_y + botaook2_y) {
			modo_jogo = 2;
		}

		else if (posX > botao4_pos_x && posX < botao4_pos_x + botao4_x && posY > botao4_pos_y && posY < botao4_pos_y + botao4_y) {
			escolha = 4;
		}

		else if ((modo_jogo == 9) && posX > botaosair_pos_x && posX < botaosair_pos_x + botaosair_x && posY > botaosair_pos_y && posY < botaosair_pos_y + botaosair_y) {
			exit(-1);
		}

	}

	temp_pos.posX = posX;
	temp_pos.posY = posY;
	temp_pos.posZ = posZ;

	return temp_pos;
}

/* Recebe coordenadas do rato no mundo real, e chama a função GetOGLPos para as "traduzir" */
void Detecta_Pos(GLint botao, GLint estado, GLint xRato, GLint yRato)
{
	if (botao == GLUT_LEFT_BUTTON && estado == GLUT_DOWN && start_face == 1)
	{
		GetOGLPos(xRato, yRato);
	}
	glutSwapBuffers();
}

void Desenha_Cubo()
{
	/* Perpective has X max = 80 AND Y max = 80 */
	int largura_quad = 25;

	glBindTexture(GL_TEXTURE_2D, texture_id[0]);

	// FRONT
	glBegin(GL_QUADS);

	glColor3f(1.0, 1.0, 1.0);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-largura_quad, largura_quad, largura_quad);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(largura_quad, largura_quad, largura_quad);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(largura_quad, -largura_quad, largura_quad);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-largura_quad, -largura_quad, largura_quad);



	// BACK

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-largura_quad, largura_quad, -largura_quad);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(largura_quad, largura_quad, -largura_quad);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(largura_quad, -largura_quad, -largura_quad);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-largura_quad, -largura_quad, -largura_quad);

	// TOP

	glTexCoord2f(0.0, 0.0);
	glVertex3f(largura_quad, largura_quad, largura_quad);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(largura_quad, largura_quad, -largura_quad);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-largura_quad, largura_quad, -largura_quad);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-largura_quad, largura_quad, largura_quad);

	// BOTTOM

	glTexCoord2f(0.0, 0.0);
	glVertex3f(largura_quad, -largura_quad, largura_quad);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(largura_quad, -largura_quad, -largura_quad);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-largura_quad, -largura_quad, -largura_quad);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-largura_quad, -largura_quad, largura_quad);

	// RIGTH

	glTexCoord2f(0.0, 0.0);
	glVertex3f(largura_quad, largura_quad, largura_quad);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(largura_quad, largura_quad, -largura_quad);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(largura_quad, -largura_quad, -largura_quad);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(largura_quad, -largura_quad, largura_quad);

	//LEFT

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-largura_quad, largura_quad, largura_quad);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-largura_quad, largura_quad, -largura_quad);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-largura_quad, -largura_quad, -largura_quad);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-largura_quad, -largura_quad, largura_quad);

	glEnd();
}

void loadTextureFromFile()
{
	// Habilita o uso de textura 
	glEnable(GL_TEXTURE_2D);

	// Define a forma de armazenamento dos pixels na textura (1= alihamento por byte)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Define quantas texturas serão usadas no programa 
	glGenTextures(3, texture_id);  // 1 = uma textura;
	// texture_id = vetor que guardas os números das texturas

	// Define o número da textura do cubo.
	texture_id[0] = 1001;
	texture_id[1] = 1002;
	texture_id[2] = 1003;
	texture_id[3] = 1004;
	texture_id[4] = 1005;
	texture_id[5] = 1006;
	texture_id[6] = 1007;
	texture_id[7] = 1008;
	texture_id[8] = 1009;
	texture_id[8] = 1010;
	texture_id[9] = 1011;
	texture_id[10] = 1012;

	glBindTexture(GL_TEXTURE_2D, texture_id[0]);

	// carrega a uma imagem TGA 
	image_t temp_image;
	tgaLoad("Crate2.tga", &temp_image, TGA_FREE | TGA_LOW_QUALITY);
	glBindTexture(GL_TEXTURE_2D, texture_id[1]);
	tgaLoad("floor.tga", &temp_image, TGA_FREE | TGA_LOW_QUALITY);
	glBindTexture(GL_TEXTURE_2D, texture_id[2]);
	tgaLoad("wall.tga", &temp_image, TGA_FREE | TGA_LOW_QUALITY);
	glBindTexture(GL_TEXTURE_2D, texture_id[3]);
	tgaLoad("ceiling.tga", &temp_image, TGA_FREE | TGA_LOW_QUALITY);
	glBindTexture(GL_TEXTURE_2D, texture_id[4]);
	tgaLoad("info_normal.tga", &temp_image, TGA_FREE | TGA_LOW_QUALITY);
	glBindTexture(GL_TEXTURE_2D, texture_id[5]);
	tgaLoad("info_vista.tga", &temp_image, TGA_FREE | TGA_LOW_QUALITY);
	glBindTexture(GL_TEXTURE_2D, texture_id[6]);
	tgaLoad("Menu1.tga", &temp_image, TGA_FREE | TGA_LOW_QUALITY);
	glBindTexture(GL_TEXTURE_2D, texture_id[7]);
	tgaLoad("vitoria_X.tga", &temp_image, TGA_FREE | TGA_LOW_QUALITY);
	glBindTexture(GL_TEXTURE_2D, texture_id[8]);
	tgaLoad("vitoria_O.tga", &temp_image, TGA_FREE | TGA_LOW_QUALITY);
	glBindTexture(GL_TEXTURE_2D, texture_id[9]);
	tgaLoad("empate.tga", &temp_image, TGA_FREE | TGA_LOW_QUALITY);
	glBindTexture(GL_TEXTURE_2D, texture_id[10]);
	tgaLoad("creditos.tga", &temp_image, TGA_FREE | TGA_LOW_QUALITY);
}

void Desenha_Chao()
{
	int largura_chao = 500, z_chao = 35;


	glBindTexture(GL_TEXTURE_2D, texture_id[1]);


	glColor3f(1, 1, 1);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(largura_chao, -z_chao, largura_chao);
	glTexCoord2f(10.0f, 0.0f);
	glVertex3f(largura_chao, -z_chao, -largura_chao);
	glTexCoord2f(10.0f, 10.0f);
	glVertex3f(-largura_chao, -z_chao, -largura_chao);
	glTexCoord2f(0.0f, 10.0f);
	glVertex3f(-largura_chao, -z_chao, largura_chao);
	glEnd();


}

void Desenha_Tecto()
{
	int largura_chao = 500, z_tecto = 400;


	glBindTexture(GL_TEXTURE_2D, texture_id[3]);


	glColor3f(1, 1, 1);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(largura_chao, z_tecto, largura_chao);
	glTexCoord2f(10.0f, 0.0f);
	glVertex3f(largura_chao, z_tecto, -largura_chao);
	glTexCoord2f(10.0f, 10.0f);
	glVertex3f(-largura_chao, z_tecto, -largura_chao);
	glTexCoord2f(0.0f, 10.0f);
	glVertex3f(-largura_chao, z_tecto, largura_chao);
	glEnd();


}

void Desenha_Paredes() {

	int largura_paredes = 500, z_chao = 50, altura = 400;

	glBindTexture(GL_TEXTURE_2D, texture_id[2]);

	glColor3f(1.0, 1.0, .8);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 0.0); // Parede direita
	glVertex3f(largura_paredes, -z_chao, largura_paredes);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(largura_paredes, -z_chao, -largura_paredes);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(largura_paredes, altura, -largura_paredes);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(largura_paredes, altura, largura_paredes);

	glTexCoord2f(0.0, 0.0);			// Parede do fundo
	glVertex3f(largura_paredes, -z_chao, -largura_paredes);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-largura_paredes, -z_chao, -largura_paredes);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-largura_paredes, altura, -largura_paredes);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(largura_paredes, altura, -largura_paredes);


	glTexCoord2f(0.0, 0.0);  // Parede da esquerda
	glVertex3f(-largura_paredes, -z_chao, -largura_paredes);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-largura_paredes, -z_chao, largura_paredes);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-largura_paredes, altura, largura_paredes);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-largura_paredes, altura, -largura_paredes);


	glTexCoord2f(0.0, 0.0); // Parede de frente
	glVertex3f(-largura_paredes, -z_chao, largura_paredes);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(largura_paredes, -z_chao, largura_paredes);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(largura_paredes, altura, largura_paredes);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-largura_paredes, altura, largura_paredes);

	glEnd();
}

void Menu_Inicial()
{
	/* Desenha quadrado para colocar posterior imagem de fundo*/
	start_face = 1;
	sndPlaySoundA("main.wav", SND_ASYNC | SND_LOOP | SND_NOSTOP);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f(1.0, 1.0, 1.0);

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texture_id[6]);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-125, -95, -0.1);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(125, -95, -0.1);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(125, 95, -0.1);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-125, 95, -0.1);

	glEnd();

	glDisable(GL_TEXTURE_2D);

	//glColor3f(1.0, 0.0, 0.0);

	//glBegin(GL_QUADS);
	//glVertex3f(botao4_pos_x, botao4_pos_y, 0);
	//glVertex3f(botao4_pos_x, botao4_pos_y + botao4_y, 0);
	//glVertex3f(botao4_pos_x + botao4_x, botao4_pos_y + botao4_y, 0);
	//glVertex3f(botao4_pos_x + botao4_x, botao4_pos_y, 0);
	//glColor3f(1.0, 1.0, 1.0); // Colocar cor branca para o texto abaixo
	//glEnd();

	///* Escreve no botao <OK> */
	//glRasterPos3f(botao4_pos_x +5, botao4_pos_y + 5, 1);
	//int i;
	//char *text = "OK\0";
	//for (i = 0; text[i] != '\0'; i++)
	//	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
}

void Menu_Info_Normal(){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texture_id[4]);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-125, -95, -0.1);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(125, -95, -0.1);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(125, 95, -0.1);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-125, 95, -0.1);

	glEnd();

	glDisable(GL_TEXTURE_2D);

	///* Desenha botao <OK> */
	//glBegin(GL_QUADS);
	//glColor3f(1.0, 0.0, 0.0);
	//glVertex3f(botaook_pos_x, botaook_pos_y, 0);
	//glVertex3f(botaook_pos_x, botaook_pos_y + botaook_y, 0);
	//glVertex3f(botaook_pos_x + botaook_x, botaook_pos_y + botaook_y, 0);
	//glVertex3f(botaook_pos_x + botaook_x, botaook_pos_y, 0);
	//glColor3f(1.0, 1.0, 1.0); // Colocar cor branca para o texto abaixo
	//glEnd();

	///* Escreve no botao <OK> */
	//glRasterPos3f(botaook_pos_x + 20, botaook_pos_y + 5, 1);
	//int i;
	//char *text = "Jogar\0";
	//for (i = 0; text[i] != '\0'; i++)
	//	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);

}

void Menu_Info_Vista()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texture_id[5]);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-125, -95, -0.1);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(125, -95, -0.1);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(125, 95, -0.1);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-125, 95, -0.1);

	glEnd();

	glDisable(GL_TEXTURE_2D);

	///* Desenha botao <OK> */

	//glColor3f(1.0, 0.0, 0.0);

	//glBegin(GL_QUADS);
	//glVertex3f(botaook2_pos_x, botaook2_pos_y, 0);
	//glVertex3f(botaook2_pos_x, botaook2_pos_y + botaook2_y, 0);
	//glVertex3f(botaook2_pos_x + botaook2_x, botaook2_pos_y + botaook2_y, 0);
	//glVertex3f(botaook2_pos_x + botaook2_x, botaook2_pos_y, 0);
	//glColor3f(1.0, 1.0, 1.0); // Colocar cor branca para o texto abaixo
	//glEnd();

	///* Escreve no botao <OK> */
	//glRasterPos3f(botaook2_pos_x + 20, botaook2_pos_y + 5, 1);
	//int i;
	//char *text = "OK\0";
	//for (i = 0; text[i] != '\0'; i++)
	//	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
}

/* <WASD> gira cubo 90 graus */
void keyboard(unsigned char key, GLint x, GLint y)
{

	if (modo_jogo == 2) {

		double k = 20;					        // o que a janela aumenta ou desloca
		double l = 7;

		double nx = mira_x - camara_x;		   // vector de orientação da câmara (x)
		double ny = mira_y - camara_y;		   // vector de orientação da câmara (y)
		double nz = mira_z - camara_z;		   // vector de orientação da câmara (z)

		double n = sqrt(nx*nx + ny*ny + nz*nz);	                      // norma do vector

		double x_mira = mira_x;
		double y_mira = mira_y;
		double z_mira = mira_z;

		double xc = camara_x;
		double yc = camara_y;
		double zc = camara_z;

		double teta = k / 180 * 3.14159265 / 10;

		nx /= n;						                                 // normalização
		ny /= n;
		nz /= n;

		switch (key)
		{
		case ESCAPE:			     // tecla de escape, para saída da aplicação
			exit(0);
			break;

		case 'w':				                              // olhar para cima
			mira_y += l;
			break;

		case 's':			                                 // olhar para baixo
			mira_y -= l;
			break;

		case 'd':			  	                         // olhar para a direita
			mira_z = cos(-teta)*(z_mira - zc) - sin(-teta)*(x_mira - xc) + zc;
			mira_x = sin(-teta)*(z_mira - zc) + cos(-teta)*(x_mira - xc) + xc;
			break;

		case 'a':			  	                        // olhar para a esquerda
			mira_z = cos(teta)*(z_mira - zc) - sin(teta)*(x_mira - xc) + zc;
			mira_x = sin(teta)*(z_mira - zc) + cos(teta)*(x_mira - xc) + xc;
			break;

		case ' ':			  	                              // andar em frente
			camara_x += nx*k;
			mira_x += nx*k;
			camara_y += ny*k;
			mira_y += ny*k;
			camara_z += nz*k;
			mira_z += nz*k;
			break;

		case 'c':			  	                              // andar para trás
			camara_x -= nx*k;
			mira_x -= nx*k;
			camara_y -= ny*k;
			mira_y -= ny*k;
			camara_z -= nz*k;
			mira_z -= nz*k;
			break;

		default:			  	             // por defeito, não há nenhuma ação
			break;
		}
	}


	if (modo_jogo == 9 && escolha == 4) {

		switch (key)
		{
		case ' ':
			escolha = 0;
			break;

		case 'c':
		case 'C':
			const char* output = "https://bitbucket.org/batalhao_c17_univ/tic_tac_toe_3d"; // Copiar para o clipboard
			const size_t len = strlen(output) + 1;
			HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
			memcpy(GlobalLock(hMem), output, len);
			GlobalUnlock(hMem);
			OpenClipboard(0);
			EmptyClipboard();
			SetClipboardData(CF_TEXT, hMem);
			CloseClipboard();
			break;
		}

	}

	if ((modo_jogo == 0 || modo_jogo == 1) && start_face == 0 && (face_activa2 != 4 && face_activa2 != 5)) //Teclas so existem para o modo de jogo Normal e face livre 
	{
		switch (key)
		{
		case 'a':
			if (roda_dir == 0 && roda_cima == 0 && roda_baixo == 0){
				roda_esq = 1;
				sndPlaySoundA("move_box.wav", SND_ASYNC);
			}
			break;
		case 'A':
			if (roda_dir == 0 && roda_cima == 0 && roda_baixo == 0){
				roda_esq = 1;
				sndPlaySoundA("move_box.wav", SND_ASYNC);
			}
			break;
		case 'd':
			if (roda_esq == 0 && roda_cima == 0 && roda_baixo == 0) {
				roda_dir = 1;
				sndPlaySoundA("move_box.wav", SND_ASYNC);
			}
			break;
		case 'D':
			if (roda_esq == 0 && roda_cima == 0 && roda_baixo == 0) {
				roda_dir = 1;
				sndPlaySoundA("move_box.wav", SND_ASYNC);
			}
			break;
		case ' ':

			if (jogos[face_activa2] == -1) {
				start_face = 1;
				sndPlaySoundA("space.wav", SND_ASYNC);
			}

			if (game_over == 1)
				restart = 1;

			break;

		case 'w':
			if (roda_esq == 0 && roda_dir == 0 && roda_baixo == 0) {
				roda_cima = 1;
				sndPlaySoundA("move_box.wav", SND_ASYNC);
			}
			break;
		case 'W':
			if (roda_esq == 0 && roda_dir == 0 && roda_baixo == 0) {
				roda_cima = 1;
				sndPlaySoundA("move_box.wav", SND_ASYNC);
			}
			break;
		case 's':
			if (roda_esq == 0 && roda_dir == 0 && roda_cima == 0) {
				roda_baixo = 1;
				sndPlaySoundA("move_box.wav", SND_ASYNC);
			}
			break;
		case 'S':
			if (roda_esq == 0 && roda_dir == 0 && roda_cima == 0) {
				roda_baixo = 1;
				sndPlaySoundA("move_box.wav", SND_ASYNC);
			}
			break;

		case 'Z':
			Copia_Dados_Face(face_frontal, face_aux);
			Inicia_Dados_Face(face_frontal);
			jogos0 = jogos[0];
			jogos[0] = -1;
			guarda_face_activa = face_activa2;
			face_activa2 = 0;
			modo_jogo = 3; // modo automatico
			break;
		case 'z':
			Copia_Dados_Face(face_frontal, face_aux);
			Inicia_Dados_Face(face_frontal);
			jogos0 = jogos[0];
			jogos[0] = -1;
			guarda_face_activa = face_activa2;
			face_activa2 = 0;
			modo_jogo = 3;
			break;


		}
	}

	if ((modo_jogo == 0 || modo_jogo == 1) && start_face == 0 && (face_activa2 == 4 || face_activa2 == 5)) //Teclas so existem para o modo de jogo Normal e face livre 
	{
		switch (key)
		{
		case ' ':

			if (jogos[face_activa2] == -1) {
				start_face = 1;
				sndPlaySoundA("space.wav", SND_ASYNC);
			}

			if (game_over == 1)
				restart = 1;

			break;

		case 'w':
			roda_cima = 1;
			sndPlaySoundA("move_box.wav", SND_ASYNC);
			break;
		case 'W':
			roda_cima = 1;
			sndPlaySoundA("move_box.wav", SND_ASYNC);
			break;
		case 's':
			roda_baixo = 1;
			sndPlaySoundA("move_box.wav", SND_ASYNC);
			break;
		case 'S':
			roda_baixo = 1;
			sndPlaySoundA("move_box.wav", SND_ASYNC);
			break;

		case 'Z':
			Copia_Dados_Face(face_frontal, face_aux);
			Inicia_Dados_Face(face_frontal);
			jogos0 = jogos[0];
			jogos[0] = -1;
			guarda_face_activa = face_activa2;
			face_activa2 = 0;
			modo_jogo = 3; // modo automatico
			break;
		case 'z':
			Copia_Dados_Face(face_frontal, face_aux);
			Inicia_Dados_Face(face_frontal);
			jogos0 = jogos[0];
			jogos[0] = -1;
			guarda_face_activa = face_activa2;
			face_activa2 = 0;
			modo_jogo = 3;
			break;

		case 'a':
			sndPlaySoundA("error.wav", SND_ASYNC);
			break;
		case 'A':
			sndPlaySoundA("error.wav", SND_ASYNC);
			break;
		case 'd':
			sndPlaySoundA("error.wav", SND_ASYNC);
			break;
		case 'D':
			sndPlaySoundA("error.wav", SND_ASYNC);
			break;
		}
	}
}

void tecla_especial_premida(int tecla, int x, int y)
{

	if (modo_jogo == 2) {

		double k = 20;					        // o que a janela aumenta ou desloca

		double nx = mira_x - camara_x;		   // vector de orientação da câmara (x)
		double ny = mira_y - camara_y;		   // vector de orientação da câmara (y)
		double nz = mira_z - camara_z;		   // vector de orientação da câmara (z)

		double n = sqrt(nx*nx + ny*ny + nz*nz);	                      // norma do vector

		double x_mira = mira_x;
		double y_mira = mira_y;
		double z_mira = mira_z;

		double xc = camara_x;
		double yc = camara_y;
		double zc = camara_z;

		double teta = k / 180 * 3.14159265 / 10;

		nx /= n;						                                 // normalização
		ny /= n;
		nz /= n;

		switch (tecla)
		{
		case GLUT_KEY_UP:			                  // tecla de seta para cima
			camara_y += k;
			mira_y += k;
			break;

		case GLUT_KEY_DOWN:			                 // tecla de seta para baixo
			camara_y -= k;
			mira_y -= k;
			break;

		case GLUT_KEY_LEFT:			                    // tecla para a esquerda
			camara_z -= nx*k;
			camara_x += nz*k;
			mira_z -= nx*k;
			mira_x += nz*k;
			break;

		case GLUT_KEY_RIGHT:		                     // tecla para a direita
			camara_z += nx*k;
			camara_x -= nz*k;
			mira_z += nx*k;
			mira_x -= nz*k;
			break;

		default:				             // por defeito, não há nenhuma ação
			break;
		}
	}
}

void Inicia_Valores() {

	/* Inicializar variaveis - COMECA AQUI */
	for (int i = 0; i < 9; i++)
	{
		face_frontal.pos_click[i] = 0;
		face_direita.pos_click[i] = 0;
		face_esquerda.pos_click[i] = 0;
		face_baixo.pos_click[i] = 0;
		face_cima.pos_click[i] = 0;
		face_tras.pos_click[i] = 0;
		face_frontal.simbolo[i] = 2; //Inicializar nem a 0 nem a 1 (que sao os valores pretendidos)
		face_direita.simbolo[i] = 2; //Inicializar nem a 0 nem a 1 (que sao os valores pretendidos)
		face_esquerda.simbolo[i] = 2; //Inicializar nem a 0 nem a 1 (que sao os valores pretendidos)
		face_baixo.simbolo[i] = 2; //Inicializar nem a 0 nem a 1 (que sao os valores pretendidos)
		face_cima.simbolo[i] = 2; //Inicializar nem a 0 nem a 1 (que sao os valores pretendidos)
		face_tras.simbolo[i] = 2; //Inicializar nem a 0 nem a 1 (que sao os valores pretendidos)

	}
	face_frontal.margem_face = 4;		face_frontal.num_jogada = 0;
	face_cima.margem_face = 4;			face_cima.num_jogada = 0;
	face_baixo.margem_face = 4;			face_baixo.num_jogada = 0;
	face_tras.margem_face = 4;			face_tras.num_jogada = 0;
	face_esquerda.margem_face = 4;		face_esquerda.num_jogada = 0;
	face_direita.margem_face = 4;		face_direita.num_jogada = 0;

	/* Inicializar variaveis - ACABA AQUI */

	game_over = 0;
	//define quem ganhou em cada face
	jogos[0] = -1;
	jogos[1] = -1;
	jogos[2] = -1;
	jogos[3] = -1;
	jogos[4] = -1;
	jogos[5] = -1;
	jogo_final = -1; // 0 - empate | 1- jogador X | 2- Jogador O
	conta_x = 0, conta_o = 0, conta_empate = 0;
	escolha = 0;
	aux = 0;
	contax = (char*)malloc(100 * sizeof(int));
	contay = (char*)malloc(100 * sizeof(int));

	xRotated = 0;
	yRotated = 0;
	play = 0;
}

void cria_menus_rato() {

	menu1 = glutCreateMenu(Menu_Rato1);
	glutAddMenuEntry("Voltar ao menu inicial", 1);
	glutAddMenuEntry("Mostrar controlos", 2);
	glutAddMenuEntry("Sair", 3);
}

void Menu_Rato1(int value) {

	if (value == 1) { // Volta menu inicial
		glClearColor(.0, .0, .0, .0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Inicia_Valores();
		glutDetachMenu(GLUT_RIGHT_BUTTON);
		modo_jogo = 9;
	}

	if (value == 2) {
		glClearColor(.0, .0, 1.0, .0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glutDetachMenu(GLUT_RIGHT_BUTTON);
		modo_jogo = 9;
		escolha = 1;
		start_face_aux = start_face;
		start_face = 1;
	}
	if (value == 3)
		exit(-1);
}

void cria_menus_rato2() {

	menu2 = glutCreateMenu(Menu_Rato2);
	glutAddMenuEntry("Voltar ao menu inicial", 1);
	glutAddMenuEntry("Mostrar controlos", 2);
	glutAddMenuEntry("Sair", 3);
}

void Menu_Rato2(int value) {

	if (value == 1) { // Volta menu inicial
		glClearColor(.0, .0, .0, .0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Inicia_Valores();
		glutDetachMenu(GLUT_RIGHT_BUTTON);
		modo_jogo = 9;
	}

	if (value == 2) {
		glClearColor(.0, .0, 1.0, .0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glutDetachMenu(GLUT_RIGHT_BUTTON);
		modo_jogo = 9;
		escolha = 3;
		start_face_aux = start_face;
		start_face = 1;
	}
	if (value == 3)
		exit(-1);
}

void Mensagens_Victoria()
{

	if (modo_jogo == 0 || modo_jogo == 1) {


		if (game_over == 1) {


			if (jogo_final == 1 || jogo_final == 2) {

				if (play == 0)
					sndPlaySoundA("win.wav", SND_ASYNC);

				play = 1;

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glColor3f(1.0, 1.0, 1.0);

				glEnable(GL_TEXTURE_2D);
				glutDetachMenu(GLUT_RIGHT_BUTTON);

				glBindTexture(GL_TEXTURE_2D, texture_id[7]);

				glBegin(GL_QUADS);

				glTexCoord2f(0.0, 0.0);
				glVertex3f(-125, -95, -0.1);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(125, -95, -0.1);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(125, 95, -0.1);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(-125, 95, -0.1);

				glEnd();
				glDisable(GL_TEXTURE_2D);

				int i;
				// vitorias x
				sprintf(contax, "%d", conta_x);
				glColor3f(1.0, 1.0, 1.0);

				glRasterPos3f(8, -57, 1);

				for (i = 0; contax[i] != '\0'; i++)
					glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, contax[i]);

				sprintf(contax, "%d", conta_x);
				glColor3f(1.0, 1.0, 1.0);
				// vitorias bola
				sprintf(contay, "%d", conta_o);
				glRasterPos3f(8, -67, 1);

				for (i = 0; contay[i] != '\0'; i++)
					glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, contay[i]);

				if (restart == 1) {
					Inicia_Valores();
					modo_jogo = 9;
				}
			}

			if (jogo_final == 2) {

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glColor3f(1.0, 1.0, 1.0);

				glEnable(GL_TEXTURE_2D);
				glutDetachMenu(GLUT_RIGHT_BUTTON);

				glBindTexture(GL_TEXTURE_2D, texture_id[8]);

				glBegin(GL_QUADS);

				glTexCoord2f(0.0, 0.0);
				glVertex3f(-125, -95, -0.1);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(125, -95, -0.1);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(125, 95, -0.1);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(-125, 95, -0.1);

				glEnd();
				glDisable(GL_TEXTURE_2D);

				int i;
				// vitorias x
				sprintf(contax, "%d", conta_x);
				glColor3f(1.0, 1.0, 1.0);

				glRasterPos3f(8, -57, 1);

				for (i = 0; contax[i] != '\0'; i++)
					glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, contax[i]);

				sprintf(contax, "%d", conta_x);
				glColor3f(1.0, 1.0, 1.0);
				// vitorias bola
				sprintf(contay, "%d", conta_o);
				glRasterPos3f(8, -67, 1);

				for (i = 0; contay[i] != '\0'; i++)
					glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, contay[i]);

				if (restart == 1) {
					Inicia_Valores();
					modo_jogo = 9;
				}
			}

			if (jogo_final == 0) {

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glColor3f(1.0, 1.0, 1.0);

				glEnable(GL_TEXTURE_2D);
				glutDetachMenu(GLUT_RIGHT_BUTTON);

				glBindTexture(GL_TEXTURE_2D, texture_id[9]);

				glBegin(GL_QUADS);

				glTexCoord2f(0.0, 0.0);
				glVertex3f(-125, -95, -0.1);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(125, -95, -0.1);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(125, 95, -0.1);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(-125, 95, -0.1);

				glEnd();
				glDisable(GL_TEXTURE_2D);

				int i;
				// vitorias x
				sprintf(contax, "%d", conta_x);
				glColor3f(1.0, 1.0, 1.0);

				glRasterPos3f(8, -57, 1);

				for (i = 0; contax[i] != '\0'; i++)
					glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, contax[i]);

				sprintf(contax, "%d", conta_x);
				glColor3f(1.0, 1.0, 1.0);
				// vitorias bola
				sprintf(contay, "%d", conta_o);
				glRasterPos3f(8, -67, 1);

				for (i = 0; contay[i] != '\0'; i++)
					glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, contay[i]);

				if (restart == 1) {
					Inicia_Valores();
					modo_jogo = 9;
				}


			}
		}
	}

}

void Modo_Automatico()
{
	/* FACE FRONTAL */
	// Limpar e guardar dados face frontal

	int num, i = 0;
	bool repeated = false; // Detecta se determinada posicao da grelha ja foi jogada

	num = rand() % 9; // Armazena posicao da grelha

	//APAGAR FACE FRONTAL DEPOIS DE COPIAR

	if (face_frontal.pos_click[num] > 0)	// Verifica se jogada já foi feita
		repeated = true;

	if (repeated == false)
	{
		face_frontal.pos_click[num] = 1;
		face_frontal.num_jogada++;
	}
	repeated = false;
	Sleep(700);


	if (jogos[0] != -1) { // Se foi encontrado vencedor ou empate 
		jogos[0] = jogos0;
		Copia_Dados_Face(face_aux, face_frontal);
		face_activa2 = guarda_face_activa;
		modo_jogo = 0;
	}

}

void Determina_Turno_PC()
{
	/* FACE FRONTAL */
	/* Primeiro o PC ve se consegue ganhar */

	if (face_activa == 0)
	{
		face_frontal.num_jogada++;
		short int PC_simbolo = face_frontal.num_jogada % 2;
		short int jgdr_simbolo = !PC_simbolo;

		// Linha da esquerda vertical
		if (face_frontal.simbolo[0] == 2 && face_frontal.simbolo[1] == PC_simbolo && face_frontal.simbolo[2] == PC_simbolo)
			face_frontal.pos_click[0]=1;
		else if (face_frontal.simbolo[0] == PC_simbolo && face_frontal.simbolo[1] == 2 && face_frontal.simbolo[2] == PC_simbolo)
			face_frontal.pos_click[1] = 1;
		else if (face_frontal.simbolo[0] == PC_simbolo && face_frontal.simbolo[1] == PC_simbolo && face_frontal.simbolo[2] == 2)
			face_frontal.pos_click[2] = 1;

		// Linha vertical meio
		else if (face_frontal.simbolo[3] == 2 && face_frontal.simbolo[4] == PC_simbolo && face_frontal.simbolo[5] == PC_simbolo)
			face_frontal.pos_click[3] = 1;
		else if (face_frontal.simbolo[3] == PC_simbolo && face_frontal.simbolo[4] == 2 && face_frontal.simbolo[5] == PC_simbolo)
			face_frontal.pos_click[4] = 1;
		else if (face_frontal.simbolo[3] == PC_simbolo && face_frontal.simbolo[4] == PC_simbolo && face_frontal.simbolo[5] == 2)
			face_frontal.pos_click[5] = 1;

		// Linha direita vertical
		else if (face_frontal.simbolo[6] == 2 && face_frontal.simbolo[7] == PC_simbolo && face_frontal.simbolo[8] == PC_simbolo)
			face_frontal.pos_click[6] = 1;
		else if (face_frontal.simbolo[6] == PC_simbolo && face_frontal.simbolo[7] == 2 && face_frontal.simbolo[8] == PC_simbolo)
			face_frontal.pos_click[7] = 1;
		else if (face_frontal.simbolo[6] == PC_simbolo && face_frontal.simbolo[7] == PC_simbolo && face_frontal.simbolo[8] == 2)
			face_frontal.pos_click[8] = 1;


		// Linha de topo horizontal
		else if (face_frontal.simbolo[0] == 2 && face_frontal.simbolo[3] == PC_simbolo && face_frontal.simbolo[6] == PC_simbolo)
			face_frontal.pos_click[0] = 1;
		else if (face_frontal.simbolo[0] == PC_simbolo && face_frontal.simbolo[3] == 2 && face_frontal.simbolo[6] == PC_simbolo)
			face_frontal.pos_click[3] = 1;
		else if (face_frontal.simbolo[0] == PC_simbolo && face_frontal.simbolo[3] == PC_simbolo && face_frontal.simbolo[6] == 2)
			face_frontal.pos_click[6] = 1;

		// Linha meio horizontal
		else if (face_frontal.simbolo[1] == 2 && face_frontal.simbolo[4] == PC_simbolo && face_frontal.simbolo[7] == PC_simbolo)
			face_frontal.pos_click[1] = 1;
		else if (face_frontal.simbolo[1] == PC_simbolo && face_frontal.simbolo[4] == 2 && face_frontal.simbolo[7] == PC_simbolo)
			face_frontal.pos_click[4] = 1;
		else if (face_frontal.simbolo[1] == PC_simbolo && face_frontal.simbolo[4] == PC_simbolo && face_frontal.simbolo[7] == 2)
			face_frontal.pos_click[7] = 1;

		// Linha horizontal baixo
		else if (face_frontal.simbolo[2] == 2 && face_frontal.simbolo[5] == PC_simbolo && face_frontal.simbolo[8] == PC_simbolo)
			face_frontal.pos_click[2] = 1;
		else if (face_frontal.simbolo[2] == PC_simbolo && face_frontal.simbolo[5] == 2 && face_frontal.simbolo[8] == PC_simbolo)
			face_frontal.pos_click[5] = 1;
		else if (face_frontal.simbolo[2] == PC_simbolo && face_frontal.simbolo[5] == PC_simbolo && face_frontal.simbolo[8] == 2)
			face_frontal.pos_click[8] = 1;

		// linha diagonal cima-baixo esq-dir
		else if (face_frontal.simbolo[0] == 2 && face_frontal.simbolo[4] == PC_simbolo && face_frontal.simbolo[8] == PC_simbolo)
			face_frontal.pos_click[0] = 1;
		else if (face_frontal.simbolo[0] == PC_simbolo && face_frontal.simbolo[4] == 2 && face_frontal.simbolo[8] == PC_simbolo)
			face_frontal.pos_click[4] = 1;
		else if (face_frontal.simbolo[0] == PC_simbolo && face_frontal.simbolo[4] == PC_simbolo && face_frontal.simbolo[8] == 2)
			face_frontal.pos_click[8] = 1;

		// Linha diagonal baixo-cima dir-esq
		else if (face_frontal.simbolo[2] == 2 && face_frontal.simbolo[4] == PC_simbolo && face_frontal.simbolo[6] == PC_simbolo)
			face_frontal.pos_click[2] = 1;
		else if (face_frontal.simbolo[2] == PC_simbolo && face_frontal.simbolo[4] == 2 && face_frontal.simbolo[6] == PC_simbolo)
			face_frontal.pos_click[4] = 1;
		else if (face_frontal.simbolo[2] == PC_simbolo && face_frontal.simbolo[4] == PC_simbolo && face_frontal.simbolo[6] == 2)
			face_frontal.pos_click[6] = 1;



		// Depois ve se consegue evitar derrota

		// Linha da esquerda vertical
		else if (face_frontal.simbolo[0] == 2 && face_frontal.simbolo[1] == jgdr_simbolo && face_frontal.simbolo[2] == jgdr_simbolo)
			face_frontal.pos_click[0] = 1;
		else if (face_frontal.simbolo[0] == jgdr_simbolo && face_frontal.simbolo[1] == 2 && face_frontal.simbolo[2] == jgdr_simbolo)
			face_frontal.pos_click[1] = 1;
		else if (face_frontal.simbolo[0] == jgdr_simbolo && face_frontal.simbolo[1] == jgdr_simbolo && face_frontal.simbolo[2] == 2)
			face_frontal.pos_click[2] = 1;

		// Linha vertical meio
		else if (face_frontal.simbolo[3] == 2 && face_frontal.simbolo[4] == jgdr_simbolo && face_frontal.simbolo[5] == jgdr_simbolo)
			face_frontal.pos_click[3] =1;
		else if (face_frontal.simbolo[3] == jgdr_simbolo && face_frontal.simbolo[4] == 2 && face_frontal.simbolo[5] == jgdr_simbolo)
			face_frontal.pos_click[4] = 1;
		else if (face_frontal.simbolo[3] == jgdr_simbolo && face_frontal.simbolo[4] == jgdr_simbolo && face_frontal.simbolo[5] == 2)
			face_frontal.pos_click[5] = 1;

		// Linha direita vertical
		else if (face_frontal.simbolo[6] == 2 && face_frontal.simbolo[7] == jgdr_simbolo && face_frontal.simbolo[8] == jgdr_simbolo)
			face_frontal.pos_click[6] = 1;
		else if (face_frontal.simbolo[6] == jgdr_simbolo && face_frontal.simbolo[7] == 2 && face_frontal.simbolo[8] == jgdr_simbolo)
			face_frontal.pos_click[7] = 1;
		else if (face_frontal.simbolo[6] == jgdr_simbolo && face_frontal.simbolo[7] == jgdr_simbolo && face_frontal.simbolo[8] == 2)
			face_frontal.pos_click[8] = 1;


		// Linha de topo horizontal
		else if (face_frontal.simbolo[0] == 2 && face_frontal.simbolo[3] == jgdr_simbolo && face_frontal.simbolo[6] == jgdr_simbolo)
			face_frontal.pos_click[0] = 1;
		else if (face_frontal.simbolo[0] == jgdr_simbolo && face_frontal.simbolo[3] == 2 && face_frontal.simbolo[6] == jgdr_simbolo)
			face_frontal.pos_click[3] = 1;
		else if (face_frontal.simbolo[0] == jgdr_simbolo && face_frontal.simbolo[3] == jgdr_simbolo && face_frontal.simbolo[6] == 2)
			face_frontal.pos_click[6] = 1;

		// Linha meio horizontal
		else if (face_frontal.simbolo[1] == 2 && face_frontal.simbolo[4] == jgdr_simbolo && face_frontal.simbolo[7] == jgdr_simbolo)
			face_frontal.pos_click[1] = 1;
		else if (face_frontal.simbolo[1] == jgdr_simbolo && face_frontal.simbolo[4] == 2 && face_frontal.simbolo[7] == jgdr_simbolo)
			face_frontal.pos_click[4] = 1;
		else if (face_frontal.simbolo[1] == jgdr_simbolo && face_frontal.simbolo[4] == jgdr_simbolo && face_frontal.simbolo[7] == 2)
			face_frontal.pos_click[7] = 1;

		// Linha horizontal baixo
		else if (face_frontal.simbolo[2] == 2 && face_frontal.simbolo[5] == jgdr_simbolo && face_frontal.simbolo[8] == jgdr_simbolo)
			face_frontal.pos_click[2] = 1;
		else if (face_frontal.simbolo[2] == jgdr_simbolo && face_frontal.simbolo[5] == 2 && face_frontal.simbolo[8] == jgdr_simbolo)
			face_frontal.pos_click[5] = 1;
		else if (face_frontal.simbolo[2] == jgdr_simbolo && face_frontal.simbolo[5] == jgdr_simbolo && face_frontal.simbolo[8] == 2)
			face_frontal.pos_click[8] = 1;

		// linha diagonal cima-baixo esq-dir
		else if (face_frontal.simbolo[0] == 2 && face_frontal.simbolo[4] == jgdr_simbolo && face_frontal.simbolo[8] == jgdr_simbolo)
			face_frontal.pos_click[0] = 1;
		else if (face_frontal.simbolo[0] == jgdr_simbolo && face_frontal.simbolo[4] == 2 && face_frontal.simbolo[8] == jgdr_simbolo)
			face_frontal.pos_click[4] = 1;
		else if (face_frontal.simbolo[0] == jgdr_simbolo && face_frontal.simbolo[4] == jgdr_simbolo && face_frontal.simbolo[8] == 2)
			face_frontal.pos_click[8] = 1;

		// Linha diagonal baixo-cima esq-dir
		else if (face_frontal.simbolo[2] == 2 && face_frontal.simbolo[4] == jgdr_simbolo && face_frontal.simbolo[6] == jgdr_simbolo)
			face_frontal.pos_click[2] = 1;
		else if (face_frontal.simbolo[2] == jgdr_simbolo && face_frontal.simbolo[4] == 2 && face_frontal.simbolo[6] == jgdr_simbolo)
			face_frontal.pos_click[4] = 1;
		else if (face_frontal.simbolo[2] == jgdr_simbolo && face_frontal.simbolo[4] == jgdr_simbolo && face_frontal.simbolo[6] == 2)
			face_frontal.pos_click[6] = 1;


		// Caso contrario, joga aleatoriamente
		else
		{
			bool encontrado = false;
			bool repeated = false; // Detecta se determinada posicao da grelha ja foi jogada
			while (!encontrado && jogos[0] == -1)
			{
				int num, i = 0;

				num = rand() % 9; // Armazena posicao da grelha

				if (face_frontal.pos_click[num] > 0)	// Verifica se jogada já foi feita
					repeated = true;

				if (repeated == false)
				{
					face_frontal.pos_click[num] = 1;
					encontrado = true;
				}
				repeated = false;
			}
		}
	}

	/* FACE DIREITA */
	/* Primeiro o PC ve se consegue ganhar */

	else if (face_activa == 1)
	{
		face_direita.num_jogada++;
		short int PC_simbolo = face_direita.num_jogada % 2;
		short int jgdr_simbolo = !PC_simbolo;
		// Linha da esquerda vertical
		if (face_direita.simbolo[0] == 2 && face_direita.simbolo[1] == PC_simbolo && face_direita.simbolo[2] == PC_simbolo)
			face_direita.pos_click[0] = 1;
		else if (face_direita.simbolo[0] == PC_simbolo && face_direita.simbolo[1] == 2 && face_direita.simbolo[2] == PC_simbolo)
			face_direita.pos_click[1] = 1;
		else if (face_direita.simbolo[0] == PC_simbolo && face_direita.simbolo[1] == PC_simbolo && face_direita.simbolo[2] == 2)
			face_direita.pos_click[2] = 1;

		// Linha vertical meio
		else if (face_direita.simbolo[3] == 2 && face_direita.simbolo[4] == PC_simbolo && face_direita.simbolo[5] == PC_simbolo)
			face_direita.pos_click[3] = 1;
		else if (face_direita.simbolo[3] == PC_simbolo && face_direita.simbolo[4] == 2 && face_direita.simbolo[5] == PC_simbolo)
			face_direita.pos_click[4] = 1;
		else if (face_direita.simbolo[3] == PC_simbolo && face_direita.simbolo[4] == PC_simbolo && face_direita.simbolo[5] == 2)
			face_direita.pos_click[5] = 1;

		// Linha direita vertical
		else if (face_direita.simbolo[6] == 2 && face_direita.simbolo[7] == PC_simbolo && face_direita.simbolo[8] == PC_simbolo)
			face_direita.pos_click[6] = 1;
		else if (face_direita.simbolo[6] == PC_simbolo && face_direita.simbolo[7] == 2 && face_direita.simbolo[8] == PC_simbolo)
			face_direita.pos_click[7] = 1;
		else if (face_direita.simbolo[6] == PC_simbolo && face_direita.simbolo[7] == PC_simbolo && face_direita.simbolo[8] == 2)
			face_direita.pos_click[8] = 1;


		// Linha de topo horizontal
		else if (face_direita.simbolo[0] == 2 && face_direita.simbolo[3] == PC_simbolo && face_direita.simbolo[6] == PC_simbolo)
			face_direita.pos_click[0] = 1;
		else if (face_direita.simbolo[0] == PC_simbolo && face_direita.simbolo[3] == 2 && face_direita.simbolo[6] == PC_simbolo)
			face_direita.pos_click[3] = 1;
		else if (face_direita.simbolo[0] == PC_simbolo && face_direita.simbolo[3] == PC_simbolo && face_direita.simbolo[6] == 2)
			face_direita.pos_click[6] = 1;

		// Linha meio horizontal
		else if (face_direita.simbolo[1] == 2 && face_direita.simbolo[4] == PC_simbolo && face_direita.simbolo[7] == PC_simbolo)
			face_direita.pos_click[1] = 1;
		else if (face_direita.simbolo[1] == PC_simbolo && face_direita.simbolo[4] == 2 && face_direita.simbolo[7] == PC_simbolo)
			face_direita.pos_click[4] = 1;
		else if (face_direita.simbolo[1] == PC_simbolo && face_direita.simbolo[4] == PC_simbolo && face_direita.simbolo[7] == 2)
			face_direita.pos_click[7] = 1;

		// Linha horizontal baixo
		else if (face_direita.simbolo[2] == 2 && face_direita.simbolo[5] == PC_simbolo && face_direita.simbolo[8] == PC_simbolo)
			face_direita.pos_click[2] = 1;
		else if (face_direita.simbolo[2] == PC_simbolo && face_direita.simbolo[5] == 2 && face_direita.simbolo[8] == PC_simbolo)
			face_direita.pos_click[5] = 1;
		else if (face_direita.simbolo[2] == PC_simbolo && face_direita.simbolo[5] == PC_simbolo && face_direita.simbolo[8] == 2)
			face_direita.pos_click[8] = 1;

		// linha diagonal cima-baixo esq-dir
		else if (face_direita.simbolo[0] == 2 && face_direita.simbolo[4] == PC_simbolo && face_direita.simbolo[8] == PC_simbolo)
			face_direita.pos_click[0] = 1;
		else if (face_direita.simbolo[0] == PC_simbolo && face_direita.simbolo[4] == 2 && face_direita.simbolo[8] == PC_simbolo)
			face_direita.pos_click[4] = 1;
		else if (face_direita.simbolo[0] == PC_simbolo && face_direita.simbolo[4] == PC_simbolo && face_direita.simbolo[8] == 2)
			face_direita.pos_click[8] = 1;

		// Linha diagonal baixo-cima dir-esq
		else if (face_direita.simbolo[2] == 2 && face_direita.simbolo[4] == PC_simbolo && face_direita.simbolo[6] == PC_simbolo)
			face_direita.pos_click[2] = 1;
		else if (face_direita.simbolo[2] == PC_simbolo && face_direita.simbolo[4] == 2 && face_direita.simbolo[6] == PC_simbolo)
			face_direita.pos_click[4] = 1;
		else if (face_direita.simbolo[2] == PC_simbolo && face_direita.simbolo[4] == PC_simbolo && face_direita.simbolo[6] == 2)
			face_direita.pos_click[6] = 1;



		// Depois ve se consegue evitar derrota

		// Linha da esquerda vertical
		else if (face_direita.simbolo[0] == 2 && face_direita.simbolo[1] == jgdr_simbolo && face_direita.simbolo[2] == jgdr_simbolo)
			face_direita.pos_click[0] = 1;
		else if (face_direita.simbolo[0] == jgdr_simbolo && face_direita.simbolo[1] == 2 && face_direita.simbolo[2] == jgdr_simbolo)
			face_direita.pos_click[1] = 1;
		else if (face_direita.simbolo[0] == jgdr_simbolo && face_direita.simbolo[1] == jgdr_simbolo && face_direita.simbolo[2] == 2)
			face_direita.pos_click[2] = 1;

		// Linha vertical meio
		else if (face_direita.simbolo[3] == 2 && face_direita.simbolo[4] == jgdr_simbolo && face_direita.simbolo[5] == jgdr_simbolo)
			face_direita.pos_click[3] = 1;
		else if (face_direita.simbolo[3] == jgdr_simbolo && face_direita.simbolo[4] == 2 && face_direita.simbolo[5] == jgdr_simbolo)
			face_direita.pos_click[4] = 1;
		else if (face_direita.simbolo[3] == jgdr_simbolo && face_direita.simbolo[4] == jgdr_simbolo && face_direita.simbolo[5] == 2)
			face_direita.pos_click[5] = 1;

		// Linha direita vertical
		else if (face_direita.simbolo[6] == 2 && face_direita.simbolo[7] == jgdr_simbolo && face_direita.simbolo[8] == jgdr_simbolo)
			face_direita.pos_click[6] = 1;
		else if (face_direita.simbolo[6] == jgdr_simbolo && face_direita.simbolo[7] == 2 && face_direita.simbolo[8] == jgdr_simbolo)
			face_direita.pos_click[7] = 1;
		else if (face_direita.simbolo[6] == jgdr_simbolo && face_direita.simbolo[7] == jgdr_simbolo && face_direita.simbolo[8] == 2)
			face_direita.pos_click[8] = 1;


		// Linha de topo horizontal
		else if (face_direita.simbolo[0] == 2 && face_direita.simbolo[3] == jgdr_simbolo && face_direita.simbolo[6] == jgdr_simbolo)
			face_direita.pos_click[0] = 1;
		else if (face_direita.simbolo[0] == jgdr_simbolo && face_direita.simbolo[3] == 2 && face_direita.simbolo[6] == jgdr_simbolo)
			face_direita.pos_click[3] = 1;
		else if (face_direita.simbolo[0] == jgdr_simbolo && face_direita.simbolo[3] == jgdr_simbolo && face_direita.simbolo[6] == 2)
			face_direita.pos_click[6] = 1;

		// Linha meio horizontal
		else if (face_direita.simbolo[1] == 2 && face_direita.simbolo[4] == jgdr_simbolo && face_direita.simbolo[7] == jgdr_simbolo)
			face_direita.pos_click[1] = 1;
		else if (face_direita.simbolo[1] == jgdr_simbolo && face_direita.simbolo[4] == 2 && face_direita.simbolo[7] == jgdr_simbolo)
			face_direita.pos_click[4] = 1;
		else if (face_direita.simbolo[1] == jgdr_simbolo && face_direita.simbolo[4] == jgdr_simbolo && face_direita.simbolo[7] == 2)
			face_direita.pos_click[7] = 1;

		// Linha horizontal baixo
		else if (face_direita.simbolo[2] == 2 && face_direita.simbolo[5] == jgdr_simbolo && face_direita.simbolo[8] == jgdr_simbolo)
			face_direita.pos_click[2] = 1;
		else if (face_direita.simbolo[2] == jgdr_simbolo && face_direita.simbolo[5] == 2 && face_direita.simbolo[8] == jgdr_simbolo)
			face_direita.pos_click[5] = 1;
		else if (face_direita.simbolo[2] == jgdr_simbolo && face_direita.simbolo[5] == jgdr_simbolo && face_direita.simbolo[8] == 2)
			face_direita.pos_click[8] = 1;

		// linha diagonal cima-baixo esq-dir
		else if (face_direita.simbolo[0] == 2 && face_direita.simbolo[4] == jgdr_simbolo && face_direita.simbolo[8] == jgdr_simbolo)
			face_direita.pos_click[0] = 1;
		else if (face_direita.simbolo[0] == jgdr_simbolo && face_direita.simbolo[4] == 2 && face_direita.simbolo[8] == jgdr_simbolo)
			face_direita.pos_click[4] = 1;
		else if (face_direita.simbolo[0] == jgdr_simbolo && face_direita.simbolo[4] == jgdr_simbolo && face_direita.simbolo[8] == 2)
			face_direita.pos_click[8] = 1;

		// Linha diagonal baixo-cima dir-esq
		else if (face_direita.simbolo[2] == 2 && face_direita.simbolo[4] == jgdr_simbolo && face_direita.simbolo[6] == jgdr_simbolo)
			face_direita.pos_click[2] = 1;
		else if (face_direita.simbolo[2] == jgdr_simbolo && face_direita.simbolo[4] == 2 && face_direita.simbolo[6] == jgdr_simbolo)
			face_direita.pos_click[4] = 1;
		else if (face_direita.simbolo[2] == jgdr_simbolo && face_direita.simbolo[4] == jgdr_simbolo && face_direita.simbolo[6] == 2)
			face_direita.pos_click[6] = 1;


		// Caso contrario, joga aleatoriamente
		else
		{
			bool encontrado = false;
			bool repeated = false; // Detecta se determinada posicao da grelha ja foi jogada
			while (!encontrado && jogos[1] == -1)
			{
				int num, i = 0;

				num = rand() % 9; // Armazena posicao da grelha

				if (face_direita.pos_click[num] > 0)	// Verifica se jogada já foi feita
					repeated = true;

				if (repeated == false)
				{
					face_direita.pos_click[num] = 1;
					encontrado = true;
				}
				repeated = false;
			}
		}
	}

	/* FACE ESQUERDA */
	/* Primeiro o PC ve se consegue ganhar */

	else if (face_activa == 2)
	{
		face_esquerda.num_jogada++;
		short int PC_simbolo = face_esquerda.num_jogada % 2;
		short int jgdr_simbolo = !PC_simbolo;

		// Linha da esquerda vertical
		if (face_esquerda.simbolo[0] == 2 && face_esquerda.simbolo[1] == PC_simbolo && face_esquerda.simbolo[2] == PC_simbolo)
			face_esquerda.pos_click[0] = 1;
		else if (face_esquerda.simbolo[0] == PC_simbolo && face_esquerda.simbolo[1] == 2 && face_esquerda.simbolo[2] == PC_simbolo)
			face_esquerda.pos_click[1] = 1;
		else if (face_esquerda.simbolo[0] == PC_simbolo && face_esquerda.simbolo[1] == PC_simbolo && face_esquerda.simbolo[2] == 2)
			face_esquerda.pos_click[2] = 1;

		// Linha vertical meio
		else if (face_esquerda.simbolo[3] == 2 && face_esquerda.simbolo[4] == PC_simbolo && face_esquerda.simbolo[5] == PC_simbolo)
			face_esquerda.pos_click[3] = 1;
		else if (face_esquerda.simbolo[3] == PC_simbolo && face_esquerda.simbolo[4] == 2 && face_esquerda.simbolo[5] == PC_simbolo)
			face_esquerda.pos_click[4] = 1;
		else if (face_esquerda.simbolo[3] == PC_simbolo && face_esquerda.simbolo[4] == PC_simbolo && face_esquerda.simbolo[5] == 2)
			face_esquerda.pos_click[5] = 1;

		// Linha direita vertical
		else if (face_esquerda.simbolo[6] == 2 && face_esquerda.simbolo[7] == PC_simbolo && face_esquerda.simbolo[8] == PC_simbolo)
			face_esquerda.pos_click[6] = 1;
		else if (face_esquerda.simbolo[6] == PC_simbolo && face_esquerda.simbolo[7] == 2 && face_esquerda.simbolo[8] == PC_simbolo)
			face_esquerda.pos_click[7] = 1;
		else if (face_esquerda.simbolo[6] == PC_simbolo && face_esquerda.simbolo[7] == PC_simbolo && face_esquerda.simbolo[8] == 2)
			face_esquerda.pos_click[8] = 1;


		// Linha de topo horizontal
		else if (face_esquerda.simbolo[0] == 2 && face_esquerda.simbolo[3] == PC_simbolo && face_esquerda.simbolo[6] == PC_simbolo)
			face_esquerda.pos_click[0] = 1;
		else if (face_esquerda.simbolo[0] == PC_simbolo && face_esquerda.simbolo[3] == 2 && face_esquerda.simbolo[6] == PC_simbolo)
			face_esquerda.pos_click[3] = 1;
		else if (face_esquerda.simbolo[0] == PC_simbolo && face_esquerda.simbolo[3] == PC_simbolo && face_esquerda.simbolo[6] == 2)
			face_esquerda.pos_click[6] = 1;

		// Linha meio horizontal
		else if (face_esquerda.simbolo[1] == 2 && face_esquerda.simbolo[4] == PC_simbolo && face_esquerda.simbolo[7] == PC_simbolo)
			face_esquerda.pos_click[1] = 1;
		else if (face_esquerda.simbolo[1] == PC_simbolo && face_esquerda.simbolo[4] == 2 && face_esquerda.simbolo[7] == PC_simbolo)
			face_esquerda.pos_click[4] = 1;
		else if (face_esquerda.simbolo[1] == PC_simbolo && face_esquerda.simbolo[4] == PC_simbolo && face_esquerda.simbolo[7] == 2)
			face_esquerda.pos_click[7] = 1;

		// Linha horizontal baixo
		else if (face_esquerda.simbolo[2] == 2 && face_esquerda.simbolo[5] == PC_simbolo && face_esquerda.simbolo[8] == PC_simbolo)
			face_esquerda.pos_click[2] = 1;
		else if (face_esquerda.simbolo[2] == PC_simbolo && face_esquerda.simbolo[5] == 2 && face_esquerda.simbolo[8] == PC_simbolo)
			face_esquerda.pos_click[5] = 1;
		else if (face_esquerda.simbolo[2] == PC_simbolo && face_esquerda.simbolo[5] == PC_simbolo && face_esquerda.simbolo[8] == 2)
			face_esquerda.pos_click[8] = 1;

		// linha diagonal cima-baixo esq-dir
		else if (face_esquerda.simbolo[0] == 2 && face_esquerda.simbolo[4] == PC_simbolo && face_esquerda.simbolo[8] == PC_simbolo)
			face_esquerda.pos_click[0] = 1;
		else if (face_esquerda.simbolo[0] == PC_simbolo && face_esquerda.simbolo[4] == 2 && face_esquerda.simbolo[8] == PC_simbolo)
			face_esquerda.pos_click[4] = 1;
		else if (face_esquerda.simbolo[0] == PC_simbolo && face_esquerda.simbolo[4] == PC_simbolo && face_esquerda.simbolo[8] == 2)
			face_esquerda.pos_click[8] = 1;

		// Linha diagonal baixo-cima dir-esq
		else if (face_esquerda.simbolo[2] == 2 && face_esquerda.simbolo[4] == PC_simbolo && face_esquerda.simbolo[6] == PC_simbolo)
			face_esquerda.pos_click[2] = 1;
		else if (face_esquerda.simbolo[2] == PC_simbolo && face_esquerda.simbolo[4] == 2 && face_esquerda.simbolo[6] == PC_simbolo)
			face_esquerda.pos_click[4] = 1;
		else if (face_esquerda.simbolo[2] == PC_simbolo && face_esquerda.simbolo[4] == PC_simbolo && face_esquerda.simbolo[6] == 2)
			face_esquerda.pos_click[6] = 1;



		// Depois ve se consegue evitar derrota

		// Linha da esquerda vertical
		else if (face_esquerda.simbolo[0] == 2 && face_esquerda.simbolo[1] == jgdr_simbolo && face_esquerda.simbolo[2] == jgdr_simbolo)
			face_esquerda.pos_click[0] = 1;
		else if (face_esquerda.simbolo[0] == jgdr_simbolo && face_esquerda.simbolo[1] == 2 && face_esquerda.simbolo[2] == jgdr_simbolo)
			face_esquerda.pos_click[1] = 1;
		else if (face_esquerda.simbolo[0] == jgdr_simbolo && face_esquerda.simbolo[1] == jgdr_simbolo && face_esquerda.simbolo[2] == 2)
			face_esquerda.pos_click[2] = 1;

		// Linha vertical meio
		else if (face_esquerda.simbolo[3] == 2 && face_esquerda.simbolo[4] == jgdr_simbolo && face_esquerda.simbolo[5] == jgdr_simbolo)
			face_esquerda.pos_click[3] = 1;
		else if (face_esquerda.simbolo[3] == jgdr_simbolo && face_esquerda.simbolo[4] == 2 && face_esquerda.simbolo[5] == jgdr_simbolo)
			face_esquerda.pos_click[4] = 1;
		else if (face_esquerda.simbolo[3] == jgdr_simbolo && face_esquerda.simbolo[4] == jgdr_simbolo && face_esquerda.simbolo[5] == 2)
			face_esquerda.pos_click[5] = 1;

		// Linha direita vertical
		else if (face_esquerda.simbolo[6] == 2 && face_esquerda.simbolo[7] == jgdr_simbolo && face_esquerda.simbolo[8] == jgdr_simbolo)
			face_esquerda.pos_click[6] = 1;
		else if (face_esquerda.simbolo[6] == jgdr_simbolo && face_esquerda.simbolo[7] == 2 && face_esquerda.simbolo[8] == jgdr_simbolo)
			face_esquerda.pos_click[7] = 1;
		else if (face_esquerda.simbolo[6] == jgdr_simbolo && face_esquerda.simbolo[7] == jgdr_simbolo && face_esquerda.simbolo[8] == 2)
			face_esquerda.pos_click[8] = 1;


		// Linha de topo horizontal
		else if (face_esquerda.simbolo[0] == 2 && face_esquerda.simbolo[3] == jgdr_simbolo && face_esquerda.simbolo[6] == jgdr_simbolo)
			face_esquerda.pos_click[0] = 1;
		else if (face_esquerda.simbolo[0] == jgdr_simbolo && face_esquerda.simbolo[3] == 2 && face_esquerda.simbolo[6] == jgdr_simbolo)
			face_esquerda.pos_click[3] = 1;
		else if (face_esquerda.simbolo[0] == jgdr_simbolo && face_esquerda.simbolo[3] == jgdr_simbolo && face_esquerda.simbolo[6] == 2)
			face_esquerda.pos_click[6] = 1;

		// Linha meio horizontal
		else if (face_esquerda.simbolo[1] == 2 && face_esquerda.simbolo[4] == jgdr_simbolo && face_esquerda.simbolo[7] == jgdr_simbolo)
			face_esquerda.pos_click[1] = 1;
		else if (face_esquerda.simbolo[1] == jgdr_simbolo && face_esquerda.simbolo[4] == 2 && face_esquerda.simbolo[7] == jgdr_simbolo)
			face_esquerda.pos_click[4] = 1;
		else if (face_esquerda.simbolo[1] == jgdr_simbolo && face_esquerda.simbolo[4] == jgdr_simbolo && face_esquerda.simbolo[7] == 2)
			face_esquerda.pos_click[7] = 1;

		// Linha horizontal baixo
		else if (face_esquerda.simbolo[2] == 2 && face_esquerda.simbolo[5] == jgdr_simbolo && face_esquerda.simbolo[8] == jgdr_simbolo)
			face_esquerda.pos_click[2] = 1;
		else if (face_esquerda.simbolo[2] == jgdr_simbolo && face_esquerda.simbolo[5] == 2 && face_esquerda.simbolo[8] == jgdr_simbolo)
			face_esquerda.pos_click[5] = 1;
		else if (face_esquerda.simbolo[2] == jgdr_simbolo && face_esquerda.simbolo[5] == jgdr_simbolo && face_esquerda.simbolo[8] == 2)
			face_esquerda.pos_click[8] = 1;

		// linha diagonal cima-baixo esq-dir
		else if (face_esquerda.simbolo[0] == 2 && face_esquerda.simbolo[4] == jgdr_simbolo && face_esquerda.simbolo[8] == jgdr_simbolo)
			face_esquerda.pos_click[0] = 1;
		else if (face_esquerda.simbolo[0] == jgdr_simbolo && face_esquerda.simbolo[4] == 2 && face_esquerda.simbolo[8] == jgdr_simbolo)
			face_esquerda.pos_click[4] = 1;
		else if (face_esquerda.simbolo[0] == jgdr_simbolo && face_esquerda.simbolo[4] == jgdr_simbolo && face_esquerda.simbolo[8] == 2)
			face_esquerda.pos_click[8] = 1;

		// Linha diagonal baixo-cima dir-esq
		else if (face_esquerda.simbolo[2] == 2 && face_esquerda.simbolo[4] == jgdr_simbolo && face_esquerda.simbolo[6] == jgdr_simbolo)
			face_esquerda.pos_click[2] = 1;
		else if (face_esquerda.simbolo[2] == jgdr_simbolo && face_esquerda.simbolo[4] == 2 && face_esquerda.simbolo[6] == jgdr_simbolo)
			face_esquerda.pos_click[4] = 1;
		else if (face_esquerda.simbolo[2] == jgdr_simbolo && face_esquerda.simbolo[4] == jgdr_simbolo && face_esquerda.simbolo[6] == 2)
			face_esquerda.pos_click[6] = 1;


		// Caso contrario, joga aleatoriamente
		else
		{
			bool encontrado = false;
			bool repeated = false; // Detecta se determinada posicao da grelha ja foi jogada
			while (!encontrado && jogos[2] == -1)
			{
				int num, i = 0;

				num = rand() % 9; // Armazena posicao da grelha

				if (face_esquerda.pos_click[num] > 0)	// Verifica se jogada já foi feita
					repeated = true;

				if (repeated == false)
				{
					face_esquerda.pos_click[num] = 1;
					encontrado = true;
				}
				repeated = false;
			}
		}
	}

	/* FACE TRAS */
	/* Primeiro o PC ve se consegue ganhar */

	else if (face_activa == 3)
	{
		face_tras.num_jogada++;
		short int PC_simbolo = face_tras.num_jogada % 2;
		short int jgdr_simbolo = !PC_simbolo;

		// Linha da esquerda vertical
		if (face_tras.simbolo[0] == 2 && face_tras.simbolo[1] == PC_simbolo && face_tras.simbolo[2] == PC_simbolo)
			face_tras.pos_click[0] = 1;
		else if (face_tras.simbolo[0] == PC_simbolo && face_tras.simbolo[1] == 2 && face_tras.simbolo[2] == PC_simbolo)
			face_tras.pos_click[1] = 1;
		else if (face_tras.simbolo[0] == PC_simbolo && face_tras.simbolo[1] == PC_simbolo && face_tras.simbolo[2] == 2)
			face_tras.pos_click[2] = 1;

		// Linha vertical meio
		else if (face_tras.simbolo[3] == 2 && face_tras.simbolo[4] == PC_simbolo && face_tras.simbolo[5] == PC_simbolo)
			face_tras.pos_click[3] = 1;
		else if (face_tras.simbolo[3] == PC_simbolo && face_tras.simbolo[4] == 2 && face_tras.simbolo[5] == PC_simbolo)
			face_tras.pos_click[4] = 1;
		else if (face_tras.simbolo[3] == PC_simbolo && face_tras.simbolo[4] == PC_simbolo && face_tras.simbolo[5] == 2)
			face_tras.pos_click[5] = 1;

		// Linha direita vertical
		else if (face_tras.simbolo[6] == 2 && face_tras.simbolo[7] == PC_simbolo && face_tras.simbolo[8] == PC_simbolo)
			face_tras.pos_click[6] = 1;
		else if (face_tras.simbolo[6] == PC_simbolo && face_tras.simbolo[7] == 2 && face_tras.simbolo[8] == PC_simbolo)
			face_tras.pos_click[7] = 1;
		else if (face_tras.simbolo[6] == PC_simbolo && face_tras.simbolo[7] == PC_simbolo && face_tras.simbolo[8] == 2)
			face_tras.pos_click[8] = 1;


		// Linha de topo horizontal
		else if (face_tras.simbolo[0] == 2 && face_tras.simbolo[3] == PC_simbolo && face_tras.simbolo[6] == PC_simbolo)
			face_tras.pos_click[0] = 1;
		else if (face_tras.simbolo[0] == PC_simbolo && face_tras.simbolo[3] == 2 && face_tras.simbolo[6] == PC_simbolo)
			face_tras.pos_click[3] = 1;
		else if (face_tras.simbolo[0] == PC_simbolo && face_tras.simbolo[3] == PC_simbolo && face_tras.simbolo[6] == 2)
			face_tras.pos_click[6] = 1;

		// Linha meio horizontal
		else if (face_tras.simbolo[1] == 2 && face_tras.simbolo[4] == PC_simbolo && face_tras.simbolo[7] == PC_simbolo)
			face_tras.pos_click[1] = 1;
		else if (face_tras.simbolo[1] == PC_simbolo && face_tras.simbolo[4] == 2 && face_tras.simbolo[7] == PC_simbolo)
			face_tras.pos_click[4] = 1;
		else if (face_tras.simbolo[1] == PC_simbolo && face_tras.simbolo[4] == PC_simbolo && face_tras.simbolo[7] == 2)
			face_tras.pos_click[7] = 1;

		// Linha horizontal baixo
		else if (face_tras.simbolo[2] == 2 && face_tras.simbolo[5] == PC_simbolo && face_tras.simbolo[8] == PC_simbolo)
			face_tras.pos_click[2] = 1;
		else if (face_tras.simbolo[2] == PC_simbolo && face_tras.simbolo[5] == 2 && face_tras.simbolo[8] == PC_simbolo)
			face_tras.pos_click[5] = 1;
		else if (face_tras.simbolo[2] == PC_simbolo && face_tras.simbolo[5] == PC_simbolo && face_tras.simbolo[8] == 2)
			face_tras.pos_click[8] = 1;

		// linha diagonal cima-baixo esq-dir
		else if (face_tras.simbolo[0] == 2 && face_tras.simbolo[4] == PC_simbolo && face_tras.simbolo[8] == PC_simbolo)
			face_tras.pos_click[0] = 1;
		else if (face_tras.simbolo[0] == PC_simbolo && face_tras.simbolo[4] == 2 && face_tras.simbolo[8] == PC_simbolo)
			face_tras.pos_click[4] = 1;
		else if (face_tras.simbolo[0] == PC_simbolo && face_tras.simbolo[4] == PC_simbolo && face_tras.simbolo[8] == 2)
			face_tras.pos_click[8] = 1;

		// Linha diagonal baixo-cima dir-esq
		else if (face_tras.simbolo[2] == 2 && face_tras.simbolo[4] == PC_simbolo && face_tras.simbolo[6] == PC_simbolo)
			face_tras.pos_click[2] = 1;
		else if (face_tras.simbolo[2] == PC_simbolo && face_tras.simbolo[4] == 2 && face_tras.simbolo[6] == PC_simbolo)
			face_tras.pos_click[4] = 1;
		else if (face_tras.simbolo[2] == PC_simbolo && face_tras.simbolo[4] == PC_simbolo && face_tras.simbolo[6] == 2)
			face_tras.pos_click[6] = 1;



		// Depois ve se consegue evitar derrota

		// Linha da esquerda vertical
		else if (face_tras.simbolo[0] == 2 && face_tras.simbolo[1] == jgdr_simbolo && face_tras.simbolo[2] == jgdr_simbolo)
			face_tras.pos_click[0] = 1;
		else if (face_tras.simbolo[0] == jgdr_simbolo && face_tras.simbolo[1] == 2 && face_tras.simbolo[2] == jgdr_simbolo)
			face_tras.pos_click[1] = 1;
		else if (face_tras.simbolo[0] == jgdr_simbolo && face_tras.simbolo[1] == jgdr_simbolo && face_tras.simbolo[2] == 2)
			face_tras.pos_click[2] = 1;

		// Linha vertical meio
		else if (face_tras.simbolo[3] == 2 && face_tras.simbolo[4] == jgdr_simbolo && face_tras.simbolo[5] == jgdr_simbolo)
			face_tras.pos_click[3] = 1;
		else if (face_tras.simbolo[3] == jgdr_simbolo && face_tras.simbolo[4] == 2 && face_tras.simbolo[5] == jgdr_simbolo)
			face_tras.pos_click[4] = 1;
		else if (face_tras.simbolo[3] == jgdr_simbolo && face_tras.simbolo[4] == jgdr_simbolo && face_tras.simbolo[5] == 2)
			face_tras.pos_click[5] = 1;

		// Linha direita vertical
		else if (face_tras.simbolo[6] == 2 && face_tras.simbolo[7] == jgdr_simbolo && face_tras.simbolo[8] == jgdr_simbolo)
			face_tras.pos_click[6] = 1;
		else if (face_tras.simbolo[6] == jgdr_simbolo && face_tras.simbolo[7] == 2 && face_tras.simbolo[8] == jgdr_simbolo)
			face_tras.pos_click[7] = 1;
		else if (face_tras.simbolo[6] == jgdr_simbolo && face_tras.simbolo[7] == jgdr_simbolo && face_tras.simbolo[8] == 2)
			face_tras.pos_click[8] = 1;


		// Linha de topo horizontal
		else if (face_tras.simbolo[0] == 2 && face_tras.simbolo[3] == jgdr_simbolo && face_tras.simbolo[6] == jgdr_simbolo)
			face_tras.pos_click[0] = 1;
		else if (face_tras.simbolo[0] == jgdr_simbolo && face_tras.simbolo[3] == 2 && face_tras.simbolo[6] == jgdr_simbolo)
			face_tras.pos_click[3] = 1;
		else if (face_tras.simbolo[0] == jgdr_simbolo && face_tras.simbolo[3] == jgdr_simbolo && face_tras.simbolo[6] == 2)
			face_tras.pos_click[6] = 1;

		// Linha meio horizontal
		else if (face_tras.simbolo[1] == 2 && face_tras.simbolo[4] == jgdr_simbolo && face_tras.simbolo[7] == jgdr_simbolo)
			face_tras.pos_click[1] = 1;
		else if (face_tras.simbolo[1] == jgdr_simbolo && face_tras.simbolo[4] == 2 && face_tras.simbolo[7] == jgdr_simbolo)
			face_tras.pos_click[4] = 1;
		else if (face_tras.simbolo[1] == jgdr_simbolo && face_tras.simbolo[4] == jgdr_simbolo && face_tras.simbolo[7] == 2)
			face_tras.pos_click[7] = 1;

		// Linha horizontal baixo
		else if (face_tras.simbolo[2] == 2 && face_tras.simbolo[5] == jgdr_simbolo && face_tras.simbolo[8] == jgdr_simbolo)
			face_tras.pos_click[2] = 1;
		else if (face_tras.simbolo[2] == jgdr_simbolo && face_tras.simbolo[5] == 2 && face_tras.simbolo[8] == jgdr_simbolo)
			face_tras.pos_click[5] = 1;
		else if (face_tras.simbolo[2] == jgdr_simbolo && face_tras.simbolo[5] == jgdr_simbolo && face_tras.simbolo[8] == 2)
			face_tras.pos_click[8] = 1;

		// linha diagonal cima-baixo esq-dir
		else if (face_tras.simbolo[0] == 2 && face_tras.simbolo[4] == jgdr_simbolo && face_tras.simbolo[8] == jgdr_simbolo)
			face_tras.pos_click[0] = 1;
		else if (face_tras.simbolo[0] == jgdr_simbolo && face_tras.simbolo[4] == 2 && face_tras.simbolo[8] == jgdr_simbolo)
			face_tras.pos_click[4] = 1;
		else if (face_tras.simbolo[0] == jgdr_simbolo && face_tras.simbolo[4] == jgdr_simbolo && face_tras.simbolo[8] == 2)
			face_tras.pos_click[8] = 1;

		// Linha diagonal baixo-cima dir-esq
		else if (face_tras.simbolo[2] == 2 && face_tras.simbolo[4] == jgdr_simbolo && face_tras.simbolo[6] == jgdr_simbolo)
			face_tras.pos_click[2] = 1;
		else if (face_tras.simbolo[2] == jgdr_simbolo && face_tras.simbolo[4] == 2 && face_tras.simbolo[6] == jgdr_simbolo)
			face_tras.pos_click[4] = 1;
		else if (face_tras.simbolo[2] == jgdr_simbolo && face_tras.simbolo[4] == jgdr_simbolo && face_tras.simbolo[6] == 2)
			face_tras.pos_click[6] = 1;


		// Caso contrario, joga aleatoriamente
		else
		{
			bool encontrado = false;
			bool repeated = false; // Detecta se determinada posicao da grelha ja foi jogada
			while (!encontrado && jogos[3] == -1)
			{
				int num, i = 0;

				num = rand() % 9; // Armazena posicao da grelha

				if (face_tras.pos_click[num] > 0)	// Verifica se jogada já foi feita
					repeated = true;

				if (repeated == false)
				{
					face_tras.pos_click[num] = 1;
					encontrado = true;
				}
				repeated = false;
			}
		}
	}

	/* FACE BAIXO */
	/* Primeiro o PC ve se consegue ganhar */

	else if (face_activa == 4)
	{
		face_baixo.num_jogada++;
		short int PC_simbolo = face_baixo.num_jogada % 2;
		short int jgdr_simbolo = !PC_simbolo;

		// Linha da esquerda vertical
		if (face_baixo.simbolo[0] == 2 && face_baixo.simbolo[1] == PC_simbolo && face_baixo.simbolo[2] == PC_simbolo)
			face_baixo.pos_click[0] = 1;
		else if (face_baixo.simbolo[0] == PC_simbolo && face_baixo.simbolo[1] == 2 && face_baixo.simbolo[2] == PC_simbolo)
			face_baixo.pos_click[1] = 1;
		else if (face_baixo.simbolo[0] == PC_simbolo && face_baixo.simbolo[1] == PC_simbolo && face_baixo.simbolo[2] == 2)
			face_baixo.pos_click[2] = 1;

		// Linha vertical meio
		else if (face_baixo.simbolo[3] == 2 && face_baixo.simbolo[4] == PC_simbolo && face_baixo.simbolo[5] == PC_simbolo)
			face_baixo.pos_click[3] = 1;
		else if (face_baixo.simbolo[3] == PC_simbolo && face_baixo.simbolo[4] == 2 && face_baixo.simbolo[5] == PC_simbolo)
			face_baixo.pos_click[4] = 1;
		else if (face_baixo.simbolo[3] == PC_simbolo && face_baixo.simbolo[4] == PC_simbolo && face_baixo.simbolo[5] == 2)
			face_baixo.pos_click[5] = 1;

		// Linha direita vertical
		else if (face_baixo.simbolo[6] == 2 && face_baixo.simbolo[7] == PC_simbolo && face_baixo.simbolo[8] == PC_simbolo)
			face_baixo.pos_click[6] = 1;
		else if (face_baixo.simbolo[6] == PC_simbolo && face_baixo.simbolo[7] == 2 && face_baixo.simbolo[8] == PC_simbolo)
			face_baixo.pos_click[7] = 1;
		else if (face_baixo.simbolo[6] == PC_simbolo && face_baixo.simbolo[7] == PC_simbolo && face_baixo.simbolo[8] == 2)
			face_baixo.pos_click[8] = 1;


		// Linha de topo horizontal
		else if (face_baixo.simbolo[0] == 2 && face_baixo.simbolo[3] == PC_simbolo && face_baixo.simbolo[6] == PC_simbolo)
			face_baixo.pos_click[0] = 1;
		else if (face_baixo.simbolo[0] == PC_simbolo && face_baixo.simbolo[3] == 2 && face_baixo.simbolo[6] == PC_simbolo)
			face_baixo.pos_click[3] = 1;
		else if (face_baixo.simbolo[0] == PC_simbolo && face_baixo.simbolo[3] == PC_simbolo && face_baixo.simbolo[6] == 2)
			face_baixo.pos_click[6] = 1;

		// Linha meio horizontal
		else if (face_baixo.simbolo[1] == 2 && face_baixo.simbolo[4] == PC_simbolo && face_baixo.simbolo[7] == PC_simbolo)
			face_baixo.pos_click[1] = 1;
		else if (face_baixo.simbolo[1] == PC_simbolo && face_baixo.simbolo[4] == 2 && face_baixo.simbolo[7] == PC_simbolo)
			face_baixo.pos_click[4] = 1;
		else if (face_baixo.simbolo[1] == PC_simbolo && face_baixo.simbolo[4] == PC_simbolo && face_baixo.simbolo[7] == 2)
			face_baixo.pos_click[7] = 1;

		// Linha horizontal baixo
		else if (face_baixo.simbolo[2] == 2 && face_baixo.simbolo[5] == PC_simbolo && face_baixo.simbolo[8] == PC_simbolo)
			face_baixo.pos_click[2] = 1;
		else if (face_baixo.simbolo[2] == PC_simbolo && face_baixo.simbolo[5] == 2 && face_baixo.simbolo[8] == PC_simbolo)
			face_baixo.pos_click[5] = 1;
		else if (face_baixo.simbolo[2] == PC_simbolo && face_baixo.simbolo[5] == PC_simbolo && face_baixo.simbolo[8] == 2)
			face_baixo.pos_click[8] = 1;

		// linha diagonal cima-baixo esq-dir
		else if (face_baixo.simbolo[0] == 2 && face_baixo.simbolo[4] == PC_simbolo && face_baixo.simbolo[8] == PC_simbolo)
			face_baixo.pos_click[0] = 1;
		else if (face_baixo.simbolo[0] == PC_simbolo && face_baixo.simbolo[4] == 2 && face_baixo.simbolo[8] == PC_simbolo)
			face_baixo.pos_click[4] = 1;
		else if (face_baixo.simbolo[0] == PC_simbolo && face_baixo.simbolo[4] == PC_simbolo && face_baixo.simbolo[8] == 2)
			face_baixo.pos_click[8] = 1;

		// Linha diagonal baixo-cima dir-esq
		else if (face_baixo.simbolo[2] == 2 && face_baixo.simbolo[4] == PC_simbolo && face_baixo.simbolo[6] == PC_simbolo)
			face_baixo.pos_click[2] = 1;
		else if (face_baixo.simbolo[2] == PC_simbolo && face_baixo.simbolo[4] == 2 && face_baixo.simbolo[6] == PC_simbolo)
			face_baixo.pos_click[4] = 1;
		else if (face_baixo.simbolo[2] == PC_simbolo && face_baixo.simbolo[4] == PC_simbolo && face_baixo.simbolo[6] == 2)
			face_baixo.pos_click[6] = 1;



		// Depois ve se consegue evitar derrota

		// Linha da esquerda vertical
		else if (face_baixo.simbolo[0] == 2 && face_baixo.simbolo[1] == jgdr_simbolo && face_baixo.simbolo[2] == jgdr_simbolo)
			face_baixo.pos_click[0] = 1;
		else if (face_baixo.simbolo[0] == jgdr_simbolo && face_baixo.simbolo[1] == 2 && face_baixo.simbolo[2] == jgdr_simbolo)
			face_baixo.pos_click[1] = 1;
		else if (face_baixo.simbolo[0] == jgdr_simbolo && face_baixo.simbolo[1] == jgdr_simbolo && face_baixo.simbolo[2] == 2)
			face_baixo.pos_click[2] = 1;

		// Linha vertical meio
		else if (face_baixo.simbolo[3] == 2 && face_baixo.simbolo[4] == jgdr_simbolo && face_baixo.simbolo[5] == jgdr_simbolo)
			face_baixo.pos_click[3] = 1;
		else if (face_baixo.simbolo[3] == jgdr_simbolo && face_baixo.simbolo[4] == 2 && face_baixo.simbolo[5] == jgdr_simbolo)
			face_baixo.pos_click[4] = 1;
		else if (face_baixo.simbolo[3] == jgdr_simbolo && face_baixo.simbolo[4] == jgdr_simbolo && face_baixo.simbolo[5] == 2)
			face_baixo.pos_click[5] = 1;

		// Linha direita vertical
		else if (face_baixo.simbolo[6] == 2 && face_baixo.simbolo[7] == jgdr_simbolo && face_baixo.simbolo[8] == jgdr_simbolo)
			face_baixo.pos_click[6] = 1;
		else if (face_baixo.simbolo[6] == jgdr_simbolo && face_baixo.simbolo[7] == 2 && face_baixo.simbolo[8] == jgdr_simbolo)
			face_baixo.pos_click[7] = 1;
		else if (face_baixo.simbolo[6] == jgdr_simbolo && face_baixo.simbolo[7] == jgdr_simbolo && face_baixo.simbolo[8] == 2)
			face_baixo.pos_click[8] = 1;


		// Linha de topo horizontal
		else if (face_baixo.simbolo[0] == 2 && face_baixo.simbolo[3] == jgdr_simbolo && face_baixo.simbolo[6] == jgdr_simbolo)
			face_baixo.pos_click[0] = 1;
		else if (face_baixo.simbolo[0] == jgdr_simbolo && face_baixo.simbolo[3] == 2 && face_baixo.simbolo[6] == jgdr_simbolo)
			face_baixo.pos_click[3] = 1;
		else if (face_baixo.simbolo[0] == jgdr_simbolo && face_baixo.simbolo[3] == jgdr_simbolo && face_baixo.simbolo[6] == 2)
			face_baixo.pos_click[6] = 1;

		// Linha meio horizontal
		else if (face_baixo.simbolo[1] == 2 && face_baixo.simbolo[4] == jgdr_simbolo && face_baixo.simbolo[7] == jgdr_simbolo)
			face_baixo.pos_click[1] = 1;
		else if (face_baixo.simbolo[1] == jgdr_simbolo && face_baixo.simbolo[4] == 2 && face_baixo.simbolo[7] == jgdr_simbolo)
			face_baixo.pos_click[4] = 1;
		else if (face_baixo.simbolo[1] == jgdr_simbolo && face_baixo.simbolo[4] == jgdr_simbolo && face_baixo.simbolo[7] == 2)
			face_baixo.pos_click[7] = 1;

		// Linha horizontal baixo
		else if (face_baixo.simbolo[2] == 2 && face_baixo.simbolo[5] == jgdr_simbolo && face_baixo.simbolo[8] == jgdr_simbolo)
			face_baixo.pos_click[2] = 1;
		else if (face_baixo.simbolo[2] == jgdr_simbolo && face_baixo.simbolo[5] == 2 && face_baixo.simbolo[8] == jgdr_simbolo)
			face_baixo.pos_click[5] = 1;
		else if (face_baixo.simbolo[2] == jgdr_simbolo && face_baixo.simbolo[5] == jgdr_simbolo && face_baixo.simbolo[8] == 2)
			face_baixo.pos_click[8] = 1;

		// linha diagonal cima-baixo esq-dir
		else if (face_baixo.simbolo[0] == 2 && face_baixo.simbolo[4] == jgdr_simbolo && face_baixo.simbolo[8] == jgdr_simbolo)
			face_baixo.pos_click[0] = 1;
		else if (face_baixo.simbolo[0] == jgdr_simbolo && face_baixo.simbolo[4] == 2 && face_baixo.simbolo[8] == jgdr_simbolo)
			face_baixo.pos_click[4] = 1;
		else if (face_baixo.simbolo[0] == jgdr_simbolo && face_baixo.simbolo[4] == jgdr_simbolo && face_baixo.simbolo[8] == 2)
			face_baixo.pos_click[8] = 1;

		// Linha diagonal baixo-cima dir-esq
		else if (face_baixo.simbolo[2] == 2 && face_baixo.simbolo[4] == jgdr_simbolo && face_baixo.simbolo[6] == jgdr_simbolo)
			face_baixo.pos_click[2] = 1;
		else if (face_baixo.simbolo[2] == jgdr_simbolo && face_baixo.simbolo[4] == 2 && face_baixo.simbolo[6] == jgdr_simbolo)
			face_baixo.pos_click[4] = 1;
		else if (face_baixo.simbolo[2] == jgdr_simbolo && face_baixo.simbolo[4] == jgdr_simbolo && face_baixo.simbolo[6] == 2)
			face_baixo.pos_click[6] = 1;


		// Caso contrario, joga aleatoriamente
		else
		{
			bool encontrado = false;
			bool repeated = false; // Detecta se determinada posicao da grelha ja foi jogada
			while (!encontrado && jogos[4] == -1)
			{
				int num, i = 0;

				num = rand() % 9; // Armazena posicao da grelha

				if (face_baixo.pos_click[num] > 0)	// Verifica se jogada já foi feita
					repeated = true;

				if (repeated == false)
				{
					face_baixo.pos_click[num] = 1;
					encontrado = true;
				}
				repeated = false;
			}
		}
	}
	/* FACE CIMA */
	/* Primeiro o PC ve se consegue ganhar */

	else if (face_activa == 5)
	{
		face_cima.num_jogada++;
		short int PC_simbolo = face_cima.num_jogada % 2;
		short int jgdr_simbolo = !PC_simbolo;

		// Linha da esquerda vertical
		if (face_cima.simbolo[0] == 2 && face_cima.simbolo[1] == PC_simbolo && face_cima.simbolo[2] == PC_simbolo)
			face_cima.pos_click[0] = 1;
		else if (face_cima.simbolo[0] == PC_simbolo && face_cima.simbolo[1] == 2 && face_cima.simbolo[2] == PC_simbolo)
			face_cima.pos_click[1] = 1;
		else if (face_cima.simbolo[0] == PC_simbolo && face_cima.simbolo[1] == PC_simbolo && face_cima.simbolo[2] == 2)
			face_cima.pos_click[2] = 1;

		// Linha vertical meio
		else if (face_cima.simbolo[3] == 2 && face_cima.simbolo[4] == PC_simbolo && face_cima.simbolo[5] == PC_simbolo)
			face_cima.pos_click[3] = 1;
		else if (face_cima.simbolo[3] == PC_simbolo && face_cima.simbolo[4] == 2 && face_cima.simbolo[5] == PC_simbolo)
			face_cima.pos_click[4] = 1;
		else if (face_cima.simbolo[3] == PC_simbolo && face_cima.simbolo[4] == PC_simbolo && face_cima.simbolo[5] == 2)
			face_cima.pos_click[5] = 1;

		// Linha direita vertical
		else if (face_cima.simbolo[6] == 2 && face_cima.simbolo[7] == PC_simbolo && face_cima.simbolo[8] == PC_simbolo)
			face_cima.pos_click[6] = 1;
		else if (face_cima.simbolo[6] == PC_simbolo && face_cima.simbolo[7] == 2 && face_cima.simbolo[8] == PC_simbolo)
			face_cima.pos_click[7] = 1;
		else if (face_cima.simbolo[6] == PC_simbolo && face_cima.simbolo[7] == PC_simbolo && face_cima.simbolo[8] == 2)
			face_cima.pos_click[8] = 1;


		// Linha de topo horizontal
		else if (face_cima.simbolo[0] == 2 && face_cima.simbolo[3] == PC_simbolo && face_cima.simbolo[6] == PC_simbolo)
			face_cima.pos_click[0] = 1;
		else if (face_cima.simbolo[0] == PC_simbolo && face_cima.simbolo[3] == 2 && face_cima.simbolo[6] == PC_simbolo)
			face_cima.pos_click[3] = 1;
		else if (face_cima.simbolo[0] == PC_simbolo && face_cima.simbolo[3] == PC_simbolo && face_cima.simbolo[6] == 2)
			face_cima.pos_click[6] = 1;

		// Linha meio horizontal
		else if (face_cima.simbolo[1] == 2 && face_cima.simbolo[4] == PC_simbolo && face_cima.simbolo[7] == PC_simbolo)
			face_cima.pos_click[1] = 1;
		else if (face_cima.simbolo[1] == PC_simbolo && face_cima.simbolo[4] == 2 && face_cima.simbolo[7] == PC_simbolo)
			face_cima.pos_click[4] = 1;
		else if (face_cima.simbolo[1] == PC_simbolo && face_cima.simbolo[4] == PC_simbolo && face_cima.simbolo[7] == 2)
			face_cima.pos_click[7] = 1;

		// Linha horizontal baixo
		else if (face_cima.simbolo[2] == 2 && face_cima.simbolo[5] == PC_simbolo && face_cima.simbolo[8] == PC_simbolo)
			face_cima.pos_click[2] = 1;
		else if (face_cima.simbolo[2] == PC_simbolo && face_cima.simbolo[5] == 2 && face_cima.simbolo[8] == PC_simbolo)
			face_cima.pos_click[5] = 1;
		else if (face_cima.simbolo[2] == PC_simbolo && face_cima.simbolo[5] == PC_simbolo && face_cima.simbolo[8] == 2)
			face_cima.pos_click[8] = 1;

		// linha diagonal cima-baixo esq-dir
		else if (face_cima.simbolo[0] == 2 && face_cima.simbolo[4] == PC_simbolo && face_cima.simbolo[8] == PC_simbolo)
			face_cima.pos_click[0] = 1;
		else if (face_cima.simbolo[0] == PC_simbolo && face_cima.simbolo[4] == 2 && face_cima.simbolo[8] == PC_simbolo)
			face_cima.pos_click[4] = 1;
		else if (face_cima.simbolo[0] == PC_simbolo && face_cima.simbolo[4] == PC_simbolo && face_cima.simbolo[8] == 2)
			face_cima.pos_click[8] = 1;

		// Linha diagonal baixo-cima dir-esq
		else if (face_cima.simbolo[2] == 2 && face_cima.simbolo[4] == PC_simbolo && face_cima.simbolo[6] == PC_simbolo)
			face_cima.pos_click[2] = 1;
		else if (face_cima.simbolo[2] == PC_simbolo && face_cima.simbolo[4] == 2 && face_cima.simbolo[6] == PC_simbolo)
			face_cima.pos_click[4] = 1;
		else if (face_cima.simbolo[2] == PC_simbolo && face_cima.simbolo[4] == PC_simbolo && face_cima.simbolo[6] == 2)
			face_cima.pos_click[6] = 1;



		// Depois ve se consegue evitar derrota

		// Linha da esquerda vertical
		else if (face_cima.simbolo[0] == 2 && face_cima.simbolo[1] == jgdr_simbolo && face_cima.simbolo[2] == jgdr_simbolo)
			face_cima.pos_click[0] = 1;
		else if (face_cima.simbolo[0] == jgdr_simbolo && face_cima.simbolo[1] == 2 && face_cima.simbolo[2] == jgdr_simbolo)
			face_cima.pos_click[1] = 1;
		else if (face_cima.simbolo[0] == jgdr_simbolo && face_cima.simbolo[1] == jgdr_simbolo && face_cima.simbolo[2] == 2)
			face_cima.pos_click[2] = 1;

		// Linha vertical meio
		else if (face_cima.simbolo[3] == 2 && face_cima.simbolo[4] == jgdr_simbolo && face_cima.simbolo[5] == jgdr_simbolo)
			face_cima.pos_click[3] = 1;
		else if (face_cima.simbolo[3] == jgdr_simbolo && face_cima.simbolo[4] == 2 && face_cima.simbolo[5] == jgdr_simbolo)
			face_cima.pos_click[4] = 1;
		else if (face_cima.simbolo[3] == jgdr_simbolo && face_cima.simbolo[4] == jgdr_simbolo && face_cima.simbolo[5] == 2)
			face_cima.pos_click[5] = 1;

		// Linha direita vertical
		else if (face_cima.simbolo[6] == 2 && face_cima.simbolo[7] == jgdr_simbolo && face_cima.simbolo[8] == jgdr_simbolo)
			face_cima.pos_click[6] = 1;
		else if (face_cima.simbolo[6] == jgdr_simbolo && face_cima.simbolo[7] == 2 && face_cima.simbolo[8] == jgdr_simbolo)
			face_cima.pos_click[7] = 1;
		else if (face_cima.simbolo[6] == jgdr_simbolo && face_cima.simbolo[7] == jgdr_simbolo && face_cima.simbolo[8] == 2)
			face_cima.pos_click[8] = 1;


		// Linha de topo horizontal
		else if (face_cima.simbolo[0] == 2 && face_cima.simbolo[3] == jgdr_simbolo && face_cima.simbolo[6] == jgdr_simbolo)
			face_cima.pos_click[0] = 1;
		else if (face_cima.simbolo[0] == jgdr_simbolo && face_cima.simbolo[3] == 2 && face_cima.simbolo[6] == jgdr_simbolo)
			face_cima.pos_click[3] = 1;
		else if (face_cima.simbolo[0] == jgdr_simbolo && face_cima.simbolo[3] == jgdr_simbolo && face_cima.simbolo[6] == 2)
			face_cima.pos_click[6] = 1;

		// Linha meio horizontal
		else if (face_cima.simbolo[1] == 2 && face_cima.simbolo[4] == jgdr_simbolo && face_cima.simbolo[7] == jgdr_simbolo)
			face_cima.pos_click[1] = 1;
		else if (face_cima.simbolo[1] == jgdr_simbolo && face_cima.simbolo[4] == 2 && face_cima.simbolo[7] == jgdr_simbolo)
			face_cima.pos_click[4] = 1;
		else if (face_cima.simbolo[1] == jgdr_simbolo && face_cima.simbolo[4] == jgdr_simbolo && face_cima.simbolo[7] == 2)
			face_cima.pos_click[7] = 1;

		// Linha horizontal baixo
		else if (face_cima.simbolo[2] == 2 && face_cima.simbolo[5] == jgdr_simbolo && face_cima.simbolo[8] == jgdr_simbolo)
			face_cima.pos_click[2] = 1;
		else if (face_cima.simbolo[2] == jgdr_simbolo && face_cima.simbolo[5] == 2 && face_cima.simbolo[8] == jgdr_simbolo)
			face_cima.pos_click[5] = 1;
		else if (face_cima.simbolo[2] == jgdr_simbolo && face_cima.simbolo[5] == jgdr_simbolo && face_cima.simbolo[8] == 2)
			face_cima.pos_click[8] = 1;

		// linha diagonal cima-baixo esq-dir
		else if (face_cima.simbolo[0] == 2 && face_cima.simbolo[4] == jgdr_simbolo && face_cima.simbolo[8] == jgdr_simbolo)
			face_cima.pos_click[0] = 1;
		else if (face_cima.simbolo[0] == jgdr_simbolo && face_cima.simbolo[4] == 2 && face_cima.simbolo[8] == jgdr_simbolo)
			face_cima.pos_click[4] = 1;
		else if (face_cima.simbolo[0] == jgdr_simbolo && face_cima.simbolo[4] == jgdr_simbolo && face_cima.simbolo[8] == 2)
			face_cima.pos_click[8] = 1;

		// Linha diagonal baixo-cima dir-esq
		else if (face_cima.simbolo[2] == 2 && face_cima.simbolo[4] == jgdr_simbolo && face_cima.simbolo[6] == jgdr_simbolo)
			face_cima.pos_click[2] = 1;
		else if (face_cima.simbolo[2] == jgdr_simbolo && face_cima.simbolo[4] == 2 && face_cima.simbolo[6] == jgdr_simbolo)
			face_cima.pos_click[4] = 1;
		else if (face_cima.simbolo[2] == jgdr_simbolo && face_cima.simbolo[4] == jgdr_simbolo && face_cima.simbolo[6] == 2)
			face_cima.pos_click[6] = 1;


		// Caso contrario, joga aleatoriamente
		else
		{
			bool encontrado = false;
			bool repeated = false; // Detecta se determinada posicao da grelha ja foi jogada
			while (!encontrado && jogos[5] == -1)
			{
				int num, i = 0;

				num = rand() % 9; // Armazena posicao da grelha

				if (face_cima.pos_click[num] > 0)	// Verifica se jogada já foi feita
					repeated = true;

				if (repeated == false)
				{
					face_cima.pos_click[num] = 1;
					encontrado = true;
				}
				repeated = false;
			}
		}
	}
	PC_turno = 0;
}

void Copia_Dados_Face(face_cubo &A, face_cubo &B)
{
	B.margem_face = A.margem_face;
	B.num_jogada = A.num_jogada;

	for (int i = 0; i < 9; i++) {
		B.pos_click[i] = A.pos_click[i];
		B.simbolo[i] = A.simbolo[i];
	}
}

void Inicia_Dados_Face(face_cubo &A)
{
	A.margem_face = 4;
	A.num_jogada = 0;

	for (int i = 0; i < 9; i++) {
		A.pos_click[i] = 0;
		A.simbolo[i] = 2;
	}
}

void Creditos() {

	/* Desenha quadrado para colocar posterior imagem de fundo*/

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f(1.0, 1.0, 1.0);

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texture_id[10]);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-125, -95, -0.1);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(125, -95, -0.1);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(125, 95, -0.1);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-125, 95, -0.1);

	glEnd();

	glDisable(GL_TEXTURE_2D);

}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	if (modo_jogo == 9) //	Menu inicial
	{
		gluLookAt(0, 0, 200, 0, 0, 0, 0, 1, 0);

		if (escolha == 0) {
			Menu_Inicial();
			glutSwapBuffers();
		}

		else if (escolha == 1 || escolha == 2) {
			Menu_Info_Normal();
			glutSwapBuffers();
		}
		else if (escolha == 3) {
			Menu_Info_Vista();
			glutSwapBuffers();
		}
		else if (escolha == 4) {
			Creditos();
			glutSwapBuffers();
		}


	}
	else  // Modo de jogo <vs PC> <2 players> ou <free view>
	{

		if (modo_jogo == 2) {	// free view
			// posicionamento da câmara virtual com função gluLookAt
			glutSetMenu(menu2);
			glutAttachMenu(GLUT_RIGHT_BUTTON);
			gluLookAt(camara_x, camara_y, camara_z, mira_x, mira_y, mira_z, 0, 1, 0);
			start_face = 1;
		}
		else
			gluLookAt(0, 0, 200, 0, 0, 0, 0, 1, 0);


		glEnable(GL_TEXTURE_2D);
		if (modo_jogo == 0 || modo_jogo == 1) // <2 players> ou <VS PC>
		{
			glutSetMenu(menu1);
			glutAttachMenu(GLUT_RIGHT_BUTTON);


			if (roda_esq == 1) {
				yRotated -= 0.5;
				if (face_activa == 0) // id da face: 0-frontal | 1-direita | 2-esquerda | 3-tras | 4-baixo | 5-cima
					face_activa2 = 1;
				else if (face_activa == 1)
					face_activa2 = 3;
				else if (face_activa == 3)
					face_activa2 = 2;
				else if (face_activa == 2)
					face_activa2 = 0;
			}
			else if (roda_dir == 1) {
				yRotated += 0.5;
				if (face_activa == 0) // id da face: 0-frontal | 1-direita | 2-esquerda | 3-tras | 4-baixo | 5-cima
					face_activa2 = 2;
				else if (face_activa == 2)
					face_activa2 = 3;
				else if (face_activa == 3)
					face_activa2 = 1;
				else if (face_activa == 1)
					face_activa2 = 0;
			}

			else if (roda_cima == 1) {
				xRotated += 0.5;

				if (yRotated == 0) {

					if (face_activa == 0) // id da face: 0-frontal | 1-direita | 2-esquerda | 3-tras | 4-baixo | 5-cima
						face_activa2 = 5;
					else if (face_activa == 5)
						face_activa2 = 3;
					else if (face_activa == 3)
						face_activa2 = 4;
					else if (face_activa == 4)
						face_activa2 = 0;
				}
				else if (yRotated == -90 || yRotated == 270){

					if (face_activa == 1)
						face_activa2 = 5;
					else if (face_activa == 5)
						face_activa2 = 2;
					else if (face_activa == 2)
						face_activa2 = 4;
					else if (face_activa == 4)
						face_activa2 = 1;
				}
				if (yRotated == -180 || yRotated == 180){

					if (face_activa == 3)
						face_activa2 = 5;
					else if (face_activa == 5)
						face_activa2 = 0;
					else if (face_activa == 0)
						face_activa2 = 4;
					else if (face_activa == 4)
						face_activa2 = 3;
				}
				if (yRotated == 90 || yRotated == -270){

					if (face_activa == 2)
						face_activa2 = 5;
					else if (face_activa == 5)
						face_activa2 = 1;
					else if (face_activa == 1)
						face_activa2 = 4;
					else if (face_activa == 4)
						face_activa2 = 2;
				}

			}
			else if (roda_baixo == 1) {
				xRotated -= 0.5;

				if (yRotated == 0) {

					if (face_activa == 0) // id da face: 0-frontal | 1-direita | 2-esquerda | 3-tras | 4-baixo | 5-cima
						face_activa2 = 4;
					else if (face_activa == 4)
						face_activa2 = 3;
					else if (face_activa == 3)
						face_activa2 = 5;
					else if (face_activa == 5)
						face_activa2 = 0;
				}
				else if (yRotated == -90 || yRotated == 270){

					if (face_activa == 1)
						face_activa2 = 4;
					else if (face_activa == 4)
						face_activa2 = 2;
					else if (face_activa == 2)
						face_activa2 = 5;
					else if (face_activa == 5)
						face_activa2 = 1;
				}
				else if (yRotated == -180 || yRotated == 180){

					if (face_activa == 3)
						face_activa2 = 4;
					else if (face_activa == 4)
						face_activa2 = 0;
					else if (face_activa == 0)
						face_activa2 = 5;
					else if (face_activa == 5)
						face_activa2 = 3;
				}
				if (yRotated == 90 || yRotated == -270){

					if (face_activa == 2)
						face_activa2 = 4;
					else if (face_activa == 4)
						face_activa2 = 1;
					else if (face_activa == 1)
						face_activa2 = 5;
					else if (face_activa == 5)
						face_activa2 = 2;
				}
			}



			if ((yRotated == 0 || yRotated == 90 || yRotated == 180 || yRotated == 270 || yRotated == 360 || yRotated == -90 || yRotated == -180 || yRotated == -270 || yRotated == -360) && (roda_esq == 1 || roda_dir == 1))
			{
				roda_esq = 0;
				roda_dir = 0;
				face_activa = face_activa2;
			}
			if ((xRotated == 0 || xRotated == 90 || xRotated == 180 || xRotated == 270 || xRotated == 360 || xRotated == -90 || xRotated == -180 || xRotated == -270 || xRotated == -360) && (roda_cima == 1 || roda_baixo == 1))
			{
				roda_cima = 0;
				roda_baixo = 0;
				face_activa = face_activa2;
			}

			if (yRotated == 360 || yRotated == -360) {
				yRotated = 0;
			}
			if (xRotated == 360 || xRotated == -360) {
				xRotated = 0;
			}


			if (yRotated == 360 || yRotated == -360) {
				yRotated = 0;
			}
			if (xRotated == 360 || xRotated == -360) {
				xRotated = 0;
			}


			glMatrixMode(GL_MODELVIEW);

			glPushMatrix();

			glRotatef(xRotated, 1.0, 0.0, .0);
			glRotatef(yRotated, 0.0, 1.0, .0);

		}

		else if (modo_jogo == 3) {
			Modo_Automatico();
		}

		Desenha_Cubo();

		Detecta_Jogada();
		if (modo_jogo == 1 && PC_turno == 1)
			Determina_Turno_PC();

		glPopMatrix();

		Desenha_Chao();
		Desenha_Tecto();
		Desenha_Paredes();
		Mensagens_Victoria();

		glRotatef(xRotated, 1.0, 0.0, .0);
		glRotatef(yRotated, 0.0, 1.0, .0);

		glutSwapBuffers();
	}


	/* DEBUG */

	//printf("FACE ACTIVA : %d \n", face_activa2);
	//printf("START_FACE : %d E FACE ACTIVA : %d  JOGO[FACE ACTIVA] : %d\n", start_face, face_activa2, jogos[face_activa]);

	//printf("XROTATEF : %f \n", xRotated);
	//printf("YROTATEF : %f \n", yRotated);

	//printf("Roda dir: %d \n", roda_dir);
	//printf("Roda esq: %d \n", roda_esq);
	//printf("Roda cima: %d \n", roda_cima);
	//printf("Roda baixo: %d \n", roda_baixo);

	glutPostRedisplay();

}



/************************************************************/
/*					MAIN									*/
/************************************************************/

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")  // Esconder consola

int main(int argc, char* argv[]){

	srand(time(NULL)); // Seed para gerar num pseudo aleatorios

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(largura, altura);
	glutCreateWindow("GALO 3D");
	glutReshapeFunc(redimensiona_janela);

	cria_menus_rato();
	cria_menus_rato2();

	glEnable(GL_DEPTH_TEST);

	loadTextureFromFile(); // carrega texturas

	Inicia_Valores();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(tecla_especial_premida);
	glutMouseFunc(Detecta_Pos);

	glutMainLoop();

	free(contax);
	free(contay);

	system("PAUSE");
	return 0;
}