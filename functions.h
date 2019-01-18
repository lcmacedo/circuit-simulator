/*
 * Universidade Federal do Rio de Janeiro
 * Escola Politecnica
 * Departamento de Eletronica e Computacao
 * Circuitos Eletricos II - 2017.1
 * Professor Antonio Carlos M. de Queiroz
 * Autor: Lucas Correa Macedo
 */
 
#include "const.h"
#include "types.h"

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

errorType
SolveSystem (double [MAX_NODES + 1][MAX_NODES + 2], unsigned);

errorType
GetNodeNumber (char *, char [MAX_NODES + 1][MAX_NAME + 2], unsigned *, unsigned *);

double
GetDiodeConductance (netlistElement, double *);

double
GetDiodeCurrentSourceValue (netlistElement, double, double *);

errorType
SearchNetlist (netlistElement *, unsigned, char *, netlistElement *);

double
GetDiodeCapacitance (netlistElement, double *);

errorType
SolveComplexSystem (double complex [MAX_NODES + 1][MAX_NODES + 2], unsigned);

#endif
