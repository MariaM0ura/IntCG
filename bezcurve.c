/*
 * Copyright (c) 1993-1997, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED 
 * Permission to use, copy, modify, and distribute this software for 
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that 
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission. 
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * US Government Users Restricted Rights 
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(R) is a registered trademark of Silicon Graphics, Inc.
 */

/*  bezcurve.c			
 *  This program uses evaluators to draw a Bezier curve.
 */

#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

static int nSamples = 80;    // número de amostras por spline
static int showCtrl = 1;     // mostrar pontos de controle

// spline 0 (branca) 
GLfloat ctrl[3][4][3] = {
    { { -1.3500184f, -2.1803460f, 0.0f },
      {  3.5259477f,  2.2716231f, 0.0f },
      { -4.0000000f,  2.3010674f, 0.0f },
      {  1.8240707f, -2.3923445f, 0.0f } },

    // ctrl[1] (azul) e ctrl[2] (roxa) serão calculados
    { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} },
    { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} }
};

// fatores de escala: azul deve ser maior que os outros dois
const float blueScaleDefault = 1.40f;   // maior que 1.0 (azul será maior)
const float purpleScaleDefault = 1.00f; // roxo mesmo tamanho que branco

// calcula splines encadeadas e centraliza o conjunto
void compute_chained_and_center(float blueScale, float purpleScale)
{
    // Wk = ctrl[0][k]; W0 = ctrl[0][0]; W3 = ctrl[0][3]
    GLfloat W0[3], W3[3];
    for (int i=0;i<3;i++) { W0[i] = ctrl[0][0][i]; W3[i] = ctrl[0][3][i]; }

    // calcula azul: B_k = W3 + s*(Wk - W0)
    for (int k=0; k<4; ++k) {
        for (int i=0;i<3;i++) {
            ctrl[1][k][i] = W3[i] + blueScale * (ctrl[0][k][i] - W0[i]);
        }
    }

    // guarda B3
    GLfloat B3[3];
    for (int i=0;i<3;i++) B3[i] = ctrl[1][3][i];

    // roxa: R_k = B3 + s2*(Wk - W0)
    for (int k=0; k<4; ++k) {
        for (int i=0;i<3;i++) {
            ctrl[2][k][i] = B3[i] + purpleScale * (ctrl[0][k][i] - W0[i]);
        }
    }

    // garantir continuidade exata:
    for (int i=0;i<3;i++) {
        ctrl[1][0][i] = ctrl[0][3][i];
        ctrl[2][0][i] = ctrl[1][3][i];
    }

    // --- agora centralia o conjunto de todos os pontos no (0,0) ---
    GLfloat cx = 0.0f, cy = 0.0f, cz = 0.0f;
    for (int s=0; s<3; ++s) for (int k=0; k<4; ++k) {
        cx += ctrl[s][k][0];
        cy += ctrl[s][k][1];
        cz += ctrl[s][k][2];
    }
    cx /= (3.0f * 4.0f);
    cy /= (3.0f * 4.0f);
    cz /= (3.0f * 4.0f);

    // subtrai centroid de todos os pontos => centraliza
    for (int s=0; s<3; ++s) for (int k=0; k<4; ++k) {
        ctrl[s][k][0] -= cx;
        ctrl[s][k][1] -= cy;
        ctrl[s][k][2] -= cz;
    }
}

void init(void)
{
    // aplica escalas e centraliza
    compute_chained_and_center(blueScaleDefault, purpleScaleDefault);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void drawBezier(GLfloat control[4][3], int samples)
{
    glMap1f(GL_MAP1_VERTEX_3, 0.0f, 1.0f, 3, 4, &control[0][0]);
    glEnable(GL_MAP1_VERTEX_3);

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= samples; ++i) {
        GLfloat t = (GLfloat)i / (GLfloat)samples;
        glEvalCoord1f(t);
    }
    glEnd();
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);

    // spline 0 - branca
    glLineWidth(4.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawBezier(ctrl[0], nSamples);

    // spline 1 - azul (maior)
    glLineWidth(6.0f);
    glColor3f(0.0f, 1.0f, 1.0f); // ciano
    drawBezier(ctrl[1], nSamples);

    // spline 2 - roxa (mesmo tamanho do branco)
    glLineWidth(4.0f);
    glColor3f(1.0f, 0.0f, 1.0f);
    drawBezier(ctrl[2], nSamples);

    // pontos de controle (opcional)
    if (showCtrl) {
        glPointSize(6.0f);
        for (int s = 0; s < 3; ++s) {
            glColor3f(1.0f, 1.0f, 0.0f);
            glBegin(GL_POINTS);
            for (int i = 0; i < 4; ++i) {
                glVertex3fv(&ctrl[s][i][0]);
            }
            glEnd();

            glLineWidth(1.0f);
            glColor3f(0.4f, 0.4f, 0.4f);
            glBegin(GL_LINE_STRIP);
            for (int i = 0; i < 4; ++i) glVertex3fv(&ctrl[s][i][0]);
            glEnd();
        }
    }

    glFlush();
}

void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
        glOrtho(-5.0, 5.0, -5.0*(GLfloat)h/(GLfloat)w, 5.0*(GLfloat)h/(GLfloat)w, -5.0, 5.0);
    else
        glOrtho(-5.0*(GLfloat)w/(GLfloat)h, 5.0*(GLfloat)w/(GLfloat)h, -5.0, 5.0, -5.0, 5.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 27: // ESC
            exit(0);
            break;
        case '+':
        case '=':
            nSamples += 10;
            if (nSamples > 2000) nSamples = 2000;
            glutPostRedisplay();
            break;
        case '-':
        case '_':
            nSamples -= 10;
            if (nSamples < 4) nSamples = 4;
            glutPostRedisplay();
            break;
        case 'c':
        case 'C':
            showCtrl = !showCtrl;
            glutPostRedisplay();
            break;
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize (900, 700);
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("Bezier - encadeadas e centralizadas");
    init ();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc (keyboard);
    glutMainLoop();
    return 0;
}
