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

 double EPSILON = 0.0000001;
 double SMALL_EPSILON = 0.00000001;
 double ALPHA=0.9;
double sum=0;
 int NUM_THREADS=1;

struct tableauD{
    int *tabD;
    int Nbsommet;
};

struct neud_sommet_suc_prob{
    int sommet;
    int successeur;
    double probabilite;
    struct neud_sommet_suc_prob* suivant;
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

struct list_chainne_de_sommet{
    int sommet; 
    struct list_chainne_de_sommet* suiv;    //a changer il faut creer une structure pour ne pas recomencer a la recherche ... 
}; 


struct tableau_liste_chainne{
    struct list_chainne_de_sommet** tab;
    int Nbsommet;
};


// à ajouter en haut du fichier, avant lecturefichier_matrixm
struct list_chainne_de_sommet* stack(struct list_chainne_de_sommet* liste, int sommet);


void sup_aretes(double proba_minimale,struct contigue* tab,int Nbvaleurs){
    for(int i=0;i<Nbvaleurs;i++){
        if(tab[i].probabilite<proba_minimale){
            tab[i].probabilite=0;
        }
    }
}





struct contigue * lecturefichier_matrixm(const char* filename,int *NbLignes,int *Nbcol, int *Nbval,struct tableauD** tabD,struct tableau_liste_chainne* graphe_suc,struct tableau_liste_chainne* graphe_pred){
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
    (*graphe_suc).tab=malloc(*Nbcol*sizeof(struct list_chainne_de_sommet*));
    (*graphe_pred).tab=malloc(*Nbcol*sizeof(struct list_chainne_de_sommet*));

     for(int i=0;i<*Nbcol;i++){
        graphe_pred->tab[i]=NULL;
        graphe_suc->tab[i]=NULL;
     }
     graphe_pred->Nbsommet=*Nbcol;

     graphe_suc->Nbsommet=*Nbcol;


        for(int i=0;i<*Nbval;i++){
        int pred=0,suc=0;
        
        fscanf(file, "%d %d", &pred, &suc);
        graphe_suc->tab[pred-1]=stack(graphe_suc->tab[pred-1],suc); //ici stack joue le role de inser tete 
        graphe_pred->tab[suc-1]=stack(graphe_pred->tab[suc-1],pred);

        tabcontigue[i].pred=pred;
        tabcontigue[i].suc=suc;
        nbsuc[pred-1]++;
        
    }
   // affichertab(tableauSommets,*NbLignes);

// si on met une distribution uniforme sur le sommet si il a des sucesseurs ou bien on met c'est un dargling node et au quelle cas le tabD est a 1 au sommet 
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
        printf("Sommet %d: %lf\n", i+1, vecteur[i]);
    }
}



 void multvectmmat_old_sans_surf(struct contigue* tableauSommets,double* vecteur1, double *vecteur0,int Nbarcs, int Nbsommet, int Nbsommet_globale, int Nbvaleurs) {
    // float *vecteur1=malloc(Nbsommet*sizeof(float));
    memcpy(vecteur1,vecteur0,Nbsommet_globale*sizeof(double));
    

    for(int i=0;i<Nbarcs;i++){
        int t=tableauSommets[i].suc-1;
        vecteur1[t]=0;
    }// ici ce quon fait c'est que on le vecteur 1 dois recevoir une copie du vecteur 0 sauf pour les sommet de la cfc actuelle qu'on dois chercher a calculer la distribution stat 
    
    


    for(int i=0;i<Nbvaleurs;i++){
        double sum=0.0;
        int pred=tableauSommets[i].pred;
        int suc=tableauSommets[i].suc;
        double prob=tableauSommets[i].probabilite;
        vecteur1[suc-1]+=vecteur0[pred-1]*prob;//pas oublier les -1 car les sommets commencent à 1 dans le fichier

        
}
}



 void multvectmmat(struct contigue* tableauSommets,double* vecteur1, double *vecteur0, int Nbsommet, int Nbvaleurs) {
    // double *vecteur1=malloc(Nbsommet*sizeof(double));
    #pragma omp single
    memset(vecteur1, 0, Nbsommet * sizeof(double));

    #pragma omp for
    for(int i=0;i<Nbvaleurs;i++){
        double sum=0.0;
        int pred=tableauSommets[i].pred;
        int suc=tableauSommets[i].suc;
        double prob=tableauSommets[i].probabilite;


        //atomic        #pragma omp atomic
        #pragma omp atomic
        vecteur1[suc-1]+=vecteur0[pred-1]*prob;//pas oublier les -1 car les sommets commencent à 1 dans le fichier

        
       // vecteur1[tableauSommets[i].suc-1]+=vecteur0[tableauSommets[i].pred-1]*tableauSommets[i].probabilite;//pas oublier les -1 car les sommets commencent à 1 dans le fichier

}
}



double norme_a1(double *vecteur1, double *vecteur0, int Nbsommet) {
     double sum=0.0;
    for(int i=0;i<Nbsommet;i++){
        sum+=fabs(vecteur1[i]-vecteur0[i]);
    }

   printf("Norme L1: %Lf\n", sum);
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

    //un seul vas la fiare 
    #pragma omp single
    memcpy(res,tab,Nbvaleurs*sizeof(struct contigue));

    
    //partge  
    #pragma omp for
    for(int i=0;i<Nbvaleurs;i++){
        //atomic pas necessaire on a pas d'access concurent sur les memes elements de res
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
    #pragma omp single
     sum=0;
    #pragma omp for reduction(+:sum)
    for(int i =0;i<tabD->Nbsommet;i++){
        sum+=vect_ligne[tabD->tabD[i]]*UnSurN_mult_alpha;
    }
   // #pragma omp single il ne faut pas faire le signle car tout les threads doivnet mettre a jour leurs premier terme car on ne sais pas qui vas entrer dans le sigle et calucler la constante 
    //    #pragma omp single
    // {
    //     printf("le vecteur passe en param dans bis est ");
    //     afficher_vecteur(vect_ligne,6);
    //     printf("la somme est %f\n", sum);
    // }
    *res=sum;
}

void calculer_cst(double *res,double* veucteur, struct tableauD *tabD,double unsurN,int nbelts){
    /*le resultat sera exris dans vecteur*/
   // double premier_terme=mult_vectligne_vectcol(veucteur,vecteurF_P_ALPHA_unsurn,nbsommet);
    double premier_terme=0.0;
    mult_vectligne_vectcol_BIS(&premier_terme, veucteur,tabD,unsurN*ALPHA);
    #pragma omp single
    {
    double deuxiemme_terme=(1-ALPHA)*unsurN;// penser a mettre 1-alpha en param pour ne pas le calculer a chaque fois
    double constante=premier_terme+deuxiemme_terme;

    // printf("je suis dans cst et voici la constante: %f\n", constante);

    // printf("et vioi le vecteur passe en param ");
    // afficher_vecteur(veucteur,nbelts);

    *res=constante;
    }
}



double* addition_deux_vecteurs(double* vecteur1, double* vecteur0, int nbelts){
    for(int i=0;i<nbelts;i++){
        vecteur1[i]=vecteur1[i]+vecteur0[i];
    }
}




void afficher_contigue(struct contigue* tab,int Nbvaleurs){
    for(int i=0;i<Nbvaleurs;i++){
        printf("Pred: %d, Suc: %d, Probabilite: %f\n", tab[i].pred, tab[i].suc, tab[i].probabilite);
    }
}


//creation de la liste
struct list_chainne_de_sommet* creer_liste(int sommet){
    struct list_chainne_de_sommet* nouvelle_liste=malloc(sizeof(struct list_chainne_de_sommet));
    nouvelle_liste->sommet=sommet;
    nouvelle_liste->suiv=NULL;
    return nouvelle_liste;
}


//enlever un sommet
struct list_chainne_de_sommet* pop(struct list_chainne_de_sommet* liste){
    if(liste==NULL){
        return NULL;
    }
    struct list_chainne_de_sommet* temp=liste;
    liste=liste->suiv;
    free(temp);
    return liste;
}

//rend le premier sommet sa valeurs
int peek(struct list_chainne_de_sommet* list){
    if(list==NULL){
        return -1; // ou une autre valeur pour indiquer que la liste est vide
    }
    return list->sommet;
}

//ajouter un nouveau sommet dans la stack 
struct list_chainne_de_sommet* stack(struct list_chainne_de_sommet* liste,int sommet){
    struct list_chainne_de_sommet* temp=creer_liste(sommet);
    temp->suiv=liste;
    liste=temp;
    return liste;
}


//cette fonction rend -1 si tout le les sommets on ete visite , et retourne le prochain sommet a visiter sinon
//elle rend l'index et non le sommet index =sommet -1
int tout_visite(int *tab,int n,int *depart){
    while(*depart<n && tab[*depart]!=-1){
        (*depart)++;
    }


    if((*depart)<n){
        return *depart;
    }
    else return -1;
}




void libmemstack(struct list_chainne_de_sommet **liste){
    struct list_chainne_de_sommet *temp;

    while((*liste)!=NULL){
        temp=(*liste);
        (*liste)=(*liste)->suiv;
        free(temp);
    }
    liste=NULL;

}


void afficher_liste_chainne(struct list_chainne_de_sommet *liste){
    while(liste!=NULL){
        printf("%d \n",liste->sommet);
        liste=liste->suiv;
    }
}



int* coposants_connexe(struct tableau_liste_chainne graph, int nbsommets,int *nbcc ,int **nb_sommets_par_cc){

    int * cc=malloc(nbsommets*sizeof(int));// qui contiendra les numeors des cc par sommet
    memset(cc,-1,nbsommets*sizeof(int));
    int c=0;
    int * nbsommets_par_cc_inner=malloc(nbsommets*sizeof(int));
    memset(nbsommets_par_cc_inner,0,nbsommets*sizeof(int));

    struct list_chainne_de_sommet* successeur=NULL;

    struct list_chainne_de_sommet* stack_iterations=NULL; //oours les iterations du DFS
    int sommet_courant=0;
    int index_visited=0;
    int numero_cc=-1;// pour pouvoir l'incrementer en suite dans la premiere while et que ca fasse 0

    while((c=tout_visite(cc,nbsommets,&index_visited))!=-1){
        int sommet_courant=c;
        stack_iterations=stack(stack_iterations,sommet_courant);
        numero_cc++;
        
        // debut du dfs avec le sommet courant
        while(stack_iterations!=NULL){

            sommet_courant=peek(stack_iterations);

            if(cc[sommet_courant]==-1){

            
            cc[sommet_courant]=numero_cc;// on met le numero de la cc dans le tableau des cc

            nbsommets_par_cc_inner[numero_cc]++;// on incremente le nombre de sommet de la cc actuelle
        }   
            successeur=graph.tab[sommet_courant];
            


            while(successeur!=NULL&&cc[successeur->sommet-1]!=-1){
                successeur=successeur->suiv;
            }

            if(successeur!=NULL){
                stack_iterations=stack(stack_iterations,successeur->sommet-1);
            }else{
                stack_iterations=pop(stack_iterations);
            }

        }
        


    }


    libmemstack(&stack_iterations);
    *nbcc=numero_cc+1;
    *nb_sommets_par_cc=realloc(nbsommets_par_cc_inner,(*nbcc)*sizeof(int));
    return cc;
}





// dificulté a mettre cette algo en it 
int* kosaraju(struct tableau_liste_chainne graph, struct tableau_liste_chainne graph_pred, int nbsommets,int *nbcfc ,int **nb_sommets_par_cfc){
    //cration de la liste visité
    int* visited=malloc(nbsommets*sizeof(int));
    memset(visited,-1,nbsommets*sizeof(int));
    //creation de la stack
    struct list_chainne_de_sommet* stack_visited=NULL;//la pile qui garde l'ordre de visite de premier dfs 
    struct list_chainne_de_sommet* stack_iterations=NULL;
    //paroucr en largeur
    int cfc_counter=-1;// pour pouvoir l'incrementer en suite dans la premiere while et que ca fasse 0
    int sommet_courant=0;
    struct list_chainne_de_sommet* successeur=NULL;
    int predecesseurs;//est utilise dans la deuxiemme partie de cette algo 
    *nb_sommets_par_cfc=malloc(nbsommets*sizeof(int));//on ne sais pas combien de cfc on vas trouver donc on alloue pour le pire cas ou chaque sommet est une cfc et on vas desalouer apres;
    memset(*nb_sommets_par_cfc,0,nbsommets*sizeof(int));

    int c=0;
    int index_visited=0;

    while( ( c=tout_visite(visited,nbsommets,&index_visited))!=-1){ 
        stack_iterations=stack(stack_iterations,c);
        sommet_courant=c;
        //   printf("le sommet courant est %d",sommet_courant);
        //sleep(5);
        while(stack_iterations!=NULL){
            // printf("les iterations\n");
            // afficher_liste_chainne(stack_iterations);
            // sleep(7);


            //dans la stack du paroucr en profondeur on prends le sommet actuelle et on ajoute au visité
            sommet_courant=peek(stack_iterations);

            visited[sommet_courant]=0;
            successeur=graph.tab[sommet_courant];

            //rechercher le prochain successeur non visitééé
            while(successeur!=NULL&&visited[successeur->sommet-1]!=-1){
                successeur=successeur->suiv;
            }
            
            //on a soit un sommet qui a des successeurs non visité au quelle on vas les parcourire donc on continue le DFS
            //sinon on pop dans la stack des des iterations pour ne pas faire un algo recurcif ...
            if(successeur!=NULL){
                stack_iterations = stack(stack_iterations,successeur->sommet-1);
            }else{
                stack_visited=stack(stack_visited,sommet_courant);
                stack_iterations = pop(stack_iterations);
            }
        }
    }

    memset(visited,-1,nbsommets*sizeof(int));



    // printf("=========affichage de visited==========\n");
    // afficher_liste_chainne(stack_visited);    

    while(stack_visited!=NULL){

        while(stack_visited!=NULL&&visited[peek(stack_visited)]!=-1){
            stack_visited=pop(stack_visited);
        }

        if(stack_visited!=NULL){
            cfc_counter++; // on augmente le nombre de cfc trouvé

            
            stack_iterations=stack(stack_iterations,peek(stack_visited));// on a trouve un sommet dans la stack des visités(replis dans la premmière partie) qui n'est pas visite et donc on le met dans la stack du parcour 
            stack_visited=pop(stack_visited);

            // la on commance le parcour avec le graphe inversé cad des predecesseurs 
            while(stack_iterations!=NULL){



                sommet_courant=peek(stack_iterations);
                if(visited[sommet_courant]==-1)
                (*nb_sommets_par_cfc)[cfc_counter]++;
                visited[sommet_courant]=cfc_counter;//visited prend la valeur du numero de la cfc 
                successeur=graph_pred.tab[sommet_courant];


                // printf("cfc counter est %d\n",cfc_counter);
                // printf("le sommet courant:%d\n",sommet_courant);
                // printf("les iterations :\n");
                // afficher_liste_chainne(stack_iterations);

               // sleep(8);

                while(successeur!=NULL&&visited[successeur->sommet-1]!=-1){
                    successeur=successeur->suiv;
                }
                
       

                // printf("etat de suc \n");
                // if(successeur==NULL){
                //     printf("alerete:");
                // }else{
                //     printf("successur valeur %d",successeur->sommet);
                // }

                if(successeur!=NULL){

                    //printf("la stack it vas ietre mis a jour \n la valeur insere est :%d\n",successeur->sommet-1);
                    stack_iterations = stack(stack_iterations,successeur->sommet-1);
                }else{
                    stack_iterations = pop(stack_iterations);
                }
            }


        }
    }


    *nbcfc=cfc_counter+1;
    (*nb_sommets_par_cfc)=realloc((*nb_sommets_par_cfc),(*nbcfc)*sizeof(int));
    libmemstack(&stack_visited);
    libmemstack(&stack_iterations);
    return visited;
}


double norme_a1_old(double *vecteur1, double *vecteur0, int Nbsommet) {
    double sum=0.0;
    for(int i=0;i<Nbsommet;i++){
        sum+=fabs(vecteur1[i]-vecteur0[i]);
       // printf("sum=%lf",sum);
    }
    return sum;
}


double * recherchep_sans_surf(struct contigue* tableauSommets, double *vecteur0,int Nbarcs,int Nbsommet_globale, int Nbsommet, int Nbvaleurs) {
    double *vecteur1=malloc(Nbsommet_globale*sizeof(double));
    memcpy(vecteur1,vecteur0,Nbsommet_globale*sizeof(double));
    int cpt=0; //pour arreter ce page rank

    for(int i=0;i<Nbarcs;i++){
        int t=tableauSommets[i].suc-1;
        vecteur1[t]=0;
    }// ici ce quon fait c'est que on le vecteur 1 dois recevoir une copie du vecteur 0 sauf pour les sommet de la cfc actuelle qu'on dois chercher a calculer la distribution stat 
    // donc on dois mettre un 0  a ces sommets 


    
    if(vecteur1==NULL){
        printf("erreur allocation vecteur1\n");
        exit(1);}



    multvectmmat_old_sans_surf(tableauSommets,vecteur1,vecteur0,Nbarcs,Nbsommet,Nbsommet_globale,Nbvaleurs);

        printf("ici1\n");
    // printf("veucteur0: ");
    // afficher_vecteur(vecteur0,Nbsommet);
    // printf("veucteur1: ");
    // afficher_vecteur(vecteur1,Nbsommet);

    while (norme_a1_old(vecteur1,vecteur0,Nbsommet_globale)>EPSILON&&cpt<2)
    {
        cpt++;

       // printf("%lf\n",norme_a1_old(vecteur1,vecteur0,Nbsommet));

        double *temp = vecteur0;
        vecteur0 = vecteur1;
        vecteur1 = temp;
        // clock_t start = clock();
        
        multvectmmat_old_sans_surf(tableauSommets,vecteur1,vecteur0,Nbarcs,Nbsommet,Nbsommet_globale,Nbvaleurs);

    }
    free(vecteur0);
    return vecteur1;
}





void afficher_graph_suc_pred(struct tableau_liste_chainne tab){
    for(int i=0;i<tab.Nbsommet;i++){
        struct list_chainne_de_sommet*temp=tab.tab[i];
        while(temp!=NULL){
            printf("sommet %d, -> %d\n",i+1,temp->sommet);
            temp=temp->suiv;
        }
    }
}



//le graphe dois etre fortement connexe pour que la recherche de la page rank converge ,,,n'est plus vrai maintenant
double * recherchep(struct contigue* tableauSommets, double *vecteur0,struct tableauD *tabD, int Nbsommet, int Nbvaleurs,int *nbiter,int use_mall_epsilon) {


    

    double *vecteur1=malloc(Nbsommet*sizeof(double));
    memset(vecteur1, 0, Nbsommet * sizeof(double));
    // double *vecteurD_Mult1surN_MultAlpha=malloc(Nbsommet*sizeof(double));
     double UnSurN=1.0/Nbsommet;
    struct contigue * aP=malloc(Nbvaleurs*sizeof(struct contigue));
    if(aP==NULL){
        printf("erreur allocation aP\n");
        exit(1);
    }
    int stop=0;
     double deuxieme_terme=0;
     clock_t start, end;
 // pour la reduction puisque on peut pas declarer somme globale une fois en parallel

 int cpt=0;

 omp_set_num_threads(NUM_THREADS); 
#pragma omp parallel
{
    #pragma omp single
    NUM_THREADS=omp_get_num_threads();

    // #pragma omp single 
    // {
    //     (*nbiter)++;
    // }

    //pour avoir ap 
    mult_scalair(aP,tableauSommets,ALPHA,Nbvaleurs);

    //axP
    multvectmmat(aP,vecteur1,vecteur0,Nbsommet,Nbvaleurs);

   
    calculer_cst(&deuxieme_terme,vecteur0,tabD,UnSurN,Nbsommet);



    #pragma omp for
    for (int i=0;i<Nbsommet;i++){
        vecteur1[i]=vecteur1[i]+deuxieme_terme;
    }  




// #pragma omp single
//     {
//         printf("Deuxieme terme: %f\n", deuxieme_terme);
//         printf("Vecteur apres multiplication matrice-vecteur:\n");
//         afficher_vecteur(vecteur1,Nbsommet);
//     }

// enlever cette while mettre un test qui sera a faux une fois convergé , donc le test de conv sera un test a l'interrieur 

    while (!stop)
    {

        //single
        #pragma omp single
        {
            double *temp = vecteur0;
            vecteur0 = vecteur1;
            vecteur1 = temp;
            //  start = clock();
        }



        multvectmmat(aP,vecteur1,vecteur0,Nbsommet,Nbvaleurs);

        #pragma omp single
        {
        deuxieme_terme=0;
        }



        calculer_cst(&deuxieme_terme,vecteur0,tabD,UnSurN,Nbsommet);



        
        #pragma omp for 
        for (int i=0;i<Nbsommet;i++){
            vecteur1[i]=vecteur1[i]+deuxieme_terme;
        }

        // #pragma omp single
        // {
        // double temp=0.0;
        // for(int i=0;i<Nbsommet;i++){
        //     temp+=vecteur1[i];
        // }   
        // printf("somme du vecteur1 a l'iteration %d: %f\n", cpt, temp);
        //     cpt++;
        //     sleep(1);

        // }



        #pragma omp single
        {
            (*nbiter)++;

            if(use_mall_epsilon){
                if(norme_a1(vecteur1,vecteur0,Nbsommet)<SMALL_EPSILON){
                stop=1;
            }
            }else{
            
                if(norme_a1(vecteur1,vecteur0,Nbsommet)<EPSILON){
                    stop=1;
                }
            }
        }

    }

    
    }

    free(aP);       // Libérer aP
    //free(vecteur0); // Libérer l'autre vecteur (celui qu'on ne retourne pas)

    return vecteur1;
}


void transformation_matrice(struct contigue* tableauSommets,int nbsommet,int * nbsommet_par_cfc,int* numcfc_du_sommet,int nbcfc,int nbarcs,struct contigue*** resultat1,int** resultat2){
    
    // tableau de pointeur sur tableau contigue de sous mat cfc vertion liste chainne 
    struct neud_sommet_suc_prob **tableau_de_sousmatrice_cfc=malloc(nbcfc*sizeof(struct neud_sommet_suc_prob*));

        for(int i=0;i<nbcfc;i++){
            tableau_de_sousmatrice_cfc[i]=NULL;
        }
 
    int * nb_succeseur_par_sommet=malloc(nbsommet*sizeof(int));// vas nous servire a savoir le nombre de succeseur de chaque sommet car on retiens pas sertens arcs car non meme cfc 
    memset(nb_succeseur_par_sommet,0,nbsommet*sizeof(int));
    
    double * probabilite_cummle_par_arc_supprime=malloc(nbsommet*sizeof(double));//la c'est les probas cumule des arcs non retenue 
    memset(probabilite_cummle_par_arc_supprime,0,nbsommet*sizeof(double));


    int * nbarcs_par_cfc=malloc(nbcfc*sizeof(int));
    memset(nbarcs_par_cfc,0,nbcfc*sizeof(int));
    //remplisage des sous matrices representé par une lise chainne pour linstant et non un tableau car on 
    //ne connais pas combien il y a de d'acs dans une cfc 
    for(int i=0;i<nbarcs;i++){
        //extraction des informations de la matrice initiale
        int sommet=tableauSommets[i].pred;
        int successeur=tableauSommets[i].suc;
        double probabilite=tableauSommets[i].probabilite;

        //extraction du numero cfc du sommet 
        int numero_cfc=numcfc_du_sommet[sommet-1];
        int numero_cfc_successeur=numcfc_du_sommet[successeur-1];

        if(numero_cfc!=numero_cfc_successeur){
            probabilite_cummle_par_arc_supprime[sommet-1]+=probabilite;// on cumule la proba de tous les arcs qui partent du meme sommet et qui sont supprimé pour pouvoir les ajouter a la proba de l'arc qui vas etre retenue
            continue;// on ne garde que les arcs qui ont leur sommet et successeur dans la meme cfc 
        }
        
        nb_succeseur_par_sommet[sommet-1]++;

        //creation + ajout des infs pour le temp 
        struct neud_sommet_suc_prob * temp=malloc(sizeof(struct neud_sommet_suc_prob));
        temp->sommet=sommet;
        temp->successeur=successeur;
        temp->probabilite=probabilite;

        // l'inser tete dans le tableu de sous matrices 
        temp->suivant=tableau_de_sousmatrice_cfc[numero_cfc];
        tableau_de_sousmatrice_cfc[numero_cfc]=temp;

        nbarcs_par_cfc[numero_cfc]++;

    }


    printf("premiere partie faite\n");

    struct contigue **tableau_de_sousmatrice_cfc_contigue=malloc(nbcfc*sizeof(struct contigue*));

    if(tableau_de_sousmatrice_cfc_contigue==NULL){
        printf("we got'em");
    }

    for(int i=0;i<nbcfc;i++){
        tableau_de_sousmatrice_cfc_contigue[i]=malloc(nbarcs_par_cfc[i]*sizeof(struct contigue));
        
            if(tableau_de_sousmatrice_cfc_contigue[i]==NULL){
        printf("we got'em");
    }

        struct neud_sommet_suc_prob * temp=tableau_de_sousmatrice_cfc[i];
        int j=0;
        while(temp!=NULL){
            tableau_de_sousmatrice_cfc_contigue[i][j].pred=temp->sommet;
            tableau_de_sousmatrice_cfc_contigue[i][j].suc=temp->successeur;
            tableau_de_sousmatrice_cfc_contigue[i][j].probabilite=temp->probabilite+probabilite_cummle_par_arc_supprime[temp->sommet-1]/nb_succeseur_par_sommet[temp->sommet-1];// ici on renormalise donc on ajoute la proba cumule des arcs supprimé divisé par le nombre de succeseur du sommet pour ne pas changer la somme des proba qui partent du meme sommet et donc ne pas changer le page rank
            temp=temp->suivant;
            j++;
        }

     //   printf("la cfc %d\n",i);
    }


for (int i=0;i<nbcfc;i++){
    while(tableau_de_sousmatrice_cfc[i]!=NULL){
        struct neud_sommet_suc_prob * temp=tableau_de_sousmatrice_cfc[i];
        tableau_de_sousmatrice_cfc[i]=tableau_de_sousmatrice_cfc[i]->suivant;
        free(temp);
    }
}
free(tableau_de_sousmatrice_cfc);
free(nb_succeseur_par_sommet);
free(probabilite_cummle_par_arc_supprime);


*resultat1=tableau_de_sousmatrice_cfc_contigue;
*resultat2=nbarcs_par_cfc;
}



//cette fonction vas nous creer un graphe concentre a partir du graphe initial en ne gardant que les arcs inter cfc, et toutes les probabilites sont a 0 et vont etre mis a jour apres
/**
 * @brief 
 * 
 * @param tableauSommets 
 * @param vecteur0 
 * @param Nbsommets 
 * @param Nbarcs 
 * @param cfc 
 * @param nbcfc_totale 
 * @param nbsommets_par_cfc 
 * @param nbcfc_non_triviales 
 * @param graphe_concentre_result 
 * @param nb_arcs_graphe_concentre_result 
 * @param mapping_cfc_non_triviales_to_1 
 * @param mapping_1_to_cfc_non_triviales 
 */
void graphe_concentre(struct contigue* tableauSommets,double * vecteur0,int Nbsommets,int Nbarcs,int * cfc,int nbcfc_totale, int * nbsommets_par_cfc,struct contigue ** graphe_concentre_result, int * nb_arcs_graphe_concentre_result,struct tableauD * tabD_result){

        int nb_arcs_graphe_concentre = Nbarcs;
    

    struct tableauD tabD;
    tabD.Nbsommet=0;
    tabD.tabD=malloc(nbcfc_totale*sizeof(int));


    struct list_chainne_cfcdest_proba{
        int cfc_dest;
        double proba;
        struct list_chainne_cfcdest_proba * suiv;
    }; 

    struct list_chainne_cfcdest_proba ** tableau_cfcsucceseur=malloc(nbcfc_totale*sizeof(struct list_chainne_cfcdest_proba*));

    for(int i=0;i<nbcfc_totale;i++){
        tableau_cfcsucceseur[i]=NULL;
    }

    struct contigue * graphe_concentre=malloc(nb_arcs_graphe_concentre*sizeof(struct contigue));

    double*  tableau_proba_cumule_par_cfc=malloc(nbcfc_totale*sizeof(double));
    memset(tableau_proba_cumule_par_cfc,0,nbcfc_totale*sizeof(double));

    int index=0; // pour remplir le graphe concentre
    for(int i=0;i<Nbarcs;i++){
        int sommet=tableauSommets[i].pred;
        int successeur=tableauSommets[i].suc;
        int cfc_sommet=cfc[sommet-1];
        int cfc_successeur=cfc[successeur-1];

        if(cfc_sommet!=cfc_successeur){ // on ne garde que les arcs qui partent d'une cfc non triviale vers une cfc non triviale
          //  if( nbsommets_par_cfc[cfc_sommet]>1&&nbsommets_par_cfc[cfc_successeur]>1){
                if(vecteur0[sommet-1]*tableauSommets[i].probabilite>0){ // on ne garde que les arcs qui ont une proba non nulle dans le graphe concentre pour ne pas avoir des arcs avec une proba nulle qui vas pas changer le page rank et qui vas juste alourdir le graphe concentre et donc ralentir la convergence du page rank
                    struct list_chainne_cfcdest_proba *temp=tableau_cfcsucceseur[cfc_sommet];
                    while(temp!=NULL&&temp->cfc_dest!=cfc_successeur){
                        temp=temp->suiv;
                    }
                    if(temp==NULL){
                        temp=malloc(sizeof(struct list_chainne_cfcdest_proba));
                        temp->cfc_dest=cfc_successeur;
                        temp->proba=vecteur0[sommet-1]*tableauSommets[i].probabilite;// la proba de l'arc dans le graphe concentre est la proba de l'arc dans le graphe initial multiplié par la proba du sommet successeur dans le vecteur0 pour prendre en compte la renormalisation qui a été faite dans la transformation de la matrice
                        temp->suiv=tableau_cfcsucceseur[cfc_sommet];
                        tableau_cfcsucceseur[cfc_sommet]=temp;
                        tableau_proba_cumule_par_cfc[cfc_sommet]+=vecteur0[sommet-1]*tableauSommets[i].probabilite;
                    }
                    else{
                        temp->proba+=vecteur0[sommet-1]*tableauSommets[i].probabilite;// la proba de l'arc dans le graphe concentre est la proba de l'arc dans le graphe initial multiplié par la proba du sommet successeur dans le vecteur0 pour prendre en compte la renormalisation qui a été faite dans la transformation de la matrice
                            tableau_proba_cumule_par_cfc[cfc_sommet]+=vecteur0[sommet-1]*tableauSommets[i].probabilite;
                    }

            }
            //}   
        }
    }


    for(int i=0;i<nbcfc_totale;i++){
        struct list_chainne_cfcdest_proba * temp=tableau_cfcsucceseur[i];
         if(temp==NULL){ // si la cfc est non triviale et n'a pas de succeseur dans le graphe concentre alors on ajoute dans le tablau D la cfc normalise a 0 et on continue si non le tableau d onctiendra des cfc a -1 
            tabD.tabD[tabD.Nbsommet]=i;// dans le tableau d on stock l'index 0 based pas 1 based 
            tabD.Nbsommet++;
         }
        while(temp!=NULL){// pas la paine de verif le nombre de sommets car on ajoute des temps uniquement quand nbsommetcfc > 1
            if(tableau_proba_cumule_par_cfc[i]==0){
                printf("we got a problem\n");
                exit(1);
            }
            graphe_concentre[index].pred=i+1;
            graphe_concentre[index].suc=temp->cfc_dest+1;
            graphe_concentre[index].probabilite=temp->proba/tableau_proba_cumule_par_cfc[i];// on renormalise la proba de l'arc dans le graphe concentre en divisant par la proba cumule des arcs qui partent de la meme cfc pour que la somme des proba qui partent de la meme cfc soit 1 et donc ne pas changer le page rank
            index++;
            temp=temp->suiv;
        }
    }


    for(int i=0;i<nbcfc_totale;i++){
        struct list_chainne_cfcdest_proba * temp=tableau_cfcsucceseur[i];
        while(temp!=NULL){
            struct list_chainne_cfcdest_proba * temp2=temp;
            temp=temp->suiv;
            free(temp2);
        }
    }

    memset(tableau_proba_cumule_par_cfc,0,nbcfc_totale*sizeof(double));

    for(int i=0;i<index;i++){
        tableau_proba_cumule_par_cfc[graphe_concentre[i].pred-1]+=graphe_concentre[i].probabilite;
    }

    printf("la proba cumule pour la cfc 1 est %f\n",tableau_proba_cumule_par_cfc[1]);
    for(int i=0;i<nbcfc_totale;i++){
        if(tableau_proba_cumule_par_cfc[i]!=1&&tableau_proba_cumule_par_cfc[i]!=0){
            printf("we got a problem in the renormalisation of the graphe concentre %lf %d\n",tableau_proba_cumule_par_cfc[i],i);
        }
        else if(tableau_proba_cumule_par_cfc[i]==0){
            int j=0;
            while(j<tabD.Nbsommet){
                if(tabD.tabD[j]==i){
                    break;
                }
                j++;
            }
            if(j==tabD.Nbsommet){
                printf("we got a problem in the construction of the tableau D for cfc %d\n",i);
                exit(1);
            }
        }
    }

    free(tableau_cfcsucceseur);
    free(tableau_proba_cumule_par_cfc);
    graphe_concentre=realloc(graphe_concentre,(index)*sizeof(struct contigue));

     *graphe_concentre_result=graphe_concentre;
     *nb_arcs_graphe_concentre_result=index;
     *tabD_result=tabD;
}


void ecritutrfichier(struct contigue* graphe_concentre,int nb_arcs_graphe_concentre, char* nom_fichier){
    FILE *f=fopen(nom_fichier,"w");
    if(f==NULL){
        printf("erreur ouverture fichier\n");
        exit(1);
    }
    fprintf(f,"%d %d %d\n",nb_arcs_graphe_concentre,nb_arcs_graphe_concentre,nb_arcs_graphe_concentre);
    for(int i=0;i<nb_arcs_graphe_concentre;i++){
        fprintf(f,"%d %d %.15lf\n",graphe_concentre[i].pred,graphe_concentre[i].suc,graphe_concentre[i].probabilite);
    }
    fclose(f);
}

void ecrituretabD(struct tableauD tabD, char* nom_fichier){
    FILE *f=fopen(nom_fichier,"w");
    if(f==NULL){
        printf("erreur ouverture fichier\n");
        exit(1);
    }
    fprintf(f,"%d\n",tabD.Nbsommet);
    for(int i=0;i<tabD.Nbsommet;i++){
        fprintf(f,"%d\n",tabD.tabD[i]);
    }
    fclose(f);
}


int main(){

int Nbsommet = 0;
int Nbacrs=0;

int Nblignes=0;
int Nbcol=0;
int Nbval=0;



struct tableau_liste_chainne graph_suc;
graph_suc.Nbsommet=0;
graph_suc.tab=NULL;
struct tableau_liste_chainne graph_pred;
graph_pred.Nbsommet=0;
graph_pred.tab=NULL;




/*initialisation du tableu D qui est est ce que la ligne na pas de suc*/
struct tableauD* tabD=malloc(sizeof(struct tableauD));



/*POUR LE FORMAT MTX*/
struct contigue *tableauSommets = lecturefichier_matrixm("wiki.mtx",&Nblignes,&Nbacrs,&Nbval,&tabD,&graph_suc,&graph_pred);
//afficher_contigue(tableauSommets,Nbval);
Nbsommet=Nblignes;








// afficher_graph_suc_pred(graph_suc);

// printf("les predecesseurs");
// afficher_graph_suc_pred(graph_pred);




int nbcfc;// nombre de composantes fortement connexes
int *nb_sommets_par_cfc;

int* cfc=kosaraju(graph_suc,graph_pred,Nbsommet,&nbcfc,&nb_sommets_par_cfc);//cfc[i] est le numero de la composante fortement connexe du sommet i+1


for(int i=0;i<Nbsommet;i++){
    struct list_chainne_de_sommet* temp=  graph_suc.tab[i];
    while(graph_suc.tab[i]!=NULL){
        graph_suc.tab[i]=graph_suc.tab[i]->suiv;
        free(temp);
        temp=graph_suc.tab[i];
    }

        temp=  graph_pred.tab[i];
    while(graph_pred.tab[i]!=NULL){
        graph_pred.tab[i]=graph_pred.tab[i]->suiv;
        free(temp);
        temp=graph_pred.tab[i];
    }

}




printf("kosaraju done\n");  


printf("le nombre de sommets est %d\n", Nbsommet);
printf("le nombre de cfc est %d\n", nbcfc);



struct contigue **tableau_de_sousmatrice_cfc_contigue;
int *nbarcs_par_cfc;
transformation_matrice(tableauSommets,Nbsommet,nb_sommets_par_cfc,cfc,nbcfc,Nbval,&tableau_de_sousmatrice_cfc_contigue,&nbarcs_par_cfc);



struct contigue * graphe_conc;
int nb_arcs_graphe_conc;

printf("transformation de la matrice faite\n");


//===========================================

double *vecteur0=malloc(Nbsommet*sizeof(double));
memset(vecteur0,0,Nbsommet*sizeof(double));

int * cfc_traite=malloc(nbcfc*sizeof(double));

memset(cfc_traite,0,nbcfc*sizeof(int));


// //on donne le numero de la cfc et il nous rend le numero de la cfc dans l'espace reduit commancant par 0
// int * mapping_cfc_non_triviales_to_1=malloc(nbcfc*sizeof(int));
// memset(mapping_cfc_non_triviales_to_1,-1,nbcfc*sizeof(int));

// int * mapping_1_to_cfc_non_triviales=malloc(nbcfc*sizeof(int));
// memset(mapping_1_to_cfc_non_triviales,-1,nbcfc*sizeof(int));

// int index_non_triviale=1;

// for(int i=0;i<Nbsommet;i++){
//     int numero_cfc=cfc[i];
//     if(nb_sommets_par_cfc[numero_cfc]>1)
//         vecteur0[i]=1.0/nb_sommets_par_cfc[numero_cfc];// pour les cfc non triviale on initialise le vecteur de base a la distribution uniforme pour que la recherche du page rank local converge plus vite
//         if(cfc_traite[numero_cfc]==0){
//         cfc_traite[numero_cfc]=1;
//         if(nb_sommets_par_cfc[numero_cfc]>1){
//             mapping_cfc_non_triviales_to_1[numero_cfc]=index_non_triviale;
//             mapping_1_to_cfc_non_triviales[index_non_triviale]=numero_cfc;
//             index_non_triviale++;
//         }
//     }
// }

// mapping_1_to_cfc_non_triviales = realloc(mapping_1_to_cfc_non_triviales,index_non_triviale*sizeof(int));







printf("la somme des elements du vecteur0 est %f\n", sum);
printf("le nombre de cfc de base est %d\n", nbcfc);

//===========================================



for(int i=0;i<Nbsommet;i++){
    int numero_cfc=cfc[i];
        vecteur0[i]=1.0/nb_sommets_par_cfc[numero_cfc];// pour les cfc non triviale on initialise le vecteur de base a la distribution uniforme pour que la recherche du page rank local converge plus vite
}


sum=0;
for(int i=0;i<Nbsommet;i++){
    sum+=vecteur0[i];
}

printf("la somme des elements du vecteur0 est %f\n", sum);
printf("dois ertre egale a %d\n", nbcfc);



int cc=0;


for (int i=0;i<nbcfc;i++){
    if(nb_sommets_par_cfc[i]>1){
        vecteur0=recherchep_sans_surf(tableau_de_sousmatrice_cfc_contigue[i],
            vecteur0,
            nbarcs_par_cfc[i],
            Nbsommet,
            nb_sommets_par_cfc[i],
            nbarcs_par_cfc[i]
        );
    }
}

printf("recherche sans surf done\n");


        sum=0;
    for(int i=0;i<Nbsommet;i++){
        sum+=vecteur0[i];
    }   
    printf("la somme des elements du vecteur0 est %f\n", sum);

int nbiter=0;
struct tableauD tabD_graphe_concentre;


graphe_concentre(tableauSommets,
    vecteur0,
    Nbsommet,
    Nbval,
    cfc,
    nbcfc,
    nb_sommets_par_cfc,
    &graphe_conc,
    &nb_arcs_graphe_conc,
    &tabD_graphe_concentre
);

printf("graphe concentre done\n");



double* vecteur_graph_reduit= malloc(nbcfc*sizeof(double));
memset(vecteur_graph_reduit, 0, nbcfc*sizeof(double));


vecteur_graph_reduit=recherchep(graphe_conc,
    vecteur_graph_reduit,
    &tabD_graphe_concentre,
    nbcfc,
    nb_arcs_graphe_conc,
    &nbiter,
    1
    );


//    vecteur_graph_reduit=recherchep(tableauSommets,vecteur0,tabD,Nbsommet,Nbval,&nbiter);


double titi=0;

for(int i=0;i<nbcfc;i++){
    titi+=vecteur_graph_reduit[i];
}
printf("la somme des elements du vecteur_graph_reduit est %f\n", titi);


for(int i=0;i<nbcfc;i++){
    free(tableau_de_sousmatrice_cfc_contigue[i]);
}

free(tableau_de_sousmatrice_cfc_contigue);
free(nbarcs_par_cfc);


// Combiner : distribution locale × poids global de la CFC
// for(int i = 0; i < Nbsommet; i++){
//     int cfc_sommet = cfc[i];
//     int mapped = mapping_cfc_non_triviales_to_1[cfc_sommet];
//     if(mapped > 0){
//         // mapped est 1-based, vecteur_graph_reduit est 0-based
//         vecteur0[i] *= vecteur_graph_reduit[mapped - 1];
//     } else {
//         // CFC triviale : PageRank local nul pour l'instant
//         vecteur0[i] = 0.0;
//     }
// }

// // Vérification : la somme doit être ~1
// double check = 0;
// for(int i = 0; i < Nbsommet; i++) check += vecteur0[i];
// printf("Somme vecteur0 avant recherchep final (doit ≈ 1): %f\n", check);
// sleep(5);

// free(cfc);

// double somme=0;
// for(int i=0;i<Nbsommet;i++){   
//     somme+=vecteur0[i];
// }



// printf("la somme des elements du vecteur0 est %f\n", somme);

// exit(0);

printf("combinaison done\n");

double a = 0.1; // à tuner entre 0 et 1

for(int i=0;i<Nbsommet;i++){
    int cfc_sommet = cfc[i];
    double poids_structure = vecteur_graph_reduit[cfc_sommet];
    double poids_taille = (double)nb_sommets_par_cfc[cfc_sommet] / Nbsommet;
    vecteur0[i] *= a * poids_structure + (1-a) * poids_taille;
}

double papa=0;
for(int i=0;i<Nbsommet;i++){   
    papa+=vecteur0[i];
}
printf("Somme finale: %f\n", papa);

nbiter=0;
vecteur0=recherchep(tableauSommets,vecteur0,tabD,Nbsommet,Nbval,&nbiter,0);

 double somme=0;
for(int i=0;i<Nbsommet;i++){
    somme+=vecteur0[i];
}

printf("FIN la somme des elements du vecteur0 est %f\n", somme);

printf("le nombre d'iteration est %d\n", nbiter);  


//a faire je dois creer un autre tableau de trie par l'apartencance des sommets a leur cfc en utilisent le nombre de cfc par sommet, 
// je creer un nouveau tableau contigue 
//je parcoure une seul fois l'ancien tableau contigue et je remplis le nouveau en utilisant le numero de la cfc de chaque sommet 





// printf("testt");
// double tab_alpha[19];
// for(int i=0; i<19; i++){
//     tab_alpha[i]=0.05*(i+1);  // 0.05, 0.10, ..., 1.0
// }
// double tab_epsilone[10]={1e-1, 1e-2, 1e-3, 1e-4, 1e-5, 1e-6, 1e-7, 1e-8, 1e-9, 1e-10};


// double temps[10][10][10];
// for(int i=0;i<10;i++){
//     for(int j=0;j<10;j++){
//         for(int k=0;k<10;k++){
//             temps[i][j][k]=0;
//         }
//     }
// }

// FILE*file = fopen("resultats.txt", "w");
// if(file == NULL) {
//     printf("Erreur ouverture fichier resultats.txt\n");
//     return 1;
// }
// setvbuf(file, NULL, _IONBF, 0);  // Pas de buffer, écriture immédiate

// fprintf(file, "alpha, epsilon, num_threads, temps_execution\n");
// fprintf(file, "varriation pour le alpha \n");

// for(int j=0;j<18;j++){
//     ALPHA=tab_alpha[j];
    
//     double *vecteur0=malloc(Nbsommet*sizeof(double));
//     vecteur0[0]=1;
//     for(int j=1;j<Nbsommet;j++){
//         vecteur0[j]=0;
//     }
//     int nbiter=0;
//         double start = omp_get_wtime();  // <-- pas clock()
//     vecteur0=recherchep(tableauSommets,vecteur0,tabD,Nbsommet,Nbval,&nbiter);
//     double end = omp_get_wtime();


//     double temps_execution = end - start;
//     fprintf(file, "%f, %e, %d, %f\n", ALPHA, EPSILON, NUM_THREADS, temps_execution);
//     free(vecteur0);
// }


// printf("first part done\n");
// sleep(5);

// fprintf(file, "\n");

// // fprintf(file, "varriation pour le alpha & epsilon\n");

// // for(int i=0;i<10;i++)
// // for(int j=0;j<18;j++){
// //     ALPHA=tab_alpha[j];
// //     EPSILON=tab_epsilone[i];

// //     double *vecteur0=malloc(Nbsommet*sizeof(double));
// //     vecteur0[0]=1;
// //     for(int j=1;j<Nbsommet;j++){
// //         vecteur0[j]=0;
// //     }


// //     double start = omp_get_wtime();
// //     vecteur0=recherchep(tableauSommets,vecteur0,tabD,Nbsommet,Nbval);
// //     double end = omp_get_wtime();


// //     double temps_execution = end - start;
// //     fprintf(file, "%f, %e, %d, %f\n", ALPHA, EPSILON, NUM_THREADS, temps_execution);
// //     free(vecteur0);
// // }

// // printf("second part done\n");
// // sleep(5);

// fprintf(file, "\n");

// fprintf(file, "varriation pour le  epsilon \n");

// ALPHA=0.9;
// for(int j=0;j<10;j++){
//     EPSILON=tab_epsilone[j];

//     double *vecteur0=malloc(Nbsommet*sizeof(double));
//     vecteur0[0]=1;
//     for(int j=1;j<Nbsommet;j++){
//         vecteur0[j]=0;
//     }

//     int nbiter=0;
//     double start = omp_get_wtime();
//     vecteur0=recherchep(tableauSommets,vecteur0,tabD,Nbsommet,Nbval,&nbiter);
//     double end = omp_get_wtime();


//     double temps_execution = end - start;
//     fprintf(file, "%f, %e, %d, %f\n", ALPHA, EPSILON, NUM_THREADS, temps_execution);
//     free(vecteur0);
// }


// printf("third part done\n");

   // fclose(file);
    free(tableauSommets);
    free(tabD->tabD);
    free(tabD);




//affichertab(tableauSommets,Nbsommet);
// double *vecteur0=malloc(Nbsommet*sizeof(double));
//     vecteur0[0]=1;
//     for(int i=1;i<Nbsommet;i++){
//         vecteur0[i]=0;
//     }


//     vecteur0=recherchep(tableauSommets,vecteur0,tabD,Nbsommet,Nbval);

//     fclose(file);
//     free(vecteur0);
//     free(tableauSommets);


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