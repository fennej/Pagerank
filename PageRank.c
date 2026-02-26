#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

typedef struct successeur{ 
    int sommet;
    float probabilite;
    struct successeur* suivant;
}*Successeur;

const float EPSILON = 0.0001;

struct resultat{
    int * successeur_sommet;
   struct successeur ** successeur_probabilite;
};


struct contigue{
    int pred;
    int suc;
    float probabilite;
};


struct contigue * lecturefichier_matrixm(const char* filename,int *NbLignes,int *Nbcol, int *Nbval){
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

     float *nbsuc =malloc(*NbLignes*sizeof(float));
     for(int i=0;i<*NbLignes;i++){
        nbsuc[i]=0;
     }

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
    nbsuc[i]=1.0/nbsuc[i];
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


 void multvectmmat(struct contigue* tableauSommets,float* vecteur1, float *vecteur0, int Nbsommet, int Nbvaleurs) {
    // float *vecteur1=malloc(Nbsommet*sizeof(float));

    for(int i=0;i<Nbsommet;i++){
        vecteur1[i]=0;
    }

    for(int i=0;i<Nbvaleurs;i++){
        float sum=0.0;
        int pred=tableauSommets[i].pred;
        int suc=tableauSommets[i].suc;
        float prob=tableauSommets[i].probabilite;

        vecteur1[suc-1]+=vecteur0[pred-1]*prob;//pas oublier les -1 car les sommets commencent à 1 dans le fichier
        
}
}





float norme_a1(float *vecteur1, float *vecteur0, int Nbsommet) {
    float sum=0.0;
    for(int i=0;i<Nbsommet;i++){
        sum+=fabs(vecteur1[i]-vecteur0[i]);
    }
    return sum;
}

void afficher_vecteur(float *vecteur, int Nbsommet) {
    for(int i=0;i<Nbsommet;i++){
        printf("Sommet %d: %f\n", i+1, vecteur[i]);
    }
}




//le graphe dois etre fortement connexe pour que la recherche de la page rank converge
float * recherchep(struct contigue* tableauSommets, float *vecteur0, int Nbsommet, int Nbvaleurs) {
    float *vecteur1=malloc(Nbsommet*sizeof(float));
    for(int i=0;i<Nbsommet;i++){
        vecteur1[i]=0;
    }


    // printf("first dump:\n");   
    // afficher_vecteur(vecteur0,Nbsommet);
    // printf("vector1:\n");   
    // afficher_vecteur(vecteur1,Nbsommet);    


    multvectmmat(tableauSommets,vecteur1,vecteur0,Nbsommet,Nbvaleurs);

    // printf("after first multiplication:\n");
    // printf("vector0:\n");   
    // afficher_vecteur(vecteur0,Nbsommet);
    // printf("vector1:\n");
    // afficher_vecteur(vecteur1,Nbsommet);    

    while (norme_a1(vecteur1,vecteur0,Nbsommet)>EPSILON)
    {
        float *temp=vecteur0;
        //vecteur0=vecteur1;

       memcpy(vecteur0,vecteur1,Nbsommet*sizeof(float));
        clock_t start = clock();
        


        multvectmmat(tableauSommets,vecteur1,vecteur0,Nbsommet,Nbvaleurs);

        // printf("vector0:\n");   
        // afficher_vecteur(vecteur0,Nbsommet);
        // printf("vector1:\n");
        // afficher_vecteur(vecteur1,Nbsommet);    

        clock_t end = clock();
        printf("Temps d'execution de la multiplication matrice-vecteur: %f secondes\n", ((double)(end - start)) / CLOCKS_PER_SEC);


      //  free(temp);

        float sum=0;
        for(int i=0;i<Nbsommet;i++){
            sum+=vecteur1[i];
        }

        printf("Somme des elements du vecteur: %f\n", sum);
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


/*POUR LE FORMAT MTX*/
struct contigue *tableauSommets = lecturefichier_matrixm("wiki.mtx",&Nblignes,&Nbacrs,&Nbval);
//afficher_contigue(tableauSommets,Nbval);
Nbsommet=Nblignes;
//affichertab(tableauSommets,Nbsommet);
float *vecteur0=malloc(Nbsommet*sizeof(float));
    vecteur0[0]=1;
    for(int i=1;i<Nbsommet;i++){
        vecteur0[i]=0;
    }


    printf("Vecteur initial:\n");

   // afficher_vecteur(vecteur0,Nbsommet);
    vecteur0=recherchep(tableauSommets,vecteur0,Nbsommet,Nbval);
    printf("PageRank:\n");
  //  afficher_vecteur(vecteur0,Nbsommet);

    float sum=0;
    for (int i = 0; i < Nbsommet; i++)
    {
        sum+=vecteur0[i];
    
    }
    printf("Somme des elements du vecteur: %f\n", sum);


    free(vecteur0);
    free(tableauSommets);
/*POUR LE OFRMAT TXT*/
/*   
//struct resultat *res = lecturefichier_matrixm("wiki.mtx",&Nblignes,&Nbacrs,&Nbval);
     struct successeur** tableauSommets= lecturefichier("Harvard500.mtx",&Nbsommet,&Nbacrs);
    //struct successeur** tableauSommets=res->successeur_probabilite;

    affichertab(tableauSommets,Nbsommet);
    float *vecteur0=malloc(Nbsommet*sizeof(float));
    
    //affichertab(tableauSommets,Nblignes);
    //float *vecteur0=malloc(Nblignes*sizeof(float)); 

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


    // float *vecteur0=malloc(Nbsommet*sizeof(float));


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
    return 0;
}   