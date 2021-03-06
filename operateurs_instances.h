#ifndef OPERATEURS_INSTANCES_H_INCLUDED
#define OPERATEURS_INSTANCES_H_INCLUDED
#include "litteraux.h"
#include "pile.h"
#include "operateurs.h"
#include "analyseur.h"
#include <algorithm>

// OPERATEURS
class Additionner : public OperateurNumerique
{

    public:
        Additionner() : OperateurNumerique(2) {}
        Litteral_calculable& calcul(Litteral_calculable **L_Tab) const
        {
            Litteral_calculable *L1 = L_Tab[0];
            Litteral_calculable *L2 = L_Tab[1];
            Litteral_calculable &Res = *L1 + *L2;
            return Res;
        }
};
class Soustraire : public OperateurNumerique
{

    public:
        Soustraire() : OperateurNumerique(2) {}
        Litteral_calculable& calcul(Litteral_calculable **L_Tab) const
        {
            Litteral_calculable *L1 = L_Tab[0];
            Litteral_calculable *L2 = L_Tab[1];
            Litteral_calculable &Res = *L1 - *L2;
            return Res;
        }
};
class Multiplier : public OperateurNumerique
{
    public:
        Multiplier() : OperateurNumerique(2) {}
        Litteral_calculable& calcul(Litteral_calculable **L_Tab) const
        {
            Litteral_calculable *L1 = L_Tab[0];
            Litteral_calculable *L2 = L_Tab[1];
            Litteral_calculable &Res = *L1 * *L2;
            return Res;
        }
};
class Diviser : public OperateurNumerique
{

    public:
        Diviser() : OperateurNumerique(2) {}
        Litteral_calculable& calcul(Litteral_calculable **L_Tab) const
        {
            Litteral_calculable *L1 = L_Tab[0];
            Litteral_calculable *L2 = L_Tab[1];
            Litteral_calculable &Res = *L1 / *L2;
            return Res;
        }
};

class Div : public OperateurNumerique
{

    public:
        Div() : OperateurNumerique(2) {}
        Litteral_calculable& calcul(Litteral_calculable **L_Tab) const
        {

            Litteral_calculable *L1 = L_Tab[0];
            Litteral_calculable *L2 = L_Tab[1];
            QString str1 = L1->toStr();
            QString str2 = L2->toStr();
            Forme_fraction *f1 = 0;
            f1 = a->creerUneLitteraleEntiere(str1);
            Forme_fraction *f2 = 0;
            f2 = a->creerUneLitteraleEntiere(str2);
            if(f1 != 0 && f2 != 0)
            {
                int i1 = f1->getNumerateur();
                int i2 = f2->getNumerateur();
                int res = i2/i1;
                Litteral_calculable &Res =  *(new Litteral_numerique(*(new Forme_fraction(res)), *(new Forme_fraction(0))));
                return Res;
            }
            else
                throw ExceptionOperateur("");
        }
};


class Mod : public OperateurNumerique
{

    public:
        Mod() : OperateurNumerique(2) {}
        Litteral_calculable& calcul(Litteral_calculable **L_Tab) const
        {

            Litteral_calculable *L1 = L_Tab[0];
            Litteral_calculable *L2 = L_Tab[1];
            QString str1 = L1->toStr();
            QString str2 = L2->toStr();
            Forme_fraction *f1 = 0;
            f1 = a->creerUneLitteraleEntiere(str1);
            Forme_fraction *f2 = 0;
            f2 = a->creerUneLitteraleEntiere(str2);
            if(f1 != 0 && f2 != 0)
            {
                int i1 = f1->getNumerateur();
                int i2 = f2->getNumerateur();
                int res = i2%i1;
                Litteral_calculable &Res =  *(new Litteral_numerique(*(new Forme_fraction(res)), *(new Forme_fraction(0))));
                return Res;
            }
            else
                throw ExceptionOperateur("");
        }
};

class Neg : public OperateurNumerique
{

    public:
        Neg() : OperateurNumerique(1) {}
        Litteral_calculable& calcul(Litteral_calculable **L_Tab) const
        {

            Litteral_calculable *L1 =  new Litteral_numerique(*(new Forme_fraction(-1)), *(new Forme_fraction(0)));
            Litteral_calculable *L2 = L_Tab[0];
            Litteral_calculable &Res = *L1 * *L2;
            return Res;
        }
};

class Num : public OperateurNumerique
{

    public:
        Num() : OperateurNumerique(1) {}
        Litteral_calculable& calcul(Litteral_calculable **L_Tab) const
        {

            QString s;
            Litteral_calculable *L = L_Tab[0];
            s = L->toStr();
            int place;
            int dollar;
            dollar = s.indexOf('$');
            place = s.indexOf('/');
            Forme_fraction* f = 0;
            if(place != -1&& dollar == -1){
            f = a->creerUneLitteraleRationelle(s);
            }
            else if(place == -1 && dollar == -1)f = a->creerUneLitteraleEntiere(s);

            if(f != 0 )
            {
                int num = f->getNumerateur();
                Litteral_numerique &Res =  *(new Litteral_numerique(*(new Forme_fraction(num)), *(new Forme_fraction(0))));
                return Res;
            }
            else
            {
                throw ExceptionOperateur("Litterale reele ou complexe");
            }

        }
};

class Den : public OperateurNumerique
{

    public:
        Den() : OperateurNumerique(1) {}
        Litteral_calculable& calcul(Litteral_calculable **L_Tab) const
        {

            QString s;
            Litteral_calculable *L = L_Tab[0];
            s = L->toStr();
            int place;
            int dollar;
            place = s.indexOf('/');
            dollar = s.indexOf('$');
            Forme_fraction* f = 0;
            if(place != -1 && dollar ==-1){
            f = a->creerUneLitteraleRationelle(s);
            }
            else if (place == -1 && dollar ==-1) f = a->creerUneLitteraleEntiere("1");

            if(f != 0 )
            {
                int den = f->getDenominateur();
                Litteral_numerique &Res =  *(new Litteral_numerique(*(new Forme_fraction(den)), *(new Forme_fraction(0))));
                return Res;
            }
            else
            {
                throw ExceptionOperateur("Litterale reele ou complexe");
            }

        }
};


class Re : public OperateurNumerique
{

    public:
        Re() : OperateurNumerique(1) {}
        Litteral_calculable& calcul(Litteral_calculable **L_Tab) const
        {

            QString s;
            Litteral_calculable *L = L_Tab[0];
            s = L->toStr();
            int dollar;
            int point;
            int slash;
            dollar = s.indexOf('$');


            if(dollar != -1)
            {
                QString *tab = new QString[2];
                tab = a->getReEtIm(s);
                point = tab[0].indexOf("/.");
                slash = tab[0].indexOf("//");
                if(point == -1&&slash == -1)
                {
                    Forme_fraction* ln = 0;
                    ln = a->creerUneLitteraleEntiere(tab[0]);
                    if(ln != 0 )
                    {
                        Litteral_calculable &Res = *(new Litteral_numerique((*ln), *(new Forme_fraction(0))));
                        return Res;
                    }
                    else
                    {
                        throw ExceptionOperateur("Erreur");
                    }
                }
                else if(point == -1&&slash != -1)
                {
                    Forme_fraction* ln = 0;
                    ln = a->creerUneLitteraleRationelle(tab[0]);
                    if(ln != 0 )
                    {
                        Litteral_calculable &Res = *(new Litteral_numerique((*ln), *(new Forme_fraction(0))));
                        return Res;
                    }
                    else
                    {
                        throw ExceptionOperateur("erreur");
                    }
                }
                else
                {
                    Forme_decimale* ln = 0;
                    ln = a->creerUneLitteraleReel(tab[0]);
                    if(ln != 0 )
                    {
                        Litteral_calculable &Res = *(new Litteral_numerique((*ln), *(new Forme_fraction(0))));
                        return Res;
                    }
                    else
                    {
                        throw ExceptionOperateur("Erreur");
                    }
                }

            }
            else
            {

                        Litteral_calculable &Res = *L;
                        return Res;

            }


        }
};

class Im : public OperateurNumerique
{

    public:
        Im() : OperateurNumerique(1) {}
        Litteral_calculable& calcul(Litteral_calculable **L_Tab) const
        {

            QString s;
            Litteral_calculable *L = L_Tab[0];

            s = L->toStr();
            int dollar;
            int point;
            int slash;
            dollar = s.indexOf('$');


            if(dollar != -1)
            {
                QString *tab = new QString[2];
                tab = a->getReEtIm(s);
                point = tab[1].indexOf("/.");
                slash = tab[1].indexOf("//");

                if(point == -1 && slash == -1)
                {
                    Forme_fraction* ln = 0;
                    ln = a->creerUneLitteraleEntiere(tab[1]);

                    if(ln != 0 )
                    {

                        Litteral_calculable &Res = *(new Litteral_numerique((*ln), *(new Forme_fraction(0))));

                        return Res;

                    }
                    else
                    {
                        throw ExceptionOperateur("Erreur");
                    }
                }
                if(point == -1 && slash != -1)
                {
                    Forme_fraction* ln = 0;
                    ln = a->creerUneLitteraleRationelle(tab[1]);

                    if(ln != 0 )
                    {

                        Litteral_calculable &Res = *(new Litteral_numerique((*ln), *(new Forme_fraction(0))));

                        return Res;

                    }
                    else
                    {
                        throw ExceptionOperateur("Erreur");
                    }
                }
                else
                {
                    Forme_decimale* ln = 0;
                    ln = a->creerUneLitteraleReel(tab[1]);
                    if(ln != 0 )
                    {
                        Litteral_calculable &Res = *(new Litteral_numerique((*ln), *(new Forme_fraction(0))));
                        return Res;
                    }
                    else
                    {
                        throw ExceptionOperateur("Erreur");
                    }
                }

            }
            else
            {

                        Litteral_calculable &Res = *(new Litteral_numerique(*(new Forme_fraction(0)), *(new Forme_fraction(0))));
                        return Res;

            }


        }
};

class creerComplexe : public OperateurNumerique
{

    public:
        creerComplexe() : OperateurNumerique(2) {}
        Litteral_calculable& calcul(Litteral_calculable **L_Tab) const
        {


            Litteral_calculable *L1 = L_Tab[0];
            Litteral_calculable *L2 = L_Tab[1];

            int dollar1;
            int dollar2;
            dollar1 = L1->toStr().indexOf('$');
            dollar2 = L2->toStr().indexOf('$');
            if(dollar1 == -1 && dollar2 == -1){
            QString complexe = L2->toStr()+"$"+L1->toStr();
            Litteral_numerique *Ln = 0;
            Ln = a->creerUneLitteraleComplexe(complexe);

            if(Ln != 0){
                Litteral_calculable &Res = *(new Litteral_numerique(*Ln));
                return Res;
            }
            else
                throw ExceptionOperateur("Syntaxe invalide");
            }
            else
                throw ExceptionOperateur("Ne peut etre transforme en complexe");

        }
};

//OPERATEUR MANIPULATION PILE


class Drop : public Operateur
{
    public:
        Pile& operation(const Pile& P);
};

class Swap : public Operateur
{
    public:
        Pile& operation(const Pile& P);
};

class Clear : public Operateur
{
    public:
        Pile& operation(const Pile& P);
};


class Lastop : public Operateur
{
    public:
        Pile& operation(const Pile& P);
};

class Lastargs : public Operateur
{
    public:
        Pile& operation(const Pile& P);
};
/*
class Undo : public Operateur
{
    public:
        Pile& operation(const Pile& P);
        void appliquer(Computer& c);

};

class Redo : public Operateur
{
    public:
        Pile& operation(const Pile& P);
        void appliquer(Computer& c);
};
*/
class And : public OperateurNumerique
{

    public:
        And() : OperateurNumerique(2) {}
        Litteral_calculable& calcul(Litteral_calculable **L_Tab) const
        {
            Litteral_calculable *L1 = L_Tab[0];
            Litteral_calculable *L2 = L_Tab[1];
            QString l1 = L1->toStr();
            QString l2 = L2->toStr();
            bool test1 = true;
            bool test2 = true;

            if(l1 == "0")test1 = false;
            if(l2 == "0")test2 = false;

            if(test1 == true && test2 == true){
                Litteral_numerique &Res =  *(new Litteral_numerique(*(new Forme_fraction(1)), *(new Forme_fraction(0))));
                return Res;

            }
            else {
                Litteral_numerique &Res =  *(new Litteral_numerique(*(new Forme_fraction(0)), *(new Forme_fraction(0))));
                return Res;

            }

        }
};

class Or : public OperateurNumerique
{

    public:
        Or() : OperateurNumerique(2) {}
        Litteral_calculable& calcul(Litteral_calculable **L_Tab) const
        {
            Litteral_calculable *L1 = L_Tab[0];
            Litteral_calculable *L2 = L_Tab[1];
            QString l1 = L1->toStr();
            QString l2 = L2->toStr();
            bool test1 = true;
            bool test2 = true;

            if(l1 == "0")test1 = false;
            if(l2 == "0")test2 = false;

            if(test1 == true || test2 == true){
                Litteral_numerique &Res =  *(new Litteral_numerique(*(new Forme_fraction(1)), *(new Forme_fraction(0))));
                return Res;

            }
            else {
                Litteral_numerique &Res =  *(new Litteral_numerique(*(new Forme_fraction(0)), *(new Forme_fraction(0))));
                return Res;

            }

        }
};

class Not : public OperateurNumerique
{

    public:
        Not() : OperateurNumerique(1) {}
        Litteral_calculable& calcul(Litteral_calculable **L_Tab) const
        {
            Litteral_calculable *L1 = L_Tab[0];

            QString l1 = L1->toStr();

            bool test1 = false;

            if(l1 == "0")test1 = true;

            if(test1 == true){
                Litteral_numerique &Res =  *(new Litteral_numerique(*(new Forme_fraction(1)), *(new Forme_fraction(0))));
                return Res;

            }
            else {
                Litteral_numerique &Res =  *(new Litteral_numerique(*(new Forme_fraction(0)), *(new Forme_fraction(0))));
                return Res;

            }

        }
};

class Egal : public OperateurNumerique
{

    public:
        Egal() : OperateurNumerique(2) {}
        Litteral_calculable& calcul(Litteral_calculable **L_Tab) const
        {
            Litteral_calculable *L1 = L_Tab[0];
            Litteral_calculable *L2 = L_Tab[1];
            QString l1 = L1->toStr();
            QString l2 = L2->toStr();
            bool test1 = false;

            if(l1 == l2)test1 = true;


            if(test1 == true){
                Litteral_numerique &Res =  *(new Litteral_numerique(*(new Forme_fraction(1)), *(new Forme_fraction(0))));
                return Res;

            }
            else {
                Litteral_numerique &Res =  *(new Litteral_numerique(*(new Forme_fraction(0)), *(new Forme_fraction(0))));
                return Res;

            }

        }
};

class Different : public OperateurNumerique
{

    public:
        Different() : OperateurNumerique(2) {}
        Litteral_calculable& calcul(Litteral_calculable **L_Tab) const
        {
            Litteral_calculable *L1 = L_Tab[0];
            Litteral_calculable *L2 = L_Tab[1];
            QString l1 = L1->toStr();
            QString l2 = L2->toStr();
            bool test1 = false;

            if(l1 != l2)test1 = true;


            if(test1 == true){
                Litteral_numerique &Res =  *(new Litteral_numerique(*(new Forme_fraction(1)), *(new Forme_fraction(0))));
                return Res;

            }
            else {
                Litteral_numerique &Res =  *(new Litteral_numerique(*(new Forme_fraction(0)), *(new Forme_fraction(0))));
                return Res;

            }

        }
};

class InfOuEgal : public OperateurNumerique
{

    public:
        InfOuEgal() : OperateurNumerique(2) {}
        Litteral_calculable& calcul(Litteral_calculable **L_Tab) const
        {
            Litteral_calculable *L1 = L_Tab[0];
            Litteral_calculable *L2 = L_Tab[1];
            QString l1 = L1->toStr();
            QString l2 = L2->toStr();
            bool test1 = false;

            if(l2 < l1 || l2 == l1)test1 = true;


            if(test1 == true){
                Litteral_numerique &Res =  *(new Litteral_numerique(*(new Forme_fraction(1)), *(new Forme_fraction(0))));
                return Res;

            }
            else {
                Litteral_numerique &Res =  *(new Litteral_numerique(*(new Forme_fraction(0)), *(new Forme_fraction(0))));
                return Res;

            }

        }
};

class SupOuEgal : public OperateurNumerique
{

    public:
        SupOuEgal() : OperateurNumerique(2) {}
        Litteral_calculable& calcul(Litteral_calculable **L_Tab) const
        {
            Litteral_calculable *L1 = L_Tab[0];
            Litteral_calculable *L2 = L_Tab[1];
            QString l1 = L1->toStr();
            QString l2 = L2->toStr();
            bool test1 = false;

            if(l2 > l1 || l2 == l1)test1 = true;


            if(test1 == true){
                Litteral_numerique &Res =  *(new Litteral_numerique(*(new Forme_fraction(1)), *(new Forme_fraction(0))));
                return Res;

            }
            else {
                Litteral_numerique &Res =  *(new Litteral_numerique(*(new Forme_fraction(0)), *(new Forme_fraction(0))));
                return Res;

            }

        }
};

class Inferieur : public OperateurNumerique
{

    public:
        Inferieur() : OperateurNumerique(2) {}
        Litteral_calculable& calcul(Litteral_calculable **L_Tab) const
        {
            Litteral_calculable *L1 = L_Tab[0];
            Litteral_calculable *L2 = L_Tab[1];
            QString l1 = L1->toStr();
            QString l2 = L2->toStr();
            bool test1 = false;

            if(l2 < l1)test1 = true;


            if(test1 == true){
                Litteral_numerique &Res =  *(new Litteral_numerique(*(new Forme_fraction(1)), *(new Forme_fraction(0))));
                return Res;

            }
            else {
                Litteral_numerique &Res =  *(new Litteral_numerique(*(new Forme_fraction(0)), *(new Forme_fraction(0))));
                return Res;

            }

        }
};

class Superieur : public OperateurNumerique
{

    public:
        Superieur() : OperateurNumerique(2) {}
        Litteral_calculable& calcul(Litteral_calculable **L_Tab) const
        {
            Litteral_calculable *L1 = L_Tab[0];
            Litteral_calculable *L2 = L_Tab[1];
            QString l1 = L1->toStr();
            QString l2 = L2->toStr();
            bool test1 = false;

            if(l2 > l1)test1 = true;


            if(test1 == true){
                Litteral_numerique &Res =  *(new Litteral_numerique(*(new Forme_fraction(1)), *(new Forme_fraction(0))));
                return Res;

            }
            else {
                Litteral_numerique &Res =  *(new Litteral_numerique(*(new Forme_fraction(0)), *(new Forme_fraction(0))));
                return Res;

            }

        }
};
class Eval : public Operateur
{
    public:
        Eval(Analyseur* pt_analyseur=0) : Operateur(pt_analyseur) {}
        Analyseur* getAnalyseur() const { return a; }
        void setAnalyseur(Analyseur& newA) { a = &newA; }
         Pile& operation(const Pile& P){Pile res = P; return res;}
        void appliquer(Computer& C);
};
class Dupliquer : public Operateur
{
    public:
        Pile& operation(const Pile& P);
};
class Sto : public Operateur
{
     public:
        Sto(Analyseur* A) : Operateur(A) {}
        Pile& operation(const Pile& P);
};

#endif // OPERATEURS_INSTANCES_H_INCLUDED
