/*
 * Universidade Federal do Rio de Janeiro
 * Escola Politecnica
 * Departamento de Eletronica e Computacao
 * Circuitos Eletricos II - 2017.1
 * Professor Antonio Carlos M. de Queiroz
 * Autor: Lucas Correa Macedo
 */

#include "const.h"

#ifndef TYPES_H
#define TYPES_H

typedef struct netlistElement 
{
  char name [MAX_NAME];
  double value, iS, vT, c0, c1;
  unsigned a, b, c, d, x, y;
  
  /* Transistor */
  unsigned e; /* Emitter node */
  char type [MAX_TYPE_LENGTH]; /* NPN or PNP */
  double alpha, alphaR, iSbe, vTbe, iSbc, vTbc, vA, c0be, c1be, c0bc, c1bc; 
  
  /* Inductive coupling */
  char inductorA [MAX_NAME], inductorB [MAX_NAME]; /* Inductors */
  
  /* Sources */
  double modulus, phase;
} netlistElement;

typedef enum
{
	ok,
	singularSystem,
	maxNodesExceeded,
	unknownElement,
	maxElementsExceeded,
	doesNotConverge,
	netlistElementNotFound,
	invalidParameter,
	errorOpeningFile
} errorType;

typedef struct plotParameters
{
	char scale [MAX_TYPE_LENGTH]; /* LIN or OCT or DEC */
	unsigned numPoints;
	double beginning, ending;
} plotParameters;

#endif
