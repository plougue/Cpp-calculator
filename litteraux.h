#ifndef LITTERAUX_H_INCLUDED
#define LITTERAUX_H_INCLUDED


#include <iostream>
#include <sstream>
#include <cstring>

using namespace std;
// GESTION DES EXCEPTION

class Exception
{
    protected:
        string erreur;
    public:
        Exception(const char* err) : erreur((string)err) {}
        Exception(string err) : erreur(err) {}
        virtual  void afficher() { cout << erreur; }
};

enum ErrLitteral{calculImpossible,autreErreurLitterale};
class ExceptionLitteral : public Exception
{
    private:
        ErrLitteral id_erreur;
    public:
        ExceptionLitteral(const char* err, ErrLitteral idErr=autreErreurLitterale) :  Exception(err), id_erreur(idErr) {}
        void afficher() { cout << "Erreur avec les litt�raux :" << erreur; }
};


// GESTION DES REELS

enum TypeReel {fraction, decimal};
class Reel
{
    protected:
        TypeReel type;
    public:
        Reel(TypeReel T) : type(T) {}
        // On d�finit une concat�nation avec un flux ostream&
        virtual ostream& concat(ostream& f) const=0;

        virtual Reel& operator+(const Reel& terme) const;
        virtual Reel& operator-(const Reel& terme) const;
        virtual Reel& operator*(const Reel& terme) const;
        virtual Reel& operator/(const Reel& terme) const;
        virtual Reel& negatif() const { throw ExceptionLitteral("Pas de n�gation d�finie pour ce type de R�el"); }


        // On d�finit une fonction qui permet d'obtenir une valeur flottante du r�el
        virtual float getValeur() const = 0;

        // On d�finit des op�rations par d�faut bas�es sur cette valeur flottante
        virtual bool operator==(const Reel& R) const
        {
            return R.getValeur() == (*this).getValeur();
        }
        virtual bool operator!=(const Reel& R) const
        {
            return R.getValeur() != (*this).getValeur();
        }

};

class Forme_decimale : public Reel
{
    private:
        float valeur;
    public:
        Forme_decimale(float v) : valeur(v), Reel(decimal) {}
        ostream& concat(ostream& f) const { return f << valeur;}
        float getValeur() const { return valeur;}
        Reel& negatif() const { return * (new Forme_decimale(-valeur)) ; }


};

int PGCD(int a, int b);
class Forme_fraction : public Reel
{
    private:
        int numerateur;
        int denominateur;
    public:
        Forme_fraction(int a, int b=1) : numerateur(a), denominateur(b), Reel(fraction){ if(b==0) b=1; (*this).simplifier();}
        ostream& concat(ostream& f) const { if(denominateur != 1) return f << numerateur << "/" << denominateur; else return f << numerateur;}
        Forme_fraction& fraction_simple() const;
        void simplifier() { int pgcd = PGCD(numerateur, denominateur);  numerateur /= pgcd; denominateur /= pgcd;}
        int getNumerateur() const { return numerateur;}
        int getDenominateur() const { return denominateur;}
        float getValeur() const { return (float)numerateur/(float)denominateur;}

        Reel& negatif() const { return * (new Forme_fraction(-numerateur, denominateur)) ; }

};
bool operator==(const Forme_fraction &f1, const Forme_fraction &f2);
bool operator!=(const Forme_fraction &f1, const Forme_fraction &f2);

class Computer;
class Litteral
{
    public:
        virtual Litteral* eval(Computer& c) const = 0;
        virtual ostream& concat(ostream& f) const { throw ExceptionLitteral("Pas de concat pour ce litt�ral"); }
        virtual Litteral& copie() const = 0;
        virtual string toStr() const = 0;
};

enum TypeLCalculable {expression, numerique};
class Litteral_calculable : public Litteral
{
    protected:
        TypeLCalculable type;
    public:
        virtual Litteral* eval(Computer& c) const = 0;
        Litteral_calculable(TypeLCalculable T):type(T) {}
        virtual Litteral_calculable& operator+(const Litteral_calculable& L) const;
        virtual Litteral_calculable& operator-(const Litteral_calculable& L) const;
        virtual Litteral_calculable& operator*(const Litteral_calculable& L) const;
        virtual Litteral_calculable& operator/(const Litteral_calculable& L) const;
        virtual TypeLCalculable getType() const {return type;};
        virtual string toStr() const = 0;
};

// GESTION DES LITTERAUX NUMERIQUES
class Litteral_expression : public Litteral_calculable
{
    private:
        string exp;
    public:
        Litteral_expression(const char* str) : exp(str), Litteral_calculable(expression) {  }
        Litteral_expression(string str) : exp(str), Litteral_calculable(expression) {  }
        virtual bool isExpression() const { return true; }
        Litteral& copie() const;
        ostream& concat(ostream& f) const { return (f << "'" << exp << "'");}
//        string toStr() const { return exp; }
        string toStr() const { ostringstream stream; stream << exp; return stream.str(); }

        Litteral* eval(Computer& c) const;
};
class Litteral_numerique : public Litteral_calculable
{
    private:
        Reel& partie_reele;
        Reel& partie_imaginaire;
    public:
        Litteral_numerique(Reel& re, Reel& im) : partie_reele(re), partie_imaginaire(im), Litteral_calculable(numerique) {}
        Litteral_numerique(float re, float im=0)
        : partie_reele(*(new Forme_decimale(re))), partie_imaginaire(*(new Forme_decimale(im))), Litteral_calculable(numerique){}
        Litteral_numerique(const Litteral_numerique& L) : partie_reele(L.getRe()), partie_imaginaire(L.getIm()), Litteral_calculable(numerique) { }
         // On red�finit le constructeur de recopie car on traite des membres r�f�rences. Celui ci utilise les constructeur de recopie des classes r��les et deriv�es de r�eles
        Reel& getRe() const{return partie_reele;}
        Reel& getIm() const{return partie_imaginaire;}
        ostream& concat(ostream& f) const;

        Litteral* eval(Computer& c) const  { return &this->copie(); };
        virtual bool isExpression() const { return false; }
        string toStr() const { ostringstream stream; partie_reele.concat(stream); if(partie_imaginaire != Forme_decimale(0)) { stream << "$"; partie_imaginaire.concat(stream); } return stream.str(); }
        Litteral& copie() const{ return * (new Litteral_numerique(*this));}
};

// GESTION DES LITTERAUX EXPRESSION

string toRPN(string exp);
class Litteral_programme : public Litteral
{
    private:
        string commande;
    public:
        Litteral_programme(string com) : commande(com) {}
        Litteral_programme(const char* com) : commande((string)com) {}
        Litteral* eval(Computer &c) const;
        string toStr() const { }
        Litteral& copie() const{ }
};
class Litteral_atome : public Litteral
{
    private:
        string exp;
    public:
        Litteral_atome(const char* str) : exp(str) {  }
};




ostream& operator<<(ostream& f, const Reel& reel);
ostream& operator<<(ostream& f, const Litteral& L);


void test();
#endif // LITTERAUX_H_INCLUDED
