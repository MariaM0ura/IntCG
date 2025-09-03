#include <GL/glut.h>
#include <stdlib.h>

GLfloat ecursivo[4][3] = {
   {1.0, 0.1, 0.0},   // P1 
   {0.0, 1.0, 0.0},   // P2
   {1.0, 1.0, 0.0},   // P3
   {0.5, 0.1, 0.0}    // P4
};

GLfloat ecursivo2[4][3] = {
   {1.5, 0.1, 0.0},   // P1 
   {0.5, 1.5, 0.0},   // P2
   {2.0, 1.5, 0.0},   // P3
   {1.0, 0.1, 0.0}    // P4
};

GLfloat ecursivo3[4][3] = {
   {2.0, 0.1, 0.0},   // P1 
   {1.0, 1.0, 0.0},   // P2
   {2.5, 1.0, 0.0},   // P3
   {1.5, 0.1, 0.0}    // P4
};




void init(void)
{
   glClearColor(0.0, 0.0, 0.0, 0.0);
   glShadeModel(GL_FLAT);
   glEnable(GL_MAP1_VERTEX_3);
}

void drawBezier(GLfloat ctrlpoints[4][3], float r, float g, float b) {
   int i;

   // Define o mapa para os pontos da curva
   glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints[0][0]);
   glColor3f(r, g, b);

   // Desenha a curva
   glBegin(GL_LINE_STRIP);
   for (i = 0; i <= 30; i++) 
      glEvalCoord1f((GLfloat) i/30.0);
   glEnd();

   // Mostra os pontos de controle (opcional)
   glPointSize(5.0);
   glColor3f(1.0, 1.0, 0.0);
   glBegin(GL_POINTS);
   for (i = 0; i < 4; i++) 
      glVertex3fv(&ctrlpoints[i][0]);
   glEnd();

   glColor3f(1.0, 1.0, 1.0); // volta para branco
}

void display(void)
{
   glClear(GL_COLOR_BUFFER_BIT);

   glColor3f(1.0, 1.0, 1.0);

   
   drawBezier(ecursivo, 0.0, 0.0, 1.0);
   drawBezier(ecursivo2, 0.0, 1.0, 0.0);
   drawBezier(ecursivo3, 1.0, 0.0, 0.0);

   glFlush();
}

void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (w <= h)
      glOrtho(-2.0, 2.0, -2.0*(GLfloat)h/(GLfloat)w, 
               2.0*(GLfloat)h/(GLfloat)w, -5.0, 5.0);
   else
      glOrtho(-2.0*(GLfloat)w/(GLfloat)h, 
               2.0*(GLfloat)w/(GLfloat)h, -2.0, 2.0, -5.0, 5.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
   if (key == 27) exit(0);
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
   glutInitWindowSize (500, 500);
   glutInitWindowPosition (100, 100);
   glutCreateWindow ("Bezier");
   init ();
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutKeyboardFunc (keyboard);
   glutMainLoop();
   return 0;
}
