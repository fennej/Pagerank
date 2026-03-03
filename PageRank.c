#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <omp.h>


typedef struct successeur{ 
    int sommet;
    double probabilite;
    struct successeur* suivant;
}*Successeur;

const double EPSILON = 0.000001;
const double ALPHA=0.9;


struct tableauD{
    int *tabD;
    int Nbsommet;
};


struct resultat{
    int * successeur_sommet;
   struct successeur ** successeur_probabilite;
};


struct contigue{
    int pred;
    int suc;
    double probabilite;
};


struct contigue * lecturefichier_matrixm(const char* filename,int *NbLignes,int *Nbcol, int *Nbval,struct tableauD** tabD){
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erreur ouverture fichier\n");
        return NULL;
    }


    char buffer[100];
    while(fgets(buffer,sizeof(buffer),file)!=NULL){
        if(buffer[0]=='%'){
            continue;
        }else{
    char temp;
        sscanf(buffer,"%d %d %d",NbLignes,Nbcol,Nbval);
        break;
        }
    }

     double *nbsuc =malloc(*NbLignes*sizeof(double));
     for(int i=0;i<*NbLignes;i++){
        nbsuc[i]=0;
     }

     (*tabD)->tabD=malloc(*NbLignes*sizeof(int));
     for(int i=0;i<*NbLignes;i++){
        (*tabD)->tabD[i]=0;
     }
     (*tabD)->Nbsommet=0;

     
    struct contigue *tabcontigue=malloc(*Nbval*sizeof(struct contigue));


        for(int i=0;i<*Nbval;i++){
        int pred=0,suc=0;
        
        fscanf(file, "%d %d", &pred, &suc);

        tabcontigue[i].pred=pred;
        tabcontigue[i].suc=suc;
        nbsuc[pred-1]++;
        
    }
   // affichertab(tableauSommets,*NbLignes);


   for(int i=0;i<*NbLignes;i++){
    if(nbsuc[i]>0)
    nbsuc[i]=1.0/nbsuc[i];
    else{
        (*tabD)->tabD[(*tabD)->Nbsommet]=i;
        (*tabD)->Nbsommet++;
    }
   }


    for(int i=0;i<*Nbval;i++){
        tabcontigue[i].probabilite=nbsuc[tabcontigue[i].pred-1];
    }

    return tabcontigue;
}



void afficherliste(Successeur s){
    while(s!=NULL){
        printf("Sommet: %d, Probabilite: %f\n", s->sommet, s->probabilite);
        s=s->suivant;
    }
}



void affichertab(struct successeur** tableau,int Nbsomments){
    for(int i=0;i<Nbsomments;i++){
        printf("pred du sommet %d:\n", i+1);
        Successeur temp=tableau[i];
        afficherliste(temp);
    }
}


void afficher_vecteur(double *vecteur, int Nbsommet) {
    for(int i=0;i<Nbsommet;i++){
        printf("Sommet %d: %f\n", i+1, vecteur[i]);
    }
}


 void multvectmmat(struct contigue* tableauSommets,double* vecteur1, double *vecteur0, int Nbsommet, int Nbvaleurs) {
    // double *vecteur1=malloc(Nbsommet*sizeof(double));
    memset(vecteur1, 0, Nbsommet * sizeof(double));


    for(int i=0;i<Nbvaleurs;i++){
        double sum=0.0;
        int pred=tableauSommets[i].pred;
        int suc=tableauSommets[i].suc;
        double prob=tableauSommets[i].probabilite;

        vecteur1[suc-1]+=vecteur0[pred-1]*prob;//pas oublier les -1 car les sommets commencent à 1 dans le fichier

        
       // vecteur1[tableauSommets[i].suc-1]+=vecteur0[tableauSommets[i].pred-1]*tableauSommets[i].probabilite;//pas oublier les -1 car les sommets commencent à 1 dans le fichier

}
}



double norme_a1(double *vecteur1, double *vecteur0, int Nbsommet) {
    double sum=0.0;
    for(int i=0;i<Nbsommet;i++){
        sum+=fabs(vecteur1[i]-vecteur0[i]);
    }
    return sum;
}





double mult_vectligne_vectcol(double *vect_ligne,double* vect_col,int nbrdesommet){
    double res=0;
for (int i=0;i<nbrdesommet;i++){
    res+=vect_ligne[i]*vect_col[i];
}
return res;
}

void mult_scalair(struct contigue *res,struct contigue *tab,double scalair,int Nbvaleurs){
    //pour trouver le alphaP 
    memcpy(res,tab,Nbvaleurs*sizeof(struct contigue));

    for(int i=0;i<Nbvaleurs;i++){
        res[i].probabilite*=scalair;
    }
}

double * mult_vect_scalair(double *vecteur,double scalair,int nbsommet){
    for(int i=0;i<nbsommet;i++){
        vecteur[i]=vecteur[i]*scalair;
    }
    return vecteur;
}

double mult_vectligne_vectcol_BIS(double* res, double *vect_ligne,struct tableauD *tabD,double UnSurN_mult_alpha){
    double sum=0;
    for(int i =0;i<tabD->Nbsommet;i++){
        sum+=vect_ligne[tabD->tabD[i]]*UnSurN_mult_alpha;
    }
    *res=sum;
}

double calculer_cst(double* veucteur, struct tableauD *tabD,double unsurN,int nbelts){
    /*le resultat sera exris dans vecteur*/
   // double premier_terme=mult_vectligne_vectcol(veucteur,vecteurF_P_ALPHA_unsurn,nbsommet);
    double premier_terme=0.0;
    mult_vectligne_vectcol_BIS(&premier_terme,veucteur,tabD,unsurN*ALPHA);
    double deuxiemme_terme=(1-ALPHA)*unsurN;// penser a mettre 1-alpha en param pour ne pas le calculer a chaque fois
    double constante=premier_terme+deuxiemme_terme;
    return constante;
}



double* addition_deux_vecteurs(double* vecteur1, double* vecteur0, int nbelts){
    for(int i=0;i<nbelts;i++){
        vecteur1[i]=vecteur1[i]+vecteur0[i];
    }
}





//le graphe dois etre fortement connexe pour que la recherche de la page rank converge ,,,n'est plus vrai maintenant
double * recherchep(struct contigue* tableauSommets, double *vecteur0,struct tableauD *tabD, int Nbsommet, int Nbvaleurs) {
    double *vecteur1=malloc(Nbsommet*sizeof(double));
    memset(vecteur1, 0, Nbsommet * sizeof(double));
    // double *vecteurD_Mult1surN_MultAlpha=malloc(Nbsommet*sizeof(double));
     double UnSurN=1.0/Nbsommet;
    struct contigue * aP=malloc(Nbvaleurs*sizeof(struct contigue));





    // for(int i=0;i<Nbsommet;i++){
    //     vecteurD_Mult1surN_MultAlpha[i]=ALPHA*UnSurN*vecteurD[i];
    // }
  


    mult_scalair(aP,tableauSommets,ALPHA,Nbvaleurs);

    multvectmmat(aP,vecteur1,vecteur0,Nbsommet,Nbvaleurs);

    double deuxieme_terme=calculer_cst(vecteur0,tabD,UnSurN,Nbsommet);


    for (int i=0;i<Nbsommet;i++){
        vecteur1[i]=vecteur1[i]+deuxieme_terme;
    }


    while (norme_a1(vecteur1,vecteur0,Nbsommet)>EPSILON)
    {
        double *temp = vecteur0;
        vecteur0 = vecteur1;
        vecteur1 = temp;
        clock_t start = clock();



        multvectmmat(aP,vecteur1,vecteur0,Nbsommet,Nbvaleurs);
        double deuxieme_terme=calculer_cst(vecteur0,tabD,UnSurN,Nbsommet);
        

        for (int i=0;i<Nbsommet;i++){
            vecteur1[i]=vecteur1[i]+deuxieme_terme;
        }



        clock_t end = clock();
        printf("Temps d'execution de la multiplication matrice-vecteur: %f secondes\n", ((double)(end - start)) / CLOCKS_PER_SEC);


      //  free(temp);

        double sum=0;
        for(int i=0;i<Nbsommet;i++){
            sum+=vecteur1[i];
        }

        printf("Somme des elements du vecteur: %.12lf\n", sum);
    }

    
    
    return vecteur1;
}




void afficher_contigue(struct contigue* tab,int Nbvaleurs){
    for(int i=0;i<Nbvaleurs;i++){
        printf("Pred: %d, Suc: %d, Probabilite: %f\n", tab[i].pred, tab[i].suc, tab[i].probabilite);
    }
}







int main(){

int Nbsommet = 0;
int Nbacrs=0;

int Nblignes=0;
int Nbcol=0;
int Nbval=0;


/*initialisation du tableu D qui est est ce que la ligne na pas de suc*/
struct tableauD* tabD=malloc(sizeof(struct tableauD));


/*POUR LE FORMAT MTX*/
struct contigue *tableauSommets = lecturefichier_matrixm("wiki.mtx",&Nblignes,&Nbacrs,&Nbval,&tabD);
//afficher_contigue(tableauSommets,Nbval);
Nbsommet=Nblignes;
//affichertab(tableauSommets,Nbsommet);
double *vecteur0=malloc(Nbsommet*sizeof(double));
    vecteur0[0]=1;
    for(int i=1;i<Nbsommet;i++){
        vecteur0[i]=0;
    }



    printf("Vecteur initial:\n");

   // afficher_vecteur(vecteur0,Nbsommet);
    vecteur0=recherchep(tableauSommets,vecteur0,tabD,Nbsommet,Nbval);
    printf("PageRank:\n");
  //  afficher_vecteur(vecteur0,Nbsommet);

    double sum=0;
    for (int i = 0; i < Nbsommet; i++)
    {
        sum+=vecteur0[i];
    
    }
    printf("Somme des elements du vecteur: %.9lf\n", sum);


    free(vecteur0);
    free(tableauSommets);
/*POUR LE OFRMAT TXT*/
/*   
//struct resultat *res = lecturefichier_matrixm("wiki.mtx",&Nblignes,&Nbacrs,&Nbval);
     struct successeur** tableauSommets= lecturefichier("Harvard500.mtx",&Nbsommet,&Nbacrs);
    //struct successeur** tableauSommets=res->successeur_probabilite;

    affichertab(tableauSommets,Nbsommet);
    double *vecteur0=malloc(Nbsommet*sizeof(double));
    
    //affichertab(tableauSommets,Nblignes);
    //double *vecteur0=malloc(Nblignes*sizeof(double)); 

    vecteur0[0]=1;
    for(int i=1;i<Nbsommet;i++){
        vecteur0[i]=0;
    }

    //   vecteur0[0]=1;
    // for(int i=1;i<Nblignes;i++){
    //     vecteur0[i]=0;
    // }

    printf("Vecteur initial:\n");

    afficher_vecteur(vecteur0,Nbsommet);
    vecteur0=recherchep(tableauSommets,vecteur0,Nbsommet);
    printf("PageRank:\n");
    afficher_vecteur(vecteur0,Nbsommet);
*/

    /*FIN PART TXT*/



    /*   afficher_vecteur(vecteur0,Nblignes);
    vecteur0=recherchep(tableauSommets,vecteur0,Nblignes);
    printf("PageRank:\n");
    afficher_vecteur(vecteur0,Nblignes);*/




    // affichertab(tableauSommets,Nbsommet);


    // double *vecteur0=malloc(Nbsommet*sizeof(double));


    // vecteur0[0]=1;
    // for(int i=1;i<Nbsommet;i++){
    //     vecteur0[i]=0;
    // }

    // printf("Vecteur initial:\n");

    // afficher_vecteur(vecteur0,Nbsommet);

    // vecteur0=recherchep(tableauSommets,vecteur0,Nbsommet);
    // printf("PageRank:\n");
    // afficher_vecteur(vecteur0,Nbsommet);







    // free(vecteur0);
    // for(int i=0;i<Nbsommet;i++){
    //     Successeur temp=tableauSommets[i];
    //     while(temp!=NULL){
    //         Successeur suivant=temp->suivant;
    //         free(temp);
    //         temp=suivant;
    //     }
    // }
    // free(tableauSommets);





/*// Process vectors in chunks that fit in L1 cache (~32KB)
#define CHUNK_SIZE 8192  // Adjust based on your CPU

for(int chunk = 0; chunk < Nbsommet; chunk += CHUNK_SIZE) {
    int chunk_end = (chunk + CHUNK_SIZE < Nbsommet) ? chunk + CHUNK_SIZE : Nbsommet;
    
    // Process only this chunk
    for(int i = chunk; i < chunk_end; i++) {
        vecteur1[i] = ...;
    }
}*/





//quand on veut que une varriable change dans la main on donne un pointeur meme chose pour les structures
    return 0;
}   