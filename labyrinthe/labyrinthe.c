#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <math.h>
#include <assert.h>
#include <time.h>
#include  <MLV/MLV_all.h>


#define HAUTEUR_MAX 100
#define TAILLE_FENETRE_X 1000
#define TAILLE_FENETRE_Y 1000


static unsigned int nbr_fusion = 0;


typedef struct coordonnees_t {
	int x;
	int y;
}coordonnees_t;

typedef struct unionfind_t {
	coordonnees_t taille;
	coordonnees_t ** pere;
	unsigned int ** rang;
}unionfind_t;

typedef struct labyrinthe_t	{
	coordonnees_t taille;
	int ** murs_hori;
	int ** murs_vert;
}labyrinthe_t;


typedef struct element { 
	coordonnees_t coordonnees; 
	struct element *suivant;
}Element;

typedef struct file{ 
	Element *debut; 
	Element *fin; 
	int taille; 
}File;
/*Initialise la file*/
void initialisation (File * f){ 
	f->debut = NULL; 
	f->fin = NULL; 
	f->taille = 0; 
}

void initialiser_coordonnees_t(int x, int y, coordonnees_t * coordonne){
	assert( coordonne != NULL);
	assert(x > 0);
	assert(y > 0);

	(*coordonne).x = x;
	(*coordonne).y = y;
}
/*fonction qui initialise le labyrinthe elle remplis les murs avec des 1*/
int initialiser_labyrinthe(labyrinthe_t* laby, int x, int y){

	int i,j;
	initialiser_coordonnees_t(x, y, &(*laby).taille);
	
	(*laby).murs_vert = (int **)malloc( (*laby).taille.x * sizeof(int *)); 
    for (i=0; i< (*laby).taille.x; i++) 
         (*laby).murs_vert[i] = (int *)malloc(((*laby).taille.y + 1) * sizeof(int)); 

    (*laby).murs_hori = (int **)malloc( (*laby).taille.x * sizeof(int *)); 
    for (i = 0; i < (*laby).taille.x ; i++)
         (*laby).murs_hori[i] = (int *)malloc(((*laby).taille.y + 1) * sizeof(int)); 
  

  	if((*laby).murs_hori == NULL || (*laby).murs_vert == NULL ){
  		return 0;
  	}
  	else{

	 for (i = 0; i <  (*laby).taille.x; i++) 
      for (j = 0; j < (*laby).taille.y; j++) {
         (*laby).murs_hori[i][j] =  1; 
         (*laby).murs_vert[i][j] =  1; 
      }

	(*laby).murs_vert[(*laby).taille.x-1][(*laby).taille.y-1] = 0; 

	return 1;
	}
}
/*fonction qui initialise le rang des case avec des 0 en utulisant malloc  et qui initialise les pere de chaque case*/
int initialiser_case(unionfind_t *grille ,int x ,int y){
	int  i ;
	int  j ;

	initialiser_coordonnees_t(x, y , &(grille)->taille);

	grille -> rang = (unsigned int **) malloc(grille -> taille.x * sizeof(unsigned int *)); 
	    for (i=0 ; i <  grille -> taille.x ; i++) 
	        grille->rang[i] = (unsigned int *)malloc(grille -> taille.y * sizeof(unsigned int)); 

    grille -> pere = (coordonnees_t **) malloc (grille->taille.x * sizeof(coordonnees_t *));
     	for (i=0 ; i < grille -> taille.x ; i++)
     		grille -> pere[i] = (coordonnees_t *) malloc (grille->taille.y * sizeof(coordonnees_t ));
     		
	if((*grille).rang == NULL || (*grille).pere == NULL ){
  		return 0;
  	}
  	else{

	    for(i = 0 ; i < grille ->taille.x ; i++){
	    	
	    	for(j = 0 ; j < grille -> taille.y ; j++){
	    		grille -> pere[i][j] . x = i ;
	    		grille -> pere[i][j] . y = j ;	
	    		
	    	}

	    }
	    return 1 ;
	}
}

/*fonction qui trouve le representent d'un ensemble ,
dans ce cas elle retrouve le represent de l'ensemble qui contient casee */
coordonnees_t trouver( unionfind_t *grille , coordonnees_t casee){

 	if(grille->pere[casee.x][casee.y].x != casee.x || grille->pere[casee.x][casee.y].y != casee.y){

		grille->pere[casee.x][casee.y] = trouver(grille ,grille->pere[casee.x][casee.y]);	
	}
	return grille->pere[casee.x][casee.y];
}
/*fonction qui fusionne entre deux cases voisines en utilisant la fonction trouver et en comparent les rang de chaque cases*/
void fusion_par_rang( unionfind_t *grille , coordonnees_t ensemble1 , coordonnees_t ensemble2){

	coordonnees_t case_pere_actuelle;
	coordonnees_t case_pere_voisine;

	nbr_fusion++ ;
	
	

	case_pere_actuelle = trouver(grille,ensemble1);
	case_pere_voisine = trouver(grille,ensemble2);

	if((case_pere_actuelle.x != case_pere_voisine.x) || (case_pere_actuelle.y != case_pere_voisine.y)){

		

		if(grille->rang[case_pere_actuelle.x][case_pere_actuelle.y] > grille->rang[case_pere_voisine.x][case_pere_voisine.y]){

			grille->pere[case_pere_voisine.x][case_pere_voisine.y].x = case_pere_actuelle.x ;
			grille->pere[case_pere_voisine.x][case_pere_voisine.y].y = case_pere_actuelle.y ; 
		} 

		else if(grille->rang[case_pere_actuelle.x][case_pere_actuelle.y] < grille->rang[case_pere_voisine.x][case_pere_voisine.y]){

			grille->pere[case_pere_actuelle.x][case_pere_actuelle.y].x = case_pere_voisine.x;
			grille->pere[case_pere_actuelle.x][case_pere_actuelle.y].y = case_pere_voisine.y;

		}

		else{
			grille->pere[case_pere_voisine.x][case_pere_voisine.y].x = case_pere_actuelle.x ;
			grille->pere[case_pere_voisine.x][case_pere_voisine.y].y = case_pere_actuelle.y ; 
			grille->rang[case_pere_actuelle.x][case_pere_actuelle.y]++ ;
		} 
	}
}


/*optio nunique : fonction qui casse les murs de la grille et fusionnes les cases de representants differents*/
void casser_murs_unique(labyrinthe_t *laby , unionfind_t *grille ){
	
	coordonnees_t case_actuelle;
	coordonnees_t case_voisine;

	coordonnees_t representent_actuelle;
	coordonnees_t representent_voisine;

	int h_ou_v;

	h_ou_v = rand() % 2 ;

	
		if(h_ou_v == 1){ /*Horizontal*/

				case_actuelle.x = rand() % (((*grille).taille.x)-1 );
				case_actuelle.y = rand() % (*grille).taille.y ;
				case_voisine.x = (case_actuelle.x)+1;
				case_voisine.y = (case_actuelle.y) ;

				representent_actuelle = trouver(grille,case_actuelle);
				representent_voisine = trouver(grille,case_voisine);

			if(((*laby).murs_hori[case_actuelle.x][case_actuelle.y] ==  1) 
				&&  (representent_voisine.x != representent_actuelle.x 
				|| representent_voisine.y != representent_actuelle.y)){ 
			/*casser le mur horizontal sans casser le dernier murs*/

		   		(*laby).murs_hori[case_actuelle.x][case_actuelle.y] =  0; 

				fusion_par_rang(grille,case_actuelle,case_voisine);
			}
		}
		if(h_ou_v == 0){ /*Vertical*/

				case_actuelle.x = rand() % (*grille).taille.x ;
				case_actuelle.y = rand() %(((*grille).taille.y)-1) ;

				case_voisine.x = case_actuelle.x;
				case_voisine.y = (case_actuelle.y)+1;

				representent_actuelle = trouver(grille,case_actuelle);
				representent_voisine = trouver(grille,case_voisine);

		   	if ((*laby).murs_vert[case_actuelle.x][case_actuelle.y] ==  1 
		   		&&  (representent_voisine.x!=  representent_actuelle.x || representent_voisine.y!=  representent_actuelle.y )){ 
		   	
		  		(*laby).murs_vert[case_actuelle.x][case_actuelle.y] =  0; 

		  		
				fusion_par_rang(grille,case_actuelle,case_voisine);
			}
		  	
		}
	}
	/*fonction qui casse les murs aleatoirements et fusionne les cases */
void casser_murs_aleatoirememnt(labyrinthe_t *laby , unionfind_t *grille){
	
	coordonnees_t case_actuelle;
	coordonnees_t case_voisine;

	int h_ou_v;

	h_ou_v = rand() % 2 ;
	
		if(h_ou_v == 1){ /*Horizontal*/

				case_actuelle.x = rand() % ( ((*grille).taille.x)-1 );
				case_actuelle.y = rand() % (*grille).taille.y ;
				case_voisine.x = (case_actuelle.x)+1;
				case_voisine.y = (case_actuelle.y) ;

			if(((*laby).murs_hori[case_actuelle.x][case_actuelle.y] ==  1) ){ /*casser le mur horizontal sans casser le dernier murs*/

		   		(*laby).murs_hori[case_actuelle.x][case_actuelle.y] =  0; 

				fusion_par_rang(grille,case_actuelle,case_voisine);
			}
		}

		if(h_ou_v == 0){ /*Vertical*/

				case_actuelle.x = rand() % (*grille).taille.x ;
				case_actuelle.y = rand() %( ((*grille).taille.y)-1) ;

				

				case_voisine.x = case_actuelle.x;
				case_voisine.y = (case_actuelle.y)+1;

		   	if ((*laby).murs_vert[case_actuelle.x][case_actuelle.y] ==  1){ /*casser le mur vertical sans casser le dernier mur*/
		  		(*laby).murs_vert[case_actuelle.x][case_actuelle.y] =  0; 

		  		
				fusion_par_rang(grille,case_actuelle,case_voisine);
			}  	
		}
	}

int en_filer (File * suite, Element * courant, coordonnees_t coord){ 
	Element *nouveau_element; 
	
	if ((nouveau_element = (Element *) malloc (sizeof (Element))) == NULL) 
		return 0; 
	
	nouveau_element->coordonnees.x =  coord.x;
	nouveau_element->coordonnees.y =  coord.y;  
	
	if(courant == NULL){ 
		if(suite->taille == 0) 
			suite->fin = nouveau_element; 
		nouveau_element->suivant = suite->debut; 
		suite->debut = nouveau_element; 
	}else { 
		if(courant->suivant == NULL) 
			suite->fin = nouveau_element; 

		nouveau_element->suivant = courant->suivant; 
		courant->suivant = nouveau_element; 
	} 

	suite->taille++; 
	return 1; 
}

int de_filer (File * suite){ 
	Element *supp_element; 
	if (suite->taille == 0) 
		return 0; 
	supp_element = suite->debut; 
	suite->debut = suite->debut->suivant; 
	free (supp_element); 
	suite->taille--; 

	return 1; 
}


/*fonction qui affiche le labyrinthe dans le terminal (mode : simple)*/
void affichage_ascii(labyrinthe_t laby){

	int i,j;
	for (j = 0; j < laby.taille.y; j++)
		{
			printf("+--");
		}
	printf("+");
	printf("\n");

	for (i=0; i < laby.taille.x ; i++)
	{	
		if (i == 0)
			printf(" ");
		else{
			printf("|");
		}
		for (j = 0; j < laby.taille.y ; j++)
		{
			if(laby.murs_vert[i][j] == 1)
				printf("  |");
			else{
				printf("   ");
			}
		}

		printf("\n");
		for (j = 0; j < laby.taille.y; j++)
		{
			if (laby.murs_hori[i][j]==1)
				printf("+--");
			else{
				printf("+  ");
			}
		}
		printf("+");
		printf("\n");
	}
	printf("\n");	
}
void affichage_ascii_utf8(labyrinthe_t laby){
    int i;
    int j;
    char* intersections[2][2][2][2] = 
   	{{{{" ","╶"},{"╴","─"}},{{"╵","└"},{"┘","┴"}}},
     {{{"╷","┌"},{"┐","┬"}},{{"│","├"},{"┤","┼"}}} };


    for(j = 0; j < laby.taille.y; j++){
        if(j == 0){
            printf("%s%s", intersections[0][0][0][1], intersections[0][0][1][1]);
        }
        else if(laby.murs_vert[0][j-1] == 0){
            printf("%s%s", intersections[0][0][1][1], intersections[0][0][1][1]);
        }
        else{
            printf("%s%s", intersections[1][0][1][1], intersections[0][0][1][1]);
        }
    }
    printf("%s", intersections[1][0][1][0]);
    printf("\n");

    for(i = 0; i < laby.taille.x; i++){
        if(i == 0)
            printf("%s", intersections[1][0][0][1]);
        else if(i == laby.taille.x - 1)
        	printf("%s", intersections[0][1][0][1]);
        else{
            if(laby.murs_hori[i][0] == 0)
                printf("%s", intersections[1][1][0][0]);
            else
                printf("%s", intersections[1][1][0][1]);
        }
        for(j = 0; j < laby.taille.y; j++){
            if(laby.murs_hori[i][j] == 0){
                printf("%s", intersections[0][0][0][0]);
            }
            else
                printf("%s", intersections[0][0][1][1]);
            if(i == laby.taille.x - 2 && j == laby.taille.y - 1)
                printf("%s", intersections[0][1][1][0]);

            else if(i == laby.taille.x - 1 && j == laby.taille.y - 1)
                printf("%s", intersections[0][0][1][0]);
            
            else if(laby.murs_vert[i][j] == 1){
                if(laby.murs_hori[i][j] == 1){
                    if(j+1 < laby.taille.y && laby.murs_hori[i][j+1] == 1){
                        if(i+1 < laby.taille.x && laby.murs_vert[i+1][j] == 1){
                            printf("%s", intersections[1][1][1][1]);
                        }
                        else{
                            printf("%s", intersections[0][1][1][1]);
                        }
                    }
                    else{
                        if(i+1 < laby.taille.x && laby.murs_vert[i+1][j] == 1){
                            printf("%s", intersections[1][1][1][0]);
                        }
                        else{
                            printf("%s", intersections[0][1][1][0]);
                        }
                    }
                }
                else{
                    if(j+1 < laby.taille.y && laby.murs_hori[i][j+1] == 1){
                        if(i+1 < laby.taille.x && laby.murs_vert[i+1][j] == 1){
                            printf("%s", intersections[1][1][0][1]);
                        }
                        else{
                            printf("%s", intersections[0][1][0][1]);
                        }
                    }
                    else{
                        if(i+1 < laby.taille.y && laby.murs_vert[i+1][j] == 1){
                            printf("%s", intersections[1][1][0][0]);
                        }
                        else{
                            printf("%s", intersections[0][1][0][0]);
                        }
                    }

                }
            }
            else{
                if(laby.murs_hori[i][j] == 1){
                    if(j+1 < laby.taille.y && laby.murs_hori[i][j+1] == 1){
                        if(i+1 < laby.taille.x && laby.murs_vert[i+1][j] == 1){
                            printf("%s", intersections[1][0][1][1]);
                        }
                        else{
                            printf("%s", intersections[0][0][1][1]);
                        }
                    }
                    else{
                        if(i+1 < laby.taille.x && laby.murs_vert[i+1][j] == 1){
                            printf("%s", intersections[1][0][1][0]);
                        }
                        else{
                            printf("%s", intersections[0][0][1][0]);
                        }
                    }
                }
                else{
                    if(j+1 < laby.taille.y && laby.murs_hori[i][j+1] == 1){
                        if(i+1 < laby.taille.x && laby.murs_vert[i+1][j] == 1){
                            printf("%s", intersections[1][0][0][1]);
                        }
                        else{
                            printf("%s", intersections[0][0][0][1]);
                        }
                    }
                    else{
                        if(i+1 < laby.taille.x && laby.murs_vert[i+1][j] == 1){
                            printf("%s", intersections[1][0][0][0]);
                        }
                        else{
                            printf("%s", intersections[0][0][0][0]);
                        }
                    }
                }

            }
        }
        printf("\n");

    }
   
}

void affichage_graphique(labyrinthe_t laby, int hauteur , int largeur){
    int i,j,marge;
    float taille_case_x, taille_case_y;
    int x,y;
  
    marge=30;
  	
 	
    if (laby.taille.y <= 17 && laby.taille.x <= 17){
    	taille_case_x = taille_case_y = 50;
    	MLV_draw_line( marge,taille_case_y+marge,marge,((laby.taille.x+0.6)*taille_case_y) , MLV_COLOR_WHITE);
   		MLV_draw_line( marge,marge,(laby.taille.y+0.6)*taille_case_x,marge, MLV_COLOR_WHITE);	
    }

	else{	
		taille_case_x=(TAILLE_FENETRE_X-2*marge)/(laby.taille.y);
		taille_case_y=(TAILLE_FENETRE_Y-2*marge)/(laby.taille.x);

		MLV_draw_line(marge,taille_case_y+marge,marge,((laby.taille.x)*taille_case_y) + marge,MLV_COLOR_WHITE );
   		MLV_draw_line( marge,marge,((laby.taille.y)*taille_case_x)+ marge,marge, MLV_COLOR_WHITE);
   		
	}
   
   	for (i=0; i < laby.taille.x ; i++){
   		for (j = 0; j < laby.taille.y ; j++){
   			x=j*taille_case_x+marge;
		  	y=i*taille_case_y+marge;
            if( laby.murs_vert[i][j] ==1 && (i!=laby.taille.x-1 || j!=laby.taille.y-1 ) )
                MLV_draw_line( x+taille_case_x,y,x+taille_case_x,y+taille_case_y, MLV_COLOR_WHITE);
            if( laby.murs_hori[i][j] ==1 )
                MLV_draw_line( x,y+taille_case_y,x+taille_case_x,y+taille_case_y, MLV_COLOR_WHITE );
        }
   	}
    MLV_actualise_window();
}

/*fonction qui teste si il existe un chemin valide or  
le representant de la premiere case est le meme que celui du dernier */
int est_chemin_valide(unionfind_t grille){
	coordonnees_t derniere_case, premiere_case; 

	coordonnees_t ancentre1 ;
	coordonnees_t ancentre2 ;

	premiere_case.x = 0;
	premiere_case.y = 0;

	derniere_case .x =  (grille.taille.x) - 1 ;
	derniere_case .y =  (grille.taille.y) - 1 ;

	ancentre1 = trouver(&grille,premiere_case);
	ancentre2 = trouver(&grille,derniere_case);



	if((ancentre1.x == ancentre2.x) && (ancentre1.y == ancentre2.y) ){
		return 1;
	}
	return 0;
}
/*option graine :fonction qui prend l'argument --graine=x et renvoie le x en entier  */



/*option taille : fonction qui prend l'argument --taille hauteur x largeur  
    et renvoie la hauteur et la largeur en entier (passage par adresse)  */
void argument_taille(char *arguments ,int *hauteur ,int *largeur){

    int i= 0;
    
    
    char arguments_1[20] ;

 
    strcpy(arguments_1 ,arguments+9);
 
  
     while(arguments_1[i] != 'x'){
           i++ ;
    }
    i = i + 1;
    *hauteur = atoi(arguments_1);
    *largeur = atoi(arguments_1+i);
    
}



int main(int argc ,char *argv[]  ){	
	labyrinthe_t laby;
	unionfind_t grille;
	int hauteur, largeur;
	int i;

	/*Déclaration des options */
	char options[15];
	int affichage_texte;
	int graine ; 
	int attente;
	int acces;
	int unique;
	int victor;
	int appliquer_graine;

	/*On inialise les options */
	affichage_texte = 0;
	graine =0 ;
	attente = 0;
	acces = 0;
	unique = 0;
	victor = 0;
	
	/*On initialise la hatueur et largeur par defaut a 6x8*/
	hauteur = 6;
	largeur = 8;

	
	/*On récupére les arguments passer par l'ulisateur*/
	for (i = 1; i <  argc; i++)
	{	
		strcpy(options,argv[i]);

		if (strncmp(options, "--graine=", 8 ) == 0){
			graine = atoi(options + 9);
			appliquer_graine = 1;
		}
		if (strncmp(options, "--taille=",8 )==0){
			strcpy(options, argv[i]);
			
			argument_taille(options,&hauteur ,&largeur);
		}
		
		if (strncmp(options, "--attente=", 9 ) == 0){
			attente = atoi(options + 10);
		}
		if (strcmp(options, "--mode=texte") == 0)
			affichage_texte = 1;
		if (strcmp(options, "--acces") == 0){
			acces = 1;
		}
		if (strcmp(options, "--unique") == 0){
			unique = 1;
		}
		if (strcmp(options, "--victor")==0 && acces == 0)
			victor = 1;
	}



	if (affichage_texte == 0)
		MLV_create_window( "labyrinthe", "labyrinthe", TAILLE_FENETRE_X,TAILLE_FENETRE_Y );
	
	initialiser_labyrinthe(&laby, hauteur , largeur);
	initialiser_case(&grille , hauteur , largeur);
	

	if (appliquer_graine == 1)
	 	srand(graine);
	else
		srand(time(NULL));
	
	
	if (attente == 0){
		if (unique == 0 && acces == 1 && victor == 0){ /*Si on veut obtenir un chemin accessible la condition d'arret dépond du nombre de fusion effectuer*/
			while(nbr_fusion != (hauteur*largeur) -1)
				casser_murs_unique(&laby, &grille);
		}else{ 
			while(est_chemin_valide(grille) == 0){
				if (unique == 0 && acces == 0 && victor == 0)
					casser_murs_aleatoirememnt(&laby, &grille);	
				else if (unique == 1 && acces == 0 && victor == 0)
						casser_murs_unique(&laby, &grille);
				
			}
		}
		if (affichage_texte == 1)
			affichage_ascii_utf8(laby);
		else{
			MLV_actualise_window();
			affichage_graphique(laby, hauteur, largeur);
			MLV_wait_seconds( 30 );
		}
		
	}else if (attente != 0){ /*Si on a un temps d'attente*/
		if (affichage_texte == 1){ /*Si on est dans un affichage ascii*/
			if (unique == 0 && acces == 1 && victor ==0){
				while(nbr_fusion != (hauteur*largeur) -1){
						casser_murs_unique(&laby, &grille);
						affichage_ascii_utf8(laby);
					}
				}else{
					while(est_chemin_valide(grille) == 0){
						if (unique == 0 && acces == 0 && victor == 0)
							casser_murs_aleatoirememnt(&laby, &grille);	
						else if (unique == 1 && acces == 0 && victor == 0)
							casser_murs_unique(&laby, &grille);

						affichage_ascii_utf8(laby);
						usleep(attente*1000); /*Microseconde en milliseconde*/
				}
			}
		}else if (affichage_texte == 0){
			if (unique == 0 && acces == 1 && victor ==0){
			while(nbr_fusion != (hauteur*largeur) -1){
				casser_murs_unique(&laby, &grille);
				affichage_graphique(laby, hauteur, largeur);
				MLV_actualise_window();
				MLV_wait_milliseconds(attente);
				MLV_clear_window(MLV_COLOR_BLACK);
			}
			MLV_actualise_window();
			affichage_graphique(laby, hauteur, largeur);
			MLV_wait_seconds( 30 );
		}else{
			while(est_chemin_valide(grille) == 0){
				if (unique == 0 && acces == 0 && victor == 0)
					casser_murs_aleatoirememnt(&laby, &grille);	
				else if (unique == 1 && acces == 0 && victor == 0)
					casser_murs_unique(&laby, &grille);

				affichage_graphique(laby, hauteur, largeur);
				MLV_actualise_window();
				MLV_wait_milliseconds(attente);
				MLV_clear_window(MLV_COLOR_BLACK);
			}
			MLV_actualise_window();
			affichage_graphique(laby, hauteur, largeur);

			MLV_wait_seconds( 30 );
		}
		}
		
	}

	
	return 0;
}