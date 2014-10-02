/************************************************************************
  			BlobResult.cpp
  			
FUNCIONALITAT: Implementaci� de la classe CBlobResultShared
AUTOR: Inspecta S.L.
MODIFICACIONS (Modificaci�, Autor, Data):
 
**************************************************************************/

#include "BlobResultShared.h"
//! Show errors functions: only works for windows releases
#ifdef _SHOW_ERRORS
	#include <afx.h>			//suport per a CStrings
	#include <afxwin.h>			//suport per a AfxMessageBox
#endif

using namespace std;
using namespace cv;

/**************************************************************************
		Constructors / Destructors
**************************************************************************/


CBlobResultShared::CBlobResultShared()
{
}


CBlobResultShared::CBlobResultShared(IplImage *source, IplImage *mask,int numThreads)
{
    Mat s(source),m(mask);
    detect(s,m,numThreads);
//    Blob_vector temp;
//	if(mask!=NULL){
//		Mat temp = Mat::zeros(Size(source->width,source->height),CV_8UC1);
//		Mat(source).copyTo(temp,Mat(mask));
//		compLabeler.set(numThreads,temp);
//        compLabeler.doLabeling(temp);
//	}
//	else{
//		compLabeler.set(numThreads,source);
//        compLabeler.doLabeling(temp);
//	}
//    m_blobs.reserve(temp.size());
//    for(CBlob *b : temp)
//        m_blobs.emplace_back(b);
}
/**
- FUNCTION:
- FUNCTIONALITY: Constructor from an image. Fills an object with all the blobs in
	the image, OPENCV 2 interface
- PARAMETERS:
	- source: Mat to extract the blobs from, CV_8UC1
	- mask: optional mask to apply. The blobs will be extracted where the mask is
			not 0. All the neighbouring blobs where the mask is 0 will be extern blobs
	- numThreads: number of labelling threads. 
- RESULT:
	- object with all the blobs in the image.
- RESTRICTIONS:
- AUTHOR: Saverio Murgia & Luca Nardelli
- CREATION DATE: 06-04-2013.
- MODIFICATION: Date. Author. Description.
*/
CBlobResultShared::CBlobResultShared(Mat &source, const Mat &mask,int numThreads){
    detect(source,mask,numThreads);
//	if(mask.data){
//		Mat temp=Mat::zeros(source.size(),source.type());
//		source.copyTo(temp,mask);
//		compLabeler.set(numThreads,temp);
//		compLabeler.doLabeling(m_blobs);
//	}
//	else{
//		compLabeler.set(numThreads,source);
//		compLabeler.doLabeling(m_blobs);
//	}
}


/**
- FUNCTION: detect
- FUNCTIONALITY: detects blob in the image
- PARAMETERS:
	- source: Mat to extract the blobs from, CV_8UC1
	- mask: optional mask to apply. The blobs will be extracted where the mask is
			not 0. All the neighbouring blobs where the mask is 0 will be extern blobs
	- numThreads: number of labelling threads. 
- RESULT:
	- the object will contain the detected blobs.
- RESTRICTIONS:
- AUTHOR: Saverio Murgia & Luca Nardelli
- CREATION DATE: 10-04-2014.
- MODIFICATION: Date. Author. Description.
*/
void CBlobResultShared::detect( cv::Mat &source, const cv::Mat &mask /*= cv::Mat()*/,int numThreads/*=1*/ )
{
	m_blobs.clear();
    Blob_vector tVec;
	if(mask.data){
		Mat temp=Mat::zeros(source.size(),source.type());
		source.copyTo(temp,mask);
		compLabeler.set(numThreads,temp);
        compLabeler.doLabeling(tVec);
	}
	else{
		compLabeler.set(numThreads,source);
        compLabeler.doLabeling(tVec);
	}
    m_blobs.reserve(tVec.size());
    for(CBlob *b : tVec)
        m_blobs.emplace_back(b);
}


/**
- FUNCI�: CBlobResultShared
- FUNCIONALITAT: Constructor de c�pia. Inicialitza la seq��ncia de blobs 
			   amb els blobs del par�metre.
- PAR�METRES:
	- source: objecte que es copiar�
- RESULTAT:
    - objecte CBlobResultShared amb els blobs de l'objecte source
- RESTRICCIONS:
- AUTOR: Ricard Borr�s
- DATA DE CREACI�: 25-05-2005.
- MODIFICACI�: Data. Autor. Descripci�.
*/
/**
- FUNCTION: CBlobResultShared
- FUNCTIONALITY: Copy constructor
- PARAMETERS:
	- source: object to copy
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
CBlobResultShared::CBlobResultShared( const CBlobResultShared &source )
{	
	// creem el nou a partir del passat com a par�metre
	//m_blobs = Blob_vector( source.GetNumBlobs() );
	m_blobs.reserve(source.GetNumBlobs());
	// copiem els blobs de l'origen a l'actual
    Shared_Blob_Vector::const_iterator pBlobsSrc = source.m_blobs.begin();
    Shared_Blob_Vector::iterator pBlobsDst = m_blobs.begin();
	while( pBlobsSrc != source.m_blobs.end() )
	{
		// no podem cridar a l'operador = ja que Blob_vector �s un 
		// vector de CBlob*. Per tant, creem un blob nou a partir del
		// blob original
        m_blobs.emplace_back(new CBlob(**pBlobsSrc));
        ++pBlobsSrc;
	}
}



/**
- FUNCI�: ~CBlobResultShared
- FUNCIONALITAT: Destructor estandard.
- PAR�METRES:
- RESULTAT:
	- Allibera la mem�ria reservada de cadascun dels blobs de la classe
- RESTRICCIONS:
- AUTOR: Ricard Borr�s
- DATA DE CREACI�: 25-05-2005.
- MODIFICACI�: Data. Autor. Descripci�.
*/
/**
- FUNCTION: ~CBlobResultShared
- FUNCTIONALITY: Destructor
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
CBlobResultShared::~CBlobResultShared()
{
	ClearBlobs();
}

/**************************************************************************
		Operadors / Operators
**************************************************************************/




/**************************************************************************
		Operacions / Operations
**************************************************************************/




#ifdef MATRIXCV_ACTIU

/**
- FUNCI�: GetResult
- FUNCIONALITAT: Calcula el resultat especificat sobre tots els blobs de la classe
- PAR�METRES:
	- evaluador: Qualsevol objecte derivat de COperadorBlob
- RESULTAT:
	- Retorna un array de double's amb el resultat per cada blob
- RESTRICCIONS:
- AUTOR: Ricard Borr�s
- DATA DE CREACI�: 25-05-2005.
- MODIFICACI�: Data. Autor. Descripci�.
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
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double_vector CBlobResultShared::GetResult( blobOperator *evaluador ) const
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

	// avaluem la funci� en tots els blobs
	while( itBlobs != m_blobs.end() )
	{
		*itResult = (*evaluador)(**itBlobs);
		itBlobs++;
		itResult++;
	}
	return result;
}
#endif



/*
- FUNCTION: GetBlob
- FUNCTIONALITY: Gets the n-th blob (without ordering the blobs)
- PARAMETERS:
	- indexblob: index in the blob array
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
CBlob CBlobResultShared::GetBlob(int indexblob) const
{	
	if( indexblob < 0 || indexblob >= GetNumBlobs() )
		RaiseError( EXCEPTION_BLOB_OUT_OF_BOUNDS );

	return *m_blobs[indexblob];
}

shared_ptr<CBlob> CBlobResultShared::GetBlob(int indexblob)
{	
	if( indexblob < 0 || indexblob >= GetNumBlobs() )
		RaiseError( EXCEPTION_BLOB_OUT_OF_BOUNDS );
	return m_blobs[indexblob];
}




/**
- FUNCI�: ClearBlobs
- FUNCIONALITAT: Elimina tots els blobs de l'objecte
- PAR�METRES:
- RESULTAT: 
	- Allibera tota la mem�ria dels blobs
- RESTRICCIONS:
- AUTOR: Ricard Borr�s Navarra
- DATA DE CREACI�: 25-05-2005.
- MODIFICACI�: Data. Autor. Descripci�.
*/
/*
- FUNCTION: ClearBlobs
- FUNCTIONALITY: Clears all the blobs from the object and releases all its memory
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
void CBlobResultShared::ClearBlobs()
{
	m_blobs.clear();
}

/**
- FUNCI�: RaiseError
- FUNCIONALITAT: Funci� per a notificar errors al l'usuari (en debug) i llen�a
			   les excepcions
- PAR�METRES:
	- errorCode: codi d'error
- RESULTAT: 
	- Ensenya un missatge a l'usuari (en debug) i llen�a una excepci�
- RESTRICCIONS:
- AUTOR: Ricard Borr�s Navarra
- DATA DE CREACI�: 25-05-2005.
- MODIFICACI�: Data. Autor. Descripci�.
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
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
void CBlobResultShared::RaiseError(const int errorCode) const
{
//! Do we need to show errors?
#ifdef _SHOW_ERRORS
    CString msg, format = "Error en CBlobResultShared: %s";

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


std::shared_ptr<CBlob> CBlobResultShared::getBlobNearestTo( Point pt )
{
	float minD = FLT_MAX,d=0;
	int numBlobs = m_blobs.size();
	int indNearest = -1;
	for(int i=0;i<numBlobs;i++){
		Point diff = m_blobs[i]->getCenter() - pt;
		d = diff.x*diff.x+diff.y*diff.y;
		if(minD > d){
			indNearest = i;
			minD=d;
		}
	}
	if(indNearest!=-1)
		return m_blobs[indNearest];
	else
		return NULL;
}
