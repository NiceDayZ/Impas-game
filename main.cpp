#include <iostream>
#include <fstream>
#include <winbgim.h>
#include <stdlib.h>
#include <deque>
#include <Windows.h>
#include <mmsystem.h>

using namespace std;

#define MAX 20
//#define FUNDAL 11


struct coordinates{int x; int y;};
struct mutareCmp{coordinates moveFrom; coordinates moveTo;};
int stanga,sus,width,height,latura, numar, turn=1;
bool gata=false,close=false;
bool backgroundMusic = true;
bool tutorial = true;
bool start = true;

int TablaDeJoc[MAX][MAX];

int scor;
int dimensiuneMatrice;

int instanta = 1;
int FUNDAL = 11;
int CULOAREPIESA = 14;
char* soundtrack = "sound.wav";

deque<coordinates> stivaUndo;


//CRACIUN MIHAI - COSMIN
void afisareMatrice(){

    for(int i=1;i<=dimensiuneMatrice;i++){
        for(int j=1;j<=dimensiuneMatrice;j++){
            cout<<TablaDeJoc[i][j]<<' ';
        }cout<<'\n';
    }cout<<'\n';
}

//CRACIUN MIHAI - COSMIN
bool verificaDacaExistaMutareValida(){
    //functia verifica (in functie de a carui tura este) daca mai exista posibilitatea de a muta orice piesa
    int n = dimensiuneMatrice;
    if(turn == 1){
        for(int i=2; i<=n; i++){
            for(int j=1; j<=n; j++){
                if(TablaDeJoc[i][j] == 1 && TablaDeJoc[i-1][j] == 0){
                    return true;

                }
            }
        }
        return false;
    }else{
        for(int i=1; i<=n; i++){
            for(int j=1; j<n; j++){
                if(TablaDeJoc[i][j] == 1 && TablaDeJoc[i][j+1] == 0){
                    return true;
                }
            }
        }
        return false;
    }
}

//CRACIUN MIHAI - COSMIN
mutareCmp calculeazaMutareOptimaAI(){
    //functia calculeaza mutarea optima pe care calculatorul o poate face
    //A.I. - ul functioneaza dupa urmatorul principiu
    //1.daca exista o piesa pe care o poate bloca astfel incat jucatorul sa nu o poata folosi(deasupra piesei exista o piesa sau un rand de piese)
    //atunci face mutarea respectiva
    //2.daca dupa o cautare nu gaseste nicio piesa pe care sa o poata bloca atunci muta pur si simplu cea mai din stanga piesa libera
    //o pozitie spre dreapta

    //Dificultate Mediu spre greu

    coordinates moveFrom;
    coordinates moveTo;
    mutareCmp mutareCompleta;
    int n = dimensiuneMatrice;
    bool gasit = false;
    int k;
    unsigned int temp = rand()%100;

    if(temp>=30)
        for(int i=1; i<=n && !gasit; i++){
            for(int j=2; j<=n && !gasit; j++){
                if(TablaDeJoc[i][j] == 1 && TablaDeJoc[i+1][j]!=1){
                    k = j;
                    while(TablaDeJoc[i+1][k] != 1 && k>1){
                        k--;
                    }
                    if(TablaDeJoc[i+1][k] == 1){
                        gasit = true;
                        moveFrom.y = i+1;
                        moveFrom.x = k;
                        moveTo.y = i+1;
                        moveTo.x = j;
                    }
                }
            }
        }
    if(gasit){
        mutareCmp mutareComputer;
        mutareComputer.moveFrom = moveFrom;
        mutareComputer.moveTo = moveTo;
        return mutareComputer;
    }else{
        for(int j = 1; j<n && !gasit; j++){
            for(int i=1;i<=n && !gasit;i++){
                if(TablaDeJoc[i][j] == 1 && TablaDeJoc[i][j+1] == 0){
                    gasit = true;
                    moveFrom.y = i;
                    moveFrom.x = j;
                    moveTo.y = i;
                    moveTo.x = j+1;
                }
            }
        }

    }
    mutareCompleta.moveFrom = moveFrom;
    mutareCompleta.moveTo = moveTo;
    return mutareCompleta;
}

//CRACIUN MIHAI - COSMIN
void undoFunctionPVP(coordinates afterUndo, coordinates actual){
    //functia da undo la ultima mutare afla in stiva stivaUndo
    //functia se comporta ca si cum este o mutare normala

    //cout<<actual.y<<" "<<actual.x<<" >> "<<afterUndo.y<<" "<<afterUndo.x<<"\n";
    int linia,coloana,x,y;
    int x1, y1, x2, y2;
    int xmijloc, ymijloc;
    linia = afterUndo.y;
    coloana = afterUndo.x;
    TablaDeJoc[linia][coloana]=1;

    setcolor(FUNDAL); setfillstyle(SOLID_FILL,FUNDAL);
    x1=stanga+latura*(coloana-1); y1=sus+latura*(linia-1);
    x2=x1+latura; y2=y1+latura;
    xmijloc=(x1+x2)/2; ymijloc=(y1+y2)/2;
    bar(xmijloc-20,ymijloc-20,xmijloc+20,ymijloc+20);
    // afisez cerc
    setcolor(CULOAREPIESA); setfillstyle(SOLID_FILL,CULOAREPIESA);

    for(int k=1;k<=36; k++){
        fillellipse(xmijloc,ymijloc,k/2,k/2);
        delay(3);
    }

    linia = actual.y;
    coloana = actual.x;
    TablaDeJoc[linia][coloana]=0;

    setcolor(FUNDAL); setfillstyle(SOLID_FILL,FUNDAL);
    x1=stanga+latura*(coloana-1); y1=sus+latura*(linia-1);
    x2=x1+latura; y2=y1+latura;
    xmijloc=(x1+x2)/2; ymijloc=(y1+y2)/2;

    for(int k=0;k<=36;k++){
        fillellipse(xmijloc,ymijloc,k/2,k/2);
        delay(2);
    }
    bar(xmijloc-20,ymijloc-20,xmijloc+20,ymijloc+20);

    turn = turn%2+1;
    afisareMatrice();

}

//CRACIUN MIHAI - COSMIN
void mutareComputer(mutareCmp mutare){
    //functia muta o piesa dintr-o pozitie in alta (mutata aici ca sa nu mai complice functia punerePiesaPVC)

    int linia,coloana,x,y;
    int x1, y1, x2, y2;
    int xmijloc, ymijloc;
    linia = mutare.moveTo.y;
    coloana = mutare.moveTo.x;
    Beep(500,45);
    TablaDeJoc[linia][coloana]=1;
    stivaUndo.push_front(mutare.moveTo);



    setcolor(FUNDAL); setfillstyle(SOLID_FILL,FUNDAL);
    x1=stanga+latura*(coloana-1); y1=sus+latura*(linia-1);
    x2=x1+latura; y2=y1+latura;
    xmijloc=(x1+x2)/2; ymijloc=(y1+y2)/2;
    bar(xmijloc-20,ymijloc-20,xmijloc+20,ymijloc+20);
    // afisez cerc
    setcolor(CULOAREPIESA); setfillstyle(SOLID_FILL,CULOAREPIESA);

    for(int k=1;k<=36; k++){
        fillellipse(xmijloc,ymijloc,k/2,k/2);
        delay(3);
    }

    linia = mutare.moveFrom.y;
    coloana = mutare.moveFrom.x;
    TablaDeJoc[linia][coloana]=0;
    stivaUndo.push_front(mutare.moveFrom);

    setcolor(FUNDAL); setfillstyle(SOLID_FILL,FUNDAL);
    x1=stanga+latura*(coloana-1); y1=sus+latura*(linia-1);
    x2=x1+latura; y2=y1+latura;
    xmijloc=(x1+x2)/2; ymijloc=(y1+y2)/2;

    for(int k=0;k<=36;k++){
        fillellipse(xmijloc,ymijloc,k/2,k/2);
        delay(2);
    }
    bar(xmijloc-20,ymijloc-20,xmijloc+20,ymijloc+20);

    turn = turn%2+1;
    afisareMatrice();
}

//CRACIUN MIHAI - COSMIN
void punerePiesa(){
    int linia,coloana,x,y;
    int x1, y1, x2, y2;
    int xmijloc, ymijloc;

    // cat timp jocul nu s.a terminat se afiseaza un mesaj informativ cu privire la randul jucatorului
    if(turn == 1 && !gata){
        setcolor(YELLOW);
        outtextxy(275,50,"Tura jucatorului 1");
    }else if(turn == 2 && !gata){
        setcolor(YELLOW);
        //settextstyle(10,HORIZ_DIR, 2);
        outtextxy(275,50,"Tura jucatorului 2");
    }
    if(ismouseclick(WM_LBUTTONDOWN))
    {
       clearmouseclick(WM_LBUTTONDOWN);
       x=mousex(); y=mousey();

          linia=(y-sus)/latura+1; coloana=(x-stanga)/latura+1;

          if ((x>=stanga && x<=stanga+width && y>=sus&&y<=sus+height)){
              if (TablaDeJoc[linia][coloana]==0)
                 {  //Daca este randul jucatorului 1
                    if(turn==1){

                        bool gasit = false;
                        int i;
                        for(i=linia; !gasit && i<=dimensiuneMatrice; i++){
                            if(TablaDeJoc[i][coloana]==1) {gasit=true;}
                        }

                        if(gasit){
                            coordinates mutare;

                            Beep(700,45);
                            TablaDeJoc[linia][coloana]=1;
                            mutare.x = coloana;
                            mutare.y = linia;
                            stivaUndo.push_front(mutare);

                            // stergere
                            setcolor(FUNDAL); setfillstyle(SOLID_FILL,FUNDAL);
                            x1=stanga+latura*(coloana-1); y1=sus+latura*(linia-1);
                            x2=x1+latura; y2=y1+latura;
                            xmijloc=(x1+x2)/2; ymijloc=(y1+y2)/2;
                            bar(xmijloc-20,ymijloc-20,xmijloc+20,ymijloc+20);
                            // afisez cerc
                            setcolor(CULOAREPIESA); setfillstyle(SOLID_FILL,CULOAREPIESA);

                            for(int k=1;k<=36; k++){
                                fillellipse(xmijloc,ymijloc,k/2,k/2);
                                delay(3);
                            }

                            TablaDeJoc[i-1][coloana]=0;
                            mutare.x = coloana;
                            mutare.y = i-1;
                            stivaUndo.push_front(mutare);

                            setcolor(FUNDAL); setfillstyle(SOLID_FILL,FUNDAL);
                            x1=stanga+latura*(coloana-1); y1=sus+latura*(i-2);
                            x2=x1+latura; y2=y1+latura;
                            xmijloc=(x1+x2)/2; ymijloc=(y1+y2)/2;

                            for(int k=0;k<=36;k++){
                                fillellipse(xmijloc,ymijloc,k/2,k/2);
                                delay(2);
                            }
                            bar(xmijloc-20,ymijloc-20,xmijloc+20,ymijloc+20);

                            afisareMatrice();
                            turn=2;

                            if(!verificaDacaExistaMutareValida()){

                                gata=true;
                                setcolor(YELLOW);
                                outtextxy(275,50,"Jucatorul 2 a pierdut");

                            }
                        }
                    //daca este tura jucatorului 2
                    }else{
                        bool gasit = false;
                        int i;
                        for(i=coloana; !gasit && i>0; i--){
                            if(TablaDeJoc[linia][i]==1) {gasit=true;}
                        }

                        if(gasit){
                            coordinates mutare;
                            Beep(500,45);
                            TablaDeJoc[linia][coloana]=1;

                            //se adauga la stiva mutarea (unde a ajuns piesa)
                            mutare.x = coloana;
                            mutare.y = linia;
                            stivaUndo.push_front(mutare);

                            // se seteaza coordonatele
                            setcolor(FUNDAL); setfillstyle(SOLID_FILL,FUNDAL);
                            x1=stanga+latura*(coloana-1); y1=sus+latura*(linia-1);
                            x2=x1+latura; y2=y1+latura;
                            xmijloc=(x1+x2)/2; ymijloc=(y1+y2)/2;
                            bar(xmijloc-20,ymijloc-20,xmijloc+20,ymijloc+20);

                            setcolor(CULOAREPIESA); setfillstyle(SOLID_FILL,CULOAREPIESA);

                            //animatie de punere a piesei
                            for(int k=1; k<=36; k++){
                                fillellipse(xmijloc,ymijloc,k/2,k/2);
                                delay(3);
                            }
                            //

                            TablaDeJoc[linia][i+1]=0;
                            //se adauga la stiva mutarea (de unde a plecat piesa)
                            mutare.x = i+1;
                            mutare.y = linia;
                            stivaUndo.push_front(mutare);

                            setcolor(FUNDAL); setfillstyle(SOLID_FILL,FUNDAL);
                            x1=stanga+latura*(i); y1=sus+latura*(linia-1);
                            x2=x1+latura; y2=y1+latura;
                            xmijloc=(x1+x2)/2; ymijloc=(y1+y2)/2;

                            //animatie de stergere a piesei
                            for(int k=0;k<=36;k++){
                                fillellipse(xmijloc,ymijloc,k/2,k/2);
                                delay(2);
                            }
                            bar(xmijloc-20,ymijloc-20,xmijloc+20,ymijloc+20);
                            //

                            afisareMatrice();
                            turn=1;

                            if(!verificaDacaExistaMutareValida()){

                                gata = true;
                                setcolor(YELLOW);
                                outtextxy(275,50,"Jucatorul 1 a pierdut");

                            }

                        }
                    }


                }
        }

        //Daca se apasa pe Exit
        else if(x>=200 && x<=375 && y>=525 && y<=575) {instanta = 1; if(backgroundMusic) PlaySound(TEXT(soundtrack), NULL, SND_FILENAME | SND_ASYNC);}

        //Daca se apasa pe Undo
         else if(x>=425 && x<=600 && y>=525 && y<=575 && !stivaUndo.empty() && !gata) {
            coordinates mutare1, mutare2;
            mutare1 = stivaUndo.front();
            stivaUndo.pop_front();
            mutare2 = stivaUndo.front();
            stivaUndo.pop_front();
            undoFunctionPVP(mutare1, mutare2);

         }

     }
 }

//CRACIUN MIHAI - COSMIN
 void punerePiesaPVC(){
    int linia,coloana,x,y;
    int x1, y1, x2, y2;
    int xmijloc, ymijloc;

    //cat timp jocul nu s.a terminat se afiseaza un text informativ cu privire la randul jucatorului
    if(turn == 1 && !gata){
        setcolor(YELLOW);
        //
        outtextxy(275,50,"Tura jucatorului   ");
    }else if(turn == 2 && !gata){
        setcolor(YELLOW);
        //settextstyle(10,HORIZ_DIR, 2);
        outtextxy(275,50,"Tura Calculatorului");
    }

    if(ismouseclick(WM_LBUTTONDOWN))
    {
       clearmouseclick(WM_LBUTTONDOWN);
       x=mousex(); y=mousey();

          linia=(y-sus)/latura+1; coloana=(x-stanga)/latura+1;

          if ((x>=stanga && x<=stanga+width && y>=sus&&y<=sus+height)){
              if (TablaDeJoc[linia][coloana]==0)
                 {  //Daca este randul jucatorului 1
                    if(turn==1){

                        bool gasit = false;
                        int i;
                        for(i=linia; !gasit && i<=dimensiuneMatrice; i++){
                            if(TablaDeJoc[i][coloana]==1) {gasit=true;}
                        }

                        if(gasit){
                            coordinates mutare;

                            //Beep
                            Beep(700,45);
                            TablaDeJoc[linia][coloana]=1;

                            //se adauga la stiva mutarea (unde a ajuns piesa)
                            mutare.x = coloana;
                            mutare.y = linia;
                            stivaUndo.push_front(mutare);

                            //se seteaza coordonatele unde trebuie pusa piesa
                            setcolor(FUNDAL); setfillstyle(SOLID_FILL,FUNDAL);
                            x1=stanga+latura*(coloana-1); y1=sus+latura*(linia-1);
                            x2=x1+latura; y2=y1+latura;
                            xmijloc=(x1+x2)/2; ymijloc=(y1+y2)/2;
                            bar(xmijloc-20,ymijloc-20,xmijloc+20,ymijloc+20);


                            // afisez cerc
                            setcolor(CULOAREPIESA); setfillstyle(SOLID_FILL,CULOAREPIESA);

                            //animatie de punere piesa pe tabla
                            for(int k=1;k<=36; k++){
                                fillellipse(xmijloc,ymijloc,k/2,k/2);
                                delay(3);
                            }
                            //

                            TablaDeJoc[i-1][coloana]=0;

                            //se adauga la stiva mutarea(de unde a plecat piesa)
                            mutare.x = coloana;
                            mutare.y = i-1;
                            stivaUndo.push_front(mutare);

                            setcolor(FUNDAL); setfillstyle(SOLID_FILL,FUNDAL);
                            x1=stanga+latura*(coloana-1); y1=sus+latura*(i-2);
                            x2=x1+latura; y2=y1+latura;
                            xmijloc=(x1+x2)/2; ymijloc=(y1+y2)/2;

                            //Animatie de stergere a piesei de pe tabla
                            for(int k=0;k<=36;k++){
                                fillellipse(xmijloc,ymijloc,k/2,k/2);
                                delay(2);
                            }
                            bar(xmijloc-20,ymijloc-20,xmijloc+20,ymijloc+20);
                            //

                            afisareMatrice();
                            turn=2;

                            if(!verificaDacaExistaMutareValida()){
                                //se verifica daca calculatorul mai poate muta...in caz contrar jocul se termina
                                gata=true;
                                setcolor(YELLOW);
                                outtextxy(275,50,"Calculatorul a pierdut");

                            }else{
                                 outtextxy(275,50,"                       ");
                                 setcolor(YELLOW);
                                  outtextxy(275,50,"Mutare Calculator");
                                mutareCmp mutareCalculator = calculeazaMutareOptimaAI();
                                delay(500);
                                mutareComputer(mutareCalculator);
                                turn = 1;
                                if(!verificaDacaExistaMutareValida()){
                                    //se verifica daca mai exista vreo mutare valida...in caz contrar jocul se termina
                                    gata = true;
                                    setcolor(YELLOW);
                                    outtextxy(275,50,"                      ");
                                    outtextxy(275,50,"Ai Pierdut");

                                }

                            }
                        }
                    }
                 }
          }
            //daca se apasa pe Exit
           else if(x>=200 && x<=375 && y>=525 && y<=575) {instanta = 1; if(backgroundMusic) PlaySound(TEXT(soundtrack), NULL, SND_FILENAME | SND_ASYNC);}

           //daca se apasa pe UNDO
           else if(x>=425 && x<=600 && y>=525 && y<=575 && !stivaUndo.empty() && !gata) {

            //se da undo la 2 mutari (una fiind cea facuta de calculator
            coordinates mutare1, mutare2;
            mutare1 = stivaUndo.front();
            stivaUndo.pop_front();
            mutare2 = stivaUndo.front();
            stivaUndo.pop_front();
            undoFunctionPVP(mutare1, mutare2);

            mutare1 = stivaUndo.front();
            stivaUndo.pop_front();
            mutare2 = stivaUndo.front();
            stivaUndo.pop_front();
            //cout<<mutare1.y<<" "<<mutare1.x<<" >> "<<mutare2.y<<" "<<mutare2.x;
            undoFunctionPVP(mutare1, mutare2);
         }
    }

 }

//CRACIUN MIHAI - COSMIN
void desen(){
    //TODO: De schimbat numele functiei
    cleardevice();
    //se creaza tabla de joc (vizual)

   int i,j; numar=dimensiuneMatrice;
   width=400; height=400; latura=width/numar;
   sus=(getmaxy()-width)/2; stanga=(getmaxx()-height)/2;
   setbkcolor(BLACK);

   //TODO vezi ce doamne face functia asta de mai jos
   clearviewport();

   setcolor(CYAN);
   setfillstyle(SOLID_FILL,FUNDAL);
   for (i=1; i<=numar; i++)
       for (j=1; j<=numar; j++){

            //sterge ce era in celula (chiar daca nu era nimic inainte...doar de siguranta)
           bar(stanga+latura*(i-1),sus+latura*(j-1),stanga+latura*i,sus+latura*j);

            //patratele din tabla de joc
           rectangle(stanga+latura*(i-1),sus+latura*(j-1),stanga+latura*i,sus+latura*j);
       }

    setcolor(YELLOW);
    rectangle(200, 525, 375, 575);
    settextstyle(10,HORIZ_DIR, 3);
    outtextxy(265, 540, "Exit");

    setcolor(LIGHTCYAN);
    rectangle(425, 525, 600, 575);
    settextstyle(10,HORIZ_DIR, 3);
    //setcolor(YELLOW);
    outtextxy(490, 540, "Undo");
}

//CRACIUN MIHAI - COSMIN
void initMatriceRandom(){

    while(!stivaUndo.empty())   stivaUndo.pop_front();

    //se initiaza matricea cu n-ul ales
    //exista o sansa de 25% ca piesa sa fie pusa pe pozitia [i][j]

    int x1,x2,y1,y2,xmijloc,ymijloc;
    int n = dimensiuneMatrice;
    for(int i=1;i<=n;i++)
        for(int j=1;j<=n;j++)
            TablaDeJoc[i][j]=0;

    for(int i=1;i<=n;i++){
        for(int j=1; j<=n; j++){
            unsigned int temp = rand()%100;
            if(temp < 25){
              TablaDeJoc[i][j] = 1;

                setcolor(FUNDAL);
                setfillstyle(SOLID_FILL,FUNDAL);

                x1=stanga+latura*(j-1);
                y1=sus+latura*(i-1);

                x2=x1+latura;
                y2=y1+latura;
                xmijloc=(x1+x2)/2;
                ymijloc=(y1+y2)/2;

                bar(xmijloc-20,ymijloc-20,xmijloc+20,ymijloc+20);
                setcolor(CULOAREPIESA); setfillstyle(SOLID_FILL,CULOAREPIESA);
                fillellipse(xmijloc,ymijloc,18,18);
            }
        }
    }
}

//CRACIUN MIHAI - COSMIN
void initMatrice(){

    while(!stivaUndo.empty())   stivaUndo.pop_front();

    //se initiaza matricea pentru joc in functie de n-ul ales
    //matricea are in cadranul 3 asezate piesele (i=n/2+1...n; j=1...n/2)

    int x1,x2,y1,y2,xmijloc,ymijloc;
    int n = dimensiuneMatrice;
    for(int i=1;i<=n;i++)
        for(int j=1;j<=n;j++)
            TablaDeJoc[i][j]=0;



    for(int i=n/2+1; i<=n; i++){
        for(int j=1; j<=n/2; j++){
            TablaDeJoc[i][j] = 1;

            setcolor(FUNDAL);
            setfillstyle(SOLID_FILL,FUNDAL);

            x1=stanga+latura*(j-1);
            y1=sus+latura*(i-1);

            x2=x1+latura;
            y2=y1+latura;
            xmijloc=(x1+x2)/2;
            ymijloc=(y1+y2)/2;

            bar(xmijloc-20,ymijloc-20,xmijloc+20,ymijloc+20);
            setcolor(CULOAREPIESA); setfillstyle(SOLID_FILL,CULOAREPIESA);
            fillellipse(xmijloc,ymijloc,18,18);
        }
    }

    afisareMatrice();
}

//ARGHIR RAZVAN - VALERIU
void initMeniu1(){
    cleardevice();
    setbkcolor(BLACK);
    setcolor(YELLOW);

    line(400,0,400,225);
    line(400,275,400,325);
    line(400,375,400,425);
    //line(400,475,400, 525);

    settextstyle(3,HORIZ_DIR, 6);
    setcolor(YELLOW);
    outtextxy(325, 100, "IMPAS");

    settextstyle(3,HORIZ_DIR, 4);

    //Buton de PLAY
    rectangle(245, 225, 555, 275);
    outtextxy(355, 235, "PLAY");

    //Button de High Score
    //rectangle(245, 325, 555, 375);
    //outtextxy(276, 335, "HIGH SCORE");


    //Buton de SETTINGS
    rectangle(245, 325, 555, 375);
    outtextxy(300, 335, "SETTINGS");

    //Buton de EXIT
    rectangle(245, 425, 555, 475);
    outtextxy(365, 435, "EXIT");





}

//ARGHIR RAZVAN - VALERIU
void meniu1(){
    int x,y;
    char golireCelula[] = "                                  "; //variabila se foloseste ca sa rescrie randul actual cu spatiu gol
    bool animationPlay;
    //bool animationHighScore;
    bool animationExit;
    bool animationSettings;



    x = mousex();
    y = mousey();

    //se verifica daca pozitia actuala a cursorului este in unul dintre butoane
    bool inPlayButton = x > 245 && y > 225 && x < 555  && y < 275;
    //bool inHighScoreButton = x > 245 && y > 325 && x < 555 && y < 375;
    bool inExitButton = x > 245 && y > 425 && x < 555 && y < 475;
    bool inSettingsButton = x > 245 && y > 325 && x < 555 && y < 375;
    //bool inSoundButton = x > 620 && y > 510 && x < 750 && y < 550;




    //animatie Play
    if(inPlayButton){
        outtextxy(325, 235, "> PLAY <");
        animationPlay = true;
    }
    if(!inPlayButton && animationPlay){
        animationPlay = false;
        outtextxy(246, 235, golireCelula);
        outtextxy(355, 235, "PLAY");
    }


    //animatie Settings
    if(inSettingsButton){
         outtextxy(270, 335, "> SETTINGS <");
         animationSettings = true;
    }
    if(!inSettingsButton && animationSettings){
         animationSettings = false;
         outtextxy(246, 335, golireCelula);
         outtextxy(300, 335, "SETTINGS");
    }
    /*
    //animatie HighScore
    if(inHighScoreButton){
        outtextxy(246, 335, "> HIGH SCORE <");
        animationHighScore = true;
    }
    if(!inHighScoreButton && animationHighScore){
        animationHighScore = false;
        outtextxy(246, 335, golireCelula);
        outtextxy(276, 335, "HIGH SCORE");
    }
    //
    */
    //animatie Exit
    if(inExitButton){
        outtextxy(335, 435, "> EXIT <");
        animationExit = true;
    }
    if(!inExitButton && animationExit){
        animationExit = false;
        outtextxy(246, 435, golireCelula);
        outtextxy(365, 435, "EXIT");
    }


     if(ismouseclick(WM_LBUTTONDOWN))
    {
       clearmouseclick(WM_LBUTTONDOWN);
       x=mousex(); y=mousey();

        //daca se apasa pe PLAY
       if(inPlayButton){
            if(!tutorial)
                instanta = 11;
            else
                instanta = 4;
            Beep(700, 45);
       } //daca se apasa pe SETTINGS
       else if (inSettingsButton){
            instanta = 3;
            Beep(700, 45);
       }
       //daca se apasa pe EXIT
       else if(inExitButton){
            instanta = 0;
            Beep(700, 45);
       }

    }

}

//ARGHIR RAZVAN - VALERIU
void initMeniu11(){
    cleardevice();
    setbkcolor(BLACK);
    setcolor(YELLOW);

    line(400,0,400,200);
    line(400,300,400,335);
    line(400,435,400,475);

    settextstyle(3,HORIZ_DIR, 6);
    setcolor(YELLOW);
    outtextxy(325, 100, "IMPAS");

    settextstyle(3,HORIZ_DIR, 4);

    //Buton de PVP
    rectangle(245, 200, 555, 300);
    outtextxy(325, 205, "PLAYER");
    outtextxy(375, 235, "VS.");
    outtextxy(325, 265, "PLAYER");

    //Button de PVC
    rectangle(245, 335, 555, 435);
    outtextxy(325, 340, "PLAYER");
    outtextxy(375, 370, "VS.");
    outtextxy(290, 400, "COMPUTER");


    //Button de BACK
    rectangle(245, 475, 555, 525);
    outtextxy(350, 485, "BACK");
}

//ARGHIR RAZVAN - VALERIU
void meniu11(){
    int x,y;
    char golireCelula[] = "                                  "; //variabila se foloseste ca sa rescrie randul actual cu spatiu gol
    bool animationPVP;
    bool animationPVC;
    bool animationExit;

    x = mousex();
    y = mousey();

    //se verifica daca pozitia actuala a cursorului este in unul dintre butoane
    bool inPVPButton = x > 245 && y > 200 && x < 555 && y < 300;
    bool inPVCButton = x > 245 && y > 335 && x < 555 && y < 435;
    bool inExitButton = x > 245 && y > 475 && x < 555 && y < 525;



    //animatie PVP
    if(inPVPButton){
        outtextxy(273, 235, ">         VS.        <");
        animationPVP = true;
    }
    if(!inPVPButton && animationPVP){
        animationPVP = false;
        outtextxy(246, 235, golireCelula);
        outtextxy(375, 235, "VS.");
    }


    //animatie PVC
    if(inPVCButton){
        outtextxy(273, 370, ">         VS.        <");
        animationPVC = true;
    }
    if(!inPVCButton && animationPVC){
        animationPVC = false;
        outtextxy(246, 370, golireCelula);
        outtextxy(375, 370, "VS.");
    }


    //animatie Exit
    if(inExitButton){
        outtextxy(320, 485, "> BACK <");
        animationExit = true;
    }
    if(!inExitButton && animationExit){
        animationExit = false;
        outtextxy(246, 485, golireCelula);
        outtextxy(350, 485, "BACK");
    }


     if(ismouseclick(WM_LBUTTONDOWN))
    {
       clearmouseclick(WM_LBUTTONDOWN);
       x=mousex(); y=mousey();

       if(inPVPButton){   //daca se apasa pe PLAYER VS PLAYER
            instanta=111;
            Beep(700, 45);
       }
       if(inPVCButton){    //daca se apasa pe PLAYER VS COMPUTER
            instanta = 112;
            Beep(700, 45);
       }

        //daca se apasa pe EXIT
       if(inExitButton){
            instanta = 1;
            Beep(700, 45);
       }
    }
}

//ARGHIR RAZVAN - VALERIU
void initMeniu111(){

    cleardevice();
    setbkcolor(BLACK);
    setcolor(YELLOW);

    line(400,0,400,225);
    line(400,275,400,325);
    line(400,375,400,425);

    settextstyle(3,HORIZ_DIR, 6);
    setcolor(YELLOW);
    outtextxy(325, 100, "IMPAS");

    settextstyle(3,HORIZ_DIR, 4);

    //Buton de CLASIC
    rectangle(245, 225, 555, 275);
    outtextxy(330, 235, "CLASIC");

    //Buton de RANDOM
    rectangle(245, 325, 555, 375);
    outtextxy(310, 335, "RANDOM");


    //Buton de BACK
    rectangle(245, 425, 555, 475);
    outtextxy(350, 435, "BACK");

}

//ARGHIR RAZVAN - VALERIU
void meniu111(){
int x,y;
    char golireCelula[] = "                                  "; //variabila se foloseste ca sa rescrie randul actual cu spatiu gol
    bool animationClasic;
    bool animationRandom;
    bool animationBack;

    x = mousex();
    y = mousey();

    //se verifica daca pozitia actuala a cursorului este in unul dintre butoane
    bool inClasicButton = x > 245 && y > 225 && x < 555  && y < 275;
    bool inRandomButton = x > 245 && y > 325 && x < 555 && y < 375;
    bool inBackButton = x > 245 && y > 425 && x < 555 && y < 475;




    //animatie Clasic
    if(inClasicButton){
        outtextxy(300, 235, "> CLASIC <");
        animationClasic = true;
    }
    if(!inClasicButton && animationClasic){
        animationClasic = false;
        outtextxy(246, 235, golireCelula);
        outtextxy(330, 235, "CLASIC");
    }


    //animatie Random
    if(inRandomButton){
        outtextxy(280, 335, "> RANDOM <");
        animationRandom = true;
    }
    if(!inRandomButton && animationRandom){
        animationRandom = false;
        outtextxy(246, 335, golireCelula);
        outtextxy(310, 335, "RANDOM");
    }


    //animatie Back
    if(inBackButton){
        outtextxy(320, 435, "> BACK <");
        animationBack = true;
    }
    if(!inBackButton && animationBack){
        animationBack = false;
        outtextxy(246, 435, golireCelula);
        outtextxy(350, 435, "BACK");
    }


     if(ismouseclick(WM_LBUTTONDOWN))
    {
       clearmouseclick(WM_LBUTTONDOWN);
       x=mousex(); y=mousey();

        //daca se apasa pe PLAY
       if(inClasicButton){
            instanta = 1111;
            Beep(700, 45);
       }
       if(inRandomButton){  //daca se apasa pe RANDOM
            instanta = 1112;
            Beep(700, 45);
       }

       if(inBackButton){  //daca se apasa pe BACK
            instanta = 11;
            Beep(700, 45);
       }
    }

}

//ARGHIR RAZVAN - VALERIU
void initMeniu112(){
    cleardevice();
    setbkcolor(BLACK);
    setcolor(YELLOW);

    line(400,0,400,225);
    line(400,275,400,325);
    line(400,375,400,425);

    settextstyle(3,HORIZ_DIR, 6);
    setcolor(YELLOW);
    outtextxy(325, 100, "IMPAS");

    settextstyle(3,HORIZ_DIR, 4);

    //Buton de CLASIC
    rectangle(245, 225, 555, 275);
    outtextxy(330, 235, "CLASIC");

    //Buton de RANDOM
    rectangle(245, 325, 555, 375);
    outtextxy(310, 335, "RANDOM");


    //Buton de BACK
    rectangle(245, 425, 555, 475);
    outtextxy(350, 435, "BACK");
}

//ARGHIR RAZVAN - VALERIU
void meniu112(){
    int x,y;
    char golireCelula[] = "                                  "; //variabila se foloseste ca sa rescrie randul actual cu spatiu gol
    bool animationClasic;
    bool animationRandom;
    bool animationBack;

    x = mousex();
    y = mousey();

    //se verifica daca pozitia actuala a cursorului este in unul dintre butoane
    bool inClasicButton = x > 245 && y > 225 && x < 555  && y < 275;
    bool inRandomButton = x > 245 && y > 325 && x < 555 && y < 375;
    bool inBackButton = x > 245 && y > 425 && x < 555 && y < 475;




    //animatie Clasic
    if(inClasicButton){
        outtextxy(300, 235, "> CLASIC <");
        animationClasic = true;
    }
    if(!inClasicButton && animationClasic){
        animationClasic = false;
        outtextxy(246, 235, golireCelula);
        outtextxy(330, 235, "CLASIC");
    }


    //animatie Random
    if(inRandomButton){
        outtextxy(280, 335, "> RANDOM <");
        animationRandom = true;
    }
    if(!inRandomButton && animationRandom){
        animationRandom = false;
        outtextxy(246, 335, golireCelula);
        outtextxy(310, 335, "RANDOM");
    }


    //animatie Back
    if(inBackButton){
        outtextxy(320, 435, "> BACK <");
        animationBack = true;
    }
    if(!inBackButton && animationBack){
        animationBack = false;
        outtextxy(246, 435, golireCelula);
        outtextxy(350, 435, "BACK");
    }


     if(ismouseclick(WM_LBUTTONDOWN))
    {
       clearmouseclick(WM_LBUTTONDOWN);
       x=mousex(); y=mousey();

        //daca se apasa pe PLAY
       if(inClasicButton){
            instanta = 1121;
            Beep(700, 45);
       }
       if(inRandomButton){  //daca se apasa pe RANDOM
            instanta = 1122;
            Beep(700, 45);
       }

       if(inBackButton){  //daca se apasa pe BACK
            instanta = 11;
            Beep(700, 45);
       }
    }
}

//ARGHIR RAZVAN - VALERIU
void initMeniuAlegereDimensiune1111(){
    cleardevice();
    setbkcolor(BLACK);
    setcolor(YELLOW);

    line(400,0,400,225);
    line(400,275,400,325);
    line(400,375,400,425);

    settextstyle(3,HORIZ_DIR, 6);
    setcolor(YELLOW);
    outtextxy(325, 100, "IMPAS");

    settextstyle(3,HORIZ_DIR, 4);

    //Buton de 4x4
    rectangle(245, 225, 555, 275);
    outtextxy(370, 235, "4x4");

    //Buton de 6x6
    rectangle(245, 325, 555, 375);
    outtextxy(370, 335, "6x6");


    //Buton de 8x8
    rectangle(245, 425, 555, 475);
    outtextxy(370, 435, "8x8");
}

//ARGHIR RAZVAN - VALERIU
void meniuAlegereDimensiune1111(){
    int x,y;
    char golireCelula[] = "                                  "; //variabila se foloseste ca sa rescrie randul actual cu spatiu gol
    bool animation4x4;
    bool animation6x6;
    bool animation8x8;

    x = mousex();
    y = mousey();

    //se verifica daca pozitia actuala a cursorului este in unul dintre butoane
    bool in4x4Button = x > 245 && y > 225 && x < 555  && y < 275;
    bool in6x6Button = x > 245 && y > 325 && x < 555 && y < 375;
    bool in8x8Button = x > 245 && y > 425 && x < 555 && y < 475;

    //animatie 4x4
    if(in4x4Button){
        outtextxy(340, 235, "> 4x4 <");
        animation4x4 = true;
    }
    if(!in4x4Button && animation4x4){
        animation4x4 = false;
        outtextxy(246, 235, golireCelula);
        outtextxy(370, 235, "4x4");
    }


    //animatie 6x6
    if(in6x6Button){
        outtextxy(340, 335, "> 6x6 <");
        animation6x6 = true;
    }
    if(!in6x6Button && animation6x6){
        animation6x6 = false;
        outtextxy(246, 335, golireCelula);
        outtextxy(370, 335, "6x6");
    }


    //animatie 8x8
    if(in8x8Button){
        outtextxy(340, 435, "> 8x8 <");
        animation8x8 = true;
    }
    if(!in8x8Button && animation8x8){
        animation8x8 = false;
        outtextxy(246, 435, golireCelula);
        outtextxy(370, 435, "8x8");
    }


    if(ismouseclick(WM_LBUTTONDOWN))
    {
       clearmouseclick(WM_LBUTTONDOWN);
       x=mousex(); y=mousey();

        //daca se apasa pe 4x4
       if(in4x4Button){
            dimensiuneMatrice = 4;
            instanta = 5;
            Beep(700, 45);
       }
       else if(in6x6Button){      //daca se apasa pe 6x6
            dimensiuneMatrice = 6;
            instanta = 5;
            Beep(700, 45);
       }

       else if(in8x8Button){      //daca se apasa pe 8x8
            dimensiuneMatrice = 8;
            instanta = 5;
            Beep(700, 45);
       }
    }
}

//ARGHIR RAZVAN - VALERIU
void initMeniuAlegereDimensiun1112(){
    cleardevice();
    setbkcolor(BLACK);
    setcolor(YELLOW);

    line(400,0,400,225);
    line(400,275,400,325);
    line(400,375,400,425);

    settextstyle(3,HORIZ_DIR, 6);
    setcolor(YELLOW);
    outtextxy(325, 100, "IMPAS");

    settextstyle(3,HORIZ_DIR, 4);

    //Buton de 4x4
    rectangle(245, 225, 555, 275);
    outtextxy(370, 235, "4x4");

    //Buton de 6x6
    rectangle(245, 325, 555, 375);
    outtextxy(370, 335, "6x6");


    //Buton de 8x8
    rectangle(245, 425, 555, 475);
    outtextxy(370, 435, "8x8");
}

//ARGHIR RAZVAN - VALERIU
void meniuAlegereDimensiune1112(){
    int x,y;
    char golireCelula[] = "                                  "; //variabila se foloseste ca sa rescrie randul actual cu spatiu gol
    bool animation4x4;
    bool animation6x6;
    bool animation8x8;

    x = mousex();
    y = mousey();

    //se verifica daca pozitia actuala a cursorului este in unul dintre butoane
    bool in4x4Button = x > 245 && y > 225 && x < 555  && y < 275;
    bool in6x6Button = x > 245 && y > 325 && x < 555 && y < 375;
    bool in8x8Button = x > 245 && y > 425 && x < 555 && y < 475;

    //animatie 4x4
    if(in4x4Button){
        outtextxy(340, 235, "> 4x4 <");
        animation4x4 = true;
    }
    if(!in4x4Button && animation4x4){
        animation4x4 = false;
        outtextxy(246, 235, golireCelula);
        outtextxy(370, 235, "4x4");
    }


    //animatie 6x6
    if(in6x6Button){
        outtextxy(340, 335, "> 6x6 <");
        animation6x6 = true;
    }
    if(!in6x6Button && animation6x6){
        animation6x6 = false;
        outtextxy(246, 335, golireCelula);
        outtextxy(370, 335, "6x6");
    }


    //animatie 8x8
    if(in8x8Button){
        outtextxy(340, 435, "> 8x8 <");
        animation8x8 = true;
    }
    if(!in8x8Button && animation8x8){
        animation8x8 = false;
        outtextxy(246, 435, golireCelula);
        outtextxy(370, 435, "8x8");
    }


    if(ismouseclick(WM_LBUTTONDOWN))
    {
       clearmouseclick(WM_LBUTTONDOWN);
       x=mousex(); y=mousey();

        //daca se apasa pe 4x4
       if(in4x4Button){
            dimensiuneMatrice = 4;
            instanta = 6;
            Beep(700, 45);
       }
       else if(in6x6Button){      //daca se apasa pe 6x6
            dimensiuneMatrice = 6;
            instanta = 6;
            Beep(700, 45);
       }

       else if(in8x8Button){      //daca se apasa pe 8x8
            dimensiuneMatrice = 8;
            instanta = 6;
            Beep(700, 45);
       }
    }
}

//ARGHIR RAZVAN - VALERIU
void initMeniuAlegereDimensiun1121(){
    cleardevice();
    setbkcolor(BLACK);
    setcolor(YELLOW);

    line(400,0,400,225);
    line(400,275,400,325);
    line(400,375,400,425);

    settextstyle(3,HORIZ_DIR, 6);
    setcolor(YELLOW);
    outtextxy(325, 100, "IMPAS");

    settextstyle(3,HORIZ_DIR, 4);

    //Buton de 4x4
    rectangle(245, 225, 555, 275);
    outtextxy(370, 235, "4x4");

    //Buton de 6x6
    rectangle(245, 325, 555, 375);
    outtextxy(370, 335, "6x6");


    //Buton de 8x8
    rectangle(245, 425, 555, 475);
    outtextxy(370, 435, "8x8");
}

//ARGHIR RAZVAN - VALERIU
void meniuAlegereDimensiune1121(){
     int x,y;
    char golireCelula[] = "                                  "; //variabila se foloseste ca sa rescrie randul actual cu spatiu gol
    bool animation4x4;
    bool animation6x6;
    bool animation8x8;

    x = mousex();
    y = mousey();

    //se verifica daca pozitia actuala a cursorului este in unul dintre butoane
    bool in4x4Button = x > 245 && y > 225 && x < 555  && y < 275;
    bool in6x6Button = x > 245 && y > 325 && x < 555 && y < 375;
    bool in8x8Button = x > 245 && y > 425 && x < 555 && y < 475;

    //animatie 4x4
    if(in4x4Button){
        outtextxy(340, 235, "> 4x4 <");
        animation4x4 = true;
    }
    if(!in4x4Button && animation4x4){
        animation4x4 = false;
        outtextxy(246, 235, golireCelula);
        outtextxy(370, 235, "4x4");
    }


    //animatie 6x6
    if(in6x6Button){
        outtextxy(340, 335, "> 6x6 <");
        animation6x6 = true;
    }
    if(!in6x6Button && animation6x6){
        animation6x6 = false;
        outtextxy(246, 335, golireCelula);
        outtextxy(370, 335, "6x6");
    }


    //animatie 8x8
    if(in8x8Button){
        outtextxy(340, 435, "> 8x8 <");
        animation8x8 = true;
    }
    if(!in8x8Button && animation8x8){
        animation8x8 = false;
        outtextxy(246, 435, golireCelula);
        outtextxy(370, 435, "8x8");
    }


    if(ismouseclick(WM_LBUTTONDOWN))
    {
       clearmouseclick(WM_LBUTTONDOWN);
       x=mousex(); y=mousey();

        //daca se apasa pe 4x4
       if(in4x4Button){
            dimensiuneMatrice = 4;
            instanta = 7;
            Beep(700, 45);
       }
       else if(in6x6Button){      //daca se apasa pe 6x6
            dimensiuneMatrice = 6;
            instanta = 7;
            Beep(700, 45);
       }

       else if(in8x8Button){      //daca se apasa pe 8x8
            dimensiuneMatrice = 8;
            instanta = 7;
            Beep(700, 45);
       }
    }
}

//ARGHIR RAZVAN - VALERIU
void initMeniuAlegereDimensiun1122(){
    cleardevice();
    setbkcolor(BLACK);
    setcolor(YELLOW);

    line(400,0,400,225);
    line(400,275,400,325);
    line(400,375,400,425);

    settextstyle(3,HORIZ_DIR, 6);
    setcolor(YELLOW);
    outtextxy(325, 100, "IMPAS");

    settextstyle(3,HORIZ_DIR, 4);

    //Buton de 4x4
    rectangle(245, 225, 555, 275);
    outtextxy(370, 235, "4x4");

    //Buton de 6x6
    rectangle(245, 325, 555, 375);
    outtextxy(370, 335, "6x6");


    //Buton de 8x8
    rectangle(245, 425, 555, 475);
    outtextxy(370, 435, "8x8");
}

//ARGHIR RAZVAN - VALERIU
void meniuAlegereDimensiune1122(){
    int x,y;
    char golireCelula[] = "                                  "; //variabila se foloseste ca sa rescrie randul actual cu spatiu gol
    bool animation4x4;
    bool animation6x6;
    bool animation8x8;

    x = mousex();
    y = mousey();

    //se verifica daca pozitia actuala a cursorului este in unul dintre butoane
    bool in4x4Button = x > 245 && y > 225 && x < 555  && y < 275;
    bool in6x6Button = x > 245 && y > 325 && x < 555 && y < 375;
    bool in8x8Button = x > 245 && y > 425 && x < 555 && y < 475;

    //animatie 4x4
    if(in4x4Button){
        outtextxy(340, 235, "> 4x4 <");
        animation4x4 = true;
    }
    if(!in4x4Button && animation4x4){
        animation4x4 = false;
        outtextxy(246, 235, golireCelula);
        outtextxy(370, 235, "4x4");
    }


    //animatie 6x6
    if(in6x6Button){
        outtextxy(340, 335, "> 6x6 <");
        animation6x6 = true;
    }
    if(!in6x6Button && animation6x6){
        animation6x6 = false;
        outtextxy(246, 335, golireCelula);
        outtextxy(370, 335, "6x6");
    }


    //animatie 8x8
    if(in8x8Button){
        outtextxy(340, 435, "> 8x8 <");
        animation8x8 = true;
    }
    if(!in8x8Button && animation8x8){
        animation8x8 = false;
        outtextxy(246, 435, golireCelula);
        outtextxy(370, 435, "8x8");
    }


    if(ismouseclick(WM_LBUTTONDOWN))
    {
       clearmouseclick(WM_LBUTTONDOWN);
       x=mousex(); y=mousey();

        //daca se apasa pe 4x4
       if(in4x4Button){
            dimensiuneMatrice = 4;
            instanta = 8;
            Beep(700, 45);
       }
       else if(in6x6Button){      //daca se apasa pe 6x6
            dimensiuneMatrice = 6;
            instanta = 8;
            Beep(700, 45);
       }

       else if(in8x8Button){      //daca se apasa pe 8x8
            dimensiuneMatrice = 8;
            instanta = 8;
            Beep(700, 45);
       }
    }
}

//ARGHIR RAZVAN - VALERIU
void initMeniuSetari(){
    cleardevice();
    //line(400,0,400,600);
    settextstyle(3,HORIZ_DIR, 6);
    outtextxy(295, 30, "COLORS");

    //3 BUTOANE PENTRU 3 CULORI ALE FUNDALULUI
    setfillstyle(SOLID_FILL,LIGHTCYAN);
    bar(280, 110, 310, 140);

    setfillstyle(SOLID_FILL,LIGHTGRAY);
    bar(320, 110, 350, 140);

    setfillstyle(SOLID_FILL,LIGHTGREEN);
    bar(360, 110, 390, 140);

    //SAGEATA CARE ARATA CULOAREA FUNDALULUI ALEASA CURENT
    if(FUNDAL == 11){
         settextstyle(3,HORIZ_DIR, 4);
         outtextxy(285,150 , "^");
    }else if(FUNDAL == 7){
         settextstyle(3,HORIZ_DIR, 4);
         outtextxy(325, 150, "^");
    }else if(FUNDAL == 10){
        settextstyle(3,HORIZ_DIR, 4);
         outtextxy(365,150 , "^");
    }


    //3 BUTOANE PENTRU 3 CULORI ALE PIESELOR
    setcolor(CYAN);
    setfillstyle(SOLID_FILL,YELLOW);
    fillellipse(425, 125, 15, 15);

    setfillstyle(SOLID_FILL,LIGHTRED);
    fillellipse(465, 125, 15, 15);

    setfillstyle(SOLID_FILL,BLUE);
    fillellipse(505, 125, 15, 15);

    //SAGEATA CARE INDICA CULOAREA ALEASA CURENT
    setcolor(YELLOW);

     if(CULOAREPIESA == 14){
         settextstyle(3,HORIZ_DIR, 4);
         outtextxy(415,150 , "^");
    }else if(CULOAREPIESA == 12){
         settextstyle(3,HORIZ_DIR, 4);
         outtextxy(455, 150, "^");
    }else if(CULOAREPIESA == 1){
        settextstyle(3,HORIZ_DIR, 4);
         outtextxy(495,150 , "^");
    }

    //TEXT CARE INDICA CATRE ALEGEREA COLOANEI SONORE
    settextstyle(3,HORIZ_DIR, 6);
    outtextxy(220, 180, "SOUNDTRACK");
    settextstyle(3,HORIZ_DIR, 2);

    //5 BUTOANE PENTRU ALEGEREA DINTRE CELE 5 COLOANE SONORE
    rectangle(305, 260, 335, 290);
    outtextxy(315,265,"1");

    rectangle(345 ,260, 375, 290);
    outtextxy(355,265,"2");

    rectangle(385, 260, 415, 290);
    outtextxy(395,265,"3");

    rectangle(425, 260, 455, 290);
    outtextxy(435,265,"4");

    rectangle(465, 260, 495, 290);
    outtextxy(475,265,"5");

    settextstyle(3,HORIZ_DIR, 6);

    //BUTON PENTRU ACTIVAREA/DEZACTIVAREA TUTORIALULUI
    rectangle(200, 320, 620, 390);
    if(tutorial)
        outtextxy(210, 330,"TUTORIAL: ON");
    else
        outtextxy(210, 330,"TUTORIAL: OFF");

    //BUTON PENTRU ACTIVAREA/DEZACTIVAREA COLOANEI SONORE
    if(backgroundMusic)
        outtextxy(260, 425, "MUSIC: ON");
    else
        outtextxy(260, 425, "MUSIC: OFF");
    rectangle(250, 415, 570, 485);

    //BUTON DE BACK
    outtextxy(340, 520, "BACK");
    rectangle(250, 510, 570, 580);

}

//ARGHIR RAZVAN - VALERIU
void meniuSetari(){
    int x,y;
    char golireCelula[] = "              "; //variabila se foloseste ca sa rescrie randul actual cu spatiu gol

    bool animatieBack;

    x = mousex();
    y = mousey();

    bool fundalLightCyan = x > 280 && y > 110 && x < 310 && y < 140;
    bool fundalLightGray = x > 320 && y > 110 && x < 350 && y < 140;
    bool fundalLightGreen = x > 360 && y > 110 && x < 390 && y < 140;

    bool culoarePiesaYellow = x > 410 && y > 110 && x < 440 && y < 140;
    bool culoarePiesaLightRed = x > 410 && y > 110 && x < 480 && y < 140;
    bool culoarePiesaBlue = x > 410 && y > 110 && x < 520 && y < 140;

    bool track1 = x > 305 && y > 260 && x < 335 && y < 290;
    bool track2 = x > 345 && y > 260 && x < 375 && y < 290;
    bool track3 = x > 385 && y > 260 && x < 415 && y < 290;
    bool track4 = x > 425 && y > 260 && x < 455 && y < 290;
    bool track5 = x > 465 && y > 260 && x < 495 && y < 290;

    bool inMusicButton = x > 250 && y > 415 && x < 570 && y < 485;
    bool inTutorialButton = x > 200 && y > 320 && x < 620 && y < 390;
    bool inBackButton = x > 250 && y > 510 && x < 570 && y < 580;


    //animatie buton BACK
    if(inBackButton){
       settextstyle(3,HORIZ_DIR, 6);
       outtextxy(295, 520, "> BACK <");
       animatieBack = true;
    }
    if(!inBackButton && animatieBack){
       outtextxy(295, 520, "                 ");
       outtextxy(340, 520, "BACK");
       animatieBack = false;
    }

    if(ismouseclick(WM_LBUTTONDOWN))
    {
       clearmouseclick(WM_LBUTTONDOWN);
       x=mousex(); y=mousey();

       //MEMORAREA CULORII FUNDALULUI ALEASA SI INDICAREA ACESTEIA CU O SAGEATA
       if(fundalLightCyan){
           settextstyle(3,HORIZ_DIR, 4);
           outtextxy(285,150,golireCelula);
           outtextxy(285,150 , "^");
           FUNDAL = 11;
       }else if(fundalLightGray){
           settextstyle(3,HORIZ_DIR, 4);
           outtextxy(285,150,golireCelula);
           outtextxy(325, 150, "^");
           FUNDAL = 7;
       }else if(fundalLightGreen){
           settextstyle(3,HORIZ_DIR, 4);
           outtextxy(285,150,golireCelula);
           outtextxy(365,150 , "^");
           FUNDAL = 10;
       }else if(culoarePiesaYellow){   //MEMORAREA CULORII PIESELOR ALEASA SI INDICAREA ACESTEIA CU O SAGEATA
           settextstyle(3,HORIZ_DIR, 4);
           outtextxy(415,150,golireCelula);
           outtextxy(415,150 , "^");
           CULOAREPIESA = 14;
       }else if(culoarePiesaLightRed){
           settextstyle(3,HORIZ_DIR, 4);
           outtextxy(415,150,golireCelula);
           outtextxy(455, 150, "^");
           CULOAREPIESA = 12;
       }else if(culoarePiesaBlue){
           settextstyle(3,HORIZ_DIR, 4);
           outtextxy(415,150,golireCelula);
           outtextxy(495,150 , "^");
           CULOAREPIESA = 1;
       }else if(track1){             //SCHIMBAREA COLOANEI SONORE
            soundtrack = "sound.wav";
            if(backgroundMusic){
                PlaySound(TEXT(soundtrack), NULL, SND_FILENAME | SND_ASYNC);
            }
       }else if(track2){
            soundtrack = "sound2.wav";
            if(backgroundMusic){
                PlaySound(TEXT(soundtrack), NULL, SND_FILENAME | SND_ASYNC);
            }
       }else if(track3){
            soundtrack = "sound3.wav";
            if(backgroundMusic){
                PlaySound(TEXT(soundtrack), NULL, SND_FILENAME | SND_ASYNC);
            }
       }else if(track4){
            soundtrack = "sound4.wav";
            if(backgroundMusic){
                PlaySound(TEXT(soundtrack), NULL, SND_FILENAME | SND_ASYNC);
            }
       }else if(track5){
            soundtrack = "sound5.wav";
            if(backgroundMusic){
                PlaySound(TEXT(soundtrack), NULL, SND_FILENAME | SND_ASYNC);
            }
       }else if(inMusicButton){                   //ACTIVAREA/DEZACTIVAREA COLOANEI SONORE
            if(backgroundMusic){
                settextstyle(3,HORIZ_DIR, 6);
                outtextxy(260, 425, "MUSIC: OFF");
                PlaySound(NULL, 0,0);
                backgroundMusic = false;
            }else{
                settextstyle(3,HORIZ_DIR, 6);
                outtextxy(260, 425, "MUSIC: ON  ");
                PlaySound(TEXT(soundtrack), NULL, SND_FILENAME | SND_ASYNC);
                backgroundMusic = true;
            }
       }else if(inTutorialButton){                  //ACTIVAREA/DEZACTIVAREA TUTORIALULUI
            if(tutorial){
                settextstyle(3,HORIZ_DIR, 6);
                tutorial = false;
                outtextxy(210, 330,"TUTORIAL: OFF");
            }else{
                settextstyle(3,HORIZ_DIR, 6);
                tutorial = true;
                outtextxy(210, 330,"TUTORIAL: ON  ");
            }
       }
       else if(inBackButton){ //DACA SE APASA BUTONUL BACK
           Beep(700, 45);
           instanta=1;
       }
    }

    //

}

//CRACIUN MIHAI - COSMIN
void initMeniuTutorial(){
    cleardevice();
    //line(400,0,400,600);

    setbkcolor(BLACK);
    settextstyle(3,HORIZ_DIR, 6);
    setcolor(YELLOW);
    outtextxy(265, 70, "TUTORIAL");

    int i,j; numar=4;
    width=150; height=150; latura=width/numar;
    sus=170; stanga=100;



    setcolor(CYAN);
    setfillstyle(SOLID_FILL,FUNDAL);
    for (i=1; i<=numar; i++)
        for (j=1; j<=numar; j++){

            //sterge ce era in celula (chiar daca nu era nimic inainte...doar de siguranta)
            bar(stanga+latura*(i-1),sus+latura*(j-1),stanga+latura*i,sus+latura*j);

            //patratele din tabla de joc
            rectangle(stanga+latura*(i-1),sus+latura*(j-1),stanga+latura*i,sus+latura*j);
        }
    setfillstyle(SOLID_FILL, CULOAREPIESA);
    fillellipse(119,300,10,10);
    fillellipse(157,300,10,10);
    fillellipse(119,263,10,10);
    fillellipse(157,263,10,10);


    sus=170; stanga=325;


    setcolor(CYAN);
    setfillstyle(SOLID_FILL,FUNDAL);
    for (i=1; i<=numar; i++)
        for (j=1; j<=numar; j++){

            //sterge ce era in celula (chiar daca nu era nimic inainte...doar de siguranta)
            bar(stanga+latura*(i-1),sus+latura*(j-1),stanga+latura*i,sus+latura*j);

            //patratele din tabla de joc
            rectangle(stanga+latura*(i-1),sus+latura*(j-1),stanga+latura*i,sus+latura*j);
        }
    setfillstyle(SOLID_FILL, CULOAREPIESA);
    fillellipse(344,300,10,10);
    fillellipse(418,300,10,10);
    fillellipse(344,225,10,10);
    fillellipse(381,262,10,10);

    sus=170; stanga=550;


    setcolor(CYAN);
    setfillstyle(SOLID_FILL,FUNDAL);
    for (i=1; i<=numar; i++)
        for (j=1; j<=numar; j++){

            //sterge ce era in celula (chiar daca nu era nimic inainte...doar de siguranta)
            bar(stanga+latura*(i-1),sus+latura*(j-1),stanga+latura*i,sus+latura*j);

            //patratele din tabla de joc
            rectangle(stanga+latura*(i-1),sus+latura*(j-1),stanga+latura*i,sus+latura*j);

        }

    setfillstyle(SOLID_FILL, CULOAREPIESA);
    fillellipse(568,263,10,10); //<
    fillellipse(681,188,10,10);
    fillellipse(643,188,10,10);
    fillellipse(643,225,10,10);




    setcolor(YELLOW);
    settextstyle(3,HORIZ_DIR, 1);
    outtextxy(100, 325, "Pentru a muta o piesa");
    outtextxy(100, 345, "se apasa pe casuta");
    outtextxy(100, 365, "unde trebuie sa ajunga.");
    outtextxy(100, 385, "Fiecare jucator are");
    outtextxy(100, 405, "cate o singura mutare");
    outtextxy(100, 425, "O piesa nu poate sari");
    outtextxy(100, 445, "peste o alta piesa");

    outtextxy(325, 325, "Jucatorul 1 incepe.");
    outtextxy(325, 345, "Jucatorul 1 poate muta");
    outtextxy(325, 365, "doar pe verticala in sus");
    outtextxy(325, 385, "Jucatorul 2 poate muta");
    outtextxy(325, 405, "apoi doar pe orizontala");
    outtextxy(325, 425, "spre dreapta.");

    outtextxy(550, 325, "Scopul jocului este ca");
    outtextxy(550, 345, "unul dintre jucatori sa");
    outtextxy(550, 365, "îl blocheze pe celalalt");
    outtextxy(550, 385, "astfel incat acesta sa");
    outtextxy(550, 405, "nu mai poata sa faca");
    outtextxy(550, 425, "nicio mutare valida.");

    setcolor(YELLOW);
    rectangle(100, 525, 275, 575);
    settextstyle(3,HORIZ_DIR, 3);
    outtextxy(160, 540, "BACK");


    rectangle(525, 525, 700, 575);
    settextstyle(3,HORIZ_DIR, 3);
    //setcolor(YELLOW);
    outtextxy(590, 540, "NEXT");

}

//CRACIUN MIHAI - COSMIN
void meniuTutorial(){


    bool animationBack;
    bool animationNext;

    int x = mousex();
    int y = mousey();

    bool inTutorial1 = x > 100 && y > 170 && x < 250 && y < 320;
    bool inTutorial2 = x > 325 && y > 170 && x < 475 && y < 320;
    bool inTutorial3 = x > 550 && y > 170 && x < 700 && y < 320;
    bool inBackButton = x > 100 && y > 525 && x < 275 && y < 575;
    bool inNextButton = x > 525 && y > 525 && x < 700 && y < 575;


    if(inTutorial1){
        setcolor(BLACK);
        delay(300);
        line(105,175, 115, 185);
        line(132,175, 122, 185);
        line(105, 203, 115, 193);
        line(132, 203, 122, 193);

        delay(200);
        setfillstyle(SOLID_FILL, FUNDAL);
        setcolor(CYAN);
        bar(100,170,137,207);
        rectangle(100,170,137,207);

        for(int i = 0; i <= 100; i++){
            setcolor(FUNDAL);
            setfillstyle(SOLID_FILL, FUNDAL);
            fillellipse(119,263,i/10,i/10);
            delay(1);
        }

        for(int i = 0; i <= 100; i++){
            setcolor(CYAN);
            setfillstyle(SOLID_FILL, CULOAREPIESA);
            fillellipse(119,188,i/10,i/10);
            delay(1);
        }

        delay(1000);

        setfillstyle(SOLID_FILL, FUNDAL);
        bar(100,170,137,207);
        rectangle(100,170,137,207);

        setfillstyle(SOLID_FILL, CULOAREPIESA);
        fillellipse(119,263,10,10);

    }

    if(inTutorial2){
        setcolor(BLACK);
        delay(300);
        line(367, 175, 377, 185);
        line(394, 175, 384, 185);
        line(367, 203, 377, 193);
        line(394, 203, 384, 193);

        delay(200);

        setfillstyle(SOLID_FILL, FUNDAL);
        setcolor(CYAN);
        bar(362,170,399,207);
        rectangle(362,170,399,207);


        for(int i = 0; i<= 100; i++){
            setcolor(FUNDAL);
            setfillstyle(SOLID_FILL, FUNDAL);
            fillellipse(381,262,i/10,i/10);
            delay(1);
        }

        for(int i = 0; i<= 100; i++){
            setcolor(CYAN);
            setfillstyle(SOLID_FILL, CULOAREPIESA);
            fillellipse(381,188,i/10,i/10);
            delay(1);
        }

        delay(500);

        setcolor(BLACK);
        line(367, 212, 377, 222);
        line(394, 212, 384, 222);
        line(367, 240, 377, 230);
        line(394, 240, 384, 230);

        delay(200);

        setfillstyle(SOLID_FILL, FUNDAL);
        setcolor(CYAN);
        bar(362,207,399,244);
        rectangle(362,207,399,244);

        for(int i = 0; i<= 100; i++){
            setcolor(FUNDAL);
            setfillstyle(SOLID_FILL, FUNDAL);
            fillellipse(344,225,i/10,i/10);
            delay(1);
        }

        for(int i = 0; i<= 100; i++){
            setcolor(CYAN);
            setfillstyle(SOLID_FILL, CULOAREPIESA);
            fillellipse(381,225,i/10,i/10);
            delay(1);
        }

         delay (1000);

        setcolor(CYAN);
        setfillstyle(SOLID_FILL, FUNDAL);
        bar(362,170,399,207);
        rectangle(362,170,399,207);

        setfillstyle(SOLID_FILL, FUNDAL);
        bar(362,207,399,244);
        rectangle(362,207,399,244);

        setfillstyle(SOLID_FILL, CULOAREPIESA);
        fillellipse(344,225,10,10);
        fillellipse(381,262,10,10);
    }


    if(inTutorial3){
        setcolor(BLACK);
        delay(300);
        line(629, 249, 639, 259);
        line(656, 249, 646, 259);
        line(629, 277, 639, 267);
        line(656, 277, 646, 267);

        delay(200);
        setfillstyle(SOLID_FILL, FUNDAL);
        setcolor(CYAN);
        bar(624,244,661,281);
        rectangle(624,244,661,281);

        for(int i = 0; i <= 100; i++){
            setcolor(FUNDAL);
            setfillstyle(SOLID_FILL, FUNDAL);
            fillellipse(568,263,i/10,i/10);
            delay(1);
        }

        for(int i = 0; i <= 100; i++){
            setcolor(CYAN);
            setfillstyle(SOLID_FILL, CULOAREPIESA);
            fillellipse(642,263,i/10,i/10);
            delay(1);
        }

        delay(1000);

        setfillstyle(SOLID_FILL, FUNDAL);
        bar(624,244,661,281);
        rectangle(624,244,661,281);

        setfillstyle(SOLID_FILL, CULOAREPIESA);
        fillellipse(568,263,10,10);

    }

    if(inBackButton){
        setcolor(YELLOW);
        animationBack = true;
        outtextxy(143, 540, "> BACK <");
    }
    if(!inBackButton && animationBack){
        setcolor(YELLOW);
        animationBack = false;
        outtextxy(143, 540, "                      ");
        outtextxy(160, 540, "BACK");
    }

    if(inNextButton){
        setcolor(YELLOW);
        animationNext = true;
        outtextxy(573, 540, "> NEXT <");
    }
    if(!inNextButton && animationNext){
        setcolor(YELLOW);
        animationNext = false;
        outtextxy(573, 540, "                      ");
        outtextxy(590, 540, "NEXT");
    }

      if(ismouseclick(WM_LBUTTONDOWN))
        {
           clearmouseclick(WM_LBUTTONDOWN);
           if(inBackButton){
            Beep(700, 35);
            instanta = 1;
           }else if(inNextButton){
            Beep(700, 35);
            instanta = 11;
           }

        }
}

//CRACIUN MIHAI - COSMIN
void citireSetari(){
    int setareMuzica;
    int setareTutorial;
    int setareSoundTrack;
    int setareBackground;
    int setarePiesa;

    ifstream fin("setari");
    fin>>setareMuzica;
    if(setareMuzica)
        backgroundMusic = true;
    else
        backgroundMusic = false;

    fin>> setareTutorial;
    if(setareTutorial)
        tutorial = true;
    else
        tutorial = false;

    fin>>setareSoundTrack;
    if(setareSoundTrack == 1){
        soundtrack = "sound.wav";
    }else if(setareSoundTrack == 2){
        soundtrack = "sound2.wav";
    }else if(setareSoundTrack == 3){
        soundtrack = "sound3.wav";
    }else if(setareSoundTrack == 4){
        soundtrack = "sound4.wav";
    }else if(setareSoundTrack == 5){
        soundtrack = "sound5.wav";
    }

    fin>>setareBackground;
    FUNDAL = setareBackground;

    fin>>setarePiesa;
    CULOAREPIESA = setarePiesa;

    fin.close();
}

//CRACIUN MIHAI - COSMIN
void scriereSetari(){
    ofstream fout("setari");
    if(backgroundMusic)
        fout<<1<<' ';
    else
        fout<<0<<' ';

    if(tutorial)
        fout<<1<<' ';
    else
        fout<<0<<' ';

    if(strcmp(soundtrack,"sound.wav")==0){
        fout<<1<<' ';
    }else if(strcmp(soundtrack,"sound2.wav")==0){
        fout<<2<<' ';
    }else if(strcmp(soundtrack,"sound3.wav")==0){
        fout<<3<<' ';
    }else if(strcmp(soundtrack,"sound4.wav")==0){
        fout<<4<<' ';
    }else if(strcmp(soundtrack,"sound5.wav")==0){
        fout<<5<<' ';
    }

    fout<<FUNDAL<<' ';

    fout<<CULOAREPIESA<<' ';

    fout.close();
}



//CRACIUN MIHAI - COSMIN
int main()
{




    citireSetari();


    if(backgroundMusic)
        PlaySound(TEXT(soundtrack), NULL, SND_FILENAME | SND_ASYNC);


    initwindow(800,600,"IMPAS",300,100);

    while(true){
        if(instanta == 1){

            initMeniu1();
            while(instanta == 1){
                meniu1();
            }

        }
        if(instanta == 11){
            initMeniu11();
            while(instanta == 11){
                meniu11();
            }
        }
        if(instanta == 111){
            initMeniu111();
            while(instanta == 111){
                meniu111();
            }
        }

        if(instanta == 1111){
            initMeniuAlegereDimensiune1111();
            while(instanta == 1111){
                meniuAlegereDimensiune1111();
            }
        }

        if(instanta == 1112){
            initMeniuAlegereDimensiun1112();
            while(instanta == 1112){
                meniuAlegereDimensiune1112();
            }
        }

        if(instanta == 112){
            initMeniu112();
            while(instanta == 112){
                meniu112();
            }
        }

        if(instanta == 1121){
            initMeniuAlegereDimensiun1121();
            while(instanta == 1121){
                meniuAlegereDimensiune1121();
            }
        }

        if(instanta == 1122){
            initMeniuAlegereDimensiun1122();
            while(instanta == 1122){
                meniuAlegereDimensiune1122();
            }
        }
        if (instanta == 3){
            initMeniuSetari();
            while(instanta==3){
                meniuSetari();
            }
        }

        if(instanta == 4){
            initMeniuTutorial();
            while(instanta==4){
                meniuTutorial();
            }
        }

        if(instanta == 5){
            gata = false;
            PlaySound(NULL, 0, 0);
            desen();
            initMatrice();
            turn = 1;
            while(instanta == 5){
                punerePiesa();
            }
        }
        if(instanta == 6){
            gata = false;
            PlaySound(NULL, 0, 0);
            desen();
            initMatriceRandom();
            turn = 1;
            while(instanta == 6){
                punerePiesa();
            }
        }
        if(instanta == 7){
            gata = false;
            PlaySound(NULL, 0, 0);
            desen();
            initMatrice();
            turn = 1;
            while(instanta == 7){
                punerePiesaPVC();
            }
        }

        if(instanta == 8){
            gata = false;
            PlaySound(NULL, 0, 0);
            desen();
            initMatriceRandom();
            turn = 1;
            while(instanta == 8){
                punerePiesaPVC();
            }
        }

        if(instanta == 0){
            scriereSetari();
            closegraph();
            PlaySound(NULL, 0, 0);
            break;
        }

    }

   return 0;
}
