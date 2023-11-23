#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct label{char nome[10]; int linha;} tlabel;

tlabel llabels[20];
int cntlabels=0;

typedef enum insts {cond,
			 		atrib,
					desvio
					} tinst;

char nomeinst[][20]={"if",
				     "atrib",
				     "goto"};

typedef struct c3e{	char label[100];
					tinst inst;
					char op1[100];
					char op2[100];
					char op3[100];
					char op[5];} tc3e;

tc3e lcod[100]={0};

struct sucessor{
	int succs;
	struct sucessor *prox;
};

typedef struct sucessor tSucessor;

struct bloco{
	int numeroBloco;
	struct bloco *proxBloco;
	struct sucessor *proxSucessor;
};

typedef struct bloco tBloco;

tBloco * inicioBloco;

char * strip(char *linha){

    char *p = linha;
    
	while (*p == ' ' || *p == '\t'){
		p++;
	}
    
	return p;
}

int verificaDigito(char c){
	return (c>='0' && c<='9');
}

int isdigletter(char c){
    return (c>='a' && c<='z')||(c>='A' && c<='Z') || (c>='0' && c<='9') || c=='_';
}

char *busca_pal(char *p, char *pal){
    while (*p==' '||*p=='\t') p++;
    while (*p && isdigletter(*p))
	    *pal++=*p++;
        *pal='\0';	

    return p;
}

char * buscaBloco(char *p, char *pal){

	// Retiramos os espaços para buscar o dado
	while (!verificaDigito(*p)) p++;

	*pal++=*p++;
    *pal='\0';	

    return p;

}

int comandoHeOperacao(char c){

    char ops[]="><=!+-*%";

    for (int i=0;ops[i];i++)
	    if (c==ops[i]) return 1;
    
    return 0;
}

char *busca_op(char *p, char *pal){

    while (*p==' '||*p=='\t') p++;
    while (*p && comandoHeOperacao(*p))
	    *pal++=*p++;
        *pal='\0';	
    
    return p;
}

char *busca_label(char *p, char *lb, int linha){

    p=busca_pal(p, lb);

    while (*p != ':') p++;
        p++;
        strcpy(llabels[cntlabels].nome,lb);
        llabels[cntlabels++].linha=linha;
    
    return p;

}

void processaCriacaoNovoBlocoLista(char numero){

	tBloco * listaBloco = inicioBloco;
	tBloco * bloco;
	int numeroBloco = numero - '0';

	bloco = (struct bloco*)malloc(sizeof(tBloco));
	bloco->numeroBloco = numeroBloco;
	bloco->proxBloco = NULL;
	bloco->proxSucessor = NULL;

	if(inicioBloco == NULL){
		inicioBloco = bloco;
	}else{
		while(listaBloco->proxBloco != NULL){
			listaBloco = listaBloco->proxBloco;
		}
		listaBloco->proxBloco = bloco;
	}
}

int verificaEncontraDefinicaoBloco(char linha[100]){

	int i = 0;

	if(linha[i] == '<'){
		i++;
		if(linha[i] == 'b'){
			i++;
			if(linha[i] == 'b'){
				i++;
				if(linha[i] == ' '){
					i++;
					if(verificaDigito(linha[i])){
						processaCriacaoNovoBlocoLista(linha[i]);
						return 1;
					}
				}
			}
		}
	}

	return 0;

}

void processaInicializacaoListaSucessores(void){
	inicioBloco = NULL;
}

void processaImpressaoBlocosSucessores(void){

    tBloco * listaBloco = inicioBloco;
    tSucessor *listaSucessores;
    int qtdRegistros = 0;

    while(listaBloco != NULL){
		qtdRegistros = 0;
		
        printf("Bloco: %d: ", listaBloco->numeroBloco);

        listaSucessores = listaBloco->proxSucessor;

        while(listaSucessores != NULL){
            if(qtdRegistros == 0){
                printf("succs { %d", listaSucessores->succs);
            }else{
                printf(" %d ", listaSucessores->succs);
            }

            qtdRegistros++;

            listaSucessores = listaSucessores->prox;
        }

        if(qtdRegistros > 0){
            printf("}\n");
        }

        listaBloco = listaBloco->proxBloco;
    }

}

void processaAdicaoSucessorBloco(char pal[]){

	tBloco * listaBloco = inicioBloco;
	tSucessor * listaSucessor;
	tSucessor *sucessor;
	int numeroSucessor = pal[0] - '0';

	sucessor = (struct sucessor*)malloc(sizeof(tSucessor));
	sucessor->succs = numeroSucessor;
	sucessor->prox = NULL;

	while(listaBloco->proxBloco != NULL){
		listaBloco = listaBloco->proxBloco;
	}

	listaSucessor = listaBloco->proxSucessor;

	if(listaSucessor == NULL){
		listaBloco->proxSucessor = sucessor;
	}else{
		while(listaSucessor->prox != NULL){
			listaSucessor = listaSucessor->prox;
		}

		listaSucessor->prox = sucessor;
	}

}

int main(void){

    FILE *arq;
    arq=fopen("prog.c3e","rt");
    char linha[100];
    int pos;
	int i;

	processaInicializacaoListaSucessores();

    for (pos=0;fgets(linha,100,arq);pos++){
	    printf("Vou processar a linha %s",linha);
	    
		// Armazenamento da linha em ponteiro
		char *p=strip(linha);

		// Devolve o ponteiro na primeira ocorrência do caracater
        if (strstr(p,":")){
			verificaEncontraDefinicaoBloco(linha);
		}else{
			//p=busca_label(p,lcod[pos].label,pos);
	        char pal[20];
	        p=busca_pal(p,pal);

			if (strcmp(pal,"if")==0){
				p++;
				p++;
				p=busca_pal(p,lcod[pos].op1);
				p++;
				p=busca_op(p,lcod[pos].op);
				p++;
				p=busca_pal(p,lcod[pos].op2);
				p++;
				p++;
				p=busca_pal(p,pal); /*descarta goto*/
				p++;
			}

			if (strcmp(pal,"goto")==0){
		    	lcod[pos].inst=desvio;
				p=buscaBloco(p,pal);
				processaAdicaoSucessorBloco(pal);
	    		continue;	
			}

			if(strcmp(pal,"else")==0){
				continue;
			}

			// atribuicao
			lcod[pos].inst=atrib;
			strcpy(lcod[pos].op1,pal);
			p=busca_op(p,pal);
			p=busca_pal(p,lcod[pos].op2);
			p=busca_op(p,lcod[pos].op);
			p=busca_pal(p,lcod[pos].op3);
		}
	}

    for (i=0;i<pos;i++){
	    printf("%s %s %s %s %s %s\n",lcod[i].label,nomeinst[lcod[i].inst],
					                 lcod[i].op,lcod[i].op1,
					                 lcod[i].op2,lcod[i].op3);
	}

    printf("\nLista de labels:\n");
    
    for (i=0;i<cntlabels;i++){
	    printf("%s na linha %d\n",llabels[i].nome,llabels[i].linha);
	}

    processaImpressaoBlocosSucessores();
}
