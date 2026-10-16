# Warm Start PageRank

Étude expérimentale de deux stratégies pour accélérer le calcul du **PageRank** sur de grands graphes web lorsque le facteur d'amortissement α est proche de 1 (cas des graphes NCD, *Nearly Completely Decomposable*).

Rapport associé : *Warm Start PageRank*, tighilt idir, mai 2026.

## Contexte

Le PageRank modélise un surfeur aléatoire qui suit les hyperliens avec probabilité α et se téléporte uniformément avec probabilité 1 − α. Le vecteur PageRank π* est la distribution stationnaire de cette chaîne de Markov, calculée par itération de puissance.

Plus α est proche de 1, plus le mélange de la chaîne est lent et plus le nombre d'itérations explose. Le phénomène est marqué sur les graphes dont les nœuds forment des communautés denses en interne mais faiblement couplées entre elles.

Le point fixe calculé (avec gestion des *dangling nodes* `D` et téléportation uniforme) est :

$$\pi^* = \alpha P \pi^* + \left(\frac{1-\alpha}{N} + \frac{\alpha}{N}\sum_{i \in D}\pi^*_i\right)\mathbf{1}$$

## Les deux approches étudiées

### 1. Suppression d'arcs

On modifie le graphe pour casser les structures quasi-décomposables, puis on mesure l'effet sur la convergence en fonction de α (de 0,05 à 0,95 par pas de 0,05).

| Configuration | Principe |
| --- | --- |
| Suppression 0 % | Graphe original (référence) |
| Suppression 10 % | Chaque arc supprimé avec probabilité 10 % (tirage uniforme) |
| Suppression 20 % | Idem avec 20 % |
| Suppression intelligente | Suppression déterministe des arcs de faible poids, seuil dynamique `moyenne − 2σ` des probabilités de transition (plancher à 10 % de la moyenne si négatif) |

Après suppression, les probabilités sortantes sont renormalisées, et les nœuds qui perdent tous leurs arcs sortants deviennent des *dangling nodes*.

### 2. Warm start hiérarchique

Idée inspirée de BlockRank (Kamvar et al., 2003) : construire un vecteur initial `v⁽⁰⁾` plus proche de π* que la distribution uniforme, en exploitant la décomposition en composantes fortement connexes (CFC).

1. **Phase 1, distributions locales.** CFC par l'algorithme de Kosaraju. Pour chaque CFC de taille > 1, itération de puissance locale sans téléportation (α = 1) jusqu'à un seuil ε₁. Pour une CFC triviale, `π⁽ᵏ⁾ᵢ = 1`.
2. **Phase 2, graphe condensé.** Un sommet par CFC, un arc `Cₖ → Cₗ` s'il existe au moins un arc inter-CFC. Les poids sont les probabilités de transition inter-CFC renormalisées. PageRank ρ sur ce graphe condensé.
3. **Phase 3, vecteur initial.** Pour tout sommet `i ∈ Cₖ` :

$$v^{(0)}_i = \pi^{(k)}_i \left( a\,\rho_k + (1-a)\frac{|C_k|}{N} \right)$$

avec `a = 1` dans les expériences (le vecteur ne dépend que du graphe condensé). Ce vecteur sert de point de départ à l'itération de puissance globale.

## Protocole expérimental

- **Graphes** (SuiteSparse / LAW) :
  - `wiki` : Gleich/wikipedia-20051105
  - `wiki2` : Gleich/wikipedia-20061104
  - `wiki3` : Gleich/wikipedia-20070206
  - `eu-2005` : LAW/eu-2005
- **Implémentation** : C parallèle avec OpenMP, 10 threads, processeur Intel 12450H.
- **Convergence** : norme L1 entre deux itérations successives < ε = 10⁻⁷.
- **Warm start** : α = 0,95, PageRank local calculé pour les CFC de taille ≥ 4, `a = 1`.

## Résultats

### Suppression d'arcs

- Le temps de convergence croît de façon quasi exponentielle avec α. Pour α ≤ 0,5, toutes les configurations restent sous la seconde.
- À α = 0,95 sur `wiki2`, le graphe original demande environ 17 s. Avec 20 % d'arcs supprimés, environ 7 s (plus de 50 % de gain). Avec 10 %, environ 8,5 s.
- **Contrepartie** : plus on supprime d'arcs, plus la distribution stationnaire s'éloigne de l'originale. Distance L1 à la référence (α = 0,95) :

| Graphe | Suppr. 10 % | Suppr. 20 % | Suppr. intelligente |
| --- | --- | --- | --- |
| wiki2 | 0,607859 | 0,747371 | 0,483328 |
| wiki3 | 0,603886 | 0,747756 | 0,501776 |

- La suppression intelligente se superpose presque à la courbe du graphe original : elle ne perturbe pas les blocs fortement connexes responsables du ralentissement, donc n'apporte pas de gain de vitesse.

### Warm start (α = 0,95, ε = 10⁻⁷, 10 threads)

| Graphe | Temps warm start (s) | Overhead (s) | Temps uniforme (s) | Itérations (uniforme) | Δiter | Surcoût |
| --- | --- | --- | --- | --- | --- | --- |
| eu-2005 | 14,99 | 9,0 (60 %) | 5,51 | 204 | −25 | +172 % |
| wiki2 | 59,14 | 42,2 (71 %) | 17,51 | 204 | +7 | +238 % |
| wiki3 | 81,98 | 65,3 (80 %) | 14,94 | 127 | −38 | +449 % |
| wiki | 19,08 | 14,8 (78 %) | 4,42 | 121 | +3 | +332 % |

`Δiter = iter_uniforme − iter_warm_start` (positif = le warm start économise des itérations).

Dans l'implémentation actuelle, le warm start est **plus lent dans tous les cas** : le prétraitement représente 60 à 80 % du temps total et le gain en itérations est marginal, voire négatif. Une variante pondérant le graphe condensé par les distributions locales (`P̃^π`) donne des résultats aussi mauvais.

### Erreur d'approximation par taille de CFC

L'erreur entre `v⁽⁰⁾` et π* est concentrée sur deux catégories de composantes :

- les **CFC triviales** (taille 1), très nombreuses (erreur cumulée > 0,6 sur wiki, wiki2 et wiki3) ;
- la **composante géante** (de 752 725 sommets sur eu-2005 à 2 358 755 sur wiki3), erreur cumulée entre 0,6 et 0,9.

Les CFC de taille intermédiaire sont bien approximées (erreur < 0,01).

## Conclusions

- Pour **gagner en vitesse** au prix d'une légère modification du graphe, la suppression aléatoire d'arcs est le choix le plus pragmatique.
- Pour **conserver le PageRank exact**, le warm start hiérarchique reste prometteur mais doit être corrigé.

### Pistes d'amélioration

- Limiter le warm start aux grandes CFC (par exemple `|Cₖ| > √N`).
- Traiter les CFC triviales directement à partir de ρ, sans phase locale.
- Exploiter l'ordre topologique du DAG des CFC.
- Paralléliser la phase 1 entre CFC indépendantes.
- Utiliser un seuil de convergence local dynamique (ou un nombre d'itérations dynamique) au lieu de ε = 10⁻⁷ fixe.

## Contenu du dépôt

Le dépôt contient actuellement un prototype de PageRank en C (sans amortissement, sur petits graphes) :

| Fichier | Rôle |
| --- | --- |
| `Untitled-1.c` | Programme principal du prototype (lecture `.mtx` ou `.txt`, itération de puissance) |
| `graph.txt`, `graph_puit.txt` | Petits graphes d'exemple |
| `test.c`, `testthread.c` | Essais (allocation, threads Windows) |
| `PageRank.c`, `tempCodeRunnerFile.c` | Fichier vide / fichier temporaire de l'éditeur |
