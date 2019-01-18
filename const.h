/*
 * Universidade Federal do Rio de Janeiro
 * Escola Politecnica
 * Departamento de Eletronica e Computacao
 * Circuitos Eletricos II - 2017.1
 * Professor Antonio Carlos M. de Queiroz
 * Autor: Lucas Correa Macedo
 */

#ifndef CONST_H
#define CONST_H

#define MAX_NAME 11
#define MAX_NODES 50
#define MAX_ELEM 50
#define MAX_LINE 80

#define MAX_NR_ITERATIONS 100
#define MAX_NR_ATTEMPTS 50
#define NR_TOLERANCE 1e-9
#define MAX_RANDOM_VALUE 5
#define NR_INITIAL_APROX 0.1

#define MAX_DIODE_VOLTAGE 0.7
#define DIODE_DEFAULT_IS_VALUE 1e-9
#define DIODE_DEFAULT_VT_VALUE 0.0434294
#define DIODE_DEFAULT_C0_VALUE 5e-12
#define DIODE_DEFAULT_C1_VALUE 1e-16
#define DIODE_PHI 0.6

#define DIODE_PARALLEL_CONDUCTANCE 1e-9
#define MAX_DIODE_INVERSE_VOLTAGE 0.7

#define MAX_TYPE_LENGTH 4
#define TRANSISTOR_DEFAULT_ALPHA_VALUE 0.99
#define TRANSISTOR_DEFAULT_ALPHAR_VALUE 0.5
#define TRANSISTOR_DEFAULT_VA_VALUE 100

#define CAPACITOR_OPERATING_POINT_RESISTANCE 1e9
#define INDUCTOR_OPERATING_POINT_RESISTANCE 1e-9

#define DEFAULT_NUM_POINTS 1000
#define DEFAULT_INITIAL_FREQUENCY 1
#define DEFAULT_FINAL_FREQUENCY 10000

#define TOLG 1e-9

#endif
