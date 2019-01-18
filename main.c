/*
 * Universidade Federal do Rio de Janeiro
 * Escola Politecnica
 * Departamento de Eletronica e Computacao
 * Circuitos Eletricos II - 2017.1
 * Professor Antonio Carlos M. de Queiroz
 * Autor: Lucas Correa Macedo
 */

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <complex.h>

#include "const.h"
#include "types.h"
#include "functions.h"

//#define DEBUG
//#define DEBUG2

int
main (void)
{	
	netlistElement netlist [MAX_ELEM]; /* Netlist */
	netlistElement tempElement, tempElement2;
	
	plotParameters plotPar;
	
	unsigned numElements, numVariables, numNodes, i, j, k, numNewtonRaphsonIterations, numNewtonRaphsonAttempts, temp;
	unsigned converged, variableConverged [MAX_NODES + 1], earlyEffect; /* Boolean */
	         
	char listOfVariables [MAX_NODES + 1][MAX_NAME + 2], fileName [MAX_LINE + 1], elementType,
  nodeA [MAX_NAME], nodeB [MAX_NAME], nodeC [MAX_NAME], nodeD [MAX_NAME], nodeE [MAX_NAME], buffer [MAX_LINE + 1], *p;

	FILE *file;

	double g, systemMatrix [MAX_NODES + 1][MAX_NODES + 2], currentSolution [MAX_NODES + 1],
	initialSystemMatrix [MAX_NODES + 1][MAX_NODES + 2], diodeCurrentSourceValue, emitterDiodeCurrentSourceValue, collectorDiodeCurrentSourceValue,
	emitterDiodeConductance, collectorDiodeConductance, g1, g2, g3, i0; 
	/* initialSystemMatrix is the system matrix with the stamps 
	   of elements that have the same stamp when calculating 
		 the operating point and the frequency response */
		 
	double complex complexSystemMatrix [MAX_NODES + 1][MAX_NODES + 2], sourcePhasor, complexImpedance;
	double *frequenciesToPlot, frequencyStep;
	unsigned numFrequencies, index;
	
	unsigned exponentFormat;
	
  /* Enable two-digit exponent format */
  exponentFormat = _set_output_format (_TWO_DIGIT_EXPONENT);

  /* Print header */
  system ("cls");
  printf ("Trabalho de Circuitos Eletricos II - 2017.1\n");
  printf ("Autor: Lucas Correa Macedo\n");
  
	/* Get file name and try to open it */
	do
	{
  	printf ("Nome do arquivo com o netlist: ");
  	scanf ("%50s", fileName);
  	file = fopen (fileName, "r");
  	if (file == 0) 
    	printf ("Arquivo %s inexistente\n", fileName);
	} while (file == 0);

	/* Initializing variables */
	numElements = 0; 
	numVariables = 0; 
	strcpy (listOfVariables [0], "0");
	
	/* Set default values for the plot parameters */
	strcpy (plotPar.scale, "DEC");
	plotPar.numPoints = DEFAULT_NUM_POINTS;
	plotPar.beginning = DEFAULT_INITIAL_FREQUENCY;
	plotPar.ending = DEFAULT_FINAL_FREQUENCY;
  
  /* Reading netlist */
  printf ("Lendo netlist:\n");
  fgets (buffer, MAX_LINE, file);
  printf ("Titulo: %s", buffer);
  
  while (fgets (buffer, MAX_LINE, file)) 
	{
    numElements++;
    
    if (numElements > MAX_ELEM) 
		{
      printf ("O programa so aceita ate %d elementos\n", MAX_ELEM);
      getch ();
      exit (maxElementsExceeded);
    }
    
    buffer [0] = toupper (buffer [0]);
    elementType = buffer [0];
    
    sscanf (buffer, "%10s", netlist [numElements].name);
    p = buffer + strlen (netlist [numElements].name);    /* Beginning of parameters */
    
    /* Reads parameters depending on element type */
    
    /* Resistor, inductor and capacitor */
    if (elementType == 'R' || elementType == 'L' || elementType == 'C') 
		{
      sscanf (p, "%10s%10s%lg", nodeA, nodeB, &netlist [numElements].value);
      printf ("%s %s %s %g\n", netlist [numElements].name, nodeA, nodeB, netlist [numElements].value);
      
      if (GetNodeNumber (nodeA, listOfVariables, &numVariables, &netlist [numElements].a))
      {
      	printf ("Numero maximo de nos (%d) excedido\n", MAX_NODES);
      	getch ();
      	exit (maxNodesExceeded);
			}
			
			if (GetNodeNumber (nodeB, listOfVariables, &numVariables, &netlist [numElements].b))
      {
      	printf ("Numero maximo de nos (%d) excedido\n", MAX_NODES);
      	getch ();
      	exit (maxNodesExceeded);
			}
    }
    
    /* Controled Sources */
    else if (elementType == 'G' || elementType == 'E' || elementType == 'F' || elementType == 'H') 
		{
      sscanf (p, "%10s%10s%10s%10s%lg", nodeA, nodeB, nodeC, nodeD, &netlist [numElements].value);
      printf ("%s %s %s %s %s %g\n", netlist [numElements].name, nodeA, nodeB, nodeC, nodeD, netlist [numElements].value);
      
      if (GetNodeNumber (nodeA, listOfVariables, &numVariables, &netlist [numElements].a))
      {
      	printf ("Numero maximo de nos (%d) excedido\n", MAX_NODES);
      	getch ();
      	exit (maxNodesExceeded);
			}
			
			if (GetNodeNumber (nodeB, listOfVariables, &numVariables, &netlist [numElements].b))
      {
      	printf ("Numero maximo de nos (%d) excedido\n", MAX_NODES);
      	getch ();
      	exit (maxNodesExceeded);
			}
			
			if (GetNodeNumber (nodeC, listOfVariables, &numVariables, &netlist [numElements].c))
      {
      	printf ("Numero maximo de nos (%d) excedido\n", MAX_NODES);
      	getch ();
      	exit (maxNodesExceeded);
			}
			
			if (GetNodeNumber (nodeD, listOfVariables, &numVariables, &netlist [numElements].d))
      {
      	printf ("Numero maximo de nos (%d) excedido\n", MAX_NODES);
      	getch ();
      	exit (maxNodesExceeded);
			}
    }
    
    /* Operational amplifier */
    else if (elementType == 'O') 
		{
      sscanf (p, "%10s%10s%10s%10s", nodeA, nodeB, nodeC, nodeD);
      printf ("%s %s %s %s %s\n", netlist [numElements].name, nodeA, nodeB, nodeC, nodeD);
      
      if (GetNodeNumber (nodeA, listOfVariables, &numVariables, &netlist [numElements].a))
      {
      	printf ("Numero maximo de nos (%d) excedido\n", MAX_NODES);
      	getch ();
      	exit (maxNodesExceeded);
			}
			
			if (GetNodeNumber (nodeB, listOfVariables, &numVariables, &netlist [numElements].b))
      {
      	printf ("Numero maximo de nos (%d) excedido\n", MAX_NODES);
      	getch ();
      	exit (maxNodesExceeded);
			}
			
			if (GetNodeNumber (nodeC, listOfVariables, &numVariables, &netlist [numElements].c))
      {
      	printf ("Numero maximo de nos (%d) excedido\n", MAX_NODES);
      	getch ();
      	exit (maxNodesExceeded);
			}
			
			if (GetNodeNumber (nodeD, listOfVariables, &numVariables, &netlist [numElements].d))
      {
      	printf ("Numero maximo de nos (%d) excedido\n", MAX_NODES);
      	getch ();
      	exit (maxNodesExceeded);
			}
    }
    
    /* Diode */
    else if (elementType == 'D')
    {
    	/* Set default values */
    	netlist [numElements].iS = DIODE_DEFAULT_IS_VALUE;
			netlist [numElements].vT = DIODE_DEFAULT_VT_VALUE; 
			netlist [numElements].c0 = DIODE_DEFAULT_C0_VALUE; 
			netlist [numElements].c1 = DIODE_DEFAULT_C1_VALUE;
    	
    	sscanf (p, "%10s%10s%lg%lg%lg%lg", nodeA, nodeB, &netlist [numElements].iS, &netlist [numElements].vT, &netlist [numElements].c0, &netlist [numElements].c1);
      printf ("%s %s %s %g %g %g %g\n", netlist [numElements].name, nodeA, nodeB, netlist [numElements].iS, netlist [numElements].vT, netlist [numElements].c0, netlist [numElements].c1);
      
      if (GetNodeNumber (nodeA, listOfVariables, &numVariables, &netlist [numElements].a))
      {
      	printf ("Numero maximo de nos (%d) excedido\n", MAX_NODES);
      	getch ();
      	exit (maxNodesExceeded);
			}
			
			if (GetNodeNumber (nodeB, listOfVariables, &numVariables, &netlist [numElements].b))
      {
      	printf ("Numero maximo de nos (%d) excedido\n", MAX_NODES);
      	getch ();
      	exit (maxNodesExceeded);
			}
		}
		
		/* Transistor */
		else if (elementType == 'Q')
    {
    	/* Set default values */
    	netlist [numElements].iSbe = DIODE_DEFAULT_IS_VALUE;
			netlist [numElements].vTbe = DIODE_DEFAULT_VT_VALUE; 
			netlist [numElements].c0be = DIODE_DEFAULT_C0_VALUE; 
			netlist [numElements].c1be = DIODE_DEFAULT_C1_VALUE;
			
			netlist [numElements].iSbc = DIODE_DEFAULT_IS_VALUE;
			netlist [numElements].vTbc = DIODE_DEFAULT_VT_VALUE; 
			netlist [numElements].c0bc = DIODE_DEFAULT_C0_VALUE; 
			netlist [numElements].c1bc = DIODE_DEFAULT_C1_VALUE;
			
			netlist [numElements].alpha = TRANSISTOR_DEFAULT_ALPHA_VALUE;
			netlist [numElements].alphaR = TRANSISTOR_DEFAULT_ALPHAR_VALUE;
			netlist [numElements].vA = TRANSISTOR_DEFAULT_VA_VALUE;
    	
    	sscanf (p, "%10s%10s%10s%10s%lg%lg%lg%lg%lg%lg%lg%lg%lg%lg%lg", nodeC, nodeB, nodeE, netlist [numElements].type, &netlist [numElements].alpha, &netlist [numElements].alphaR,
			        &netlist [numElements].iSbe, &netlist [numElements].vTbe, &netlist [numElements].iSbc, &netlist [numElements].vTbc, &netlist [numElements].vA,
			        &netlist [numElements].c0be, &netlist [numElements].c1be, &netlist [numElements].c0bc, &netlist [numElements].c1bc);
    	
      printf ("%s %s %s %s %s %g %g %g %g %g %g %g %g %g %g %g\n", netlist [numElements].name, nodeC, nodeB, nodeE, netlist [numElements].type, netlist [numElements].alpha,
			        netlist [numElements].alphaR, netlist [numElements].iSbe, netlist [numElements].vTbe, netlist [numElements].iSbc, netlist [numElements].vTbc, netlist [numElements].vA,
			        netlist [numElements].c0be, netlist [numElements].c1be, netlist [numElements].c0bc, netlist [numElements].c1bc);
      
      if (GetNodeNumber (nodeC, listOfVariables, &numVariables, &netlist [numElements].c))
      {
      	printf ("Numero maximo de nos (%d) excedido\n", MAX_NODES);
      	getch ();
      	exit (maxNodesExceeded);
			}
			
			if (GetNodeNumber (nodeB, listOfVariables, &numVariables, &netlist [numElements].b))
      {
      	printf ("Numero maximo de nos (%d) excedido\n", MAX_NODES);
      	getch ();
      	exit (maxNodesExceeded);
			}
			
			if (GetNodeNumber (nodeE, listOfVariables, &numVariables, &netlist [numElements].e))
      {
      	printf ("Numero maximo de nos (%d) excedido\n", MAX_NODES);
      	getch ();
      	exit (maxNodesExceeded);
			}
		} /* else if */
		
		/* Inductive coupling */
		else if (elementType == 'K')
		{
			sscanf (p, "%10s%10s%lg", netlist [numElements].inductorA, netlist [numElements].inductorB, &netlist [numElements].value);
      printf ("%s %s %s %g\n", netlist [numElements].name, netlist [numElements].inductorA, netlist [numElements].inductorB, netlist [numElements].value);
      
      /* Check if inductorA and inductorB are on the internal netlist */
      if (SearchNetlist (netlist, numElements, netlist [numElements].inductorA, &tempElement))
      {
      	printf ("O acoplamento entre indutores %s faz referencia a um indutor nao declarado %s\n", netlist [numElements].name, netlist [numElements].inductorA);
      	getch ();
      	exit (netlistElementNotFound);
			}
			
			if (SearchNetlist (netlist, numElements, netlist [numElements].inductorB, &tempElement))
      {
      	printf ("O acoplamento entre indutores %s faz referencia a um indutor nao declarado %s\n", netlist [numElements].name, netlist [numElements].inductorB);
      	getch ();
      	exit (netlistElementNotFound);
			}
		}
		
		/* Independent Sources */
		else if (elementType == 'I' || elementType == 'V')
		{
			sscanf (p, "%10s%10s%lg%lg%lg", nodeA, nodeB, &netlist [numElements].modulus, &netlist [numElements].phase, &netlist [numElements].value);
      printf ("%s %s %s %g %g %g\n", netlist [numElements].name, nodeA, nodeB, netlist [numElements].modulus, netlist [numElements].phase, netlist [numElements].value);
      
      if (GetNodeNumber (nodeA, listOfVariables, &numVariables, &netlist [numElements].a))
      {
      	printf ("Numero maximo de nos (%d) excedido\n", MAX_NODES);
      	getch ();
      	exit (maxNodesExceeded);
			}
			
			if (GetNodeNumber (nodeB, listOfVariables, &numVariables, &netlist [numElements].b))
      {
      	printf ("Numero maximo de nos (%d) excedido\n", MAX_NODES);
      	getch ();
      	exit (maxNodesExceeded);
			}
		}
		
    /* Comment */
    else if (elementType == '*')
		{
      printf ("Comentario: %s", buffer);
      numElements--;
    }
    
    /* Plot parameters */
    else if (elementType == '.')
		{	
      sscanf (p, "%10s%u%lg%lg", plotPar.scale, &plotPar.numPoints, &plotPar.beginning, &plotPar.ending);
      printf ("%s %s %u %g %g\n", netlist [numElements].name, plotPar.scale, plotPar.numPoints, plotPar.beginning, plotPar.ending);
      numElements--;
      
      /* Check if plotPar.scale has one of the recognized values */
			if ((strcmp (plotPar.scale, "LIN")) && (strcmp (plotPar.scale, "DEC")) && (strcmp (plotPar.scale, "OCT")))
			{
				printf ("Parametro invalido no netlist: %s", plotPar.scale);
				getch ();
				exit (invalidParameter);
			}
    }
    
    else 
		{
    	printf ("Elemento desconhecido: %s\n", buffer);
      getch ();
      exit (unknownElement);
    }
  } /* while */
  fclose (file);

  /* Adds current variables, updating the list of variables and the netlist */
  numNodes = numVariables;
  for (i = 1; i <= numElements; i++) 
	{
    elementType = netlist [i].name [0];
    
    if (elementType == 'V' || elementType == 'E' || elementType == 'F' || elementType == 'O' || elementType == 'L') 
		{
      numVariables++;
      
      if (numVariables > MAX_NODES) 
			{
        printf ("As correntes extra excederam o numero de variaveis permitido (%d)\n", MAX_NODES);
        exit (maxNodesExceeded);
      }
      
      strcpy (listOfVariables [numVariables], "j");
      strcat (listOfVariables [numVariables], netlist[i].name);
      netlist [i].x = numVariables;
    }
    
    else if (elementType == 'H') 
		{
      numVariables = numVariables + 2;
      
      if (numVariables > MAX_NODES) 
			{
        printf ("As correntes extra excederam o numero de variaveis permitido (%d)\n", MAX_NODES);
        exit (maxNodesExceeded);
      }
      
      strcpy (listOfVariables [numVariables - 1], "jx"); 
			strcat (listOfVariables [numVariables - 1], netlist [i].name);
      netlist [i].x = numVariables - 1;
      
      strcpy (listOfVariables [numVariables], "jy"); 
			strcat (listOfVariables [numVariables], netlist [i].name);
      netlist [i].y = numVariables;
    }
    
    else if (elementType == 'K')
    {
    	/* Get inductors */
    	SearchNetlist (netlist, numElements, netlist [i].inductorA, &tempElement);
    	SearchNetlist (netlist, numElements, netlist [i].inductorB, &tempElement2);
    	
    	if (tempElement.x < tempElement2.x)
    	{
    		netlist [i].x = tempElement.x;
    		netlist [i].y = tempElement2.x;
			}
			
			else
			{
				netlist [i].x = tempElement2.x;
    		netlist [i].y = tempElement.x;
			}
			
			netlist [i].value = netlist [i].value * sqrt (tempElement.value * tempElement2.value); /* M = k * sqrt (L1 * L2) */
		}
  } /* for */
  getch ();
  
  /* Print the internal variables */
  printf ("Variaveis: \n");
  for (i = 0; i <= numVariables; i++)
    printf("%d: %s\n", i, listOfVariables [i]);
  getch ();
  
  //******************************************************************************** 
	//                           Creating initial system matrix
	//********************************************************************************
  
  /* Modified nodal analysis */
  printf ("O circuito tem %d nos, %d variaveis e %d elementos\n", numNodes, numVariables, numElements);
  getch ();
  
  /* Initializing system with zeroes */
  for (i = 0; i <= numVariables; i++)
	  for (j = 0; j <= numVariables + 1; j++)
      systemMatrix [i][j] = 0;
  
  /* Adding element stamps */
  for (i = 1; i <= numElements; i++) 
	{
    elementType = netlist [i].name [0];
    
    if (elementType == 'R') 
		{
      g = (1 / netlist [i].value);
      systemMatrix [netlist [i].a][netlist [i].a]+=g;
      systemMatrix [netlist [i].b][netlist [i].b]+=g;
      systemMatrix [netlist [i].a][netlist [i].b]-=g;
      systemMatrix [netlist [i].b][netlist [i].a]-=g;
    }
    
    else if (elementType == 'G') 
		{
      g = netlist [i].value;
      systemMatrix [netlist [i].a][netlist [i].c]+=g;
      systemMatrix [netlist [i].b][netlist [i].d]+=g;
      systemMatrix [netlist [i].a][netlist [i].d]-=g;
      systemMatrix [netlist [i].b][netlist [i].c]-=g;
    }
    
    else if (elementType == 'E') 
		{
      g = netlist [i].value;
      systemMatrix [netlist [i].a][netlist [i].x]+=1;
      systemMatrix [netlist [i].b][netlist [i].x]-=1;
      systemMatrix [netlist [i].x][netlist [i].a]-=1;
      systemMatrix [netlist [i].x][netlist [i].b]+=1;
      systemMatrix [netlist [i].x][netlist [i].c]+=g;
      systemMatrix [netlist [i].x][netlist [i].d]-=g;
    }
    
    else if (elementType == 'F') 
		{
      g = netlist [i].value;
      systemMatrix [netlist [i].a][netlist [i].x]+=g;
      systemMatrix [netlist [i].b][netlist [i].x]-=g;
      systemMatrix [netlist [i].c][netlist [i].x]+=1;
      systemMatrix [netlist [i].d][netlist [i].x]-=1;
      systemMatrix [netlist [i].x][netlist [i].c]-=1;
      systemMatrix [netlist [i].x][netlist [i].d]+=1;
    }
    
    else if (elementType == 'H') 
		{
      g = netlist [i].value;
      systemMatrix [netlist [i].a][netlist [i].y]+=1;
      systemMatrix [netlist [i].b][netlist [i].y]-=1;
      systemMatrix [netlist [i].c][netlist [i].x]+=1;
      systemMatrix [netlist [i].d][netlist [i].x]-=1;
      systemMatrix [netlist [i].y][netlist [i].a]-=1;
      systemMatrix [netlist [i].y][netlist [i].b]+=1;
      systemMatrix [netlist [i].x][netlist [i].c]-=1;
      systemMatrix [netlist [i].x][netlist [i].d]+=1;
      systemMatrix [netlist [i].y][netlist [i].x]+=g;
    }
    
    else if (elementType == 'O') 
		{
      systemMatrix [netlist [i].a][netlist [i].x]+=1;
      systemMatrix [netlist [i].b][netlist [i].x]-=1;
      systemMatrix [netlist [i].x][netlist [i].c]+=1;
      systemMatrix [netlist [i].x][netlist [i].d]-=1;
    }
  } /* for */
  
  #ifdef DEBUG
    /* Prints the initial system */
    printf ("Sistema inicial:\n");
    for (k = 1; k <= numVariables; k++) 
		{
    	for (j = 1; j <= numVariables + 1; j++)
    	{
     		if (systemMatrix [k][j] != 0) 
					printf( "%+3.1f ", systemMatrix [k][j]);
     		else 
					printf (" ... ");
    	}
    	printf ("\n");
    }
    getch ();
	#endif
  
  //********************************************************************************
  //                             Operating point
  //********************************************************************************
  
	/* Copy systemMatrix to linerSystemMatrix, as it'll be used again in each iteration */
	for (i = 1; i <= numVariables; i++)
		for (j = 1; j <= numVariables + 1; j++)
			initialSystemMatrix [i][j] = systemMatrix [i][j];
  			
 	/* Initial aproximation */
	for (i = 1; i <= numVariables; i++)
		currentSolution [i] = NR_INITIAL_APROX;
  		
	converged = 0; /* false */
	for (i = 1; i <= numVariables; i++)
		variableConverged [i] = 0;
		
	numNewtonRaphsonAttempts = 0;
	srand ((unsigned) time (NULL));
 	
	for (numNewtonRaphsonIterations = 0; (converged != 1) && (numNewtonRaphsonAttempts < MAX_NR_ATTEMPTS); numNewtonRaphsonIterations++)
	{	
		if (numNewtonRaphsonIterations > MAX_NR_ITERATIONS)
		{
			numNewtonRaphsonIterations = 0;
			numNewtonRaphsonAttempts++;
  			
			/* Try random values for the variables that didn't converge */
			for (i = 1; i <= numVariables; i++)
				if (variableConverged [i] == 0) 
					currentSolution [i] = (rand () % MAX_RANDOM_VALUE);
		}
			
		/* Copy the solution in systemMatrix to currentSolution, unless it is the first iteration */
		if (numNewtonRaphsonIterations != 0)
			for (i = 1; i <= numVariables; i++)
				currentSolution [i] = systemMatrix [i][numVariables + 1];
  				
		#ifdef DEBUG /* Print current solution */
		  printf ("\nCurrent solution #%d\n", numNewtonRaphsonIterations);
		  for (i = 1; i <= numVariables; i++)
				printf ("%u ", variableConverged [i]);
			printf ("\n");
			for (i = 1; i <= numVariables; i++)
				printf ("%g ", currentSolution [i]);
			printf ("\n");
		#endif
		  			
		/* Copy initialSystemMatrix to systemMatrix */
		for (i = 1; i <= numVariables; i++)
			for (j = 1; j <= numVariables + 1; j++)
				systemMatrix [i][j] = initialSystemMatrix [i][j];
  				
		/* Add element stamps */
		for (i = 1; i <= numElements; i++) 
		{
  		elementType = netlist [i].name [0];
      
			/* Diode */
  		if (elementType == 'D')
			{
    		/* Conductance */
    		g = GetDiodeConductance (netlist [i], currentSolution);
    		systemMatrix [netlist [i].a][netlist [i].a]+=g;
    		systemMatrix [netlist [i].b][netlist [i].b]+=g;
    		systemMatrix [netlist [i].a][netlist [i].b]-=g;
    		systemMatrix [netlist [i].b][netlist [i].a]-=g;
     	
    	  /* Current source */
    		diodeCurrentSourceValue = GetDiodeCurrentSourceValue (netlist [i], g, currentSolution);
	  		systemMatrix [netlist [i].a][numVariables + 1]-=diodeCurrentSourceValue;
    		systemMatrix [netlist [i].b][numVariables + 1]+=diodeCurrentSourceValue;
  		}
  		
  		/* Current source */
  		else if (elementType == 'I') 
			{
      	g = netlist [i].value;
      	systemMatrix [netlist [i].a][numVariables + 1]-=g;
      	systemMatrix [netlist [i].b][numVariables + 1]+=g;
    	}
    
    	/* Voltage source */
    	else if (elementType == 'V') 
			{
      	systemMatrix [netlist [i].a][netlist [i].x]+=1;
      	systemMatrix [netlist [i].b][netlist [i].x]-=1;
      	systemMatrix [netlist [i].x][netlist [i].a]-=1;
      	systemMatrix [netlist [i].x][netlist [i].b]+=1;
      	systemMatrix [netlist [i].x][numVariables + 1]-=netlist [i].value;
    	}
  		
  		/* Capacitor */
  		else if (elementType == 'C') 
			{
      	g = (1 / CAPACITOR_OPERATING_POINT_RESISTANCE);
      	systemMatrix [netlist [i].a][netlist [i].a]+=g;
      	systemMatrix [netlist [i].b][netlist [i].b]+=g;
      	systemMatrix [netlist [i].a][netlist [i].b]-=g;
      	systemMatrix [netlist [i].b][netlist [i].a]-=g;
    	}
    	
  		/* Inductor */
  		else if (elementType == 'L') 
			{
      	g = INDUCTOR_OPERATING_POINT_RESISTANCE;
      	systemMatrix [netlist [i].x][netlist [i].x]+=g;
      	
      	systemMatrix [netlist [i].a][netlist [i].x]+=1;
      	systemMatrix [netlist [i].b][netlist [i].x]-=1;
      	systemMatrix [netlist [i].x][netlist [i].a]-=1;
      	systemMatrix [netlist [i].x][netlist [i].b]+=1;
    	}
    	
  		/* Transistor */
    	else if (elementType == 'Q')
			{
				/* BC Diode */
				tempElement.a = netlist [i].b;
				tempElement.b = netlist [i].c;
				tempElement.iS = netlist [i].iSbc;
				tempElement.vT = netlist [i].vTbc;
				
				if (strcmp (netlist [i].type, "PNP") == 0) /* If the transistor is PNP */
				{
					tempElement.iS*=(-1);
					tempElement.vT*=(-1);
				}
				
    		collectorDiodeConductance = GetDiodeConductance (tempElement, currentSolution);    		
    		systemMatrix [tempElement.a][tempElement.a]+=collectorDiodeConductance;
    		systemMatrix [tempElement.b][tempElement.b]+=collectorDiodeConductance;
    		systemMatrix [tempElement.a][tempElement.b]-=collectorDiodeConductance;
    		systemMatrix [tempElement.b][tempElement.a]-=collectorDiodeConductance;
    		
				systemMatrix [tempElement.a][tempElement.a]+=DIODE_PARALLEL_CONDUCTANCE;
    		systemMatrix [tempElement.b][tempElement.b]+=DIODE_PARALLEL_CONDUCTANCE;
    		systemMatrix [tempElement.a][tempElement.b]-=DIODE_PARALLEL_CONDUCTANCE;
    		systemMatrix [tempElement.b][tempElement.a]-=DIODE_PARALLEL_CONDUCTANCE;
				     	
    		collectorDiodeCurrentSourceValue = GetDiodeCurrentSourceValue (tempElement, collectorDiodeConductance, currentSolution);
	  		systemMatrix [tempElement.a][numVariables + 1]-=collectorDiodeCurrentSourceValue;
    		systemMatrix [tempElement.b][numVariables + 1]+=collectorDiodeCurrentSourceValue;
    		
    		/* BE Current Sources */
    		systemMatrix [netlist [i].e][numVariables + 1]-=(netlist [i].alphaR * collectorDiodeCurrentSourceValue);
      	systemMatrix [netlist [i].b][numVariables + 1]+=(netlist [i].alphaR * collectorDiodeCurrentSourceValue);
      	
      	systemMatrix [netlist [i].e][netlist [i].b]+=(netlist [i].alphaR * collectorDiodeConductance);
      	systemMatrix [netlist [i].b][netlist [i].c]+=(netlist [i].alphaR * collectorDiodeConductance);
      	systemMatrix [netlist [i].e][netlist [i].c]-=(netlist [i].alphaR * collectorDiodeConductance);
      	systemMatrix [netlist [i].b][netlist [i].b]-=(netlist [i].alphaR * collectorDiodeConductance);
    		
    		/* BE Diode */
    		tempElement.a = netlist [i].b;
				tempElement.b = netlist [i].e;
				tempElement.iS = netlist [i].iSbe;
				tempElement.vT = netlist [i].vTbe;
				
				if (strcmp (netlist [i].type, "PNP") == 0) /* If the transistor is PNP */
				{
					tempElement.iS*=(-1);
					tempElement.vT*=(-1);
				}
				
    		emitterDiodeConductance = GetDiodeConductance (tempElement, currentSolution);
    		systemMatrix [tempElement.a][tempElement.a]+=emitterDiodeConductance;
    		systemMatrix [tempElement.b][tempElement.b]+=emitterDiodeConductance;
    		systemMatrix [tempElement.a][tempElement.b]-=emitterDiodeConductance;
    		systemMatrix [tempElement.b][tempElement.a]-=emitterDiodeConductance;
    		
    	  systemMatrix [tempElement.a][tempElement.a]+=DIODE_PARALLEL_CONDUCTANCE;
    		systemMatrix [tempElement.b][tempElement.b]+=DIODE_PARALLEL_CONDUCTANCE;
    		systemMatrix [tempElement.a][tempElement.b]-=DIODE_PARALLEL_CONDUCTANCE;
    		systemMatrix [tempElement.b][tempElement.a]-=DIODE_PARALLEL_CONDUCTANCE;
				    		
    		emitterDiodeCurrentSourceValue = GetDiodeCurrentSourceValue (tempElement, emitterDiodeConductance, currentSolution);
	  		systemMatrix [tempElement.a][numVariables + 1]-=emitterDiodeCurrentSourceValue;
    		systemMatrix [tempElement.b][numVariables + 1]+=emitterDiodeCurrentSourceValue;	
    		
    		/* BC Current Sources */
    		systemMatrix [netlist [i].c][numVariables + 1]-=(netlist [i].alpha * emitterDiodeCurrentSourceValue);
      	systemMatrix [netlist [i].b][numVariables + 1]+=(netlist [i].alpha * emitterDiodeCurrentSourceValue);
      	
      	systemMatrix [netlist [i].c][netlist [i].b]+=(netlist [i].alpha * emitterDiodeConductance);
      	systemMatrix [netlist [i].b][netlist [i].e]+=(netlist [i].alpha * emitterDiodeConductance);
      	systemMatrix [netlist [i].c][netlist [i].e]-=(netlist [i].alpha * emitterDiodeConductance);
      	systemMatrix [netlist [i].b][netlist [i].b]-=(netlist [i].alpha * emitterDiodeConductance);
      	
      	/* Early effect */
      	earlyEffect = 0;
      	if (!(strcmp (netlist [i].type, "NPN")) && (currentSolution [netlist [i].c] > currentSolution [netlist [i].e]))
      		earlyEffect = 1;
				if (!(strcmp (netlist [i].type, "PNP")) && (currentSolution [netlist [i].c] < currentSolution [netlist [i].e]))
					earlyEffect = 1;
				
				if (earlyEffect)
				{  
      		g1 = (netlist [i].alpha * emitterDiodeConductance * (currentSolution [netlist [i].c] - currentSolution [netlist [i].e]) / netlist [i].vA);
      		g2 = ((-1) * collectorDiodeConductance * (currentSolution [netlist [i].c] - currentSolution [netlist [i].e]) / netlist [i].vA);
      		g3 = ((netlist [i].alpha * (emitterDiodeConductance * (currentSolution [netlist [i].b] - currentSolution [netlist [i].e]) + emitterDiodeCurrentSourceValue)) - 
					((collectorDiodeConductance * (currentSolution [netlist [i].b] - currentSolution [netlist [i].c])) + collectorDiodeCurrentSourceValue)) / (netlist [i].vA);
				
					i0 = (-1) * (g1 * (currentSolution [netlist [i].b] - currentSolution [netlist [i].e]) + g2 * (currentSolution [netlist [i].b] - currentSolution [netlist [i].c]));
					
					if (!(strcmp (netlist [i].type, "PNP")))
      		{
      			g1*=(-1);
      			g2*=(-1);
      			g3*=(-1);
      			i0*=(-1);
					}
					
					systemMatrix [netlist [i].c][netlist [i].b]+=g1;
      		systemMatrix [netlist [i].e][netlist [i].e]+=g1;
      		systemMatrix [netlist [i].c][netlist [i].e]-=g1;
      		systemMatrix [netlist [i].e][netlist [i].b]-=g1;
      	
      		systemMatrix [netlist [i].c][netlist [i].b]+=g2;
      		systemMatrix [netlist [i].e][netlist [i].c]+=g2;
      		systemMatrix [netlist [i].c][netlist [i].c]-=g2;
      		systemMatrix [netlist [i].e][netlist [i].b]-=g2;
      	
      		systemMatrix [netlist [i].c][netlist [i].c]+=g3;
      		systemMatrix [netlist [i].e][netlist [i].e]+=g3;
      		systemMatrix [netlist [i].c][netlist [i].e]-=g3;
      		systemMatrix [netlist [i].e][netlist [i].c]-=g3;
      	
      		systemMatrix [netlist [i].c][numVariables + 1]-=i0;
      		systemMatrix [netlist [i].e][numVariables + 1]+=i0;
      	}
			} /* else if */
			 
  		/* Print system matrix */
  		#ifdef DEBUG 
  			if (elementType == 'Q' || elementType == 'L' || elementType == 'C' || elementType == 'V' || elementType == 'I' || elementType == 'D')
  			{
					printf ("\nSistema apos a montagem de %s (n=%u)\n", netlist [i].name, numNewtonRaphsonIterations);
					for (k = 1; k <= numVariables; k++) 
					{
    				for (j = 1; j <= numVariables + 1; j++)
    				{
     					if (systemMatrix [k][j] != 0)
    						printf ("%+1.2e ", systemMatrix [k][j]);
     					else 
								printf (" ......... ");
    				}
    				printf ("\n");
    			}
    		}
			#endif
		} /* for */		
  		
		/* Solve system */
		if (SolveSystem (systemMatrix, numVariables)) 
		{
			numNewtonRaphsonIterations = MAX_NR_ITERATIONS + 1; /* Forces it to try new initial values */
  		continue;
		}
  		
		/* Compare currentSolution with the next solution in systemMatrix */
		converged = 1; /* Assuming it has converged */
		for (i = 1; (i <= numVariables); i++)
		{
		  if (fabs (currentSolution [i] - systemMatrix [i][numVariables + 1]) > NR_TOLERANCE)
		  {
		  	converged = 0; /* The system hasn't converged */
		  	variableConverged [i] = 0;
		  }
		  
		  else
		  {
		  	variableConverged [i] = 1;
		  }
		}
	} /* for */		
		
	if (!converged)
	{
		printf ("Nao convergiu\n");
		getch ();
		exit (doesNotConverge);
	}
	
	printf ("\nNumero de iteracoes: %u, Reinicios: %u\n", numNewtonRaphsonIterations, numNewtonRaphsonAttempts);
	
	/* Print operating point */
	printf ("\nPonto de Operacao:\n");
	strcpy (buffer, "Tensao");
	for (i = 1; i <= numVariables; i++) 
	{
  	if (i == numNodes + 1) 
			strcpy (buffer, "Corrente");
  	printf ("%s %s: %g\n", buffer, listOfVariables [i], systemMatrix [i][numVariables + 1]);
	}		
	getch ();
	
	/* Copy the solution in systemMatrix to currentSolution, which will be used to calculate the frequency response */
	for (i = 1; i <= numVariables; i++)
		currentSolution [i] = systemMatrix [i][numVariables + 1];
	
	/* Print transistor and diode states */
	for (i = 1; i <= numElements; i++) 
	{
		elementType = netlist [i].name [0];
		
		if (elementType == 'D')
			printf ("%s: Vd=%g Gd=%g Cdtotal=%g\n", netlist [i].name, (currentSolution [netlist[i].a] - currentSolution [netlist[i].b]), 
			GetDiodeConductance (netlist[i], currentSolution), GetDiodeCapacitance (netlist [i], currentSolution));
			
		else if (elementType == 'Q')
		{
			/* BE Diode */
			tempElement.a = netlist [i].b;
			tempElement.b = netlist [i].e;
			tempElement.iS = netlist [i].iSbe;
			tempElement.vT = netlist [i].vTbe;
			tempElement.c0 = netlist [i].c0be;
			tempElement.c1 = netlist [i].c1be;
				
			/* BC Diode */
    	tempElement2.a = netlist [i].b;
			tempElement2.b = netlist [i].c;
			tempElement2.iS = netlist [i].iSbc;
			tempElement2.vT = netlist [i].vTbc;
			tempElement2.c0 = netlist [i].c0bc;
			tempElement2.c1 = netlist [i].c1bc;
				
			if (strcmp (netlist [i].type, "PNP") == 0) /* If the transistor is PNP */
			{
				temp = tempElement.a;
				tempElement.a = tempElement.b;
				tempElement.b = temp;
				
				temp = tempElement2.a;
				tempElement2.a = tempElement2.b;
				tempElement2.b = temp;
			}
			
			printf ("%s: Vbe=%g Vbc=%g Vce=%g Cbe=%g Cbc=%g\n", netlist [i].name, (currentSolution [netlist[i].b] - currentSolution [netlist[i].e]),
			(currentSolution [netlist[i].b] - currentSolution [netlist[i].c]), (currentSolution [netlist[i].c] - currentSolution [netlist[i].e]),
			GetDiodeCapacitance (tempElement, currentSolution), GetDiodeCapacitance (tempElement2, currentSolution));
		}
  }
	
	//********************************************************************************
  //                             Frequency response
  //********************************************************************************
	
	/* Calculate how many and which frequencies will be plotted */
	if (strcmp (plotPar.scale, "LIN") == 0)
	{
		numFrequencies = plotPar.numPoints;
		frequencyStep = (plotPar.ending - plotPar.beginning) / (plotPar.numPoints - 1);
		frequenciesToPlot = (double *) malloc (plotPar.numPoints * sizeof (double));
		for (i = 0; i < plotPar.numPoints; i++)
		{
			frequenciesToPlot [i] = plotPar.beginning + (i * frequencyStep);
		}
	}
	
	else if  (strcmp (plotPar.scale, "OCT") == 0)
	{
		numFrequencies = (log2 (plotPar.ending / plotPar.beginning)) * (plotPar.numPoints - 1) + 1;
		frequenciesToPlot = (double *) malloc (numFrequencies * sizeof (double));
		frequencyStep = pow (2, (1.0 / (plotPar.numPoints - 1)));
		for (i = 0; i < numFrequencies; i++)
		{
			frequenciesToPlot [i] = plotPar.beginning * pow (frequencyStep, i);
		}
	}
		
	else if  (strcmp (plotPar.scale, "DEC") == 0)
	{
		numFrequencies = (log10 (plotPar.ending / plotPar.beginning)) * (plotPar.numPoints - 1) + 1;
		frequenciesToPlot = (double *) malloc (numFrequencies * sizeof (double));
		frequencyStep = pow (10, (1.0 / (plotPar.numPoints - 1)));
		for (i = 0; i < numFrequencies; i++)
		{
			frequenciesToPlot [i] = plotPar.beginning * pow (frequencyStep, i);
		}
	}
	
	/* Print list of frequencies to plot */
	#ifdef DEBUG2
		printf ("Numero de pontos: %u\n", numFrequencies);
		for (i = 0; i < numFrequencies; i++)
			printf ("%g\n", frequenciesToPlot [i]);
	#endif
	
	/* Change file name */
	for (i = 0; i < strlen (fileName); i++)
		if (fileName [i] == '.')
			break;
	
	fileName [i + 1] = 't';
	fileName [i + 2] = 'a';
	fileName [i + 3] = 'b';
	fileName [i + 4] = '\0';
	
	file = fopen (fileName, "w");
  if (file == 0) 
  {
  	printf ("Erro abrindo arquivo %s\n", fileName);
  	exit (errorOpeningFile);
  }
  
  /* Print header in file */
	fprintf (file, "f ");
	for (i = 1; i <= numVariables; i++)
		fprintf (file, "%sm %sf ", listOfVariables [i], listOfVariables [i]);
	fprintf (file, "\n");
	
	/* Loop calculating the frequency response for each frequency in frequenciesToPlot */
	for (index = 0; index < numFrequencies; index++)
	{
		/* Copy initialSystemMatrix to complexSystemMatrix */
		for (i = 1; i <= numVariables; i++)
				for (j = 1; j <= numVariables + 1; j++)
					complexSystemMatrix [i][j] = initialSystemMatrix [i][j];
  		
		/* Add element stamps to complexSystemMatrix */
		for (i = 1; i <= numElements; i++) 
		{
  		elementType = netlist [i].name [0];
  	
  		/* Diode */
  		if (elementType == 'D')
			{
    		/* Conductance */
    		g = GetDiodeConductance (netlist [i], currentSolution);
    		complexSystemMatrix [netlist [i].a][netlist [i].a]+=g;
    		complexSystemMatrix [netlist [i].b][netlist [i].b]+=g;
    		complexSystemMatrix [netlist [i].a][netlist [i].b]-=g;
    		complexSystemMatrix [netlist [i].b][netlist [i].a]-=g;
    		
    		complexSystemMatrix [netlist [i].a][netlist [i].a]+=DIODE_PARALLEL_CONDUCTANCE;
  			complexSystemMatrix [netlist [i].b][netlist [i].b]+=DIODE_PARALLEL_CONDUCTANCE;
  			complexSystemMatrix [netlist [i].a][netlist [i].b]-=DIODE_PARALLEL_CONDUCTANCE;
  			complexSystemMatrix [netlist [i].b][netlist [i].a]-=DIODE_PARALLEL_CONDUCTANCE;
    		
    		/* Capacitance */
    		complexImpedance = I * 2 * M_PI * frequenciesToPlot [index] * GetDiodeCapacitance (netlist [i], currentSolution);
    		complexSystemMatrix [netlist [i].a][netlist [i].a]+=complexImpedance;
    		complexSystemMatrix [netlist [i].b][netlist [i].b]+=complexImpedance;
    		complexSystemMatrix [netlist [i].a][netlist [i].b]-=complexImpedance;
    		complexSystemMatrix [netlist [i].b][netlist [i].a]-=complexImpedance;
  		}
  		
  		/* Current source */
  		else if (elementType == 'I') 
			{
    		sourcePhasor = netlist [i].modulus * cexp ((netlist [i].phase * (M_PI / 180)) * I);
				complexSystemMatrix [netlist [i].a][numVariables + 1]-=sourcePhasor;
    		complexSystemMatrix [netlist [i].b][numVariables + 1]+=sourcePhasor;
    	}
    
			/* Voltage source */
    	else if (elementType == 'V') 
			{
				sourcePhasor = netlist [i].modulus * cexp ((netlist [i].phase * (M_PI / 180)) * I);
    		complexSystemMatrix [netlist [i].a][netlist [i].x]+=1;
    		complexSystemMatrix [netlist [i].b][netlist [i].x]-=1;
    		complexSystemMatrix [netlist [i].x][netlist [i].a]-=1;
    		complexSystemMatrix [netlist [i].x][netlist [i].b]+=1;
    		complexSystemMatrix [netlist [i].x][numVariables + 1]-=sourcePhasor;
    	}
  		
  		/* Capacitor */
  		else if (elementType == 'C') 
			{
    		complexImpedance = I * netlist [i].value * 2 * M_PI * frequenciesToPlot [index];
    		complexSystemMatrix [netlist [i].a][netlist [i].a]+=complexImpedance;
    		complexSystemMatrix [netlist [i].b][netlist [i].b]+=complexImpedance;
    		complexSystemMatrix [netlist [i].a][netlist [i].b]-=complexImpedance;
    		complexSystemMatrix [netlist [i].b][netlist [i].a]-=complexImpedance;
    	}
    	
  		/* Inductor */
  		else if (elementType == 'L') 
			{
    		complexImpedance = I * netlist [i].value * 2 * M_PI * frequenciesToPlot [index];
    		complexSystemMatrix [netlist [i].x][netlist [i].x]+=complexImpedance;
     	
    		complexSystemMatrix [netlist [i].a][netlist [i].x]+=1;
    		complexSystemMatrix [netlist [i].b][netlist [i].x]-=1;
    		complexSystemMatrix [netlist [i].x][netlist [i].a]-=1;
    		complexSystemMatrix [netlist [i].x][netlist [i].b]+=1;
    	}
    
    	/* Inductive coupling */
    	else if (elementType == 'K')
    	{
    		complexSystemMatrix [netlist [i].x][netlist [i].y]+= I * 2 * M_PI * frequenciesToPlot [index] * netlist [i].value;
    		complexSystemMatrix [netlist [i].y][netlist [i].x]+= I * 2 * M_PI * frequenciesToPlot [index] * netlist [i].value;
			}
		
			/* Transistor */
			else if (elementType == 'Q')
    	{	
    		/* BC Diode */
				tempElement.a = netlist [i].b;
				tempElement.b = netlist [i].c;
				tempElement.iS = netlist [i].iSbc;
				tempElement.vT = netlist [i].vTbc;
				tempElement.c0 = netlist [i].c0be;
				tempElement.c1 = netlist [i].c1be;
				
				if (strcmp (netlist [i].type, "PNP") == 0) /* If the transistor is PNP */
				{
					tempElement.iS*=(-1);
					tempElement.vT*=(-1);
				}
				
				/* BC Diode Conductance */
				collectorDiodeConductance = (tempElement.iS / tempElement.vT) * (exp ((currentSolution [tempElement.a] - currentSolution [tempElement.b]) / tempElement.vT));
    		
    		if ((strcmp (netlist [i].type, "NPN") == 0) && (currentSolution [tempElement.a] - currentSolution [tempElement.b] > MAX_DIODE_VOLTAGE))
    			collectorDiodeConductance = (tempElement.iS / tempElement.vT) * (exp ((MAX_DIODE_VOLTAGE) / tempElement.vT));
    			
    		if ((strcmp (netlist [i].type, "PNP") == 0) && (currentSolution [tempElement.a] - currentSolution [tempElement.b] < ((-1) * MAX_DIODE_VOLTAGE))) 
    			collectorDiodeConductance = (tempElement.iS / tempElement.vT) * (exp (((-1) * MAX_DIODE_VOLTAGE) / tempElement.vT));
    		
    		complexSystemMatrix [tempElement.a][tempElement.a]+=collectorDiodeConductance;
    		complexSystemMatrix [tempElement.b][tempElement.b]+=collectorDiodeConductance;
    		complexSystemMatrix [tempElement.a][tempElement.b]-=collectorDiodeConductance;
    		complexSystemMatrix [tempElement.b][tempElement.a]-=collectorDiodeConductance;
    		
    		/* BE Current Source */
      	complexSystemMatrix [netlist [i].e][netlist [i].b]+=(netlist [i].alphaR * collectorDiodeConductance);
      	complexSystemMatrix [netlist [i].b][netlist [i].c]+=(netlist [i].alphaR * collectorDiodeConductance);
      	complexSystemMatrix [netlist [i].e][netlist [i].c]-=(netlist [i].alphaR * collectorDiodeConductance);
      	complexSystemMatrix [netlist [i].b][netlist [i].b]-=(netlist [i].alphaR * collectorDiodeConductance);
      	
      	if (strcmp (netlist [i].type, "PNP") == 0) /* If the transistor is PNP */
				{
					tempElement.iS*=(-1);
					tempElement.vT*=(-1);
					temp = tempElement.a;
					tempElement.a = tempElement.b;
					tempElement.b = temp;
				}
    		
    		/* BC Diode Capacitance */
    		complexImpedance = I * 2 * M_PI * frequenciesToPlot [index] * GetDiodeCapacitance (tempElement, currentSolution);
    		complexSystemMatrix [tempElement.a][tempElement.a]+=complexImpedance;
    		complexSystemMatrix [tempElement.b][tempElement.b]+=complexImpedance;
    		complexSystemMatrix [tempElement.a][tempElement.b]-=complexImpedance;
    		complexSystemMatrix [tempElement.b][tempElement.a]-=complexImpedance;
    		
    		/* BE Diode */
    		tempElement.a = netlist [i].b;
				tempElement.b = netlist [i].e;
				tempElement.iS = netlist [i].iSbe;
				tempElement.vT = netlist [i].vTbe;
				tempElement.c0 = netlist [i].c0be;
				tempElement.c1 = netlist [i].c1be;
				
				if (strcmp (netlist [i].type, "PNP") == 0) /* If the transistor is PNP */
				{
					tempElement.iS*=(-1);
					tempElement.vT*=(-1);
				}
				
				/* BE Diode Conductance */
    		emitterDiodeConductance = (tempElement.iS / tempElement.vT) * (exp ((currentSolution [tempElement.a] - currentSolution [tempElement.b]) / tempElement.vT));
    		
    		if ((strcmp (netlist [i].type, "NPN") == 0) && (currentSolution [tempElement.a] - currentSolution [tempElement.b] > MAX_DIODE_VOLTAGE))
    			emitterDiodeConductance = (tempElement.iS / tempElement.vT) * (exp ((MAX_DIODE_VOLTAGE) / tempElement.vT));
    		
    		if ((strcmp (netlist [i].type, "PNP") == 0) && (currentSolution [tempElement.a] - currentSolution [tempElement.b] < ((-1) * MAX_DIODE_VOLTAGE))) 
    			emitterDiodeConductance = (tempElement.iS / tempElement.vT) * (exp (((-1) * MAX_DIODE_VOLTAGE) / tempElement.vT));
    		
    		complexSystemMatrix [tempElement.a][tempElement.a]+=emitterDiodeConductance;
    		complexSystemMatrix [tempElement.b][tempElement.b]+=emitterDiodeConductance;
    		complexSystemMatrix [tempElement.a][tempElement.b]-=emitterDiodeConductance;
    		complexSystemMatrix [tempElement.b][tempElement.a]-=emitterDiodeConductance;
    		
    		/* BC Current source */
      	complexSystemMatrix [netlist [i].c][netlist [i].b]+=(netlist [i].alpha * emitterDiodeConductance);
      	complexSystemMatrix [netlist [i].b][netlist [i].e]+=(netlist [i].alpha * emitterDiodeConductance);
      	complexSystemMatrix [netlist [i].c][netlist [i].e]-=(netlist [i].alpha * emitterDiodeConductance);
      	complexSystemMatrix [netlist [i].b][netlist [i].b]-=(netlist [i].alpha * emitterDiodeConductance);
      	
      	if (strcmp (netlist [i].type, "PNP") == 0) /* If the transistor is PNP */
				{
					tempElement.iS*=(-1);
					tempElement.vT*=(-1);
					temp = tempElement.a;
					tempElement.a = tempElement.b;
					tempElement.b = temp;
				}
    		
    		/* BE Diode Capacitance */
    		complexImpedance = I * 2 * M_PI * frequenciesToPlot [index] * GetDiodeCapacitance (tempElement, currentSolution);
    		complexSystemMatrix [tempElement.a][tempElement.a]+=complexImpedance;
    		complexSystemMatrix [tempElement.b][tempElement.b]+=complexImpedance;
    		complexSystemMatrix [tempElement.a][tempElement.b]-=complexImpedance;
    		complexSystemMatrix [tempElement.b][tempElement.a]-=complexImpedance;
    		
    		#ifdef DEBUG2
    			printf ("cdc = %g\n", collectorDiodeConductance);
    			printf ("edc = %g\n", emitterDiodeConductance);
    		#endif
    		
    		/* Early effect */
      	earlyEffect = 0;
      	if (!(strcmp (netlist [i].type, "NPN")) && (currentSolution [netlist [i].c] > currentSolution [netlist [i].e]))
      		earlyEffect = 1;
				if (!(strcmp (netlist [i].type, "PNP")) && (currentSolution [netlist [i].c] < currentSolution [netlist [i].e]))
					earlyEffect = 1;
				
				if (earlyEffect)
				{  
					tempElement.a = netlist [i].b;
					tempElement.b = netlist [i].c;
					tempElement.iS = netlist [i].iSbc;
					tempElement.vT = netlist [i].vTbc;
					
					if (strcmp (netlist [i].type, "PNP") == 0) /* If the transistor is PNP */
					{
						tempElement.iS*=(-1);
						tempElement.vT*=(-1);
					}
				
					collectorDiodeCurrentSourceValue = GetDiodeCurrentSourceValue (tempElement, collectorDiodeConductance, currentSolution);
					
					tempElement.a = netlist [i].b;
					tempElement.b = netlist [i].e;
					tempElement.iS = netlist [i].iSbe;
					tempElement.vT = netlist [i].vTbe;
					
					if (strcmp (netlist [i].type, "PNP") == 0) /* If the transistor is PNP */
					{
						tempElement.iS*=(-1);
						tempElement.vT*=(-1);
					}
					
    			emitterDiodeCurrentSourceValue = GetDiodeCurrentSourceValue (tempElement, emitterDiodeConductance, currentSolution);
    			
    			#ifdef DEBUG2
    				printf ("cdcs = %g\n", collectorDiodeCurrentSourceValue);
    				printf ("edcs = %g\n", emitterDiodeCurrentSourceValue);
    			#endif
    			
      		g1 = (netlist [i].alpha * emitterDiodeConductance * (currentSolution [netlist [i].c] - currentSolution [netlist [i].e]) / netlist [i].vA);
      		g2 = ((-1) * collectorDiodeConductance * (currentSolution [netlist [i].c] - currentSolution [netlist [i].e]) / netlist [i].vA);
      		
      		g3 = ((netlist [i].alpha * (emitterDiodeConductance * (currentSolution [netlist [i].b] - currentSolution [netlist [i].e]) + emitterDiodeCurrentSourceValue)) - 
					((collectorDiodeConductance * (currentSolution [netlist [i].b] - currentSolution [netlist [i].c])) + collectorDiodeCurrentSourceValue)) / (netlist [i].vA);
					
					if (!(strcmp (netlist [i].type, "PNP")))
      		{
      			g1*=(-1);
      			g2*=(-1);
      			g3*=(-1);
					}
					
					#ifdef DEBUG2
						printf ("g1 = %g g2 = %g g3 = %g\n", g1, g2, g3);
					#endif
					
					complexSystemMatrix [netlist [i].c][netlist [i].b]+=g1;
      		complexSystemMatrix [netlist [i].e][netlist [i].e]+=g1;
      		complexSystemMatrix [netlist [i].c][netlist [i].e]-=g1;
      		complexSystemMatrix [netlist [i].e][netlist [i].b]-=g1;
      	
      		complexSystemMatrix [netlist [i].c][netlist [i].b]+=g2;
      		complexSystemMatrix [netlist [i].e][netlist [i].c]+=g2;
      		complexSystemMatrix [netlist [i].c][netlist [i].c]-=g2;
      		complexSystemMatrix [netlist [i].e][netlist [i].b]-=g2;
      	
      		complexSystemMatrix [netlist [i].c][netlist [i].c]+=g3;
      		complexSystemMatrix [netlist [i].e][netlist [i].e]+=g3;
      		complexSystemMatrix [netlist [i].c][netlist [i].e]-=g3;
      		complexSystemMatrix [netlist [i].e][netlist [i].c]-=g3;
      	}
			}
		
			/* Print complexSystemMatrix */
			#ifdef DEBUG2
				if (elementType == 'Q' || elementType == 'L' || elementType == 'C' || elementType == 'V' || elementType == 'I' || elementType == 'D' || elementType == 'K')
				{
					printf ("\nSistema apos a montagem de %s (f=%.2f)\n", netlist [i].name, frequenciesToPlot [index]);
					for (k = 1; k <= numVariables; k++) 
					{
  					for (j = 1; j <= numVariables + 1; j++)
  					{
  						if (complexSystemMatrix [k][j] != 0)
  							printf ("%+1.2e%+1.2ej ", creal (complexSystemMatrix [k][j]), cimag (complexSystemMatrix [k][j]));
  						else 
								printf (" ......... .........  ");
  					}
  					printf ("\n");
  				}
  			}
  		#endif
  	} /* for */
	
		/* Solve system */
		if (SolveComplexSystem (complexSystemMatrix, numVariables)) 
		{
			printf ("Sistema singular na frequencia %g\n", frequenciesToPlot [index]);
		}
	
		/* Write results in a file */
		fprintf (file, "%g ", frequenciesToPlot [index]);
		
		for (i = 1; i <= numVariables; i++) 
  		fprintf (file, "%g %g ", cabs (complexSystemMatrix [i][numVariables + 1]), carg (complexSystemMatrix [i][numVariables + 1]) * (180 / M_PI));
  	
		fprintf (file, "\n");
	} /* for */
	
	printf ("Analise de resposta em frequencia realizada com sucesso\nTabela escrita no arquivo %s\n", fileName);
	
	free (frequenciesToPlot);
	fclose (file);
	
	getch ();
	
	return 0;
}
