#include "../Inc/regulation.h"

#define KP 1.1 // Gain propotionnel
#define KI 0.2 // Gain intégral
#define KD 0.15 // Gain dérivé

#ifdef SIMULATEUR_T // Si le fichier simulateur.h est compilé avec ce fichier, DT vaut 100.
	#define DT 100
#endif
#ifndef SIMULATEUR_T // Si le fichier simulateur.h n'est compilé avec ce fichier, DT vaut 10.
	#define DT 10 
#endif


/**
 * @brief Pour un régulateur du type tout ou rien
 * 
 * @param consigne Température de consigne
 * @param temp Température actuelle
 * @return float La commande de chauffage
 */
float TOR(float consigne,float temp){
	if(temp > consigne){ // si la température est plus faible que la consigne -> commande à 0
		return 0;
	}else{ // sinon mettre la commande à 50
		return 50;
	}
}


/**
 * @brief Permet de régler la puissance selon un régulateur PID
 * 
 * @param consigne 
 * @param now_error 
 * @param prev_error 
 * @param sum_error 
 * @return float 
 */
float PID(float now_error, float prev_error, float* sum_error){
	/*
	 * Pour le terme Proportionnel
	 */
	float P = KP * now_error;


	/**
	 * Pour le terme Intégral
	 */
	if((now_error - prev_error) >= 0){ // Pour une erreur croissante par rapport à sa précedente
		// Afin de determiner l'aire de l'érreur, nous faisons l'aire du rectangle - le triangle de la différence des erreurs
		*sum_error += (DT * now_error) - (((now_error - prev_error) * DT) / 2);  

	}else{ // Pour une erreur décroissante par rapport à sa précedente
		// Afin de determiner l'aire de l'erreur, nous faisons l'aire du rectangle + le triangle de la différence des erreurs
		*sum_error += (DT * now_error) + (((prev_error - now_error) * DT) / 2);
	}
	float I = KI * *sum_error;


	/**
	 * Pour le terme Dérivé
	 */
	float D = KD * ((now_error - prev_error) / DT);


	/**
	 * La régulation PID se détermine est sommant les résultats obtenus précedement 
	 */
	float PID = P + I + D;
	if(PID >= 100){ // La commande de chauffage ne peut pas excéder 100
		return 100;
	}if(PID <= 0){ // La commande de chauffage ne peut pas être en dessous de 0
		return 0;
	}else{
		return PID;
	}
}


/**
 * @brief Fonction utilisée pour les tests automatics 
 * 
 * @param regul Si 1 la regulation est en TOR, si 2 la régulation est en PID
 * @param consigne Température de consigne
 * @param tabT Tableau de température
 * @param nT Taille du tableau de température
 * @return La commande de chauffage (chômage ;D) comprise entre 0 et 100 
 */
float regulationTest(int regul,float consigne,float* tabT, int nT){
	float cmd = 0; // <---
	
	if(regul == 1){
		cmd = TOR(consigne, tabT[nT-1]);
	}

	if(regul == 2){
		float sum_error = 0;
		if(nT == 1){
			cmd = KP * (consigne - tabT[0]);
		}else{
			for(int i=1; i < nT; i++){
				cmd = PID((consigne - tabT[i]), (consigne - tabT[i-1]), &sum_error);
			}	
		}
	}

	return cmd;
}