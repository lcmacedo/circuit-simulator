/*
 * Universidade Federal do Rio de Janeiro
 * Escola Politecnica
 * Departamento de Eletronica e Computacao
 * Circuitos Eletricos II - 2017.1
 * Professor Antonio Carlos M. de Queiroz
 * Autor: Lucas Correa Macedo
 */

#include <math.h>
#include <string.h>
#include <complex.h>

#include "const.h"
#include "types.h"
#include "functions.h"

/* 
 *  This function solves a linear system of equations with a number of variables equal to numVaribles
 *  systemMatrix must begin at [1][1]
 *  Returns singularSystem (1) if the system is singular 
 */
errorType
SolveSystem (double systemMatrix [MAX_NODES + 1][MAX_NODES + 2], unsigned numVariables)
{
	int i, j, l, a;
  double t, p;

  for (i = 1; i <= numVariables; i++) 
	{
  	t = 0.0;
    a = i;
    
    for (l = i; l <= numVariables; l++) 
		{
    	if (fabs (systemMatrix [l][i]) > fabs (t)) 
			{
				a = l;
				t = systemMatrix [l][i];
      }
    }
    
    if (i!=a) 
		{
      for (l = 1; l <= numVariables + 1; l++) 
			{
				p = systemMatrix [i][l];
				systemMatrix [i][l] = systemMatrix [a][l];
				systemMatrix [a][l] = p;
      }
    }
    
    if (fabs (t) < TOLG) 
      return singularSystem;
    
    for (j = numVariables + 1; j > 0; j--) 
		{
      systemMatrix [i][j] /= t;
      p = systemMatrix [i][j];
      if (p != 0)
        for (l = 1; l <= numVariables; l++) 
	  			if (l != i)
	    			systemMatrix [l][j] -= systemMatrix [l][i] * p;
    }
  }
  return ok;
}

/* 
 * This function returns, in nodeNum, the number of a node named nodeName. If nodeName is not on the list, this function assigns a number to it,
 * and updates the number of variables and the list of variables names
 * If the maximum number of nodes is exceeded, returns maxNodesExceeded (2)
 */
errorType
GetNodeNumber (char *nodeName, char listOfVariables [MAX_NODES + 1][MAX_NAME + 2], unsigned *numVariables, unsigned *nodeNum)
{
  unsigned i; 
	unsigned found; /* boolean */

  i = 0; 
	found = 0;
	
  while (!found && i <= (*numVariables))
    if (!(found = !strcmp (nodeName, listOfVariables [i]))) 
			i++;
			
  if (!found) /* nodeName not on the list */
	{
    if ((*numVariables) == MAX_NODES)
			return maxNodesExceeded;
     
    (*numVariables)++;
    strcpy (listOfVariables [(*numVariables)], nodeName);
    (*nodeNum) = (*numVariables);
  }
  
  else 
    (*nodeNum) = i;
    
  return ok;
}

/* Returns the conductance resulting from the linearization of a diode around the nodal voltages in currentSolution */
double
GetDiodeConductance (netlistElement diode, double *currentSolution)
{
	double voltage, conductance;
	
	voltage = (currentSolution [diode.a]) - (currentSolution [diode.b]);
	
	if (voltage > MAX_DIODE_VOLTAGE)
		voltage = MAX_DIODE_VOLTAGE;
		
	if (voltage < ((-1) * MAX_DIODE_INVERSE_VOLTAGE))
		voltage = (-1) * MAX_DIODE_INVERSE_VOLTAGE;
		
	conductance = (diode.iS / diode.vT) * (exp (voltage / diode.vT));
	
	return conductance;
}

/* Returns the current source value resulting from the linearization of a diode around the nodal voltages in currentSolution */
double
GetDiodeCurrentSourceValue (netlistElement diode, double conductance, double *currentSolution)
{
	double voltage, current;

	voltage = (currentSolution [diode.a]) - (currentSolution [diode.b]);
	
	if (voltage > MAX_DIODE_VOLTAGE)
		voltage = MAX_DIODE_VOLTAGE;
		
	if (voltage < ((-1) * MAX_DIODE_INVERSE_VOLTAGE))
		voltage = (-1) * MAX_DIODE_INVERSE_VOLTAGE;
		
	current = (diode.iS * (exp (voltage / diode.vT) - 1)) - (conductance * voltage);
	
	return current;
}

/* 
 * Searches the netlist for an element named elementName. If the element is found, this function returns its value in searchedElement, and returns ok
 * Otherwise, this function returns netlistElementNotFound 
 */
errorType
SearchNetlist (netlistElement *netlist, unsigned numElements, char *elementName, netlistElement *searchedElement)
{
	int index;
	
	for (index = 1; index <= numElements; index++)
	{
		if (strcmp (netlist [index].name, elementName) == 0)
		{
			(*searchedElement) = netlist [index];
			return ok;
		}
	}
	
	searchedElement = NULL;
	
	return netlistElementNotFound;
}

/* Returns the capacitance for the small signal model of a diode */
double
GetDiodeCapacitance (netlistElement diode, double *currentSolution)
{
	double voltage, capacitance, a, b;
	
	voltage = (currentSolution [diode.a]) - (currentSolution [diode.b]);
	
	if (voltage > (DIODE_PHI / 2))
		voltage = DIODE_PHI / 2;
		
	capacitance = diode.c0 / sqrt (1 - (voltage / DIODE_PHI));
	
	if (voltage > 0)
	{
		voltage = (currentSolution [diode.a]) - (currentSolution [diode.b]);
	
		if (voltage > MAX_DIODE_VOLTAGE)
		{
			a = ((diode.c1 / diode.vT) * (exp (MAX_DIODE_VOLTAGE / diode.vT)));
			b = (diode.c1 * (exp (MAX_DIODE_VOLTAGE / diode.vT) - 1)) - a * MAX_DIODE_VOLTAGE;
			capacitance += ((a * voltage) + b);
		}
		else
			capacitance += (diode.c1 * (exp (voltage / diode.vT) - 1));
	}
		
	return capacitance;
}

/* 
 *  This function solves a complex linear system of equations with a number of variables equal to numVaribles
 *  systemMatrix must begin at [1][1]
 *  Returns singularSystem (1) if the system is singular 
 */
errorType
SolveComplexSystem (double complex systemMatrix [MAX_NODES + 1][MAX_NODES + 2], unsigned numVariables)
{
	int i, j, l, a;
  double complex t, p;

  for (i = 1; i <= numVariables; i++) 
	{
  	t = 0.0;
    a = i;
    
    for (l = i; l <= numVariables; l++) 
		{
    	if (cabs (systemMatrix [l][i]) > cabs (t)) 
			{
				a = l;
				t = systemMatrix [l][i];
      }
    }
    
    if (i!=a) 
		{
      for (l = 1; l <= numVariables + 1; l++) 
			{
				p = systemMatrix [i][l];
				systemMatrix [i][l] = systemMatrix [a][l];
				systemMatrix [a][l] = p;
      }
    }
    
    if (cabs (t) < TOLG) 
      return singularSystem;
    
    for (j = numVariables + 1; j > 0; j--) 
		{
      systemMatrix [i][j] /= t;
      p = systemMatrix [i][j];
      if (p != 0)
        for (l = 1; l <= numVariables; l++) 
	  			if (l != i)
	    			systemMatrix [l][j] -= systemMatrix [l][i] * p;
    }
  }
  return ok;
}
