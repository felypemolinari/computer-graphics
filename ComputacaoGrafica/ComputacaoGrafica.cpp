#include <iostream>
#include <GL/freeglut.h>
#include <vector>
#include <cmath>

struct vertice {
	double x, y, z;
};
using lista_vertices = std::vector<vertice>;
using aresta = std::pair<int, int>;
using lista_arestas = std::vector<aresta>;

struct Poligono {
	double tamanhoLado;
	vertice posicao;
	vertice escala;
	vertice rotacao;
	lista_vertices vertices;
	lista_arestas arestas;
};

Poligono criar_cubo(double posicao_x, double posicao_y, double posicao_z, double tamanho_lado);
void desenhar(Poligono poligono);
void movimentar(Poligono& poligono, double distancia_x, double distancia_y, double distancia_z);
void escalar(Poligono& poligono, double escala_x, double escala_y, double escala_z);
void rotacionar(Poligono& poligono, double angulo_x, double angulo_y, double angulo_z);
void display();
void reshape(int largura, int altura);
void redraw(int value);
void keyboard(unsigned char key, int x, int y);
void keyboard_special(int key, int x, int y);
void mouse(int botao, int estado, int x, int y);
void mouse_movimento(int x, int y);
void mouse_roda(int roda, int direcao, int x, int y);
void imprimir_controles();

// A camera fica parada sobre o eixo Z, olhando para a origem. A projecao em
// perspectiva e feita na mao em projetar(), sem usar as matrizes do OpenGL.
const double CAMERA_Z = 8.0;      // posicao da camera no eixo Z
const double DISTANCIA_FOCAL = 4.0; // distancia da camera ate o plano de projecao
const double PI = 3.1415926536;

Poligono cubo;
int delay = 10;

// Estado do arrasto do mouse (rotaciona o cubo)
bool arrastando = false;
int mouse_ultimo_x = 0;
int mouse_ultimo_y = 0;

int main(int argc, char** argv) {

	cubo = criar_cubo(0, 0, 0, 2);

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 600);

	glutCreateWindow("Trabalho M1 - Cubo em perspectiva");
	glClearColor(1.0, 1.0, 1.0, 1.0);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboard_special);
	glutMouseFunc(mouse);
	glutMotionFunc(mouse_movimento);
	glutMouseWheelFunc(mouse_roda);
	glutTimerFunc(10, redraw, 0);

	imprimir_controles();

	glutMainLoop();

	return 0;
}


void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);

	desenhar(cubo);

	glutSwapBuffers();
}

// O sistema de coordenadas da janela vai de -1 a 1 na vertical e acompanha a
// proporcao da janela na horizontal, para o cubo nao deformar ao redimensionar.
void reshape(int largura, int altura) {
	if (altura == 0) altura = 1;
	double aspecto = double(largura) / double(altura);

	glViewport(0, 0, largura, altura);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-aspecto, aspecto, -1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
		exit(0);
		break;

	// Translacao no plano XY
	case 'w':
		movimentar(cubo, 0, 0.1, 0);
		break;
	case 's':
		movimentar(cubo, 0, -0.1, 0);
		break;
	case 'a':
		movimentar(cubo, -0.1, 0, 0);
		break;
	case 'd':
		movimentar(cubo, 0.1, 0, 0);
		break;

	// Translacao em profundidade
	case 'q':
		movimentar(cubo, 0, 0, 0.1);
		break;
	case 'e':
		movimentar(cubo, 0, 0, -0.1);
		break;

	// Escala uniforme
	case '+':
		escalar(cubo, 1.1, 1.1, 1.1);
		break;
	case '-':
		escalar(cubo, 1 / 1.1, 1 / 1.1, 1 / 1.1);
		break;

	// Escala em um eixo de cada vez
	case 'x':
		escalar(cubo, 1.1, 1, 1);
		break;
	case 'X':
		escalar(cubo, 1 / 1.1, 1, 1);
		break;
	case 'y':
		escalar(cubo, 1, 1.1, 1);
		break;
	case 'Y':
		escalar(cubo, 1, 1 / 1.1, 1);
		break;
	case 'z':
		escalar(cubo, 1, 1, 1.1);
		break;
	case 'Z':
		escalar(cubo, 1, 1, 1 / 1.1);
		break;

	// Rotacao no eixo Z
	case 'j':
		rotacionar(cubo, 0, 0, 5);
		break;
	case 'l':
		rotacionar(cubo, 0, 0, -5);
		break;

	case 'r':
		cubo = criar_cubo(0, 0, 0, 2);
		break;
	}
}

void keyboard_special(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		rotacionar(cubo, -5, 0, 0);
		break;

	case GLUT_KEY_DOWN:
		rotacionar(cubo, 5, 0, 0);
		break;

	case GLUT_KEY_LEFT:
		rotacionar(cubo, 0, -5, 0);
		break;

	case GLUT_KEY_RIGHT:
		rotacionar(cubo, 0, 5, 0);
		break;
	}
}

void mouse(int botao, int estado, int x, int y) {
	if (botao == GLUT_LEFT_BUTTON) {
		arrastando = (estado == GLUT_DOWN);
		mouse_ultimo_x = x;
		mouse_ultimo_y = y;
	}
}

// Arrastar com o botao esquerdo rotaciona o cubo: o movimento horizontal gira
// em torno do eixo Y e o vertical em torno do eixo X.
void mouse_movimento(int x, int y) {
	if (!arrastando) return;

	int delta_x = x - mouse_ultimo_x;
	int delta_y = y - mouse_ultimo_y;

	rotacionar(cubo, delta_y * 0.5, delta_x * 0.5, 0);

	mouse_ultimo_x = x;
	mouse_ultimo_y = y;
}

void mouse_roda(int roda, int direcao, int x, int y) {
	if (direcao > 0)
		escalar(cubo, 1.1, 1.1, 1.1);
	else
		escalar(cubo, 1 / 1.1, 1 / 1.1, 1 / 1.1);
}

void redraw(int value) {
	glutPostRedisplay();
	glutTimerFunc(delay, redraw, 0);
}

// Descricao direta dos 8 vertices e das 12 arestas do cubo.
Poligono criar_cubo(double posicao_x, double posicao_y, double posicao_z, double tamanho_lado) {
	Poligono novo_cubo;
	novo_cubo.tamanhoLado = tamanho_lado;

	novo_cubo.posicao = { posicao_x, posicao_y, posicao_z };
	novo_cubo.escala = { 1, 1, 1 };
	novo_cubo.rotacao = { 0, 0, 0 };

	double m = tamanho_lado / 2.0; // meia aresta, a partir do centro do cubo

	// Face de tras (z = -m), indices 0 a 3
	novo_cubo.vertices.push_back({ posicao_x - m, posicao_y - m, posicao_z - m }); // 0
	novo_cubo.vertices.push_back({ posicao_x + m, posicao_y - m, posicao_z - m }); // 1
	novo_cubo.vertices.push_back({ posicao_x + m, posicao_y + m, posicao_z - m }); // 2
	novo_cubo.vertices.push_back({ posicao_x - m, posicao_y + m, posicao_z - m }); // 3
	// Face da frente (z = +m), indices 4 a 7
	novo_cubo.vertices.push_back({ posicao_x - m, posicao_y - m, posicao_z + m }); // 4
	novo_cubo.vertices.push_back({ posicao_x + m, posicao_y - m, posicao_z + m }); // 5
	novo_cubo.vertices.push_back({ posicao_x + m, posicao_y + m, posicao_z + m }); // 6
	novo_cubo.vertices.push_back({ posicao_x - m, posicao_y + m, posicao_z + m }); // 7

	// Arestas da face de tras
	novo_cubo.arestas.push_back(aresta(0, 1));
	novo_cubo.arestas.push_back(aresta(1, 2));
	novo_cubo.arestas.push_back(aresta(2, 3));
	novo_cubo.arestas.push_back(aresta(3, 0));
	// Arestas da face da frente
	novo_cubo.arestas.push_back(aresta(4, 5));
	novo_cubo.arestas.push_back(aresta(5, 6));
	novo_cubo.arestas.push_back(aresta(6, 7));
	novo_cubo.arestas.push_back(aresta(7, 4));
	// Arestas que ligam as duas faces
	novo_cubo.arestas.push_back(aresta(0, 4));
	novo_cubo.arestas.push_back(aresta(1, 5));
	novo_cubo.arestas.push_back(aresta(2, 6));
	novo_cubo.arestas.push_back(aresta(3, 7));

	std::cout << "Vertices:\n";
	for (int i = 0; i < novo_cubo.vertices.size(); i++) {
		std::cout << i << " - " << novo_cubo.vertices[i].x << " - "
			<< novo_cubo.vertices[i].y << " - " << novo_cubo.vertices[i].z << "\n";
	}

	std::cout << "Arestas:\n";
	for (int i = 0; i < novo_cubo.arestas.size(); i++) {
		std::cout << novo_cubo.arestas[i].first << " - " << novo_cubo.arestas[i].second << "\n";
	}

	return novo_cubo;
}

// Translacao: soma o deslocamento em todos os vertices e no centro.
void movimentar(Poligono& poligono, double distancia_x, double distancia_y, double distancia_z) {
	for (int i = 0; i < poligono.vertices.size(); i++) {
		poligono.vertices[i].x += distancia_x;
		poligono.vertices[i].y += distancia_y;
		poligono.vertices[i].z += distancia_z;
	}

	poligono.posicao.x += distancia_x;
	poligono.posicao.y += distancia_y;
	poligono.posicao.z += distancia_z;
}

// Escala em torno do centro: leva o vertice para a origem, multiplica pelo
// fator de cada eixo e devolve para a posicao original.
void escalar(Poligono& poligono, double escala_x, double escala_y, double escala_z) {
	for (int i = 0; i < poligono.vertices.size(); i++) {
		double x = poligono.vertices[i].x - poligono.posicao.x;
		double y = poligono.vertices[i].y - poligono.posicao.y;
		double z = poligono.vertices[i].z - poligono.posicao.z;

		poligono.vertices[i].x = poligono.posicao.x + x * escala_x;
		poligono.vertices[i].y = poligono.posicao.y + y * escala_y;
		poligono.vertices[i].z = poligono.posicao.z + z * escala_z;
	}

	poligono.escala.x *= escala_x;
	poligono.escala.y *= escala_y;
	poligono.escala.z *= escala_z;
}

// Rotacao em torno do centro, aplicando na ordem X, Y e Z. Os angulos chegam
// em graus e sao convertidos para radianos.
void rotacionar(Poligono& poligono, double angulo_x, double angulo_y, double angulo_z) {
	double ax = (angulo_x / 180.0) * PI;
	double ay = (angulo_y / 180.0) * PI;
	double az = (angulo_z / 180.0) * PI;

	double sen_x = sin(ax), cos_x = cos(ax);
	double sen_y = sin(ay), cos_y = cos(ay);
	double sen_z = sin(az), cos_z = cos(az);

	for (int i = 0; i < poligono.vertices.size(); i++) {
		double x = poligono.vertices[i].x - poligono.posicao.x;
		double y = poligono.vertices[i].y - poligono.posicao.y;
		double z = poligono.vertices[i].z - poligono.posicao.z;

		// Rotacao no eixo X: X fica parado, Y e Z giram
		double y1 = y * cos_x - z * sen_x;
		double z1 = y * sen_x + z * cos_x;

		// Rotacao no eixo Y: Y fica parado, X e Z giram
		double x2 = x * cos_y + z1 * sen_y;
		double z2 = -x * sen_y + z1 * cos_y;

		// Rotacao no eixo Z: Z fica parado, X e Y giram
		double x3 = x2 * cos_z - y1 * sen_z;
		double y3 = x2 * sen_z + y1 * cos_z;

		poligono.vertices[i].x = poligono.posicao.x + x3;
		poligono.vertices[i].y = poligono.posicao.y + y3;
		poligono.vertices[i].z = poligono.posicao.z + z2;
	}

	poligono.rotacao.x += angulo_x;
	poligono.rotacao.y += angulo_y;
	poligono.rotacao.z += angulo_z;
}

// Projecao em perspectiva: quanto mais longe da camera o ponto esta, mais ele
// se aproxima do centro da tela. profundidade e a distancia do ponto ate a
// camera medida no eixo Z.
vertice projetar(vertice v) {
	double profundidade = CAMERA_Z - v.z;

	// Evita divisao por zero (e a inversao da imagem) quando o vertice passa
	// por tras da camera.
	if (profundidade < 0.1) profundidade = 0.1;

	vertice projetado;
	projetado.x = (DISTANCIA_FOCAL * v.x) / profundidade;
	projetado.y = (DISTANCIA_FOCAL * v.y) / profundidade;
	projetado.z = profundidade;

	return projetado;
}

// As arestas mais proximas da camera sao desenhadas mais escuras, o que ajuda
// a enxergar a profundidade no wireframe.
void cor_por_profundidade(double profundidade) {
	double t = (profundidade - (CAMERA_Z - 3.0)) / 6.0;
	if (t < 0) t = 0;
	if (t > 1) t = 1;

	glColor3d(0.1 + 0.6 * t, 0.1 + 0.6 * t, 0.3 + 0.5 * t);
}

void desenhar(Poligono poligono) {
	glBegin(GL_LINES);
	for (int i = 0; i < poligono.arestas.size(); i++) {
		int v_o = poligono.arestas[i].first;
		int v_d = poligono.arestas[i].second;

		vertice origem = projetar(poligono.vertices[v_o]);
		vertice destino = projetar(poligono.vertices[v_d]);

		cor_por_profundidade(origem.z);
		glVertex2d(origem.x, origem.y);
		cor_por_profundidade(destino.z);
		glVertex2d(destino.x, destino.y);
	}
	glEnd();
}

void imprimir_controles() {
	std::cout << "\nControles:\n";
	std::cout << "  Setas          - rotacionar nos eixos X e Y\n";
	std::cout << "  J / L          - rotacionar no eixo Z\n";
	std::cout << "  Arrastar mouse - rotacionar nos eixos X e Y\n";
	std::cout << "  W A S D        - mover no plano XY\n";
	std::cout << "  Q / E          - mover no eixo Z (aproximar / afastar)\n";
	std::cout << "  + / -          - escala uniforme\n";
	std::cout << "  Roda do mouse  - escala uniforme\n";
	std::cout << "  x X y Y z Z    - escala em um eixo por vez\n";
	std::cout << "  R              - reiniciar o cubo\n";
	std::cout << "  ESC            - sair\n\n";
}
