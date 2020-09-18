/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: vinicius.nogueira
 *
 * Created on 2 de Abril de 2020, 14:58
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef struct little_ {
    double tempo_anterior;
    double soma_areas;
    double qtd_elementos;
} little;

/**
 * Retorna um valor contido no intervalo entre (0,1]
 * @return 
 */
double aleatorio() {
	//resultado sera algo entre [0,0.999999...] proximo a 1.0
	double u = rand() / ((double) RAND_MAX + 1);
	//limitando entre (0,1]
	u = 1.0 - u;
	return u;
}

double minimo(double n1, double n2) {
	if (n1 < n2)
		return n1;
	return n2;
}

void inicia(little *l){
    l->qtd_elementos = 0.0;
    l->soma_areas = 0.0;
    l->tempo_anterior = 0.0;
}

/**
 * Simulador de um caixa onde clientes cheguem em média a cada 10 segundos,
 * e o caixa gaste em média 8 segundos para atender cada cliente.
 * 
 * Utilização ou Ocupação = fração de tempo que o caixa permanecerá ocupado.
 * Utilização = 0.8.
 * 
 * o tempo entre a chegada de clientes, bem como o tempo de atendimento devem 
 * ser gerados de maneira pseudoaleatoria através da v.a. exponencial.
 */
int main() {
	srand(time(NULL));

	little en;
    little ew_chegada;
    little ew_saida;
    
    inicia(&en);
    inicia(&ew_chegada);
    inicia(&ew_saida);

	double tempo_medio_clientes = 0.25;
	/*printf("Informe o tempo medio entre a chegada de clientes (segundos): ");
	scanf("%lF", &tempo_medio_clientes);*/
	tempo_medio_clientes = 1.0 / tempo_medio_clientes;

	double tempo_medio_atendimento = 0.15;
	/*printf("Informe o tempo medio gasto para atender cada cliente (segundos): ");
	scanf("%lF", &tempo_medio_atendimento);*/
	tempo_medio_atendimento = 1.0 / tempo_medio_atendimento;

	//tempo decorrido tempo da simulacao
	double tempo;

	double tempo_simulacao = 10000.0;
	/*printf("Informe o tempo total de simulacao (segundos): ");
	scanf("%lF", &tempo_simulacao);*/

	//armazena o tempo de chegada do proximo cliente
	double chegada_cliente = (-1.0 / tempo_medio_clientes) * log(aleatorio());

	//armazena o tempo em que o cliente que estiver em atendimento saira do comercio
	//saida_atendimento == 0.0 indica caixa ocioso
	double saida_atendimento = 0.0;
	double fila = 0.0;
	double ocupacao = 0.0;
	printf("Caculando ...");
	//logica da simulacao
	while (tempo <= tempo_simulacao) {
		//nao existe cliente sendo atendido no momento atual,
		//de modo que a simulacao pode avancar no tempo para
		//a chegada do proximo cliente
		if (!saida_atendimento)
			tempo = chegada_cliente;
		else {
			tempo = minimo(chegada_cliente, saida_atendimento);
		}

		if (tempo == chegada_cliente) {
			//printf("Chegada de cliente: %lf\n", chegada_cliente);
			//evento de chegada de cliente
			fila ++;
			//printf("fila: %lf\n", fila);
			//indica que o caixa esta ocioso
			//logo, pode-se comecar a atender
			//o cliente que acaba de chegar
			if(saida_atendimento == 0.0){
				saida_atendimento = tempo;
			}

			//gerar o tempo de chegada do proximo cliente
			chegada_cliente = tempo + (-1.0 / tempo_medio_clientes) * log(aleatorio());

			if (saida_atendimento < chegada_cliente)
                ocupacao += saida_atendimento - tempo;
            else
                ocupacao += chegada_cliente - tempo;

			//calculo -- E[N]
            en.soma_areas += en.qtd_elementos * (tempo - en.tempo_anterior);
            en.qtd_elementos++;
            en.tempo_anterior = tempo;

			//calculo -- E[W] chegada
            ew_chegada.soma_areas += ew_chegada.qtd_elementos * (tempo - ew_chegada.tempo_anterior);
            ew_chegada.qtd_elementos++;
            ew_chegada.tempo_anterior = tempo;

		} else {
			//evento de saida de cliente
			//a cabeca da fila nao consiste no cliente em atendimento.
			//o cliente que comeca a ser atendido portanto, sai da fila,
			//e passa a estar ainda no comercio, mas em atendimento no caixa.
			
			//verifico se ha cliente na fila
			if(fila){
				fila--;
				//printf("fila: %lf\n", fila);
				saida_atendimento = tempo + (-1.0 / tempo_medio_atendimento) * log(aleatorio());
				//printf("saida de cliente: %lf\n", saida_atendimento);

				if (saida_atendimento < chegada_cliente)
                    ocupacao += saida_atendimento - tempo;
                else
                    ocupacao += chegada_cliente - tempo;

				//calculo -- E[N] entrada
				en.soma_areas += en.qtd_elementos * (tempo - en.tempo_anterior);
				en.qtd_elementos--;
				en.tempo_anterior = tempo;
				
				//calculo -- E[W] saida
				ew_saida.soma_areas += ew_saida.qtd_elementos * (tempo - ew_saida.tempo_anterior);
				ew_saida.qtd_elementos++;
				ew_saida.tempo_anterior = tempo; 

			} else {
				saida_atendimento = 0.0;
			}
		}
		/*printf("==================\n");*/
	}	
	printf("Caculado ...");
	ew_chegada.soma_areas += ew_chegada.qtd_elementos * (tempo - ew_chegada.tempo_anterior);
    ew_saida.soma_areas += ew_saida.qtd_elementos * (tempo - ew_saida.tempo_anterior);

	double en_final = en.soma_areas / tempo;
    double ew = ew_chegada.soma_areas - ew_saida.soma_areas;
    ew = ew/ew_chegada.qtd_elementos;

	double lambda = ew_chegada.qtd_elementos / tempo;

	printf("===========================================\n\n");
    printf("Ocupacao: %lf\n", ocupacao/tempo);
    printf("\n=========================================\n\n");
    printf("E[N] = %lf\n", en_final);
    printf("E[W] = %lf\n", ew);
	printf("Lambda = %lf\n", lambda);
    printf("=========================================\n");
//	getchar();
//	getchar();
	return 0;
}