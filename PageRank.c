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

int NUM_THREADS=10;
double TRESHOLD= 0.00001;

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

struct IntStack{
    int *data;
    int top;
    int capacity;
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





struct contigue * lecturefichier_matrixm(const char* filename,int *NbLignes,int *Nbcol, int *Nbval,struct tableauD** tabD,struct tableau_liste_chainne* graphe_suc,struct tableau_liste_chainne* graphe_pred,int create_graphe_suc_pred){
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
    if(create_graphe_suc_pred){
    (*graphe_suc).tab=malloc(*Nbcol*sizeof(struct list_chainne_de_sommet*));
    (*graphe_pred).tab=malloc(*Nbcol*sizeof(struct list_chainne_de_sommet*));
    


     for(int i=0;i<*Nbcol;i++){
        graphe_pred->tab[i]=NULL;
        graphe_suc->tab[i]=NULL;
     }
    
     graphe_pred->Nbsommet=*Nbcol;

     graphe_suc->Nbsommet=*Nbcol;
}

        for(int i=0;i<*Nbval;i++){
        int pred=0,suc=0;
        
        //fscanf(file, "%d %d", &pred, &suc);
        // Lire avec fgets (plus safe)
        char line[256];
        fgets(line, sizeof(line), file);
        sscanf(line, "%d %d", &pred, &suc);  // Prend que les 2 premiers valeurs 
        if(create_graphe_suc_pred){
            graphe_suc->tab[pred-1]=stack(graphe_suc->tab[pred-1],suc); //ici stack joue le role de inser tete 
            graphe_pred->tab[suc-1]=stack(graphe_pred->tab[suc-1],pred);
        }

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
    fclose(file);
    return tabcontigue;
}



/**
 * @brief clacule le seuil dynamique pour la suppression des epsilon transitions dans le graphe de PageRank.
 */
double dynamic_threshold(struct contigue *tableauSommets, int Nbvaleurs) {
    // Calcul moyenne
    double mean = 0;
    for(int i = 0; i < Nbvaleurs; i++) {
        mean += tableauSommets[i].probabilite;
    }
    mean /= Nbvaleurs;
    
    // Calcul variance et écart-type
    double variance = 0;
    for(int i = 0; i < Nbvaleurs; i++) {
        double diff = tableauSommets[i].probabilite - mean;
        variance += diff * diff;
    }
    variance /= Nbvaleurs;
    double sigma = sqrt(variance);
    
    // Seuil dynamique
    double threshold = mean - 2 * sigma;
    if( threshold < 0) {// souvant negatif si les valeurs sont dispersees  de la moyenne 
        threshold = mean*0.1; //Si une transition est 10 fois plus faible que la moyenne, je considère que c’est une erreur ou un lien insignifiant
    }

    return threshold;
}



/**
 * @brief Supprime les transitions faibles (epsilon transitions) selon un seuil dynamique ou statique.
 * 
 * Filtre les arcs en ne conservant que ceux avec probabilité >= seuil. Reconstruit :
 * - Le tableau contiguë des arcs
 * - Les graphes de successeurs/prédécesseurs
 * - Le tableau des nœuds dangling (tabD)
 * 
 * @param tableauSommets Pointeur vers tableau d'arcs (sera remplacé, l'ancien EST LIBÉRÉ)
 * @param Nbsommets Nombre de sommets
 * @param Nbvaleurs Pointeur vers nombre d'arcs (mis à jour avec la nouvelle taille)
 * @param tabD Tableau des nœuds dangling (mis à jour, l'ancien EST LIBÉRÉ)
 * @param graph_suc Graphe des successeurs (sera remplacé, L'ANCIEN DOIT ÊTRE LIBÉRÉ PAR L'APPELANT)
 * @param graph_pred Graphe des prédécesseurs (sera remplacé, L'ANCIEN DOIT ÊTRE LIBÉRÉ PAR L'APPELANT)
 * @param use_dynamic_threshold 1 = seuil dynamique (mean - 2*sigma), 0 = TRESHOLD constant
 * 
 * @warning ⚠️ RESPONSABILITÉ DE L'APPELANT : Libérer graph_suc->tab et graph_pred->tab AVANT d'appeler cette fonction
 *          Sinon : fuite mémoire des anciennes listes chaînées
 * 
 * @note Cette fonction libère automatiquement tabD->tabD et tableauSommets
 * 
 * @example
 * // Libérer avant appel :
 * for(int i = 0; i < graph_suc.Nbsommet; i++) {
 *     libmemstack(&graph_suc.tab[i]);
 *     libmemstack(&graph_pred.tab[i]);
 * }
 * free(graph_suc.tab);
 * free(graph_pred.tab);
 * 
 * // Puis appeler :
 * suppression_epsilon_transitions(&tableauSommets, Nbsommets, &Nbvaleurs, &tabD, &graph_suc, &graph_pred, 1);
 */
void suppression_epsilon_transitions( struct contigue** tableauSommets,int Nbsommets,int* Nbvaleurs,struct tableauD* tabD, struct tableau_liste_chainne* graph_suc, struct tableau_liste_chainne* graph_pred,int use_dynamic_threshold){
    double threshold;

    if(use_dynamic_threshold){
         threshold=dynamic_threshold(*tableauSommets,*Nbvaleurs);
    }else{
         threshold=TRESHOLD;
    }
    printf("Seuil de suppression des epsilon transitions: %lf\n", threshold);

    sleep(5); 
    struct contigue* new_tableauSommets=malloc((*Nbvaleurs)*sizeof(struct contigue));
    int newnbvaleurs=0;    
    struct tableauD new_tabD;
    new_tabD.tabD=malloc(Nbsommets*sizeof(int));
    memset(new_tabD.tabD, 0, Nbsommets*sizeof(int));
    new_tabD.Nbsommet=0;

    struct tableau_liste_chainne new_graph_suc={
        .tab=malloc(Nbsommets*sizeof(struct list_chainne_de_sommet*)),
        .Nbsommet=Nbsommets
    };
    struct tableau_liste_chainne new_graph_pred={
        .tab=malloc(Nbsommets*sizeof(struct list_chainne_de_sommet*)),
        .Nbsommet=Nbsommets
    };
    for(int i=0;i<Nbsommets;i++){
        new_graph_suc.tab[i]=NULL;
        new_graph_pred.tab[i]=NULL;
    }



    int * nb_sucasseur_par_sommet=malloc(Nbsommets*sizeof(int));
    memset(nb_sucasseur_par_sommet, 0, Nbsommets*sizeof(int));


    for(int i=0;i<*Nbvaleurs;i++){
        if((*tableauSommets)[i].probabilite>=threshold){
            int pred=(*tableauSommets)[i].pred;
             int suc=(*tableauSommets)[i].suc;
             double probabilite=(*tableauSommets)[i].probabilite;


            new_tableauSommets[newnbvaleurs].pred=pred;
            new_tableauSommets[newnbvaleurs].suc=suc;
            new_tableauSommets[newnbvaleurs].probabilite=probabilite;
            nb_sucasseur_par_sommet[(*tableauSommets)[i].pred-1]++;

            new_graph_pred.tab[suc-1]=stack(new_graph_pred.tab[suc-1],pred);
            new_graph_suc.tab[pred-1]=stack(new_graph_suc.tab[pred-1],suc);
            newnbvaleurs++;
        }
    }

    for(int i=0;i<Nbsommets;i++){
        if(nb_sucasseur_par_sommet[i]==0){
            new_tabD.tabD[new_tabD.Nbsommet]=i;
            new_tabD.Nbsommet++;
        }
    }

    new_tableauSommets = realloc(new_tableauSommets, newnbvaleurs * sizeof(struct contigue));
    new_tabD.tabD= realloc(new_tabD.tabD, new_tabD.Nbsommet * sizeof(int));
    if(new_tableauSommets == NULL || new_tabD.tabD == NULL) {
        fprintf(stderr, "Erreur de réallocation mémoire\n");
        exit(EXIT_FAILURE);
    }

    free(tabD->tabD);
    free(*tableauSommets);
    *tableauSommets=new_tableauSommets;
    *Nbvaleurs=newnbvaleurs;
    *tabD= new_tabD;
    *graph_suc=new_graph_suc;
    *graph_pred=new_graph_pred;
    

    printf("fin de supp");
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


/**
 * @brief Multiplication matrice-vecteur pour une CFC isolée, avec réinitialisation partielle (version itérative sans parallélisation).
 * 
 * Effectue une multiplication matrice-vecteur A·v où seuls les sommets de la CFC actuelle 
 * sont modifiés. Utile pour calculer le PageRank local à l'intérieur d'une CFC non triviale.
 * 
 * Algorithme :
 * 1. Copie complète de vecteur0 vers vecteur1 (tout le vecteur global)
 * 2. Réinitialise à 0 les éléments de vecteur1 pour les successeurs de la CFC (Nbarcs sommets)
 * 3. Accumule les contributions matrice-vecteur pour les arcs : vecteur1[suc-1] += vecteur0[pred-1] * prob
 * 
 * Cette approche isole le calcul de PageRank à une CFC donnée en neutralisant les contributions
 * extérieures (provenant d'autres CFCs).
 * 
 * @param tableauSommets Tableau contiguë des arcs de la CFC (format: predecesseur, successeur, probabilité)
 * @param vecteur1 Vecteur résultat (sortie, de taille Nbsommet_globale, modifié)
 * @param vecteur0 Vecteur d'entrée (lecture seule, de taille Nbsommet_globale)
 * @param Nbarcs Nombre d'arcs dans la CFC actuelle (détermine quels éléments sont réinitialisés à 0)
 * @param Nbsommet Nombre de sommets dans la CFC actuelle
 * @param Nbsommet_globale Nombre total de sommets dans le graphe global
 * @param Nbvaleurs Nombre total d'arcs à traiter (itérations de la boucle de calcul)
 * 
 * @note Exécution séquentielle (sans OpenMP). Utilisée pour les CFCs non triviales au cours
 *       de la première phase de calcul de PageRank hiérarchique (recherchep_sans_surf).
 *       Les éléments du vecteur hors de la CFC restent identiques à vecteur0.
 */
 void multvectmmat_old_sans_surf(struct contigue* tableauSommets,double* vecteur1, double *vecteur0,int Nbarcs, int Nbsommet, int Nbsommet_globale, int Nbvaleurs) {
    // float *vecteur1=malloc(Nbsommet*sizeof(float));
   #pragma omp single
    memcpy(vecteur1,vecteur0,Nbsommet_globale*sizeof(double));
    
    #pragma omp for
    for(int i=0;i<Nbarcs;i++){
        int t=tableauSommets[i].suc-1;
        #pragma omp critical
        vecteur1[t]=0;
    }// ici ce quon fait c'est que on le vecteur 1 dois recevoir une copie du vecteur 0 sauf pour les sommet de la cfc actuelle qu'on dois chercher a calculer la distribution stat 
    
    #pragma omp for
    for(int i=0;i<Nbvaleurs;i++){
        double sum=0.0;
        int pred=tableauSommets[i].pred;
        int suc=tableauSommets[i].suc;
        double prob=tableauSommets[i].probabilite;
        
        #pragma omp atomic
        vecteur1[suc-1]+=vecteur0[pred-1]*prob;//pas oublier les -1 car les sommets commencent à 1 dans le fichier
        
}
}


/**
 * @brief Effectue la multiplication matrice-vecteur (A·v) en parallèle avec OpenMP.
 * 
 * Multiplie le tableau contiguë du graphe (représentant la matrice d'adjacence pondérée)
 * par le vecteur d'entrée vecteur0, et accumule le résultat dans vecteur1 avec opérations atomiques
 * pour éviter les conditions de course. Chaque arc (pred → suc) contribue à vecteur1[suc-1].
 * 
 * @param tableauSommets Tableau contiguë du graphe (format: predecesseur, successeur, probabilité)
 * @param vecteur1 Vecteur résultat (sortie, de taille Nbsommet) - sera réinitialisé à zéro
 * @param vecteur0 Vecteur d'entrée (lecture seule, de taille Nbsommet)
 * @param Nbsommet Nombre de sommets du graphe
 * @param Nbvaleurs Nombre d'arcs dans le graphe
 * 
 * @note Parallélisée avec #pragma omp for. Les opérations atomiques garantissent la cohérence
 *       lors d'accès concurrents au même élément de vecteur1. Le zéro initial est effectué 
 *       en section critique (#pragma omp single).
 */
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


/**
 * @brief Calcule la norme L1 (Manhattan) entre deux vecteurs : sum(|v1[i] - v0[i]|).
 * 
 * Utilisée comme critère de convergence dans l'algorithme d'itération de puissance du PageRank.
 * 
 * @param vecteur1 Premier vecteur (de taille Nbsommet)
 * @param vecteur0 Deuxième vecteur (de taille Nbsommet)
 * @param Nbsommet Nombre d'éléments dans chaque vecteur
 * 
 * @return La norme L1 (distance Manhattan) entre vecteur1 et vecteur0
 * 
 * @note Affiche la norme calculée sur stdout avec printf
 */
double norme_a1(double *vecteur1, double *vecteur0, int Nbsommet) {
     double sum=0.0;
    for(int i=0;i<Nbsommet;i++){
        sum+=fabs(vecteur1[i]-vecteur0[i]);
    }

   printf("Norme L1: %Lf\n", sum);
    return sum;
}





/**
 * @brief Calcule le produit scalaire de deux vecteurs : sum(vect_ligne[i] * vect_col[i]).
 * 
 * @param vect_ligne Premier vecteur (de taille nbrdesommet)
 * @param vect_col Deuxième vecteur (de taille nbrdesommet)
 * @param nbrdesommet Nombre d'éléments dans chaque vecteur
 * 
 * @return Le produit scalaire des deux vecteurs
 */
double mult_vectligne_vectcol(double *vect_ligne,double* vect_col,int nbrdesommet){
    double res=0;
for (int i=0;i<nbrdesommet;i++){
    res+=vect_ligne[i]*vect_col[i];
}
return res;
}
/**
 * @brief Multiplie tous les éléments d'un tableau de contigue par un scalaire en parallèle.
 * 
 * Crée d'abord une copie du tableau source tab dans res, puis multiplie chaque probabilité 
 * par le scalaire donné. Utilisé pour calculer α·P (avec α = facteur d'amortissement).
 * 
 * @param res Tableau résultat (sortie, alloué au préalable)
 * @param tab Tableau source (lecture seule)
 * @param scalair Multiplicateur (généralement ALPHA = 0.9 pour PageRank)
 * @param Nbvaleurs Nombre d'éléments dans le tableau
 * 
 * @note Parallélisée avec #pragma omp for. La copie initiale s'effectue en section critique.
 */
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
/**
 * @brief Multiplie tous les éléments d'un vecteur par un scalaire en série.
 * 
 * @param vecteur Vecteur à multiplier (modifié in-place, de taille nbsommet)
 * @param scalair Multiplicateur
 * @param nbsommet Nombre d'éléments dans le vecteur
 * 
 * @return Le vecteur multiplié (même adresse que vecteur)
 */
double * mult_vect_scalair(double *vecteur,double scalair,int nbsommet){
    for(int i=0;i<nbsommet;i++){
        vecteur[i]=vecteur[i]*scalair;
    }
    return vecteur;
}

/**
 * @brief Calcule le produit scalaire restreint aux nœuds dangling avec réduction OpenMP.
 * 
 * Effectue sum(vect_ligne[tabD->tabD[i]] * UnSurN_mult_alpha) où tabD contient les indices
 * des nœuds sans successeurs (dangling nodes). Utilisé pour calculer la contribution de 
 * redistribution uniforme du vecteur PageRank.
 * 
 * @param res Pointeur vers le résultat (sortie, simple double)
 * @param vect_ligne Vecteur source (de taille ≥ max(tabD->tabD[i]))
 * @param tabD Structure tableau des nœuds dangling (tabD->tabD[] contient les indices, tabD->Nbsommet le count)
 * @param UnSurN_mult_alpha Coefficient préalablement calculé = (1/N) * ALPHA
 * 
 * @note Utilise #pragma omp for reduction(+:sum) pour accumulation parallèle sûre.
 *       Stocke la somme dans la variable globale `sum` puis la copie dans *res.
 */
double mult_vectligne_vectcol_BIS(double* res, double *vect_ligne,struct tableauD *tabD,double UnSurN_mult_alpha){
    #pragma omp single
     sum=0;
    #pragma omp for reduction(+:sum)
    for(int i =0;i<tabD->Nbsommet;i++){
        sum+=vect_ligne[tabD->tabD[i]]*UnSurN_mult_alpha;
    }
    *res=sum;
}

/**
 * @brief Calcule la constante de renormalisation pour l'itération PageRank.
 * 
 * La constante est composée de deux termes :
 * - Terme 1 : contribution des nœuds dangling (redistribution)
 * - Terme 2 : contribution uniforme (1-ALPHA) / N
 * 
 * La valeur totale (constante = Terme1 + Terme2) est ajoutée uniformément à tous les éléments
 * du prochain vecteur itéré pour garantir la normalisation.
 * 
 * @param res Pointeur vers le résultat (sortie)
 * @param veucteur Vecteur actuel du PageRank (lecture seule)
 * @param tabD Tableau des nœuds dangling (sommets sans successeurs)
 * @param unsurN Coefficient préalablement calculé = 1 / nombre de sommets
 * @param nbelts Nombre total de sommets (pour information/débogage)
 * 
 * @note Exécutée en section critique (#pragma omp single) car met à jour *res une seule fois par itération
 */
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



/**
 * @brief Affiche le contenu d'un tableau contiguë au format (Pred, Suc, Probabilite).
 * 
 * @param tab Tableau contiguë du graphe (format: predecesseur, successeur, probabilité)
 * @param Nbvaleurs Nombre d'éléments dans le tableau
 */
void afficher_contigue(struct contigue* tab,int Nbvaleurs){
    for(int i=0;i<Nbvaleurs;i++){
        printf("Pred: %d, Suc: %d, Probabilite: %f\n", tab[i].pred, tab[i].suc, tab[i].probabilite);
    }
}


/**
 * @brief Crée un nouveau nœud de liste chaînée contenant un sommet.
 * 
 * Alloue dynamiquement un nœud initialisé avec le sommet donné et le pointeur suivant à NULL.
 * 
 * @param sommet Identifiant du sommet à stocker dans le nœud
 * @return Pointeur vers le nœud fraîchement alloué
 * 
 * @note L'appelant doit libérer la mémoire après utilisation
 */
struct list_chainne_de_sommet* creer_liste(int sommet){
    struct list_chainne_de_sommet* nouvelle_liste=malloc(sizeof(struct list_chainne_de_sommet));
    nouvelle_liste->sommet=sommet;
    nouvelle_liste->suiv=NULL;
    return nouvelle_liste;
}

struct IntStack* creer_intstack(int capacity){
    struct IntStack *stack=malloc(sizeof(struct IntStack));
    stack->data=malloc(capacity*sizeof(int));
    stack->top=-1;
    stack->capacity=capacity;
    return stack;
}

void pop_intstack(struct IntStack *stack){
    if(stack->top==-1){
    return;
    }
    int sommet=stack->data[stack->top];
    stack->top--;
}
int peek_intstack(struct IntStack *stack){
    if(stack->top==-1){
        return -1; // ou une autre valeur pour indiquer que la pile est vide
    }
    return stack->data[stack->top];
}

void stack_intstack(struct IntStack *stack,int sommet){
    if(stack->top==stack->capacity-1){
        // redimensionner la pile
        exit(EXIT_FAILURE);
    }
    stack->top++;
    stack->data[stack->top]=sommet;
}

void libmem_intstack(struct IntStack *stack){
    free(stack->data);
    free(stack);
}

/**
 * @brief Retire et désalloue le premier nœud de la pile (LIFO).
 * 
 * @param liste Pointeur vers le sommet de la pile
 * @return Pointeur vers le nouveau sommet de la pile (ou NULL si vide)
 */
struct list_chainne_de_sommet* pop(struct list_chainne_de_sommet* liste){
    if(liste==NULL){
        return NULL;
    }
    struct list_chainne_de_sommet* temp=liste;
    liste=liste->suiv;
    free(temp);
    return liste;
}

/**
 * @brief Retourne le sommet du premier nœud sans le modifier.
 * 
 * @param list Pointeur vers le premier nœud de la liste
 * @return Identifiant du sommet, ou -1 si la liste est vide
 */
int peek(struct list_chainne_de_sommet* list){
    if(list==NULL){
        return -1; // ou une autre valeur pour indiquer que la liste est vide
    }
    return list->sommet;
}

/**
 * @brief Ajoute un nouveau sommet en tête de la pile (insertion LIFO).
 * 
 * Crée un nouveau nœud et l'insère en tête de la pile chaînée existante.
 * 
 * @param liste Pointeur vers le sommet actuel de la pile
 * @param sommet Identifiant du sommet à ajouter
 * @return Pointeur vers le nouveau sommet de la pile
 * 
 * @note Modifie la structure de la pile par insertion en tête
 */
struct list_chainne_de_sommet* stack(struct list_chainne_de_sommet* liste,int sommet){
    struct list_chainne_de_sommet* temp=creer_liste(sommet);
    temp->suiv=liste;
    liste=temp;
    return liste;
}


/**
 * @brief Trouve le prochain sommet non visité dans un tableau de visite.
 * 
 * Parcourt le tableau à partir de l'index *depart pour trouver la première position 
 * où tab[i] == -1 (non visité). Met à jour *depart pour pointer après le sommet trouvé.
 * 
 * @param tab Tableau de visite où -1 indique non visité
 * @param n Taille du tableau
 * @param depart Pointeur vers l'index de départ (mis à jour après recherche)
 * @return Identifiant du premier sommet non visité, ou -1 si tous les sommets ont été visités
 * 
 * @note Modifie la valeur pointée par depart (effet de bord)
 */
int tout_visite(int *tab,int n,int *depart){
    while(*depart<n && tab[*depart]!=-1){
        (*depart)++;
    }


    if((*depart)<n){
        return *depart;
    }
    else return -1;
}



/**
 * @brief Libère toute la mémoire d'une liste chaînée et annule le pointeur.
 * 
 * Parcourt la liste et désalloue chaque nœud séquentiellement. 
 * Utile pour nettoyer les listes après leur utilisation.
 * 
 * @param liste Pointeur vers le pointeur du premier nœud (double indirection)
 * 
 * @note La liste pointée par liste est vidée complètement
 */
void libmemstack(struct list_chainne_de_sommet **liste){
    struct list_chainne_de_sommet *temp;

    while((*liste)!=NULL){
        temp=(*liste);
        (*liste)=(*liste)->suiv;
        free(temp);
    }
    liste=NULL;

}

/**
 * @brief Affiche tous les identifiants de sommets d'une liste chaînée (un par ligne).
 * 
 * @param liste Pointeur vers le premier nœud de la liste
 */
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





/**
 * @brief Détecte les composantes fortement connexes (CFC) d'un graphe orienté via l'algorithme de Kosaraju itératif.
 * 
 * Implémente l'algorithme de Kosaraju en deux passes itératives (sans récursion) :
 * 1. Premier DFS sur le graphe normal pour enregistrer l'ordre de visite des sommets
 * 2. Deuxième DFS sur le graphe inversé (graph_pred) dans l'ordre inverse de visite
 * 
 * Chaque CFC reçoit un numéro unique. Les sommets de la même CFC ont le même numéro.
 * Cet algorithme est fondamental pour décomposer le graphe avant le calcul du PageRank hiérarchique.
 * 
 * @param graph Graphe représenté en listes de chaînage des successeurs (tableau_liste_chainne)
 * @param graph_pred Graphe inverse (prédécesseurs) pour la deuxième phase de Kosaraju
 * @param nbsommets Nombre total de sommets du graphe
 * @param nbcfc Pointeur vers le nombre de CFCs (sortie : rempli avec le nombre total de CFCs trouvées)
 * @param nb_sommets_par_cfc Pointeur vers un tableau alloué (sortie : nb_sommets_par_cfc[i] = nombre de sommets dans la CFC i)
 * 
 * @return Tableau `visited` de taille nbsommets où visited[i] = numéro de CFC du sommet i (0-basé)
 * 
 * @note Les tableaux de sortie (nbcfc et nb_sommets_par_cfc) sont alloués dynamiquement avec malloc/realloc.
 *       L'appelant doit libérer ces pointeurs après utilisation.
 *       Les CFCs sont numérotées de 0 à (nbcfc-1) dans l'ordre de découverte inverse.
 *       Implémentation itérative sans récursion pour éviter les débordements de pile sur grands graphes.
 */
int* kosaraju(struct tableau_liste_chainne graph, struct tableau_liste_chainne graph_pred, int nbsommets,int *nbcfc ,int **nb_sommets_par_cfc){
    //cration de la liste visité
    int* visited=malloc(nbsommets*sizeof(int));
    memset(visited,-1,nbsommets*sizeof(int));
    //creation de la stack
    struct IntStack* stack_visited=creer_intstack(nbsommets);//la pile qui garde l'ordre de visite de premier dfs 
    struct IntStack* stack_iterations=creer_intstack(nbsommets);
    //paroucr en largeur
    int cfc_counter=-1;// pour pouvoir l'incrementer en suite dans la premiere while et que ca fasse 0
    int sommet_courant=0;
    struct list_chainne_de_sommet* successeur=NULL;
    int predecesseurs;//est utilise dans la deuxiemme partie de cette algo 
    *nb_sommets_par_cfc=malloc(nbsommets*sizeof(int));//on ne sais pas combien de cfc on vas trouver donc on alloue pour le pire cas ou chaque sommet est une cfc et on vas desalouer apres;
    memset(*nb_sommets_par_cfc,0,nbsommets*sizeof(int));
    struct list_chainne_de_sommet** vecteur_acceleration_pred_suc=malloc(nbsommets*sizeof(struct list_chainne_de_sommet*));
   
    for(int i=0;i<nbsommets;i++){
        vecteur_acceleration_pred_suc[i]=graph.tab[i];
    }


    int c=0;
    int index_visited=0;

    while( ( c=tout_visite(visited,nbsommets,&index_visited))!=-1){ 
        stack_intstack(stack_iterations,c);
        sommet_courant=c;
        //   printf("le sommet courant est %d",sommet_courant);
        //sleep(5);
        while(stack_iterations->top!=-1){
            // printf("les iterations\n");
            // afficher_liste_chainne(stack_iterations);
            // sleep(7);


            //dans la stack du paroucr en profondeur on prends le sommet actuelle et on ajoute au visité
            sommet_courant=peek_intstack(stack_iterations);

            visited[sommet_courant]=0;
            successeur=graph.tab[sommet_courant];

            successeur=vecteur_acceleration_pred_suc[sommet_courant];


            //rechercher le prochain successeur non visitééé
            while(successeur!=NULL&&visited[successeur->sommet-1]!=-1){
                successeur=successeur->suiv;
            }
            vecteur_acceleration_pred_suc[sommet_courant]=successeur;// pour accelerer la recherche et ne pas reparcourir les memes successeurs deja parcourue 

            //on a soit un sommet qui a des successeurs non visité au quelle on vas les parcourire donc on continue le DFS
            //sinon on pop dans la stack des des iterations pour ne pas faire un algo recurcif ...
            if(successeur!=NULL){
                stack_intstack(stack_iterations,successeur->sommet-1);
            }else{
                stack_intstack(stack_visited,sommet_courant);
                 pop_intstack(stack_iterations);
            }
        }
    }

    for(int i=0;i<nbsommets;i++){
        vecteur_acceleration_pred_suc[i]=graph_pred.tab[i];
    }

    memset(visited,-1,nbsommets*sizeof(int));



    // printf("=========affichage de visited==========\n");
    // afficher_liste_chainne(stack_visited);    

    while(stack_visited->top!=-1){

        while(stack_visited->top!=-1&&visited[peek_intstack(stack_visited)]!=-1){
            pop_intstack(stack_visited);
        }

        if(stack_visited->top!=-1){
            cfc_counter++; // on augmente le nombre de cfc trouvé

            
            stack_intstack(stack_iterations,peek_intstack(stack_visited));// on a trouve un sommet dans la stack des visités(replis dans la premmière partie) qui n'est pas visite et donc on le met dans la stack du parcour 
            pop_intstack(stack_visited);

            // la on commance le parcour avec le graphe inversé cad des predecesseurs 
            while(stack_iterations->top!=-1){



                sommet_courant=peek_intstack(stack_iterations);
                if(visited[sommet_courant]==-1)
                (*nb_sommets_par_cfc)[cfc_counter]++;
                visited[sommet_courant]=cfc_counter;//visited prend la valeur du numero de la cfc 
                successeur=vecteur_acceleration_pred_suc[sommet_courant];//pour accelerer la recherche des predecesseurs on utilise un vecteur d'acceleration qui contient les predecesseurs de chaque sommet
                

                // printf("cfc counter est %d\n",cfc_counter);
                // printf("le sommet courant:%d\n",sommet_courant);
                // printf("les iterations :\n");
                // afficher_liste_chainne(stack_iterations);

               // sleep(8);

                while(successeur!=NULL&&visited[successeur->sommet-1]!=-1){
                    successeur=successeur->suiv;
                }
                vecteur_acceleration_pred_suc[sommet_courant]=successeur;//pour accelerer la recherche des predecesseurs on utilise un vecteur d'acceleration qui contient les predecesseurs de chaque sommet et on le met a jour a chaque fois qu'on cherche un predecesseur non visité pour ne pas refaire la recherche a partir du debut de la liste chaînée des predecesseurs
       

                // printf("etat de suc \n");
                // if(successeur==NULL){
                //     printf("alerete:");
                // }else{
                //     printf("successur valeur %d",successeur->sommet);
                // }

                if(successeur!=NULL){

                    //printf("la stack it vas ietre mis a jour \n la valeur insere est :%d\n",successeur->sommet-1);
                    stack_intstack(stack_iterations,successeur->sommet-1);
                }else{
                    pop_intstack(stack_iterations);
                }
            }


        }
    }
    

    *nbcfc=cfc_counter+1;
    (*nb_sommets_par_cfc)=realloc((*nb_sommets_par_cfc),(*nbcfc)*sizeof(int));
    libmem_intstack(stack_iterations);
    libmem_intstack(stack_visited);
    free(vecteur_acceleration_pred_suc);
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
    int stop=0;

omp_set_num_threads(NUM_THREADS);
#pragma omp parallel
{


    #pragma omp for
    for(int i=0;i<Nbarcs;i++){
        int t=tableauSommets[i].suc-1;
       
        #pragma omp critical
        vecteur1[t]=0;
    }// ici ce quon fait c'est que on le vecteur 1 dois recevoir une copie du vecteur 0 sauf pour les sommet de la cfc actuelle qu'on dois chercher a calculer la distribution stat 
    // donc on dois mettre un 0  a ces sommets 


    #pragma omp single
    if(vecteur1==NULL){
        printf("erreur allocation vecteur1\n");
        exit(1);}



    multvectmmat_old_sans_surf(tableauSommets,vecteur1,vecteur0,Nbarcs,Nbsommet,Nbsommet_globale,Nbvaleurs);

    
    while (!stop)
    {
        #pragma omp single
        {
        cpt++;

       // printf("%lf\n",norme_a1_old(vecteur1,vecteur0,Nbsommet));

        double *temp = vecteur0;
        vecteur0 = vecteur1;
        vecteur1 = temp;
        }
        // clock_t start = clock();
        
        multvectmmat_old_sans_surf(tableauSommets,vecteur1,vecteur0,Nbarcs,Nbsommet,Nbsommet_globale,Nbvaleurs);

        #pragma omp single
        {
        if( norme_a1_old(vecteur1,vecteur0,Nbsommet_globale)<EPSILON||cpt>2){
            stop=1;
        }
        }
    }
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

/**
 * @brief Calcule le vecteur PageRank d'un graphe via l'itération de puissance avec parallélisation OpenMP.
 * 
 * Effectue des itérations successives de multiplication matrice-vecteur pour converger vers le vecteur
 * PageRank stable. Le calcul utilise le facteur d'amortissement ALPHA (par défaut 0.9) et ajoute une
 * contribution uniforme pour les nœuds dangling (sans successeurs). Les itérations s'arrêtent lorsque
 * la norme L1 entre deux itérations successives descend sous le seuil de convergence (EPSILON ou SMALL_EPSILON).
 * 
 * @param tableauSommets Tableau contiguë du graphe en format (predecesseur, successeur, probabilité)
 * @param vecteur0 Vecteur initial des probabilités/poids de chaque sommet (de taille Nbsommet)
 * @param tabD Tableau des nœuds dangling (sommets sans successeurs) utilisé pour renormalisation
 * @param Nbsommet Nombre total de sommets du graphe
 * @param Nbvaleurs Nombre total d'arcs dans le graphe
 * @param nbiter Pointeur vers le compteur d'itérations (incrémenté à chaque itération)
 * @param use_mall_epsilon Flag: 1 utilise SMALL_EPSILON (convergence stricte), 0 utilise EPSILON (convergence standard)
 * 
 * @return Pointeur vers le vecteur PageRank résultant (alloué dynamiquement, de taille Nbsommet)
 * 
 * @note Les opérations sont parallélisées avec OpenMP. Le vecteur d'entrée vecteur0 n'est pas modifié
 *       directement ; un nouveau vecteur est alloué et retourné. L'appelant doit libérer le vecteur retourné.
 *       Les CFCs non triviales (Nbsommet > 1) requièrent un appel préalable à recherchep_sans_surf() 
 *       pour initialiser vecteur0 correctement.
 */
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



/**
 * @brief Transforme la matrice d'adjacence en tableau de sous matrices correspondant aux composantes fortement connexes (CFC).
 * 
 * @param tableauSommets matricr du graphe sous forme tableau contigue predecesseur, successeur, probabilite
 * @param nbsommet nombre de sommet dans le graphe 
 * @param nbsommet_par_cfc nombre de sommets dans chaque composante fortement connexe
 * @param numcfc_du_sommet numéro de la composante fortement connexe à laquelle appartient chaque sommet
 * @param nbcfc nombre de composantes fortement connexes
 * @param nbarcs nombre d'arcs dans le graphe
 * @param resultat1 tableau de pointeurs vers les sous matrices correspondant aux composantes fortement connexes
 * @param resultat2 tableau de pointeurs vers les tableaux contenant les informations sur le nombre d'arcs dans chaque composante fortement connexe
 */
void transformation_matrice(struct contigue* tableauSommets,int nbsommet,int * nbsommet_par_cfc,int* numcfc_du_sommet,int nbcfc,int nbarcs,struct contigue*** resultat1,int** resultat2){
    // tableau de pointeur sur tableau contigue de sous mat cfc vertion liste chainne 
    struct neud_sommet_suc_prob **tableau_de_sousmatrice_cfc=malloc(nbcfc*sizeof(struct neud_sommet_suc_prob*));
    if(tableau_de_sousmatrice_cfc==NULL){
        printf("erreur allocation tableau_de_sousmatrice_cfc\n");
        exit(1);
    }

        for(int i=0;i<nbcfc;i++){
            tableau_de_sousmatrice_cfc[i]=NULL;
        }
 
    int * nb_succeseur_par_sommet=malloc(nbsommet*sizeof(int));// vas nous servire a savoir le nombre de succeseur de chaque sommet car on retiens pas sertens arcs car non meme cfc 
        if(nb_succeseur_par_sommet==NULL){
        printf("erreur allocation nb_succeseur_par_sommet\n");
        exit(1);}

    memset(nb_succeseur_par_sommet,0,nbsommet*sizeof(int));
    
    double * probabilite_cummle_par_arc_supprime=malloc(nbsommet*sizeof(double));//la c'est les probas cumule des arcs non retenue 
    if(probabilite_cummle_par_arc_supprime==NULL){
        printf("erreur allocation probabilite_cummle_par_arc_supprime\n");
        exit(1);}

    memset(probabilite_cummle_par_arc_supprime,0,nbsommet*sizeof(double));



    int * nbarcs_par_cfc=malloc(nbcfc*sizeof(int));
    if(nbarcs_par_cfc==NULL){
        printf("erreur allocation nbarcs_par_cfc\n");
        exit(1);}
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



    struct contigue **tableau_de_sousmatrice_cfc_contigue=malloc(nbcfc*sizeof(struct contigue*));

    if(tableau_de_sousmatrice_cfc_contigue==NULL){
        printf("erreur allocation tableau_de_sousmatrice_cfc_contigue\n");
        exit(1);
    }

    for(int i=0;i<nbcfc;i++){
        tableau_de_sousmatrice_cfc_contigue[i]=malloc(nbarcs_par_cfc[i]*sizeof(struct contigue));
        
        if(tableau_de_sousmatrice_cfc_contigue[i]==NULL){
            printf("erreur allocation tableau_de_sousmatrice_cfc_contigue[%d]\n", i);
            exit(1);
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



/**
 * @brief Crée un graphe concentré où chaque CFC du graphe initial devient un sommet
 *  et les arcs relient les CFCs entre elles avec des probabilités recalculées.
 *  
 *  Le graphe concentré est construit à partir du graphe initial en ne conservant que les arcs 
 *  inter-CFC. La probabilité de chaque arc est calculée comme la somme des probabilités des arcs
 *  inter-CFC du graphe initial qui partent de la même CFC source et arrivent à la même CFC 
 *  destination, pondérée par les probabilités des sommets source dans vecteur0. Seuls les arcs
 *  ayant une probabilité non-nulle sont conservés pour optimiser la convergence du calcul de PageRank.
 * 
 * @param tableauSommets Tableau contiguë du graphe initial (format: predecesseur, successeur, probabilité)
 * @param vecteur0 Vecteur des probabilités/poids de chaque sommet du graphe initial
 * @param Nbsommets Nombre total de sommets dans le graphe initial
 * @param Nbarcs Nombre total d'arcs dans le graphe initial
 * @param cfc Tableau de mapping: cfc[i] = numéro de CFC du sommet i
 * @param nbcfc_totale Nombre total de composantes fortement connexes
 * @param nbsommets_par_cfc Tableau: nbsommets_par_cfc[i] = nombre de sommets dans la CFC i
 * @param graphe_concentre_result Pointeur de sortie: tableau contiguë du graphe concentré
 * @param nb_arcs_graphe_concentre_result Pointeur de sortie: nombre d'arcs dans le graphe concentré
 * @param tabD_result Pointeur de sortie: tableau des CFCs sans successeurs (dangling nodes)
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

    // printf("la proba cumule pour la cfc 1 est %f\n",tableau_proba_cumule_par_cfc[1]);
    // for(int i=0;i<nbcfc_totale;i++){
    //     if(tableau_proba_cumule_par_cfc[i]!=1&&tableau_proba_cumule_par_cfc[i]!=0){
    //         printf("we got a problem in the renormalisation of the graphe concentre %lf %d\n",tableau_proba_cumule_par_cfc[i],i);
    //     }
    //     else if(tableau_proba_cumule_par_cfc[i]==0){
    //         int j=0;
    //         while(j<tabD.Nbsommet){
    //             if(tabD.tabD[j]==i){
    //                 break;
    //             }
    //             j++;
    //         }
    //         if(j==tabD.Nbsommet){
    //             printf("we got a problem in the construction of the tableau D for cfc %d\n",i);
    //             exit(1);
    //         }
    //     }
    // }

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

/**
 * @brief Dump le graphe concentré dans un fichier
 * 
 * Chaque sommet du graphe concentré représente une CFC du graphe original.
 * Les arcs connectent les CFCs entre elles.
 * 
 * @param graphe_concentre Tableau des arcs inter-CFCs
 * @param nb_arcs Nombre d'arcs dans le graphe concentré
 * @param nbcfc Nombre total de CFCs (sommets du graphe concentré)
 * @param nom_fichier Chemin du fichier de sortie
 */
void dump_graphe_concentre(struct contigue* graphe_concentre, int nb_arcs, int nbcfc, char* nom_fichier){
    FILE *f = fopen(nom_fichier, "w");
    if(f == NULL){
        printf("Erreur: impossible d'ouvrir %s\n", nom_fichier);
        exit(1);
    }
    
    // Header
    fprintf(f, "%d %d %d\n", nbcfc, nbcfc, nb_arcs);
    
    // Arcs : CFC_source CFC_dest probabilité
    for(int i = 0; i < nb_arcs; i++){
        fprintf(f, "%d %d %.15lf\n", graphe_concentre[i].pred, 
                graphe_concentre[i].suc, graphe_concentre[i].probabilite);
    }
    
    fclose(f);
    printf("Graphe concentré dumped dans: %s\n", nom_fichier);
}

void libmem_grapth_sucpred(struct tableau_liste_chainne graph_suc, struct tableau_liste_chainne graph_pred){
    for(int i=0;i<graph_suc.Nbsommet;i++){
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
struct contigue *tableauSommets = lecturefichier_matrixm("wiki2.mtx",&Nblignes,&Nbacrs,&Nbval,&tabD,&graph_suc,&graph_pred,1);
//afficher_contigue(tableauSommets,Nbval);
Nbsommet=Nblignes;

//suppression_epsilon_transitions(&tableauSommets,Nbsommet,&Nbval,tabD,&graph_suc,&graph_pred,1);


int nbcfc;// nombre de composantes fortement connexes
int *nb_sommets_par_cfc;


double start = omp_get_wtime();
int* cfc=kosaraju(graph_suc,graph_pred,Nbsommet,&nbcfc,&nb_sommets_par_cfc);//cfc[i] est le numero de la composante fortement connexe du sommet i+1
double end = omp_get_wtime();
printf("temps d'execution de kosaraju: %f secondes\n", end - start);


struct contigue **tableau_de_sousmatrice_cfc_contigue;
int *nbarcs_par_cfc;

printf("kosaraju done\n");  


printf("le nombre de sommets est %d\n", Nbsommet);
printf("le nombre de cfc est %d\n", nbcfc);

#pragma omp parallel sections
{

#pragma omp section
{
libmem_grapth_sucpred(graph_suc, graph_pred); // Libération de la mémoire des graphes de successeurs et prédécesseurs après l'extraction des CFCs

}




#pragma omp section
{   
transformation_matrice(tableauSommets,Nbsommet,nb_sommets_par_cfc,cfc,nbcfc,Nbval,&tableau_de_sousmatrice_cfc_contigue,&nbarcs_par_cfc);
}
}


struct contigue * graphe_conc;
int nb_arcs_graphe_conc;

printf("transformation de la matrice faite\n");


//===========================================

double *vecteur0=malloc(Nbsommet*sizeof(double));
memset(vecteur0,0,Nbsommet*sizeof(double));

int * cfc_traite=malloc(nbcfc*sizeof(double));

memset(cfc_traite,0,nbcfc*sizeof(int));

//===========================================


for(int i=0;i<Nbsommet;i++){
    int numero_cfc=cfc[i];
        vecteur0[i]=1.0/nb_sommets_par_cfc[numero_cfc];// pour les cfc non triviale on initialise le vecteur de base a la distribution uniforme pour que la recherche du page rank local converge plus vite
}

double test=0;
for(int i=0;i<Nbsommet;i++){
    test+=vecteur0[i];
}

printf("la somme des elements du vecteur0 est %f\n", test);
printf("dois ertre egale a %d\n", nbcfc);





for (int i=0;i<nbcfc;i++){
    if(nb_sommets_par_cfc[i]>70){
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


double ttt=0;
double* vecteur_graph_reduit= malloc(nbcfc*sizeof(double));
memset(vecteur_graph_reduit, 0, nbcfc*sizeof(double));
for(int i=0;i<nbcfc;i++){
    vecteur_graph_reduit[i]=1.0/nbcfc;// pour les cfc non triviale on initialise le vecteur de base a la distribution uniforme pour que la recherche du page rank local converge plus vite
    ttt+=vecteur_graph_reduit[i];
}

vecteur_graph_reduit=recherchep(graphe_conc,
    vecteur_graph_reduit,
    &tabD_graphe_concentre,
    nbcfc,
    nb_arcs_graphe_conc,
    &nbiter,
    1
    );




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


printf("combinaison done\n");

double a = 0.4; // à tuner entre 0 et 1

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