/************************************************************************
  			BlobResult.h
  			
FUNCIONALITAT: Definició de la classe CBlobResult
AUTOR: Inspecta S.L.
MODIFICACIONS (Modificació, Autor, Data):

FUNCTIONALITY: Definition of the CBlobResult class
AUTHOR: Inspecta S.L.
MODIFICATIONS (Modification, Author, Date):

**************************************************************************/


#if !defined(_CLASSE_BLOBRESULT_INCLUDED)
#define _CLASSE_BLOBRESULT_INCLUDED

#if _MSC_VER > 1000 
#pragma once
#endif // _MSC_VER > 1000

#include "BlobLibraryConfiguration.h"
#include <math.h>
#include "opencv/cxcore.h"
#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>

#ifdef MATRIXCV_ACTIU
	#include "matrixCV.h"
#else
	// llibreria STL
	#include "vector"
	//! Vector de doubles
	typedef std::vector<double> double_stl_vector;
#endif

#include <vector>		// vectors de la STL
#include <functional>
#include "Blob.h"
#include "BlobOperators.h"
#include "ComponentLabeling.h"
/**************************************************************************
	Filtres / Filters
**************************************************************************/

//! accions que es poden fer amb els filtres
//! Actions performed by a filter (include or exclude blobs)
#define B_INCLUDE				1L
#define B_EXCLUDE				2L

//! condicions sobre els filtres
//! Conditions to apply the filters
#define B_EQUAL					3L
#define B_NOT_EQUAL				4L
#define B_GREATER				5L
#define B_LESS					6L
#define B_GREATER_OR_EQUAL		7L
#define B_LESS_OR_EQUAL			8L
#define B_INSIDE			    9L
#define B_OUTSIDE			    10L


/**************************************************************************
	Excepcions / Exceptions
**************************************************************************/

//! Excepcions llençades per les funcions:
#define EXCEPTION_BLOB_OUT_OF_BOUNDS	1000
#define EXCEPCIO_CALCUL_BLOBS			1001

/** 
	Classe que conté un conjunt de blobs i permet extreure'n propietats 
	o filtrar-los segons determinats criteris.
	Class to calculate the blobs of an image and calculate some properties 
	on them. Also, the class provides functions to filter the blobs using
	some criteria.
*/
using namespace cv;
class CBlobResult  
{
public:

	//! Standard constructor, it creates an empty set of blobs
	CBlobResult();
	//! Image constructor, it creates an object with the blobs of the image
	CBlobResult(IplImage *source, IplImage *mask, uchar backgroundColor);
	//! OpenCV2 interface
	CBlobResult(Mat source, Mat mask, uchar backgroundColor);
	//! Copy constructor
	CBlobResult( const CBlobResult &source );
	//! Destructor
	virtual ~CBlobResult();

	//! operador = per a fer assignacions entre CBlobResult
	//! Assigment operator
	CBlobResult& operator=(const CBlobResult& source);
	//! operador + per concatenar dos CBlobResult
	//! Addition operator to concatenate two sets of blobs
	CBlobResult operator+( const CBlobResult& source ) const;
	
	//! Adds a blob to the set of blobs
	void AddBlob( CBlob *blob );

#ifdef MATRIXCV_ACTIU
	//! Calcula un valor sobre tots els blobs de la classe retornant una MatrixCV
	//! Computes some property on all the blobs of the class
	double_vector GetResult( funcio_calculBlob *evaluador ) const;
#endif
	//! Calcula un valor sobre tots els blobs de la classe retornant un std::vector<double>
	//! Computes some property on all the blobs of the class
	double_stl_vector GetSTLResult( funcio_calculBlob *evaluador ) const;
	
	//! Calcula un valor sobre un blob de la classe
	//! Computes some property on one blob of the class
	double GetNumber( int indexblob, funcio_calculBlob *evaluador ) const;

	//! Retorna aquells blobs que compleixen les condicions del filtre en el destination 
	//! Filters the blobs of the class using some property
	void Filter(CBlobResult &dst,
				int filterAction, funcio_calculBlob *evaluador, 
				int condition, double lowLimit, double highLimit = 0 );
	void Filter(CBlobResult &dst,
				int filterAction, funcio_calculBlob *evaluador, 
				int condition, double lowLimit, double highLimit = 0 ) const;
			
	//! Retorna l'enèssim blob segons un determinat criteri
	//! Sorts the blobs of the class acording to some criteria and returns the n-th blob
	void GetNthBlob( funcio_calculBlob *criteri, int nBlob, CBlob &dst ) const;
	
	//! Retorna el blob enèssim
	//! Gets the n-th blob of the class ( without sorting )
	CBlob GetBlob(int indexblob) const;
	CBlob *GetBlob(int indexblob);
	
	//! Elimina tots els blobs de l'objecte
	//! Clears all the blobs of the class
	void ClearBlobs();

	//! Escriu els blobs a un fitxer
	//! Prints some features of all the blobs in a file
	void PrintBlobs( char *nom_fitxer ) const;


//Metodes GET/SET

	//! Retorna el total de blobs
	//! Gets the total number of blobs
	int GetNumBlobs() const 
	{ 
		return(m_blobs.size()); 
	}


private:

	//! Funció per gestionar els errors
	//! Function to manage the errors
	void RaiseError(const int errorCode) const;

	//! Does the Filter method job
	void DoFilter(CBlobResult &dst,
				int filterAction, funcio_calculBlob *evaluador, 
				int condition, double lowLimit, double highLimit = 0) const;

	class threadMessage{
	public:
	Mat image;
	Mat mask;
	uchar backColor;
	int origin;
	int height;
	CBlobResult *res;
	threadMessage(Mat img,Mat msk,uchar backgroundCol,int org,int hei):image(img),mask(msk),backColor(backgroundCol),origin(org),height(hei),res(NULL){}
	threadMessage():image(Mat()),mask(Mat()),res(NULL){}
	~threadMessage(){ if(res!=NULL) delete res;}
	threadMessage& operator=(threadMessage &o){image=o.image;mask=o.mask;backColor=o.backColor;res=o.res;origin=o.origin;height=o.height; return *this;}
	};
	/*class threadMessage{
	public:
		CBlobResult& instance;
		int id;
		threadMessage(CBlobResult &inst,int i):instance(inst),id(i){}
		~threadMessage(){}
	};*/
	static void* thread_componentLabeling(threadMessage *msg);

protected:

	//! Vector with all the blobs
	Blob_vector		m_blobs;
};

#endif // !defined(_CLASSE_BLOBRESULT_INCLUDED)
