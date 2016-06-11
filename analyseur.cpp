

#include "analyseur.h"
#include "atomes.h"
#include <QtXml>
#include <QString>
#include "operateurs_instances.h"
#include <iostream>
#include <string>
#include <cstdlib>

// METHODE DE LA CLASSE OPERATEURFACTORY


// METHODES DE LA CLASSE COMPUTER

bool Computer::chargerFichier(const char* path)
{
        QDomDocument *dom = new QDomDocument("mon_xml");
        QFile xml_doc(path);
        if(!xml_doc.open(QIODevice::ReadOnly) && !xml_doc.open(QIODevice::ReadWrite) ) // si on peut pas read on readwrite (premi�re ouverture)
        {
            cout << "Le fichier ne peut �tre ouvert";
            return false;
        }
        else if (!dom->setContent(&xml_doc))
        {
                xml_doc.close();
                cout << "Le fichier n'est pas correct";
                return false;
        }
        else
        {
            QDomElement dom_elem = dom->documentElement();
            for(QDomNode node = dom_elem.firstChild(); ! node.isNull(); node = node.nextSibling())
            {
                if(node.nodeName().toStdString() == (string) "atomes")
                {
                    for(QDomElement element = node.firstChildElement(); ! element.isNull(); element = element.nextSibling().toElement())
                    {
                        string tag = element.tagName().toStdString();
                        string id = element.attribute("name").toStdString();
                        string value = element.attribute("value").toStdString();
                        if(tag == (string)"prog")
                        {
                            getAnalyseur().getAtomes().ajouterAtome(id, * new Litteral_programme(value));
                        }
                        if(tag == (string)"exp")
                        {
                            getAnalyseur().getAtomes().ajouterAtome(id, * new Litteral_expression(value));
                        }
                        if(tag == (string)"num")
                        {
                            getAnalyseur().getAtomes().ajouterAtome(id,  * getAnalyseur().evaluer(value));
                        }
                    }
                }
                if(node.nodeName().toStdString() == (string) "pile")
                {
                    for(QDomElement element = node.firstChildElement(); ! element.isNull(); element = element.nextSibling().toElement())
                    {
                        string tag = element.tagName().toStdString();
                        string value = element.attribute("value").toStdString();

                        if(tag == (string)"exp")
                        {
                            pileActuelle->push(* new Litteral_expression(value));
                        }
                       if(tag == (string)"prog")
                        {
                            pileActuelle->push(* new Litteral_programme(value));
                        }
                       if(tag == (string)"num")
                       {
                           pileActuelle->push( * getAnalyseur().evaluer(value)) ;
                       }
                    }
                }
            }
            xml_doc.close();
          //  pileActuelle->afficher();
        }
}
bool Computer::enregistrerFichier(const char* path)
{
    QDomDocument *dom = new QDomDocument("mon_xml");
    QFile xml_doc(path);
    if(!xml_doc.open(QIODevice::WriteOnly) ) // si on peut pas read on readwrite (premi�re ouverture)
    {
        cout << "Le fichier ne peut �tre ouvert";
        return false;
    }
    else
    {
        QXmlStreamWriter xml_stream(&xml_doc);
        xml_stream.writeStartDocument();
        xml_stream.writeStartElement("etat_controleur");
        xml_stream.writeStartElement("pile");
        for(int i = pileActuelle->getTaille(); i >= 1; i--)
        {
            Litteral &L = pileActuelle->pop();
            if(L.isCalculable() == true)
            {
                try
                {
                    Litteral_calculable &L_calc = dynamic_cast<Litteral_calculable&>(L);
                    if(L_calc.getType() == expression )
                        xml_stream.writeStartElement("exp");
                    else if(L_calc.getType() == numerique)
                        xml_stream.writeStartElement("num");
                }
                catch(bad_cast& e)
                {
                    throw(Exception("Litt�rale mal d�finie"));
                }
            }
            else
                xml_stream.writeStartElement("prog");
            xml_stream.writeAttribute("value",L.toStr().c_str());
            xml_stream.writeEndElement();

        }
        xml_stream.writeEndElement();
        xml_stream.writeStartElement("atomes");
        ListeAtomes &L_A = getAnalyseur().getAtomes();
        for(ListeAtomes::iterator_var I = L_A.begin_var(); I != L_A.end_var(); ++I)
        {
            if(I->second.getType() == expression)
                xml_stream.writeStartElement("exp");
            else
                xml_stream.writeStartElement("num");
            xml_stream.writeAttribute("name",I->first.c_str());

            xml_stream.writeAttribute("value",I->second.toStr().c_str());
            xml_stream.writeEndElement();
        }
        for(ListeAtomes::iterator_prog I = L_A.begin_prog(); I != L_A.end_prog(); ++I)
        {
            xml_stream.writeStartElement("prog");
            xml_stream.writeAttribute("name",I->first.c_str());
            xml_stream.writeAttribute("value",I->second.toStr().c_str());
            xml_stream.writeEndElement();
        }
        xml_stream.writeEndElement();
        xml_stream.writeEndElement();

        xml_stream.writeEndDocument();
        xml_doc.close();
    }
}
void Computer::pushHistorique(Pile& P, bool isUndo)
{
    if(isUndo==true && undoDisponible < maxHistorique)
    {
        historiqueUndo[undoDisponible] = &P;
        undoDisponible++;
    }
    else if(isUndo==false && redoDisponible < maxHistorique)
    {
        historiqueRedo[redoDisponible] = &P;
        redoDisponible++;
    }
    else
    {
        throw ExceptionPile("Taille de l'historique max atteinte");
    }
}
Pile& Computer::popHistorique(bool isUndo)
{
    if(isUndo && undoDisponible > 0)
    {
        undoDisponible--;
        return * (historiqueUndo[undoDisponible]);
    }
    else if(!isUndo && redoDisponible > 0)
    {
        redoDisponible--;
        return * (historiqueRedo[redoDisponible]);
    }
    else
    {
        throw ExceptionPile("Historique vide");
    }
}

Litteral& Computer::pop()
{
    return pileActuelle->pop(); /*
     pushHistorique(*pileActuelle, true);
     Pile newP(*pileActuelle);
     Litteral &res = newP.pop();
     pileActuelle = &newP;
     return res; */
}
void Computer::push(Litteral& L)
{
    pileActuelle->push(L);
   //  newP->push(L);
    // pileActuelle = newP;
}
void Computer::effectuer(ConteneurOperande** exp, unsigned int nbOp)
{
    pushHistorique(*pileActuelle, true);
    Pile *newP = new Pile(*pileActuelle);
    pileActuelle = newP;
    a.effectuer(exp,nbOp,*this);
}
void Computer::effectuer(string str)
{
    ConteneurOperande** commande = a.interpreter(str);
    unsigned int i;
    for(i = 0; commande != 0 && commande[i] != 0; i++ );
    effectuer(commande, i);
}
void Computer::undo()
{
    if(pileActuelle != 0)
    {
        pushHistorique(*pileActuelle, false);
        pileActuelle = & ( popHistorique(true) );
    }
    else
        throw Exception("Pas de pile");
}
void Computer::redo()
{
    if(pileActuelle != 0)
    {
        pushHistorique(*pileActuelle, true);
        pileActuelle = & ( popHistorique(false) );
    }
    else
        throw Exception("Pas de pile");
}

// Analyseur
Analyseur::Analyseur(): atomes(* new ListeAtomes), operateurs()
{
    addOperateursCourants();
}
Analyseur::Analyseur(map<string,Operateur&> opSupp): atomes(* new ListeAtomes), operateurs()
{
    addOperateursCourants();
    for(map<string,Operateur&>::const_iterator it = opSupp.begin(); it != opSupp.end(); it++)
    {
        if(operateurs.find(it->first) == operateurs.end())
            operateurs.insert(  pair<string, Operateur&>(it->first, it->second));
    }
}
void Analyseur::addOperateursCourants()
{
    addOperateur("DUP", * new Dupliquer);
    addOperateur("DROP", * new Drop);
    addOperateur("SWAP", * new Swap);
    addOperateur("CLEAR", * new Clear);
    addOperateur("Clear", * new Clear);
    addOperateur("+", * new Additionner);
    addOperateur("-", * new Soustraire);
    addOperateur("*", * new Multiplier);
    addOperateur("/", * new Diviser);
    addOperateur("NEG", * new Neg);
    addOperateur("MOD", * new Mod);
    addOperateur("DIV", * new Div);
    addOperateur("UNDO", * new Undo);
    addOperateur("REDO", * new Redo);
    addOperateur("NUM", * new Num);
    addOperateur("DEN", * new Den);
    addOperateur("RE", * new Re);
    addOperateur("IM", * new Im);
    addOperateur("$", * new creerComplexe);
    addOperateur("OR", * new Or);
    addOperateur("NOT", * new Not);
    addOperateur("=", * new Egal);
    addOperateur("!=", * new Different);
    addOperateur("=<", * new InfOuEgal);
    addOperateur(">=", * new SupOuEgal);
    addOperateur("<", * new Inferieur);
    addOperateur(">", * new Superieur);
    addOperateur("EVAL", * new Eval);
    addOperateur("STO", * new Sto(this));
}
Litteral_numerique* Analyseur::evaluer(string str)
{
    ConteneurOperande** commande = interpreter(str);
    unsigned int i;
    for(i = 0; commande != 0 && commande[i] != 0; i++ );
    return evaluer(commande, i);
}
void Analyseur::effectuer(ConteneurOperande** exp, unsigned int nbOp, Computer &c) {
    for(unsigned int i = 0; i < nbOp; i++)
    {
        if(exp[i]->isLitteral())
        {
            c.push(*exp[i]->getLitteral());
        }
        else
        {
            exp[i]->getOperateur()->appliquer(c);
        }
    }
}
Operateur* Analyseur::creerOperateur(string ID)
{
    if(operateurs.find(ID) != operateurs.end())
        return &(operateurs.find(ID)->second);
    else
        return 0;
}

Litteral_numerique* Analyseur::evaluer(ConteneurOperande** exp, unsigned int taille)
{
    Pile P;
    for(unsigned int i=0; i < taille; i++)
    {
        if(exp[i]->isLitteral())
        {
            P.push(*(exp[i]->getLitteral()));
        }
        else
        {
            P = exp[i]->getOperateur()->operation(P);
        }
    }
    if(P.getTaille() != 1)
        return 0;
    else
    {
        try
        {
            return dynamic_cast<Litteral_numerique*>(&(P.pop()));
        }
        catch(bad_cast e)
        {
            throw(ExceptionOperateur("EVAL n'�value que les calculables"));
        }
    }
}

Forme_fraction *Analyseur::creerUneLitteraleRationelle(const string &s){

    size_t place;
    place = s.find_first_of("\/");
    if(place != -1)
    {
    string numerateur;
    string denominateur;

    numerateur = s.substr(0, place);
    denominateur = s.substr(place+1);


    unsigned int i;
    bool pnum = true;
    bool pden = true;

    i = 0;
    if(numerateur[0]=='-'){i++;}
    for(i; i<numerateur.size(); i++){

        if(numerateur[i]>'9'||numerateur[i]<'0')
            pnum = false;

    }
        i = 0;
        if(denominateur[0]=='-'){i++;}
        for(i; i<denominateur.size(); i++){

        if(denominateur[i]>'9'||denominateur[i]<'0')
            pden = false;

    }

    if(pnum == true && pden == true){
            int *tab = getNumEtDen(s);
            Forme_fraction *frac = new Forme_fraction(tab[0], tab[1]);
            return frac;

        }
    }

}
Forme_decimale *Analyseur::creerUneLitteraleReel(const string &s){
    size_t place;

    place = s.find_first_of("\.");

    string partie_entiere;
    string partie_decimale;
    partie_entiere = s.substr(0, place);
    partie_decimale = s.substr(place+1);


    unsigned int i;
    bool pentiere = true;
    bool pdecimale = true;
    if((!partie_entiere.empty()) && (!partie_decimale.empty()))
    {
        i = 0;
        if(partie_entiere[0]=='-'){i++;}
        for(i; i<partie_entiere.size(); i++)
        {
            if(partie_entiere[i]>'9'||partie_entiere[i]<'0')
            pentiere = false;
        }

        i = 0;
        if(partie_decimale[0]=='-'){i++;}
        for(i; i<partie_decimale.size(); i++)
        {
            if(partie_decimale[i]>'9'||partie_decimale[i]<'0')
            pdecimale = false;
        }
        if(pentiere == true && pdecimale == true){

                float val = std::atof( s.c_str());
                Forme_decimale *f_dec = new Forme_decimale(val);
                return f_dec;
        }
    }

    else if(partie_entiere.empty() && !partie_decimale.empty())
    {

        partie_entiere == "0";
        i = 0;
        if(partie_decimale[0]=='-'){i++;}
        for(i; i<partie_decimale.size(); i++)
        {
            if(partie_decimale[i]>'9'||partie_decimale[i]<'0')
            pdecimale = false;
        }
        if(pdecimale == true){

            string reel = partie_entiere + "\." + partie_decimale;
            float val = atof( reel.c_str());
            Forme_decimale *f_dec = new Forme_decimale(val);
            return f_dec;
        }

    }
    else if(partie_decimale.empty() && !partie_entiere.empty())
    {

        partie_decimale == "0";
        i = 0;
        if(partie_entiere[0]=='-'){i++;}
        for(i; i<partie_entiere.size(); i++)
        {
            if(partie_entiere[i]>'9'||partie_entiere[i]<'0')
            pentiere = false;
        }
        if(pentiere == true){
            string reel = partie_entiere + "\." + partie_decimale;
            float val = atof( partie_entiere.c_str());
            Forme_decimale *f_dec = new Forme_decimale(val);
            return f_dec;
        }

    }



}
Forme_fraction *Analyseur::creerUneLitteraleEntiere(const string& s){
	unsigned int i = 0;
	bool entier = true;
	if(s[0]=='-'){i++;}
	for(i; i< s.size(); i++)
    {
        if (s[i]>'9'||s[i]<'0') entier = false;
    }

    if (entier == true){
            int entier = atoi( s.c_str());
            Forme_fraction *f_ent = new Forme_fraction(entier);
            return f_ent;
    }



}


Litteral_numerique *Analyseur::creerUneLitteraleComplexe(const string& s){
    string *type = new string[2];
    type = getReEtIm(s);
    int *slash = new int[2];
    int *point = new int[2];
    slash[0] = type[0].find_first_of("\/");
    slash[1] = type[1].find_first_of("\/");
    point[0] = type[0].find_first_of("\.");
    point[1] = type[1].find_first_of("\.");
    Reel **reel = new Reel*[2];

    for(unsigned int i=0; i<2; i++){

        if(slash[i] == -1 && point[i] == -1){

                reel[i] = creerUneLitteraleEntiere(type[i]);
        }
        else if (slash[i] != -1 && point[i] == -1){

                reel[i] = creerUneLitteraleRationelle(type[i]);
        }
        else if (slash[i] == -1 && point[i] != -1){
                reel[i] = creerUneLitteraleReel(type[i]);
        }
        else
        throw ExceptionLitteral("Syntaxe invalide");
    }
        Litteral_numerique* Lit = new Litteral_numerique(*reel[0], *reel[1]);
        return Lit;

}
 int *Analyseur::getNumEtDen(const string & fraction) {
        size_t place;
        place = fraction.find_first_of("\/");
        int *tab = new int[2];
        int numerateur;
        int denominateur;

        if(place==-1){
            numerateur = atoi(fraction.c_str());
            denominateur = 1;
            tab[0] = numerateur;
            tab[1] = denominateur;
        }
        else{

        string num;
        string den;
        num = fraction.substr(0, place);
        den = fraction.substr(place+1);
        numerateur = atoi(num.c_str());
        denominateur = atoi(den.c_str());

        tab[0] = numerateur;
        tab[1] = denominateur;
        }
        return tab;
}
string *Analyseur::getReEtIm(const string & complexe) {
        size_t place;
        place = complexe.find_first_of("\$");
        string pre;
        string pim;
        pre = complexe.substr(0, place);
        pim = complexe.substr(place+1);

        string *tab = new string[2];
        tab[0] = pre;
        tab[1] = pim;
        return tab;
}
ConteneurOperande** Analyseur::interpreter(const string &commande) {

    unsigned int i, occEsp =0, occProg = 0;
    char cEsp =' ';
    char cExp ='\'';
    char cProg1 = '\[';
    char cProg2 = '\]';
    bool expression = true;
    bool programme = true;

    unsigned int prog = 0;
    for(i=0; i<commande.size(); i++)
    {

        if(commande[i]== cProg1 ) {prog++; programme = false;}
        else if(commande[i]== cProg2 && prog==0 ) programme = true;
        else if(commande[i]== cExp && programme == true && expression == true) expression = false;
        else if(commande[i]== cExp && programme == true && expression == false) expression = true;

        if(commande[i]==cEsp && (expression == true || programme == true))
        {
            occEsp++;
        }

    }

    string *tab = new string[occEsp+1];

    unsigned int occtab=0;
    //    unsigned int j;
    unsigned int k = 0;

    while(k<commande.size())
    {
        // Si c'est un espace
        if(commande[k]==cEsp){k++;}
        //Si c'est un programme
        else if(commande[k]== cProg1 )
        {
            tab[occtab] = tab[occtab] + commande[k];
            k++;
            unsigned int fin = 1;
            do
            {
                if(commande[k]==cProg1)
                {
                    tab[occtab] = tab[occtab] + commande[k];
                    fin++;
                    k++;
                }
                else if(commande[k]==cProg2&&fin!=0){
                    tab[occtab] = tab[occtab] + commande[k];
                    fin--;
                    k++;

                }
                else if(commande[k]==cProg2&&fin==0){
                    tab[occtab] = tab[occtab] + commande[k];
                    k++;

                }
                else{
                tab[occtab] = tab[occtab] + commande[k];
                k++;
                }


            }while(fin > 0 && k<commande.size());
            tab[occtab] = tab[occtab] + commande[k];
            k++;
            occtab++;
        }

        //Si c'est une expression
        else if (commande[k]== cExp)
        {
            unsigned int conditionFin = 0;
            do
            {
                if(commande[k]==cExp){conditionFin++;}
                tab[occtab] = tab[occtab] + commande[k];
                k++;

            } while(commande[k]!=cExp && conditionFin <=1 && k<commande.size());
            tab[occtab] = tab[occtab] + commande[k];
            k++;

            occtab++;
        }
        //pour le reste
        else
        {
            while (commande[k]!=cEsp&&k< commande.size())
            {
                tab[occtab] = tab[occtab] + commande[k];
                k++;
            }
            occtab++;
        }


    }

    //cout << "Test :  \n";
    /*
    for(unsigned int t=0; t < occtab; t++){

        cout<< "Operande " << t+1 << " = " << tab[t] << "\n";
    } */

    ConteneurOperande **newtab = new ConteneurOperande*[2*occtab+1];
    //cout << "Nombre d'operande :" << occtab << "\n";
    //On a tab qui contient les Operande en string maintenant nous allons Implementer le tableau d'operande.

    unsigned int numOp;
    int slash;
    int point;
    int dollar;
    int quote;
    int crochet;
    int alphabet;
    int numOp2 = 0;
    for(numOp=0; numOp < occtab; numOp++ ){

        slash = tab[numOp].find_first_of("\/");
        point = tab[numOp].find_first_of("\.");
        dollar = tab[numOp].find_first_of("\$");
        quote = tab[numOp].find_first_of("\'");
        crochet = tab[numOp].find_first_of("\[");
        alphabet = 26 + 25;
        for(char i='A'; i <= 'Z'; i++)
            alphabet += tab[numOp].find_first_of(i);
        for(char i='a'; i <= 'z'; i++)
            alphabet += tab[numOp].find_first_of(i);
        Operateur* Op = creerOperateur(tab[numOp]);
        if(Op != 0)
        {
                ConteneurOperande *oP = new ConteneurOperande(*Op);
                newtab[numOp2] = oP;
        }
        else
        {
            //Si c'est un entier
            if(alphabet<0&&slash ==-1&&point==-1&&dollar==-1&&quote==-1&&crochet==-1)
            {
                Forme_fraction *frac1 = creerUneLitteraleEntiere(tab[numOp]);
                Forme_fraction *frac2 = creerUneLitteraleEntiere("0");
                Litteral_numerique *Lit = new Litteral_numerique(*frac1, *frac2);
                ConteneurOperande *cL = new ConteneurOperande(*Lit);
                newtab[numOp2] = cL;

            }
            //SI c'est un rationelle
            else if(slash !=-1&&point==-1&&dollar==-1&&quote==-1&&crochet==-1){
                Forme_fraction *reel1 = creerUneLitteraleRationelle(tab[numOp]);
                Forme_fraction *reel2 = creerUneLitteraleRationelle("0/1");
                Litteral_numerique *Lit = new Litteral_numerique(*reel1, *reel2);
                ConteneurOperande *cL = new ConteneurOperande(*Lit);
                newtab[numOp2] = cL;

            }
            //Si c'est un r�el
            else if(point!=-1&&slash ==-1&&dollar==-1&&quote==-1&&crochet==-1){
                Forme_decimale *dec1 = creerUneLitteraleReel(tab[numOp]);
                Forme_decimale *dec2 = creerUneLitteraleReel("0.0");
                Litteral_numerique *Lit = new Litteral_numerique(*dec1, *dec2);
                ConteneurOperande *cL= new ConteneurOperande(*Lit);
                newtab[numOp2] = cL;

            }
            //SI c'est un Complexe
            else if(dollar!=-1&&quote==-1&&crochet==-1){


                Litteral_numerique *Lit = creerUneLitteraleComplexe(tab[numOp]);

                ConteneurOperande *cL= new ConteneurOperande(*Lit);
                newtab[numOp2] = cL;
            }
            // SI c'est un programme
            else if(crochet != -1){

                tab[numOp].erase(0,1);
                tab[numOp].erase(tab[numOp].size() - 2);
                Litteral_programme *prog = new Litteral_programme(tab[numOp]);
                ConteneurOperande *cL = new ConteneurOperande(*prog);
                newtab[numOp2] = cL;
            }
            //Si c'est une Expression
            else if(quote != -1){
                tab[numOp].erase(0,1);
                tab[numOp].erase(tab[numOp].size() - 1);
                Litteral_expression *exp = new Litteral_expression(tab[numOp]);
                ConteneurOperande *cL = new ConteneurOperande(*exp);
                newtab[numOp2] = cL;
            }
            else
            {
                TypeAtome existenceAtome = getAtomes().atomeExiste(tab[numOp]);
                if(existenceAtome == NIL)
                {
                    Litteral_expression *exp = new Litteral_expression(tab[numOp]);
                    ConteneurOperande *cL = new ConteneurOperande(*exp);
                    newtab[numOp2] = cL;
                }
                else if(existenceAtome == progg)
                {
                    ConteneurOperande* ops = atomes.interpreter(tab[numOp]);
                    newtab[numOp2] = ops;
                    numOp2++;
                    newtab[numOp2] =  new ConteneurOperande(* new Eval());
                }
                else
                {
                    ConteneurOperande* ops = atomes.interpreter(tab[numOp]);
                    newtab[numOp2] = ops;
                }
            }
//      throw ExceptionLitteral("Syntaxe invalide");

        }
            numOp2++;
    }
        newtab[numOp2] = 0;
        return newtab;
    }
