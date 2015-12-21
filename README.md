# Analiza Algoritmilor - Tema 4

Repo-ul conține:
* enunț
* **teste** și **script de testare** pentru verificarea corectitudinii.
* implementare **HP** în **O(V!)** - for the record și ca să fie clar că nu asta
  trebuie făcut; *de fapt l-am scris să verific câte din grafurile generate au
  HP.  Am lăsat acolo și dovada*.
* programul de **generat graf**-uri pe care l-am folosit pentru a genera
  testele și pe care cel mai probabil îl voi folosi pentru a le genera și pe
  cele private; îl puteți folosi să vă generați noi teste.
* directorul `tests/efficiency` care va conține teste private pentru verificarea
  complexității polinomiale a algoritmilor; da da, e nedecidabil - *let me deal
  with that*.
* **robdd.cpp** - implementarea echivalenței expresiilor boolene folosind
  [Reduced Ordered Binary Decision Diagrams](https://en.wikipedia.org/wiki/Binary_decision_diagram).
  Este folosit ca subrutină de scriptul de testare.

Testele pentru verificarea corectitudinii conțin grafuri cu maxim 7 noduri.
Motivul pentru care există această limitare **nu este dificultatea** problemei
**HP**, deoarece nu această problemă trebuie rezolvată, ci transformarea **T**
pe care ați folosit-o deja pentru a demonstra **HP <p SAT** și care ar trebui să
ruleze în **O(V^3)**.  Motivul este dificultatea (aka **NP-Completitudinea**)
verificării echivalenței a 2 expresii boolene - practic ceea ce folosim pentru a
testa corectitudinea reducerii.

Așa cum am menționat deja, în fișierul **robdd.cpp** se află deja o implementare
a acestei probleme.  Această implementare este una destul de bună în practică,
mergând în timp decent până la un număr de **49-50** de variabile, în condițiile
în care worst-case-ul este exponențial.  Puteți citi mai multe despre
proprietățile **ROBDD** în link-ul de mai sus.

Legătura dintre grafurile cu 7 noduri și expresiile boolene cu 49-50 de
variabile este aceea că transformarea **T** creează **V^2** variabile, unde
**V** e numărul de noduri din graf.  Astfel, pentru a ne asigura că variabilele
cu același nume joacă același *rol* în cadrul expresiei (în *ref*-urile generate
de mine și în output-urile voastre), va trebui să respectați următoarea regulă
de formare a numelor variabilelor: `xk, k = ((i - 1)*V + j) = pe poziția i în HP
se află nodul j`; considerăm indexate de la 1 atât nodurile cât și pozițiile în
HP.

Motivul pentru care nu folosim `xij, pe poziția i în HP se află nodul j`
este că pentru grafuri cu mai mult de 11 noduri, notația devine ambiguă; *x111*
înseamnă poziția 1 - nodul 11 sau poziția 11 - nodul 1?

**Notă 1**:  Ultimele 2 teste mănâncă foarte multe resurse.  Dacă știți că aveți
un computer cu resurse limitate, mai bine nu le rulați.  Din testele mele,
acestea mănâncă în jur de 2GB RAM.

**Notă 2**:  Rularea testelor implică rularea programului **robdd.cpp** care
pentru compilare are nevoie de un compilator care suportă măcar una dintre
primele implementări ale C++11 (numită C++0x); spre exemplu, gcc 4.7 ar trebui
să meargă.  Dacă nu aveți și nu puteți face rost de un astfel de compilator,
puteți rula pe **fep.grid.pub.ro** unde vă autentificați cu numele de pe
cs.curs.  Pentru a loada **gcc 4.7** trebuie sa rulați 
`$ module load compilers/gnu-4.7.0`.

