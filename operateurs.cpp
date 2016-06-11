#include "operateurs.h"
#include "analyseur.h"
#include <typeinfo>

// METHODE DE LA CLASSE OPERATEUR

void Operateur::appliquer(Computer& PM)
{
    Pile &pileActuelle = PM.getPileActuelle();
    try
    {
        Pile &ResOp = this->operation(pileActuelle);
        PM.setPileActuelle(ResOp);
        if(estAnnulable == false)
            PM.popHistorique(true);
    }
    catch(ExceptionOperateur e)
    {
        e.afficher();
    }
}

// METHODE DE LA CLASSE OPERATEUR NUMERIQUE
Pile& OperateurNumerique::operation(const Pile &P)
{
    Pile &Res = * (new Pile(P));
    Litteral_calculable **L_tab = new Litteral_calculable*[arite];
    int taille_P = P.getTaille();
    for(int i= taille_P - 1;i >= taille_P - arite;i--) // On parcours la pile � l'envers
    {
        try
        {
            L_tab[taille_P - 1 - i] = dynamic_cast<Litteral_calculable*> ( &(Res.pop()) );
        }
        catch (bad_cast& bc)
        {
            throw(ExceptionOperateur("Un des litt�ral n'est pas calculable", litteralNonCalculable));
        }
    }
    Litteral_calculable &L_Res = calcul(L_tab);
    Res.push(L_Res);
    return Res;
}


