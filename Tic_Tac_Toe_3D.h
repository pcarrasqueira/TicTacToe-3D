#ifndef Tic_Tac_Toe_3D
#define Tic_Tac_Toe_3D

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <glut.h>
#include "tgaload.h"
#include <math.h>
#include <time.h>
#include <mmsystem.h>

//	Project->Tic...Tac..Toe..3D..Properties->Config properties->Preprocessor->C++-> EDIT -> add  _CRT_SECURE_NO_WARNINGS

struct face_cubo
{
	/* Detecta click em posicoes da grelha */
	short int pos_click[9]; //					| esq_cima  | esq_meio  | esq_baixo    |		| 0 1 2 | 
	//					| meio_cima | meio meio | meio_baixo   |  ->	| 3 4 5 |
	//					| dir_cima  | dir_meio  | dir_baixo    |		| 6 7 8 |

	short int simbolo[9]; // 0 - Bola | 1 - Cruz
	GLfloat margem_face; // Cada face tem uma determinada expessura que é preciso ter em conta
	GLint num_jogada; // Numero da jogada da face
};


void Desenha_Circulo_face_frontal(GLfloat x, GLfloat y, GLfloat z, GLfloat raio);
void Desenha_Circulo_face_direita(GLfloat x, GLfloat y, GLfloat z, GLfloat raio);
void Desenha_X_face_frontal(GLint x_esq_cima, GLint y_esq_cima, GLint z_esq_cima, GLint largura);
void Desenha_X_face_direita(GLint x_esq_cima, GLint y_esq_cima, GLint z_esq_cima, GLint largura);
void Desenha_Cubo();
void isGameOver();
void Detecta_Jogada();
void redimensiona_janela(int Larg, int Alt);
void keyboard(unsigned char key, GLint x, GLint y);
struct pos GetOGLPos(int x, int y);
void Detecta_Pos(GLint botao, GLint estado, GLint xRato, GLint yRato);
void display();
void loadTextureFromFile();
void Desenha_Chao();
void Desenha_Circulo_face_cima_baixo(GLfloat x, GLfloat y, GLfloat z, GLfloat raio);
void Desenha_X_face_baixo(GLint x_esq_cima, GLint y_esq_cima, GLint z_esq_cima, GLint largura);
void Desenha_X_face_cima(GLint x_esq_cima, GLint y_esq_cima, GLint z_esq_cima, GLint largura);
void Desenha_Paredes();
void Menu_Inicial();
void tecla_especial_premida(int tecla, int x, int y);
void Desenha_Tecto();
void Menu_Info_Normal();
void Menu_Info_Vista();
void cria_menus_rato();
void Menu_Rato1(int value);
void Inicia_Valores();
void cria_menus_rato2();
void Menu_Rato2(int value);
void Mensagens_Victoria();
void Modo_Automatico();
void Determina_Turno_PC();
void Copia_Dados_Face(face_cubo &A, face_cubo &B);
void Inicia_Dados_Face(face_cubo &A);
void Creditos();
#endif