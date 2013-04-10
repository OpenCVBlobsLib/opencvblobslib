/************************************************************************
  			BlobResult.cpp
  			
FUNCIONALITAT: Implementació de la classe CBlobResult
AUTOR: Inspecta S.L.
MODIFICACIONS (Modificació, Autor, Data):
 
**************************************************************************/

#include <limits.h>
#include <stdio.h>
#include <functional>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>
#include "BlobResult.h"
#include "Segment.h"
#include <pthread.h>
//! Show errors functions: only works for windows releases
#ifdef _SHOW_ERRORS
	#include <afx.h>			//suport per a CStrings
	#include <afxwin.h>			//suport per a AfxMessageBox
#endif

using namespace std;

/**************************************************************************
		Constructors / Destructors
**************************************************************************/


/**
- FUNCIÓ: CBlobResult
- FUNCIONALITAT: Constructor estandard.
- PARÀMETRES:
- RESULTAT:
- Crea un CBlobResult sense cap blob
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 20-07-2004.
- MODIFICACIÓ: Data. Autor. Descripció.
*/
/**
- FUNCTION: CBlobResult
- FUNCTIONALITY: Standard constructor
- PARAMETERS:
- RESULT:
	- creates an empty set of blobs
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
CBlobResult::CBlobResult()
{
	m_blobs = Blob_vector();
}

/**
- FUNCIÓ: CBlobResult
- FUNCIONALITAT: Constructor a partir d'una imatge. Inicialitza la seqüència de blobs 
			   amb els blobs resultants de l'anàlisi de blobs de la imatge.
- PARÀMETRES:
	- source: imatge d'on s'extreuran els blobs
	- mask: màscara a aplicar. Només es calcularan els blobs on la màscara sigui 
			diferent de 0. Els blobs que toquin a un pixel 0 de la màscara seran 
			considerats exteriors.
	- threshold: llindar que s'aplicarà a la imatge source abans de calcular els blobs
	- findmoments: indica si s'han de calcular els moments de cada blob
	- blackBlobs: true per buscar blobs negres a la binaritzazió (it will join all extern white blobs).
				  false per buscar blobs negres a la binaritzazió (it will join all extern black blobs).

- RESULTAT:
	- objecte CBlobResult amb els blobs de la imatge source
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 25-05-2005.
- MODIFICACIÓ: Data. Autor. Descripció.
*/
/**
- FUNCTION: CBlob
- FUNCTIONALITY: Constructor from an image. Fills an object with all the blobs in
	the image
- PARAMETERS:
	- source: image to extract the blobs from
	- mask: optional mask to apply. The blobs will be extracted where the mask is
			not 0. All the neighbouring blobs where the mask is 0 will be extern blobs
	- threshold: threshold level to apply to the image before computing blobs
	- findmoments: true to calculate the blob moments (slower) (needed to calculate elipses!)
 	- blackBlobs: true to search for black blobs in the binarization (it will join all extern white blobs).
				  false to search for white blobs in the binarization (it will join all extern black blobs).
- RESULT:
	- object with all the blobs in the image. It throws an EXCEPCIO_CALCUL_BLOBS
	  if some error appears in the BlobAnalysis function
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
CBlobResult::CBlobResult(IplImage *source, IplImage *mask, uchar backgroundColor ,Mat &labelled)
{
	bool success;
	try
	{
		success = ComponentLabeling( source, mask, backgroundColor, m_blobs,labelled);
	}
	catch(...)
	{
		success = false;
	}

	if( !success ) throw EXCEPCIO_CALCUL_BLOBS;
}
/**
- FUNCTION: CBlobResult
- FUNCTIONALITY: Constructor from an image. Fills an object with all the blobs in
	the image, OPENCV 2 interface
- PARAMETERS:
	- source: Mat to extract the blobs from, CV_8UC1
	- mask: optional mask to apply. The blobs will be extracted where the mask is
			not 0. All the neighbouring blobs where the mask is 0 will be extern blobs
- RESULT:
	- object with all the blobs in the image. It throws an EXCEPCIO_CALCUL_BLOBS
	  if some error appears in the BlobAnalysis function
- RESTRICTIONS:
- AUTHOR: Saverio Murgia & Luca Nardelli
- CREATION DATE: 06-04-2013.
- MODIFICATION: Date. Author. Description.
*/
CBlobResult::CBlobResult(Mat &source, Mat &mask, uchar backgroundColor){
	//CBlobResult(&(IplImage)source,&(IplImage)mask,backgroundColor);
	int numCores = pthread_num_processors_np();
	pthread_t *tIds = new pthread_t[numCores];
	Size sz = source.size();
	int roiHeight = sz.height/numCores;
	//Mat_<int> labels = Mat_<int>::zeros(2,source.size().width);
	ThreadMessage *mess = new ThreadMessage[numCores];
	for(int i=0;i<numCores;i++){
		mess[i].operator =(ThreadMessage(source,mask,0,i*roiHeight,roiHeight));
		pthread_create(&tIds[i],NULL,(void *(*)(void *))thread_componentLabeling,(void*)&mess[i]);
	}
	CBlobResult r;
	for(int i=0;i<numCores;i++){
		pthread_join(tIds[i],0);
		//r = r+*mess[i].res;
	}
	CBlobResult temp_result;
	//for(int i=0;i<numCores-1;i++){
	//	bool found = false;
	//	unsigned int last_found_label=0;
	//	for(int c=0;c<sz.width;c++){
	//		unsigned int prev_label = mess[i].labels.at<unsigned int>(sz.height/numCores-1,c);
	//		unsigned int following_label = mess[i+1].labels.at<unsigned int>(0,c);
	//		if(prev_label!=0 & following_label!=0 & (!found | prev_label!=last_found_label)){
	//			found=true;
	//			last_found_label=prev_label;
	//			CBlob *nextBlob = mess[i+1].res->GetBlobByID(following_label);
	//			CBlob *prevBlob=mess[i].res->GetBlobByID(prev_label);
	//			prevBlob->to_be_deleted=1;
	//			mess[i+1].res->AddBlob(prevBlob);
	//			nextBlob->JoinBlob(prevBlob);
	//		}
	//		else if(prev_label==0 | following_label==0) found=false;
	//	}
	//}
	std::map<unsigned int,BlobOverlap> overlaps;
	for(int i=numCores-1;i>0;i--){
		//cout << "RIGA: "<< i*sz.height/numCores -1 << endl;
		overlaps.clear();
		unsigned int last_found_label=0;
		unsigned int prevLabelTop = 0;
		unsigned int prevLabelBottom = 0;
		Point segStart(sz.width,i*sz.height/numCores-1),segEnd(0,i*sz.height/numCores-1);
		for(int c=0;c<sz.width;c++){
			//L'ultima riga di labelTop e la prima di labelBottom sono sovrapposte (coincidenti)
			unsigned int labelBottom = mess[i].labels.at<unsigned int>(0,c);
			unsigned int labelTop = mess[i-1].labels.at<unsigned int>(mess[i-1].labels.size().height-1,c);
			if(labelTop!= prevLabelTop && labelTop!=0){
				if(!overlaps[labelTop].sourceBlob){
					overlaps[labelTop].sourceBlob = mess[i-1].res->GetBlobByID(labelTop);
				}
			}
			if(labelBottom!=prevLabelBottom && labelBottom!=0){
				overlaps[labelTop].matchingSegments[labelBottom].push_back(Segment(Point(segStart),Point(segEnd)));
				overlaps[labelTop].blobsToJoin[labelBottom] = mess[i].res->GetBlobByID(labelBottom); //Da migliorare
			}
			if(labelBottom!=0 && labelTop!=0){
				overlaps[labelTop].matchingSegments[labelBottom][overlaps[labelTop].matchingSegments[labelBottom].size()-1].begin.x = MIN(overlaps[labelTop].matchingSegments[labelBottom][overlaps[labelTop].matchingSegments[labelBottom].size()-1].begin.x,c);
				overlaps[labelTop].matchingSegments[labelBottom][overlaps[labelTop].matchingSegments[labelBottom].size()-1].end.x = MAX(overlaps[labelTop].matchingSegments[labelBottom][overlaps[labelTop].matchingSegments[labelBottom].size()-1].end.x,c);
			}
			prevLabelTop=labelTop;
			prevLabelBottom=labelBottom;
		}
		for(map<unsigned int,BlobOverlap>::iterator it=overlaps.begin();it!=overlaps.end();it++){
				//it->second.Print();
				map<unsigned int,CBlob*>::iterator itB = it->second.blobsToJoin.begin();
				map<unsigned int,deque<Segment>>::iterator itC = it->second.matchingSegments.begin();
				for(itB,itC; itB!=it->second.blobsToJoin.end();itB++,itC++){
					it->second.sourceBlob->JoinBlobTangent(itB->second,itC->second);
					//Se ho già segnato da cancellare quel blob allora devo eliminare anche il blob che vi si è joinato prima
					//Da migliorare anche sta parte mi sa...
					if(itB->second->to_be_deleted==1){
						map<unsigned int,BlobOverlap>::iterator iter = overlaps.begin();
						for(iter;iter!=overlaps.end();iter++){
							if(iter->first == it->first)
								break;
							map<unsigned int,CBlob*>::iterator iterB = iter->second.blobsToJoin.begin();
							for(iterB;iterB!=iter->second.blobsToJoin.end();iterB++){
								if(iterB->first == itB->first)
									iter->second.sourceBlob->to_be_deleted=1;
							}
						}
					}
					else
						itB->second->to_be_deleted=1;
				}
		}
		cout << endl;
	}
	for(int i=0;i<numCores;i++){
		mess[i].res->Filter(*mess[i].res,B_EXCLUDE,CBlobGetTBDeleted(),B_EQUAL,1);
		r = r+*mess[i].res;
	}
	delete [] mess;
	delete [] tIds;
	*this = r;
}
/**
- FUNCIÓ: CBlobResult
- FUNCIONALITAT: Constructor de còpia. Inicialitza la seqüència de blobs 
			   amb els blobs del paràmetre.
- PARÀMETRES:
	- source: objecte que es copiarà
- RESULTAT:
	- objecte CBlobResult amb els blobs de l'objecte source
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 25-05-2005.
- MODIFICACIÓ: Data. Autor. Descripció.
*/
/**
- FUNCTION: CBlobResult
- FUNCTIONALITY: Copy constructor
- PARAMETERS:
	- source: object to copy
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
CBlobResult::CBlobResult( const CBlobResult &source )
{
	m_blobs = Blob_vector( source.GetNumBlobs() );
	
	// creem el nou a partir del passat com a paràmetre
	m_blobs = Blob_vector( source.GetNumBlobs() );
	// copiem els blobs de l'origen a l'actual
	Blob_vector::const_iterator pBlobsSrc = source.m_blobs.begin();
	Blob_vector::iterator pBlobsDst = m_blobs.begin();

	while( pBlobsSrc != source.m_blobs.end() )
	{
		// no podem cridar a l'operador = ja que Blob_vector és un 
		// vector de CBlob*. Per tant, creem un blob nou a partir del
		// blob original
		*pBlobsDst = new CBlob(**pBlobsSrc);
		pBlobsSrc++;
		pBlobsDst++;
	}
}



/**
- FUNCIÓ: ~CBlobResult
- FUNCIONALITAT: Destructor estandard.
- PARÀMETRES:
- RESULTAT:
	- Allibera la memòria reservada de cadascun dels blobs de la classe
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 25-05-2005.
- MODIFICACIÓ: Data. Autor. Descripció.
*/
/**
- FUNCTION: ~CBlobResult
- FUNCTIONALITY: Destructor
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
CBlobResult::~CBlobResult()
{
	ClearBlobs();
}

/**************************************************************************
		Operadors / Operators
**************************************************************************/


/**
- FUNCIÓ: operador =
- FUNCIONALITAT: Assigna un objecte source a l'actual
- PARÀMETRES:
	- source: objecte a assignar
- RESULTAT:
	- Substitueix els blobs actuals per els de l'objecte source
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 25-05-2005.
- MODIFICACIÓ: Data. Autor. Descripció.
*/
/**
- FUNCTION: Assigment operator
- FUNCTIONALITY: 
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
CBlobResult& CBlobResult::operator=(const CBlobResult& source)
{
	// si ja són el mateix, no cal fer res
	if (this != &source)
	{
		// alliberem el conjunt de blobs antic
		for( int i = 0; i < GetNumBlobs(); i++ )
		{
			delete m_blobs[i];
		}
		m_blobs.clear();
		// creem el nou a partir del passat com a paràmetre
		m_blobs = Blob_vector( source.GetNumBlobs() );
		// copiem els blobs de l'origen a l'actual
		Blob_vector::const_iterator pBlobsSrc = source.m_blobs.begin();
		Blob_vector::iterator pBlobsDst = m_blobs.begin();

		while( pBlobsSrc != source.m_blobs.end() )
		{
			// no podem cridar a l'operador = ja que Blob_vector és un 
			// vector de CBlob*. Per tant, creem un blob nou a partir del
			// blob original
			*pBlobsDst = new CBlob(**pBlobsSrc);
			pBlobsSrc++;
			pBlobsDst++;
		}
	}
	return *this;
}


/**
- FUNCIÓ: operador +
- FUNCIONALITAT: Concatena els blobs de dos CBlobResult
- PARÀMETRES:
	- source: d'on s'agafaran els blobs afegits a l'actual
- RESULTAT:
	- retorna un nou CBlobResult amb els dos CBlobResult concatenats
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 25-05-2005.
- NOTA: per la implementació, els blobs del paràmetre es posen en ordre invers
- MODIFICACIÓ: Data. Autor. Descripció.
*/
/**
- FUNCTION: + operator
- FUNCTIONALITY: Joins the blobs in source with the current ones
- PARAMETERS:
	- source: object to copy the blobs
- RESULT:
	- object with the actual blobs and the source blobs
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
CBlobResult CBlobResult::operator+( const CBlobResult& source ) const
{	
	//creem el resultat a partir dels blobs actuals
	CBlobResult resultat( *this );

	// reservem memòria per als nous blobs
	resultat.m_blobs.resize( resultat.GetNumBlobs() + source.GetNumBlobs() );

	// declarem els iterador per recòrrer els blobs d'origen i desti
	Blob_vector::const_iterator pBlobsSrc = source.m_blobs.begin();
	Blob_vector::iterator pBlobsDst = resultat.m_blobs.end();

	// insertem els blobs de l'origen a l'actual
	while( pBlobsSrc != source.m_blobs.end() )
	{
		pBlobsDst--;
		*pBlobsDst = new CBlob(**pBlobsSrc);
		pBlobsSrc++;
	}
	
	return resultat;
}

/**************************************************************************
		Operacions / Operations
**************************************************************************/

/**
- FUNCIÓ: AddBlob
- FUNCIONALITAT: Afegeix un blob al conjunt
- PARÀMETRES:
	- blob: blob a afegir
- RESULTAT:
	- modifica el conjunt de blobs actual
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 2006/03/01
- MODIFICACIÓ: Data. Autor. Descripció.
*/
void CBlobResult::AddBlob( CBlob *blob )
{
	if( blob != NULL )
		m_blobs.push_back( new CBlob( blob ) );
}


#ifdef MATRIXCV_ACTIU

/**
- FUNCIÓ: GetResult
- FUNCIONALITAT: Calcula el resultat especificat sobre tots els blobs de la classe
- PARÀMETRES:
	- evaluador: Qualsevol objecte derivat de COperadorBlob
- RESULTAT:
	- Retorna un array de double's amb el resultat per cada blob
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 25-05-2005.
- MODIFICACIÓ: Data. Autor. Descripció.
*/
/**
- FUNCTION: GetResult
- FUNCTIONALITY: Computes the function evaluador on all the blobs of the class
				 and returns a vector with the result
- PARAMETERS:
	- evaluador: function to apply to each blob (any object derived from the 
				 COperadorBlob class )
- RESULT:
	- vector with all the results in the same order as the blobs
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double_vector CBlobResult::GetResult( funcio_calculBlob *evaluador ) const
{
	if( GetNumBlobs() <= 0 )
	{
		return double_vector();
	}

	// definim el resultat
	double_vector result = double_vector( GetNumBlobs() );
	// i iteradors sobre els blobs i el resultat
	double_vector::iterator itResult = result.GetIterator();
	Blob_vector::const_iterator itBlobs = m_blobs.begin();

	// avaluem la funció en tots els blobs
	while( itBlobs != m_blobs.end() )
	{
		*itResult = (*evaluador)(**itBlobs);
		itBlobs++;
		itResult++;
	}
	return result;
}
#endif

/**
- FUNCIÓ: GetSTLResult
- FUNCIONALITAT: Calcula el resultat especificat sobre tots els blobs de la classe
- PARÀMETRES:
	- evaluador: Qualsevol objecte derivat de COperadorBlob
- RESULTAT:
	- Retorna un array de double's STL amb el resultat per cada blob
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 25-05-2005.
- MODIFICACIÓ: Data. Autor. Descripció.
*/
/**
- FUNCTION: GetResult
- FUNCTIONALITY: Computes the function evaluador on all the blobs of the class
				 and returns a vector with the result
- PARAMETERS:
	- evaluador: function to apply to each blob (any object derived from the 
				 COperadorBlob class )
- RESULT:
	- vector with all the results in the same order as the blobs
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double_stl_vector CBlobResult::GetSTLResult( funcio_calculBlob *evaluador ) const
{
	if( GetNumBlobs() <= 0 )
	{
		return double_stl_vector();
	}

	// definim el resultat
	double_stl_vector result = double_stl_vector( GetNumBlobs() );
	// i iteradors sobre els blobs i el resultat
	double_stl_vector::iterator itResult = result.begin();
	Blob_vector::const_iterator itBlobs = m_blobs.begin();

	// avaluem la funció en tots els blobs
	while( itBlobs != m_blobs.end() )
	{
		*itResult = (*evaluador)(**itBlobs);
		itBlobs++;
		itResult++;
	}
	return result;
}

/**
- FUNCIÓ: GetNumber
- FUNCIONALITAT: Calcula el resultat especificat sobre un únic blob de la classe
- PARÀMETRES:
	- evaluador: Qualsevol objecte derivat de COperadorBlob
	- indexblob: número de blob del que volem calcular el resultat.
- RESULTAT:
	- Retorna un double amb el resultat
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 25-05-2005.
- MODIFICACIÓ: Data. Autor. Descripció.
*/
/**
- FUNCTION: GetNumber
- FUNCTIONALITY: Computes the function evaluador on a blob of the class
- PARAMETERS:
	- indexBlob: index of the blob to compute the function
	- evaluador: function to apply to each blob (any object derived from the 
				 COperadorBlob class )
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double CBlobResult::GetNumber( int indexBlob, funcio_calculBlob *evaluador ) const
{
	if( indexBlob < 0 || indexBlob >= GetNumBlobs() )
		RaiseError( EXCEPTION_BLOB_OUT_OF_BOUNDS );
	return (*evaluador)( *m_blobs[indexBlob] );
}

/**
- FUNCIÓ: Filter (const version)
- FUNCIONALITAT: Filtra els blobs de la classe i deixa el resultat amb només 
			   els blobs que han passat el filtre.
			   El filtrat es basa en especificar condicions sobre un resultat dels blobs
			   i seleccionar (o excloure) aquells blobs que no compleixen una determinada
			   condicio
- PARÀMETRES:
	- dst: variable per deixar els blobs filtrats
	- filterAction:	acció de filtrat. Incloure els blobs trobats (B_INCLUDE),
				    o excloure els blobs trobats (B_EXCLUDE)
	- evaluador: Funció per evaluar els blobs (qualsevol objecte derivat de COperadorBlob
	- Condition: tipus de condició que ha de superar la mesura (FilterType) 
				 sobre cada blob per a ser considerat.
				    B_EQUAL,B_NOT_EQUAL,B_GREATER,B_LESS,B_GREATER_OR_EQUAL,
				    B_LESS_OR_EQUAL,B_INSIDE,B_OUTSIDE
	- LowLimit:  valor numèric per a la comparació (Condition) de la mesura (FilterType)
	- HighLimit: valor numèric per a la comparació (Condition) de la mesura (FilterType)
				 (només té sentit per a aquelles condicions que tenen dos valors 
				 (B_INSIDE, per exemple).
- RESULTAT:
	- Deixa els blobs resultants del filtrat a destination
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 25-05-2005.
- MODIFICACIÓ: Data. Autor. Descripció.
*/
/**
- FUNCTION: Filter (const version)
- FUNCTIONALITY: Get some blobs from the class based on conditions on measures
				 of the blobs. 
- PARAMETERS:
	- dst: where to store the selected blobs
	- filterAction:	B_INCLUDE: include the blobs which pass the filter in the result 
				    B_EXCLUDE: exclude the blobs which pass the filter in the result 
	- evaluador: Object to evaluate the blob
	- Condition: How to decide if  the result returned by evaluador on each blob
				 is included or not. It can be:
				    B_EQUAL,B_NOT_EQUAL,B_GREATER,B_LESS,B_GREATER_OR_EQUAL,
				    B_LESS_OR_EQUAL,B_INSIDE,B_OUTSIDE
	- LowLimit:  numerical value to evaluate the Condition on evaluador(blob)
	- HighLimit: numerical value to evaluate the Condition on evaluador(blob).
				 Only useful for B_INSIDE and B_OUTSIDE
- RESULT:
	- It returns on dst the blobs that accomplish (B_INCLUDE) or discards (B_EXCLUDE)
	  the Condition on the result returned by evaluador on each blob
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
/////////////////////////// FILTRAT DE BLOBS ////////////////////////////////////
void CBlobResult::Filter(CBlobResult &dst, 
						 int filterAction, 
						 funcio_calculBlob *evaluador, 
						 int condition, 
						 double lowLimit, double highLimit /*=0*/) const
							
{
	// do the job
	DoFilter(dst, filterAction, evaluador, condition, lowLimit, highLimit );
}


/**
- FUNCIÓ: Filter
- FUNCIONALITAT: Filtra els blobs de la classe i deixa el resultat amb només 
			   els blobs que han passat el filtre.
			   El filtrat es basa en especificar condicions sobre un resultat dels blobs
			   i seleccionar (o excloure) aquells blobs que no compleixen una determinada
			   condicio
- PARÀMETRES:
	- dst: variable per deixar els blobs filtrats
	- filterAction:	acció de filtrat. Incloure els blobs trobats (B_INCLUDE),
				    o excloure els blobs trobats (B_EXCLUDE)
	- evaluador: Funció per evaluar els blobs (qualsevol objecte derivat de COperadorBlob
	- Condition: tipus de condició que ha de superar la mesura (FilterType) 
				 sobre cada blob per a ser considerat.
				    B_EQUAL,B_NOT_EQUAL,B_GREATER,B_LESS,B_GREATER_OR_EQUAL,
				    B_LESS_OR_EQUAL,B_INSIDE,B_OUTSIDE
	- LowLimit:  valor numèric per a la comparació (Condition) de la mesura (FilterType)
	- HighLimit: valor numèric per a la comparació (Condition) de la mesura (FilterType)
				 (només té sentit per a aquelles condicions que tenen dos valors 
				 (B_INSIDE, per exemple).
- RESULTAT:
	- Deixa els blobs resultants del filtrat a destination
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 25-05-2005.
- MODIFICACIÓ: Data. Autor. Descripció.
*/
/**
- FUNCTION: Filter
- FUNCTIONALITY: Get some blobs from the class based on conditions on measures
				 of the blobs. 
- PARAMETERS:
	- dst: where to store the selected blobs
	- filterAction:	B_INCLUDE: include the blobs which pass the filter in the result 
				    B_EXCLUDE: exclude the blobs which pass the filter in the result 
	- evaluador: Object to evaluate the blob
	- Condition: How to decide if  the result returned by evaluador on each blob
				 is included or not. It can be:
				    B_EQUAL,B_NOT_EQUAL,B_GREATER,B_LESS,B_GREATER_OR_EQUAL,
				    B_LESS_OR_EQUAL,B_INSIDE,B_OUTSIDE
	- LowLimit:  numerical value to evaluate the Condition on evaluador(blob)
	- HighLimit: numerical value to evaluate the Condition on evaluador(blob).
				 Only useful for B_INSIDE and B_OUTSIDE
- RESULT:
	- It returns on dst the blobs that accomplish (B_INCLUDE) or discards (B_EXCLUDE)
	  the Condition on the result returned by evaluador on each blob
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
void CBlobResult::Filter(CBlobResult &dst, 
						 int filterAction, 
						 funcio_calculBlob *evaluador, 
						 int condition, 
						 double lowLimit, double highLimit /*=0*/)
							
{
	int numBlobs = GetNumBlobs();

	// do the job
	DoFilter(dst, filterAction, evaluador, condition, lowLimit, highLimit );

	// inline operation: remove previous blobs
	if( &dst == this ) 
	{
		// esborrem els primers blobs ( que són els originals )
		// ja que els tindrem replicats al final si passen el filtre
		Blob_vector::iterator itBlobs = m_blobs.begin();
		for( int i = 0; i < numBlobs; i++ )
		{
			delete *itBlobs;
			itBlobs++;
		}
		m_blobs.erase( m_blobs.begin(), itBlobs );
	}
}


//! Does the Filter method job
void CBlobResult::DoFilter(CBlobResult &dst, int filterAction, funcio_calculBlob *evaluador, 
						   int condition, double lowLimit, double highLimit/* = 0*/) const
{
	int i, numBlobs;
	bool resultavaluacio;
	double_stl_vector avaluacioBlobs;
	double_stl_vector::iterator itavaluacioBlobs;

	if( GetNumBlobs() <= 0 ) return;
	if( !evaluador ) return;
	//avaluem els blobs amb la funció pertinent	
	avaluacioBlobs = GetSTLResult(evaluador);
	itavaluacioBlobs = avaluacioBlobs.begin();
	numBlobs = GetNumBlobs();
	switch(condition)
	{
		case B_EQUAL:
			for(i=0;i<numBlobs;i++, itavaluacioBlobs++)
			{
				resultavaluacio= *itavaluacioBlobs == lowLimit;
				if( ( resultavaluacio && filterAction == B_INCLUDE ) ||
					( !resultavaluacio && filterAction == B_EXCLUDE ))
				{
					dst.m_blobs.push_back( new CBlob( GetBlob( i ) ));
				}				
			}
			break;
		case B_NOT_EQUAL:
			for(i=0;i<numBlobs;i++, itavaluacioBlobs++)
			{
				resultavaluacio = *itavaluacioBlobs != lowLimit;
				if( ( resultavaluacio && filterAction == B_INCLUDE ) ||
					( !resultavaluacio && filterAction == B_EXCLUDE ))
				{
					dst.m_blobs.push_back( new CBlob( GetBlob( i ) ));
				}
			}
			break;
		case B_GREATER:
			for(i=0;i<numBlobs;i++, itavaluacioBlobs++)
			{
				resultavaluacio= *itavaluacioBlobs > lowLimit;
				if( ( resultavaluacio && filterAction == B_INCLUDE ) ||
					( !resultavaluacio && filterAction == B_EXCLUDE ))
				{
					dst.m_blobs.push_back( new CBlob( GetBlob( i ) ));
				}
			}
			break;
		case B_LESS:
			for(i=0;i<numBlobs;i++, itavaluacioBlobs++)
			{
				resultavaluacio= *itavaluacioBlobs < lowLimit;
				if( ( resultavaluacio && filterAction == B_INCLUDE ) ||
					( !resultavaluacio && filterAction == B_EXCLUDE ))
				{
					dst.m_blobs.push_back( new CBlob( GetBlob( i ) ));
				}
			}
			break;
		case B_GREATER_OR_EQUAL:
			for(i=0;i<numBlobs;i++, itavaluacioBlobs++)
			{
				resultavaluacio= *itavaluacioBlobs>= lowLimit;
				if( ( resultavaluacio && filterAction == B_INCLUDE ) ||
					( !resultavaluacio && filterAction == B_EXCLUDE ))
				{
					dst.m_blobs.push_back( new CBlob( GetBlob( i ) ));
				}
			}
			break;
		case B_LESS_OR_EQUAL:
			for(i=0;i<numBlobs;i++, itavaluacioBlobs++)
			{
				resultavaluacio= *itavaluacioBlobs <= lowLimit;
				if( ( resultavaluacio && filterAction == B_INCLUDE ) ||
					( !resultavaluacio && filterAction == B_EXCLUDE ))
				{
					dst.m_blobs.push_back( new CBlob( GetBlob( i ) ));
				}
			}
			break;
		case B_INSIDE:
			for(i=0;i<numBlobs;i++, itavaluacioBlobs++)
			{
				resultavaluacio=( *itavaluacioBlobs >= lowLimit) && ( *itavaluacioBlobs <= highLimit); 
				if( ( resultavaluacio && filterAction == B_INCLUDE ) ||
					( !resultavaluacio && filterAction == B_EXCLUDE ))
				{
					dst.m_blobs.push_back( new CBlob( GetBlob( i ) ));
				}
			}
			break;
		case B_OUTSIDE:
			for(i=0;i<numBlobs;i++, itavaluacioBlobs++)
			{
				resultavaluacio=( *itavaluacioBlobs < lowLimit) || ( *itavaluacioBlobs > highLimit); 
				if( ( resultavaluacio && filterAction == B_INCLUDE ) ||
					( !resultavaluacio && filterAction == B_EXCLUDE ))
				{
					dst.m_blobs.push_back( new CBlob( GetBlob( i ) ));
				}
			}
			break;
	}
}
/**
- FUNCIÓ: GetBlob
- FUNCIONALITAT: Retorna un blob si aquest existeix (index != -1)
- PARÀMETRES:
	- indexblob: index del blob a retornar
- RESULTAT:
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 25-05-2005.
- MODIFICACIÓ: Data. Autor. Descripció.
*/
/*
- FUNCTION: GetBlob
- FUNCTIONALITY: Gets the n-th blob (without ordering the blobs)
- PARAMETERS:
	- indexblob: index in the blob array
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
CBlob CBlobResult::GetBlob(int indexblob) const
{	
	if( indexblob < 0 || indexblob >= GetNumBlobs() )
		RaiseError( EXCEPTION_BLOB_OUT_OF_BOUNDS );

	return *m_blobs[indexblob];
}
CBlob *CBlobResult::GetBlob(int indexblob)
{	
	if( indexblob < 0 || indexblob >= GetNumBlobs() )
		RaiseError( EXCEPTION_BLOB_OUT_OF_BOUNDS );
	return m_blobs[indexblob];
}

CBlob CBlobResult::GetBlobByID(t_labelType id) const{
	for(int i = 0;i<GetNumBlobs();i++){
		if(GetBlob(i).GetID()==id){
			return m_blobs[i];
		}
	}
	RaiseError( EXCEPTION_EXECUTE_FAULT );
	return CBlob();
}

CBlob *CBlobResult::GetBlobByID(t_labelType id){
	for(int i = 0;i<GetNumBlobs();i++){
		if(GetBlob(i)->GetID()==id){
			return m_blobs[i];
		}
	}
	RaiseError( EXCEPTION_EXECUTE_FAULT );
	return (new CBlob());
}

/**
- FUNCIÓ: GetNthBlob
- FUNCIONALITAT: Retorna l'enèssim blob segons un determinat criteri
- PARÀMETRES:
	- criteri: criteri per ordenar els blobs (objectes derivats de COperadorBlob)
	- nBlob: index del blob a retornar
	- dst: on es retorna el resultat
- RESULTAT:
	- retorna el blob nBlob a dst ordenant els blobs de la classe segons el criteri
	  en ordre DESCENDENT. Per exemple, per obtenir el blob major:
		GetNthBlob( CBlobGetArea(), 0, blobMajor );
		GetNthBlob( CBlobGetArea(), 1, blobMajor ); (segon blob més gran)
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 25-05-2005.
- MODIFICACIÓ: Data. Autor. Descripció.
*/
/*
- FUNCTION: GetNthBlob
- FUNCTIONALITY: Gets the n-th blob ordering first the blobs with some criteria
- PARAMETERS:
	- criteri: criteria to order the blob array
	- nBlob: index of the returned blob in the ordered blob array
	- dst: where to store the result
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
void CBlobResult::GetNthBlob( funcio_calculBlob *criteri, int nBlob, CBlob &dst ) const
{
	// verifiquem que no estem accedint fora el vector de blobs
	if( nBlob < 0 || nBlob >= GetNumBlobs() )
	{
		//RaiseError( EXCEPTION_BLOB_OUT_OF_BOUNDS );
		dst = CBlob();
		return;
	}

	double_stl_vector avaluacioBlobs, avaluacioBlobsOrdenat;
	double valorEnessim;

	//avaluem els blobs amb la funció pertinent	
	avaluacioBlobs = GetSTLResult(criteri);

	avaluacioBlobsOrdenat = double_stl_vector( GetNumBlobs() );

	// obtenim els nBlob primers resultats (en ordre descendent)
	std::partial_sort_copy( avaluacioBlobs.begin(), 
						    avaluacioBlobs.end(),
						    avaluacioBlobsOrdenat.begin(), 
						    avaluacioBlobsOrdenat.end(),
						    std::greater<double>() );

	valorEnessim = avaluacioBlobsOrdenat[nBlob];

	// busquem el primer blob que té el valor n-ssim
	double_stl_vector::const_iterator itAvaluacio = avaluacioBlobs.begin();

	bool trobatBlob = false;
	int indexBlob = 0;
	while( itAvaluacio != avaluacioBlobs.end() && !trobatBlob )
	{
		if( *itAvaluacio == valorEnessim )
		{
			trobatBlob = true;
			dst = CBlob( GetBlob(indexBlob));
		}
		itAvaluacio++;
		indexBlob++;
	}
}

/**
- FUNCIÓ: ClearBlobs
- FUNCIONALITAT: Elimina tots els blobs de l'objecte
- PARÀMETRES:
- RESULTAT: 
	- Allibera tota la memòria dels blobs
- RESTRICCIONS:
- AUTOR: Ricard Borràs Navarra
- DATA DE CREACIÓ: 25-05-2005.
- MODIFICACIÓ: Data. Autor. Descripció.
*/
/*
- FUNCTION: ClearBlobs
- FUNCTIONALITY: Clears all the blobs from the object and releases all its memory
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
void CBlobResult::ClearBlobs()
{
	Blob_vector::iterator itBlobs = m_blobs.begin();
	while( itBlobs != m_blobs.end() )
	{
		delete *itBlobs;
		itBlobs++;
	}

	m_blobs.clear();
}

/**
- FUNCIÓ: RaiseError
- FUNCIONALITAT: Funció per a notificar errors al l'usuari (en debug) i llença
			   les excepcions
- PARÀMETRES:
	- errorCode: codi d'error
- RESULTAT: 
	- Ensenya un missatge a l'usuari (en debug) i llença una excepció
- RESTRICCIONS:
- AUTOR: Ricard Borràs Navarra
- DATA DE CREACIÓ: 25-05-2005.
- MODIFICACIÓ: Data. Autor. Descripció.
*/
/*
- FUNCTION: RaiseError
- FUNCTIONALITY: Error handling function
- PARAMETERS:
	- errorCode: reason of the error
- RESULT:
	- in _SHOW_ERRORS version, shows a message box with the error. In release is silent.
	  In both cases throws an exception with the error.
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
void CBlobResult::RaiseError(const int errorCode) const
{
//! Do we need to show errors?
#ifdef _SHOW_ERRORS
	CString msg, format = "Error en CBlobResult: %s";

	switch (errorCode)
	{
	case EXCEPTION_BLOB_OUT_OF_BOUNDS:
		msg.Format(format, "Intentant accedir a un blob no existent");
		break;
	default:
		msg.Format(format, "Codi d'error desconegut");
		break;
	}

	AfxMessageBox(msg);

#endif
	throw errorCode;
}



/**************************************************************************
		Auxiliars / Auxiliary functions
**************************************************************************/


/**
- FUNCIÓ: PrintBlobs
- FUNCIONALITAT: Escriu els paràmetres (àrea, perímetre, exterior, mitjana) 
			   de tots els blobs a un fitxer.
- PARÀMETRES:
	- nom_fitxer: path complet del fitxer amb el resultat
- RESULTAT:
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 25-05-2005.
- MODIFICACIÓ: Data. Autor. Descripció.
*/
/*
- FUNCTION: PrintBlobs
- FUNCTIONALITY: Prints some blob features in an ASCII file
- PARAMETERS:
	- nom_fitxer: full path + filename to generate
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
void CBlobResult::PrintBlobs( char *nom_fitxer ) const
{
	double_stl_vector area, /*perimetre,*/ exterior, compacitat, longitud, 
					  externPerimeter, perimetreConvex, perimetre;
	int i;
	FILE *fitxer_sortida;

 	area      = GetSTLResult( CBlobGetArea());
	perimetre = GetSTLResult( CBlobGetPerimeter());
	exterior  = GetSTLResult( CBlobGetExterior());
	compacitat = GetSTLResult(CBlobGetCompactness());
	longitud  = GetSTLResult( CBlobGetLength());
	externPerimeter = GetSTLResult( CBlobGetExternPerimeter());
	perimetreConvex = GetSTLResult( CBlobGetHullPerimeter());

	fitxer_sortida = fopen( nom_fitxer, "w" );

	for(i=0; i<GetNumBlobs(); i++)
	{
		fprintf( fitxer_sortida, "blob %d ->\t a=%7.0f\t p=%8.2f (%8.2f extern)\t pconvex=%8.2f\t ext=%.0f\t m=%7.2f\t c=%3.2f\t l=%8.2f\n",
				 i, area[i], perimetre[i], externPerimeter[i], perimetreConvex[i], exterior[i], compacitat[i], longitud[i] );
	}
	fclose( fitxer_sortida );

}

/**
- FUNCTION: thread_componentLabeling
- FUNCTIONALITY: Static function needed to create many component labeling threads from the constructor
- PARAMETERS:
	- msg: pointer to thread message, which contains the binary image, the mask and the background color
- RESULT:
	- returns the CBlobResult created.
- RESTRICTIONS:
- AUTHOR: Saverio Murgia & Luca Nardelli
- CREATION DATE: 06-04-2013.
- MODIFICATION: Date. Author. Description.
*/
void* CBlobResult::thread_componentLabeling( ThreadMessage *msg )
{
	//int64 time=getTickCount();
	int shift = msg->origin > 0 ? msg->origin-1 : msg->origin;
	int height = msg->origin > 0 ? msg->height+1 : msg->height;
	Rect roi = Rect(0,shift,msg->image.size().width,height);
	if(msg->mask.data)
		msg->res = new CBlobResult(&(IplImage)(msg->image(roi)),&(IplImage)(msg->mask(roi)),msg->backColor,msg->labels);
	else
		msg->res = new CBlobResult(&(IplImage)(msg->image(roi)),NULL,msg->backColor,msg->labels);
	//Devo sommare l'offset di ogni punto
	int numBlobs = msg->res->GetNumBlobs();
	int numCores = pthread_num_processors_np();
	for(int i=0;i<numBlobs;i++){
		CBlob *curBlob = msg->res->GetBlob(i);
		curBlob->ShiftBlob(0,shift);
		curBlob = curBlob;
		//Per ora non mi serve
		//curBlob->OriginalImageSize(curBlob->OriginalImageSize().width,curBlob->OriginalImageSize().height*numCores);
	}
	//std::cout <<"Tempo Thread: "<<(getTickCount()-time)/getTickFrequency()<<std::endl;
	return msg;
}

void CBlobResult::BlobOverlap::Print(){
	cout << "Source Blob ID: "<< sourceBlob->GetID()<<endl;
	map<unsigned int,deque<Segment>>::iterator it;
	for( it = matchingSegments.begin();it!=matchingSegments.end();it++){
		cout << "Blob to Join ID: " << it->first << endl;
		cout << "Common Segments: " << endl;
		for(int i=0; i< it->second.size();i++){
			cout << it->second[i].begin << "-" << it->second[i].end << endl;
		}
	}
}