#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <list>
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <time.h>
#include <functional>

#include "of.h"
#include "ofOffPointsReader.h"
#include "Handler.hpp" 
#include "GL_Interactor.h"
#include "ColorRGBA.hpp"
#include "Cores.h"
#include "Point.hpp"
#include "printof.hpp"


#include "CommandComponent.hpp"
#include "MyCommands.hpp"

#include "ofVertexStarIteratorSurfaceVertex.h"


clock_t start_insert;
clock_t end_insert;
clock_t start_print;
clock_t end_print;



using namespace std;
using namespace of;

//Define o tamanho da tela.
scrInteractor *Interactor = new scrInteractor(900, 650);

//Define a malha a ser usada.

typedef of::MyofDefault2D TTraits;
typedef of::ofMesh<TTraits> TMesh;
TMesh *malha;
Handler<TMesh> meshHandler;

typedef PrintOf<TTraits> TPrintOf;

TPrintOf *Print;

typedef MyCommands<TPrintOf> TMyCommands;
typedef CommandComponent TAllCommands;

ofVtkWriter<TTraits> writer;
TAllCommands *allCommands;


//##################################################################//

////////////////////////////////////////////////////////////////////////
int type = 3;
//CASO 1 EXECUTA CRUST
//CASO 2 EXECUTA BETA-SKELETON
//CASO 3 EXECUTA ARVORE
////////////////////////////////////////////////////////////////////////

//------- ID DAS CELLS
int id_init = 170; //começa arbitrariamnte na celula 170 (São Paulo)
//------- COORD DO PONTO FINAL
double xp = 0.0;
double yp = 0.0;

//-------- ESQUELETO FUNÇÕES
void getInicio();
void solucaoEP(double xp, double yp, int id);
bool baricentrica(int &menor, double xp, double yp, int id, double &bar1, double &bar2, double &bar3);
//-----------------

//------- BUSCA PARA ENCONTRAR O TRIANGULO DE INÍCIO
void getInicio()
{
   double xp = Interactor->getPXD(); //coordenada x do clique direito
   double yp = Interactor->getPYD(); //coordenada y do clique direito
   double bar1, bar2, bar3; //coordenadas baricentricas
   int i, menor;
     
   for (i = 0; i < malha->getNumberOfCells(); i++)
   {
      bar1 = bar2 = bar3 = -1;
      baricentrica(menor, xp, yp, i, bar1, bar2, bar3);
         
      //ATUALIZA E ENCERRA SE ENCONTRAR TRIANGULO NO PONTO CLICADO
      if (bar1 > 0 && bar2 > 0 && bar3 > 0)
      {
         id_init = i;
         break;
      }
      //CLIQUE DIREITO FORA DO MAPA RESETA 
      else
      {
          id_init = 170;
      }
   }
}
//-----------------------

//----------(EXPLICAÇÃO NO README.md)
bool baricentrica(int &menor, double xp, double yp, int id, double& bar1, double& bar2, double& bar3)
{
   //CÁLCULO DAS COORDS BARICÊNTRICAS
   double xa, ya, xb, yb, xc, yc; //coordenadas dos pontos do triangulo ABC
         
   //OBTER AS COORDENADAS DOS PONTOS QUE FORMAM UM TRIANGULO----//
   xa = malha->getVertex(malha->getCell(id)->getVertexId(0))->getCoord(0);
   ya = malha->getVertex(malha->getCell(id)->getVertexId(0))->getCoord(1);
   xb = malha->getVertex(malha->getCell(id)->getVertexId(1))->getCoord(0);
   yb = malha->getVertex(malha->getCell(id)->getVertexId(1))->getCoord(1);
   xc = malha->getVertex(malha->getCell(id)->getVertexId(2))->getCoord(0);
   yc = malha->getVertex(malha->getCell(id)->getVertexId(2))->getCoord(1);

   //CALCULAR AS AREAS DOS TRIANGULOS INTERNOS
   double areaABC = 0.5*((xa*yb)-(ya*xb)+(ya*xc)-(xa*yc)+(xb*yc)-(yb*xc));
   double areaPBC = 0.5*((xp*yb)-(yp*xb)+(yp*xc)-(xp*yc)+(xb*yc)-(yb*xc));
   double areaAPC = 0.5*((xa*yp)-(ya*xp)+(ya*xc)-(xa*yc)+(xp*yc)-(yp*xc));
   double areaABP = 0.5*((xa*yb)-(ya*xb)+(ya*xp)-(xa*yp)+(xb*yp)-(yb*xp));

   //DETERMINAR AS COORDENDAS BARICENTRICAS
   bar1 = areaPBC/areaABC;
   bar2 = areaAPC/areaABC;
   bar3 = areaABP/areaABC;

   //VERIFICA SE ESTÁ NO TRIÂNGULO AS COORDS
   if (bar1 > 0 && bar2 > 0 && bar3 > 0) return true;
   else //menor: BC = 0, AC = 1, AB =2
   {
      if (bar1 < bar2 && bar1 < bar3) menor = 0;
      else if (bar2 < bar1 && bar2 < bar3) menor = 1;
      else menor = 2;
      return false;
   }
}
//-------------

//---------- JUNTA AS FUNÇÕES PARA CONCLUIR
void solucaoEP(double xp, double yp, int id)
{
   double bar1, bar2, bar3; //coordenadas baricentricas
   double xa, ya, xb, yb, xc, yc; //coordenadas dos pontos do triangulo ABC
   int prox, menor;
   int vertice[2];
   bool resp;
   
   bar1 = bar2 = bar3 = -1; //inicializar valores das coordenadas baricentricas arbitrariamente
   
   while (bar1 <= 0 || bar2 <= 0 || bar3 <= 0)
   {
      //IMPRIMIR O TRIANGULO VISITADO
      Print->Face(malha->getCell(id), dgreen);
      //CALCULO DAS COORDENADAS BARICENTRICAS
      resp = baricentrica(menor, xp, yp, id, bar1, bar2, bar3);
      if (resp) break;
      else
      {
         prox = malha->getCell(id)->getMateId(menor);
         //VERIFICAR SE CHEGOU NA FRONTEIRA
         if (prox == -1)
         {
            //IMPRIME A VERTEX POR ONDE SAIU - 3 CASOS
            if (menor == 0) //BC
            { 
               vertice[0] = 1;
               vertice[1] = 2;
            }
            else if (menor == 1) //CA
            {
               vertice[0] = 2;
               vertice[1] = 0;
            }
            else //AB
            {
               vertice[0] = 0;
               vertice[1] = 1;
            }
             
            Print->Edge(malha->getVertex(malha->getCell(id)->getVertexId(vertice[0])), malha->getVertex(malha->getCell(id)->getVertexId(vertice[1])), black, 3.0);
            break;
         }
      }
      id = prox;
   }
}
//---------------

void RenderScene(void){
	allCommands->Execute();
	Print->Vertices(malha,blue,3);
	
	//PINTA A CELL DE INÍCIO DO PERCURSO
   Print->Face(malha->getCell(id_init), yellow);
    
   //CHAMA A FUNÇÃO DE VARREDURA
   if (Interactor->getMouseRight() == true)
   {
      getInicio();
      xp = Interactor->getPXD();
      yp = Interactor->getPYD();
      solucaoEP(Interactor->getPX(), Interactor->getPY(), id_init);
      Print->FacesWireframe(malha,grey,3);
         glFinish();
         glutSwapBuffers();
   }
   //IMPRIME O CAMINHO
   if (xp != Interactor->getPX() || yp != Interactor->getPY())
   {
      solucaoEP(Interactor->getPX(), Interactor->getPY(), id_init);
      Print->FacesWireframe(malha,grey,3);
         glFinish();
         glutSwapBuffers();
   }

	Print->FacesWireframe(malha,grey,3);
	glFinish();
	glutSwapBuffers();
}

void HandleKeyboard(unsigned char key, int x, int y){	
	double coords[3];
	char *xs[10];
	allCommands->Keyboard(key);
	
	switch (key) 
    {
		case 'e':
			exit(1);
		break;
		case 'v':
			coords[0]=x;
			coords[1]=-y;
			coords[2]=0.0;
			malha->addVertex(coords);
		break;
	}
	Interactor->Refresh_List();
	glutPostRedisplay();
}

using namespace std;

int main(int *argc, char **argv)
{

   ofRuppert2D<MyofDefault2D> ruppert;
   ofPoints2DReader<MyofDefault2D> reader;
   ofVtkWriter<MyofDefault2D> writer;
   Interactor->setDraw(RenderScene);
	meshHandler.Set(new TMesh());
      char *fileBrasil = "../Brasil.off";

     
    reader.readOffFile(fileBrasil);
    
    ruppert.execute2D(reader.getLv(),reader.getLids(),true);
    //writer.write(ruppert.getMesh(),"out.vtk",reader.getNorma(),ruppert.getNumberOfInsertedVertices());
  
   meshHandler = ruppert.getMesh();
   malha = ruppert.getMesh();
  
  
   Print = new TPrintOf(meshHandler);

	allCommands = new TMyCommands(Print, Interactor);

	double a,x1,x2,y1,y2,z1,z2; 

	of::ofVerticesIterator<TTraits> iv(&meshHandler);

	iv.initialize();
	x1 = x2 = iv->getCoord(0);
	y1 = y2 = iv->getCoord(1);
	z1 = z2 = iv->getCoord(2);

	for(iv.initialize(); iv.notFinish(); ++iv){
		iv->setCoord(1, -iv->getCoord(1));
		if(iv->getCoord(0) < x1) x1 = a = iv->getCoord(0);
		if(iv->getCoord(0) > x2) x2 = a = iv->getCoord(0);
		if(iv->getCoord(1) < y1) y1 = a = iv->getCoord(1);
		if(iv->getCoord(1) > y2) y2 = a = iv->getCoord(1);
		if(iv->getCoord(2) < z1) z1 = a = iv->getCoord(2);
		if(iv->getCoord(2) > z2) z2 = a = iv->getCoord(2);
	}

	double maxdim;
	maxdim = fabs(x2 - x1);
	if(maxdim < fabs(y2 - y1)) maxdim = fabs(y2 - y1);
	if(maxdim < fabs(z2 - z1)) maxdim = fabs(z2 - z1);

	maxdim *= 0.4;
	
	Point center((x1+x2)/2.0, (y1+y2)/2.0, (y1+y2)/2.0 );
	Interactor->Init(center[0]-maxdim, center[0]+maxdim,
					center[1]-maxdim, center[1]+maxdim,
					center[2]-maxdim, center[2]+maxdim,argc,argv);

	
	
	AddKeyboard(HandleKeyboard);

	allCommands->Help(std::cout);
	std::cout<< std::endl<< "Press \"?\" key for help"<<std::endl<<std::endl;
	double t;
	
	Init_Interactor();

  
  return EXIT_SUCCESS;
}