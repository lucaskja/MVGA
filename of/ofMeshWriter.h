#ifndef OFMESHWRITER_H





#define OFMESHWRITER_H











#include "ofMacros.h"





#include "ofWriter.h"











namespace of





{











//! Salva malhas em arquivo .poly





template <class _Traits>  class ofMeshWriter : public ofWriter<_Traits>





{





public:





	typedef typename _Traits::space space;





	typedef typename _Traits::ids ids;





	typedef typename _Traits::sVertex sVertex;





	typedef typename _Traits::sCell sCell;





	typedef typename _Traits::sMesh sMesh;





	typedef typename _Traits::sGeometric sGeometric;


	typedef pair <space,space>              v2d;





	





	ofMeshWriter();





	~ofMeshWriter();





	





	bool write(sMesh* malha, char* filename,space norma=1.0,bool eok=false);


	bool writeQuad(sMesh* malha, char* filename,space norma=1.0);





	void setIniV(int n){iniV=n;};











private:








	


	bool writer(ofstream &pf, sMesh* malha,space norma=1.0);


	bool writerQuad(ofstream &pf, sMesh *malha, space norma=1.0);


	sGeometric geom;


	int iniV;


};











template <class _Traits>  ofMeshWriter<_Traits>::ofMeshWriter()





{











}











template <class _Traits>  ofMeshWriter<_Traits>::~ofMeshWriter()





{





}











template <class _Traits> bool ofMeshWriter<_Traits>::write(sMesh* malha, char *filename, space norma,bool eok)





{





	OF_ASSERT(filename);





	OF_ASSERT(malha);	





	





	ofstream pf(filename);





	geom.setMesh(malha);





	if( pf.is_open())





	{





			if(! writer(pf, malha,norma))





				return false;





		





			pf.close();





	}





	else





		return false;





	





	return true;





}








template <class _Traits> bool ofMeshWriter<_Traits>::writeQuad(sMesh* malha, char *filename, space norma)





{





	OF_ASSERT(filename);





	OF_ASSERT(malha);	





	





	ofstream pf(filename);





	geom.setMesh(malha);





	if( pf.is_open())





	{





			if(! writerQuad(pf, malha,norma))





				return false;





		





			pf.close();





	}





	else





		return false;





	





	return true;





}








template <class _Traits> bool ofMeshWriter<_Traits>::writer(ofstream &pf, sMesh* malha,space norma)





{





	int i = 1,j;





	int nmat,nbc,npoint,nnodes;


	nmat=4;nbc=3;npoint=0;





	nnodes=malha->getNumberOfVertices()-4;





	


	pf << nmat <<" "<< nbc<<" " << npoint<<" " << nnodes << endl;


	


	


	pf << "0 1.0 0.0 1.0" << endl;





	pf << "1 1.0 0.0 1.0" << endl;





	pf << "2 1.0 1.0 0.0" << endl;





	pf << "3 1.0 0.0 1.0" << endl;








	pf << "4 0 0.0 0.0" << endl;





	pf << "5 1 0.0 1.0" << endl;





	pf << "6 1 0.0 -1.0" << endl;





	//pf << "5 " << iniV-3 << endl;





	//pf << "5 " << iniV+3 << endl;


	





	





	





	ofVerticesIterator<_Traits> itv(malha);





	





	for(itv.initialize(),++itv,++itv,++itv,++itv; itv.notFinish(); ++itv)





	{








	





			pf << " " << setprecision(15) << itv->getCoord(0)/**norma*/ << " " << setprecision(15) << itv->getCoord(1)/**norma*/ << endl;


			i++;





		





	}


	


	


	


	





	





	





	ofCellsIterator<_Traits> itc(malha);





	





	for(itc.initialize(); itc.notFinish(); ++itc)





	{


		


		pf << sCell::getDimension() << " 0 1 3 "; 





	





		for(j = 0; j <= sCell::getDimension(); j++)





			pf << " " << itc->getVertex(j)-4;





		pf << endl; 





	}


	ids v1,v2;





	sVertex *vi1,*vi2;





	for(itc.initialize(); itc.notFinish(); ++itc)





	{


		


	





	





		for(j = 0; j <= sCell::getDimension(); j++)


		{





			v1 =itc->getVertex(j);


			v2 = itc->getVertex((j+1)%3);


			vi1= malha->getVertex(v1);


			vi2= malha->getVertex(v2);


			if(geom.isBEdge(&itc,v1,v2))


			{


			


				if((vi1->getCoord(0)==0.0)&&(vi2->getCoord(0)==0.0))


					pf <<  "1 4 1 2 "; 	


				else


					if((vi1->getCoord(1)==0.0)&&(vi2->getCoord(1)==0.0))


						pf <<  "1 4 1 2 "; 	


					else


						if(((vi1->getCoord(0)>0.5)&&(vi2->getCoord(0)>0.5)))


							pf<< "1 5 1 2 ";


						else 


							if (((vi1->getCoord(1)>0.5)&&(vi2->getCoord(1)>0.5)))


								pf<< "1 6 1 2 ";





				pf  << v1-4 << " " << v2-4;





				pf << endl; 


			}


		}





	}





	





	





	


	





	return true;





}








template <class _Traits> bool ofMeshWriter<_Traits>::writerQuad(ofstream &pf, sMesh* malha, space norma)





{





	int i = 1,j;





	ofList<v2d> lv;





	int nmat,nbc,npoint,nnodes;


	nmat=4;nbc=2;npoint=0;





	nnodes=malha->getNumberOfVertices()-4 + malha->getNumberOfCells()*3;





	space x,y;


	sVertex *v1,*v2;


	




















ofVerticesIterator<_Traits> itv(malha);





	





	for(itv.initialize(),++itv,++itv,++itv,++itv; itv.notFinish(); ++itv)





	{








			v2d v;


			v.first=itv->getCoord(0);


			v.second=itv->getCoord(1);


			lv.insert(v);





			





		





	}





	ofCellsIterator<_Traits> itc(malha);





for(itc.initialize(); itc.notFinish(); ++itc)





	{


		


			





		for(j = 0; j <= sCell::getDimension(); j++)


		{


			v1= malha->getVertex(itc->getVertex(j));


			v2= malha->getVertex(itc->getVertex((j+1)%3));


			x=(v1->getCoord(0)+v2->getCoord(0))*0.5;


			y=(v1->getCoord(1)+v2->getCoord(1))*0.5;


			v2d v1;


			v1.first=x;


			v1.second=y;


			lv.insert(v1);


			//iniV++;


		}


			





		





	}

















	


	pf << nmat <<" "<< nbc<<" " << npoint<<" " << nnodes << endl;


	


	


	pf << "0 1.0 0.0 1.0" << endl;





	pf << "1 1.0 0.0 1.0" << endl;





	pf << "2 1.0 1.0 0.0" << endl;





	pf << "3 1.0 0.0 1.0" << endl;








	pf << "4 0 0.0 0.0" << endl;





	pf << "5 1 1.0 0.0" << endl;





	//pf << "5 " << iniV-3 << endl;





	//pf << "5 " << iniV+3 << endl;





	


	





	v2d vi;





	for(i=0;i<lv.size();i++)


	{


		vi=lv.pos(i);


		pf << " " << setprecision(15) << vi.first/**norma*/ << " " << setprecision(15) <<vi.second/**norma*/ << endl;





	}





	





	


   nnodes=malha->getNumberOfVertices()-4;


	


	i=0;


	for(itc.initialize(); itc.notFinish(); ++itc)





	{


		


		pf << sCell::getDimension() << " 0 2 6 "; 





	





		for(j = 0; j <= sCell::getDimension(); j++)





			pf << " " << itc->getVertex(j)-4 << " " << nnodes+i*3+j;





		pf << endl; 


	i++;


	}








	ids vi1,vi2;


	i=0;


for(itc.initialize(); itc.notFinish(); ++itc)





	{


		


		





	





		for(j = 0; j <= sCell::getDimension(); j++)


		{





			vi1 =itc->getVertex(j);


			vi2 = itc->getVertex((j+1)%3);


			


			v1= malha->getVertex(vi1);


			v2= malha->getVertex(vi2);


			if(geom.isBEdge(&itc,vi1,vi2))


			{


			


				if((v1->getCoord(0)==0.0)&&(v2->getCoord(0)==0.0))


					pf <<  "1 5 2 3 "; 	


				else


					pf <<  "1 4 2 3 "; 	





				pf  << vi1-4 << " "<<nnodes+i*3+j<< " " << vi2-4;





				pf << endl; 


			}


		}





		i++;


	}


	





	


	





	return true;





}











}











#endif




