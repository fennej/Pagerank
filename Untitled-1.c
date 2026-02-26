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


struct resultat* lecturefichier_matrixm(const char* filename,int *NbLignes,int *Nbcol, int *Nbval){
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

     int *nbsuc =malloc(*NbLignes*sizeof(int));
     for(int i=0;i<*NbLignes;i++){
        nbsuc[i]=0;
     }

    struct successeur** tableauSommets= malloc(*NbLignes * sizeof(struct successeur*));

    for(int i=0;i<*NbLignes;i++){
        tableauSommets[i]=NULL;
    }
    



        for(int i=0;i<*Nbval;i++){
        int pred=0,suc=0;
        
        fscanf(file, "%d %d", &pred, &suc);

        Successeur maillon=malloc(sizeof(struct successeur));
        maillon->sommet=pred;
        maillon->suivant=tableauSommets[suc-1];
        maillon->probabilite=0.0;
        tableauSommets[suc-1]=maillon;
        nbsuc[pred-1]++;
    }
   // affichertab(tableauSommets,*NbLignes);

    struct resultat *res=malloc(sizeof(struct resultat));



    for(int i=0;i<*NbLignes;i++){
        Successeur temp=tableauSommets[i];
        while(temp!=NULL){
             temp->probabilite=1.0/nbsuc[temp->sommet-1];
            temp=temp->suivant;
        }
    }

    res->successeur_sommet=nbsuc;
    res->successeur_probabilite=tableauSommets;
    fclose(file);
    return res;
}


struct successeur** lecturefichier(const char* filename,int *Nbsommet,int *Nbacrs) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erreur ouverture fichier\n");
        return NULL;
    }


    // char buffer[100];
    // while(fgets(buffer,sizeof(buffer),file)!=NULL){
    //     if(buffer[0]=='%'){
    //         continue;
    //     }else{
    // char temp;
    // fscanf(file, "%d", Nbsommet);
    // fscanf(file, "%d", Nbacrs);
    //     }
    // }


    // Lecture du nombre de sommets et d'arcs

    char temp;
    fscanf(file, "%d", Nbsommet);
    fscanf(file, "%d", Nbacrs);

    // creation du tableau inital des sommets 
     struct successeur** tableauSommets= malloc(*Nbsommet * sizeof(struct successeur*));
        for(int i=0;i<*Nbsommet;i++){
            tableauSommets[i]=NULL;
        }
        

    for(int i=0;i<*Nbsommet;i++){
        int nbsuccesseurs;
        
        fscanf(file, "%d", &nbsuccesseurs);


        for(int k=0;k<nbsuccesseurs;k++){
            int temp;
            fscanf(file, "%d", &temp);
            Successeur suc=malloc(sizeof(struct successeur));
            suc->sommet=i+1;
            suc->probabilite=1.0/nbsuccesseurs;
            suc->suivant=tableauSommets[temp-1];
            tableauSommets[temp-1]=suc;
        }


    }

    fclose(file);
    return tableauSommets;
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


float * multvectmmat(struct successeur** tableauSommets, float *vecteur0, int Nbsommet) {



    float *vecteur1=malloc(Nbsommet*sizeof(float));

    for(int i=0;i<Nbsommet;i++){
        vecteur1[i]=0;
    }

    for(int i=0;i<Nbsommet;i++){
        float sum=0.0;
        Successeur temp=tableauSommets[i];

        while(temp!=NULL){

            sum+=vecteur0[temp->sommet-1]*temp->probabilite;
            temp=temp->suivant;
        }
        vecteur1[i]=sum;
}
return vecteur1;
}



float norme_a1(float *vecteur1, float *vecteur0, int Nbsommet) {
    float sum=0.0;
    for(int i=0;i<Nbsommet;i++){
        sum+=fabs(vecteur1[i]-vecteur0[i]);
    }
    return sum;
}

//le graphe dois etre fortement connexe pour que la recherche de la page rank converge
float * recherchep(struct successeur** tableauSommets, float *vecteur0, int Nbsommet) {
    float *vecteur1=malloc(Nbsommet*sizeof(float));
    for(int i=0;i<Nbsommet;i++){
        vecteur1[i]=0;
    }


    vecteur1=multvectmmat(tableauSommets,vecteur0,Nbsommet);


    while (norme_a1(vecteur1,vecteur0,Nbsommet)>EPSILON)
    {
        float *temp=vecteur0;
        vecteur0=vecteur1;
        clock_t start = clock();    
        vecteur1=multvectmmat(tableauSommets,vecteur0,Nbsommet);
        clock_t end = clock();
        printf("Temps d'execution de la multiplication matrice-vecteur: %f secondes\n", ((double)(end - start)) / CLOCKS_PER_SEC);

        free(temp);

        float sum=0;
        for(int i=0;i<Nbsommet;i++){
            sum+=vecteur1[i];
        }

        printf("Somme des elements du vecteur: %f\n", sum);
    }
    
    return vecteur1;
}

void afficher_vecteur(float *vecteur, int Nbsommet) {
    for(int i=0;i<Nbsommet;i++){
        printf("Sommet %d: %f\n", i+1, vecteur[i]);
    }
}



int main(){

int Nbsommet = 0;
int Nbacrs=0;

int Nblignes=0;
int Nbcol=0;
int Nbval=0;


/*POUR LE FORMAT MTX*/
struct resultat *res = lecturefichier_matrixm("wiki.mtx",&Nblignes,&Nbacrs,&Nbval);
struct successeur** tableauSommets=res->successeur_probabilite;
Nbsommet=Nblignes;
//affichertab(tableauSommets,Nbsommet);
float *vecteur0=malloc(Nbsommet*sizeof(float));
    vecteur0[0]=1;
    for(int i=1;i<Nbsommet;i++){
        vecteur0[i]=0;
    }


    printf("Vecteur initial:\n");

   // afficher_vecteur(vecteur0,Nbsommet);
    vecteur0=recherchep(tableauSommets,vecteur0,Nbsommet);
    printf("PageRank:\n");
    afficher_vecteur(vecteur0,Nbsommet);

    float sum=0;
    for (int i = 0; i < Nbsommet; i++)
    {
        sum+=vecteur0[i];
    
    }
    printf("Somme des elements du vecteur: %f\n", sum);


    free(vecteur0);
    for(int i=0;i<Nbsommet;i++){
        Successeur temp=tableauSommets[i];
        while(temp!=NULL){
            Successeur suivant=temp->suivant;
            free(temp);
            temp=suivant;
        }
    }
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