#pragma once

#include <memory>
#include <vector>

#ifdef MATRIXCV_ACTIU
    #include "matrixCV.h"
#else
    // llibreria STL
    #include "vector"
    //! Vector de doubles
    typedef std::vector<double> double_stl_vector;
#endif

/**************************************************************************
    Filtres / Filters
**************************************************************************/

//! accions que es poden fer amb els filtres
//! Actions performed by a filter (include or exclude blobs)
#define B_INCLUDE				1L
#define B_EXCLUDE				2L

enum FilterAction {FLT_INCLUDE=1,FLT_EXCLUDE};

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

enum FilterCondition {FLT_EQUAL=3,FLT_NOTEQUAL,FLT_GREATER,FLT_LESS,FLT_GREATEROREQUAL,FLT_LESSOREQUAL,FLT_INSIDE,FLT_OUTSIDE};

/**************************************************************************
    Excepcions / Exceptions
**************************************************************************/

//! Excepcions llen�ades per les funcions:
#define EXCEPTION_BLOB_OUT_OF_BOUNDS	1000
#define EXCEPCIO_CALCUL_BLOBS			1001

/**
    Classe que cont� un conjunt de blobs i permet extreure'n propietats
    o filtrar-los segons determinats criteris.
    Class to calculate the blobs of an image and calculate some properties
    on them. Also, the class provides functions to filter the blobs using
    some criteria.
*/

typedef std::vector<std::shared_ptr<CBlob>>	Shared_Blob_Vector;
// vector of blob pointers typedef
typedef std::vector<CBlob*>	Blob_vector;
