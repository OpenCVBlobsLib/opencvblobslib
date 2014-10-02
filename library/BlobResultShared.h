/************************************************************************
  			BlobResult.h
  			
FUNCIONALITAT: Definici� de la classe CBlobResult
AUTOR: Inspecta S.L.
MODIFICACIONS (Modificaci�, Autor, Data):

FUNCTIONALITY: Definition of the CBlobResult class
AUTHOR: Inspecta S.L.
MODIFICATIONS (Modification, Author, Date):

**************************************************************************/


#if !defined(_CLASSE_BLOBRESULTSHARED_INCLUDED)
#define _CLASSE_BLOBRESULTSHARED_INCLUDED

#if _MSC_VER > 1000 
#pragma once
#endif // _MSC_VER > 1000

#include "BlobLibraryConfiguration.h"
#include "ComponentLabeling.h"
#include "defines.h"
#include <math.h>
#include "opencv/cxcore.h"
#include <opencv2/opencv.hpp>
#include <deque>
#include <limits.h>
#include <stdio.h>
#include <functional>
#include <algorithm>
#include <cfloat>

#include <vector>		// vectors de la STL
#include <functional>
#include <memory>
#include <list>

#include "blob.h"
#include "BlobOperators.h"
#include "ComponentLabeling.h"

/** 
	Classe que cont� un conjunt de blobs i permet extreure'n propietats 
	o filtrar-los segons determinats criteris.
	Class to calculate the blobs of an image and calculate some properties 
	on them. Also, the class provides functions to filter the blobs using
	some criteria.
*/

//CBlobResult in which blobs are stored as shared pointers, in order to allow
//persistence of blobs.
class CBlobResultShared
{
public:

    //Constructor, opencv 1.0 and 2.0 interfaces.
    CBlobResultShared();
    CBlobResultShared(IplImage *source, IplImage *mask = NULL, int numThreads=1);
    CBlobResultShared(cv::Mat &source, const cv::Mat &mask = cv::Mat(),int numThreads=1);
    CBlobResultShared( const CBlobResultShared &source );
    //! Destructor
    virtual ~CBlobResultShared();

    // Function to detect blobs in a new image
    void detect(cv::Mat &source, const cv::Mat &mask = cv::Mat(),int numThreads=1);

    //! Retorna el blob en�ssim
    //! Gets the n-th blob of the class ( without sorting )
    CBlob GetBlob(int indexblob) const;
    std::shared_ptr<CBlob> GetBlob(int indexblob);

    //! Elimina tots els blobs de l'objecte
    //! Clears all the blobs of the class
    void ClearBlobs();

    // Returns blob with center nearest to point pt
    std::shared_ptr<CBlob> getBlobNearestTo(cv::Point pt);

//Metodes GET/SET

    //! Retorna el total de blobs
    //! Gets the total number of blobs
    int GetNumBlobs() const
    {
        return(m_blobs.size());
    }

    const Shared_Blob_Vector& getBlobVector(){return m_blobs;}


private:
    myCompLabelerGroup compLabeler;

    //! Funci� per gestionar els errors
    //! Function to manage the errors
    void RaiseError(const int errorCode) const;

protected:
    //! Vector with all the blobs
    Shared_Blob_Vector		m_blobs;
};


#endif // !defined(_CLASSE_BLOBRESULTSHARED_INCLUDED)
