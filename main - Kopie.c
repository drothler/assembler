#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int multype=0; //entscheidet, welche Multiplikation ausgef�hrt wird
char eingabe[10]; //die Usereingabe
int height1;    // die H�he von mat1
int height2;    //die H�he von mat2
int width1;     //die Breite von Mat1
int width2;     //die Breite von mat2
int arraysize1; //die Anzahl an nicht null eintr�gen in mat1
int elements1;  //die Anzahl aller elemente in mat1
int arraysize2; //die Anzahl an nicht null eintr�gen in mat1
int elements2;  //die Anzahl aller elemente in mat1
/*float ajdag1[9] = { 1, 2, 3, 1, 4, 5, 1, 2, 0 };
int acolInd1[9] = { 1, 1, 3, 4, 2, 4, 4, 4, 0 };
int aperm1[5] = { 1, 3, 2, 4, 0 };
int ajdPtr1[4] = { 1, 5, 8, 0 };

float ajdag2[11] = { 1, 2, 5, 2, 1, 4, 3, 2, 3, 1 , 0 };
int acolInd2[11] = { 1, 2, 1, 4, 2, 3, 4, 3, 4, 4, 0 };
int aperm2[5] = { 2, 4, 1, 3, 0 };
int ajdPtr2[5] = { 1, 5, 8, 10, 0 };
*/
int zeilenende=0;

float matrix2[]; //matrix 2
float matrix1[]; //matrix 1

int mat1rowentries[]; //wie viele eintr�ge eine Zeile von Mat1 hat
int mat2rowentries[]; ////wie viele eintr�ge eine Zeile von Mat1 hat

int perm1[]; // das Permuataionsarray von mat1
int perm2[];    // das Permuataionsarray von mat2

int rows1[];  //ein Hilfsarray, das angibt, wie viele zeilen es mit 1,2,3,... eintr�gen es in der Matrix1 gibt
int rows2[];   //ein Hilfsarray, das angibt, wie viele zeilen es mit 1,2,3,... eintr�gen es in der Matrix2 gibt

int zweitematrix=0;
int maxnumb=0;
 double multiplikationTime=0 ;

//extern void matr_mult_coos(float *a, float *b, float *result); //Die Multiplikation der Matrizen im Coordinate Scheme
extern void matr_mult_jds(float *jdag1,int *colInd1,int *perm1,int *jdPtr1,float *jdag2,int *colInd2,int *perm2,int *jdPtr2,float *jdagResult,int *colIndResult,int *permResult,int *jdPtrResult) ;  //Die Multiplikation der Matrizen im Jagged Diagonal Storage Scheme

int main()
{

    while(multype!=1 &&multype!=2) //der USer wird gebeen zu entscheiden welche art von Multiplikation er machen m�chte
    {
        printf("Please select what kind of matrices you want to multiply. (1)Coordinate Scheme (2)Jagged Diagonal Storage: ");
        scanf("%s",&eingabe);
       multype= atoi(eingabe);
    }
    struct timespec s,e; // das Zeitmessen wird gestartet
    clock_gettime(CLOCK_MONOTONIC,&s);
    double start= s.tv_sec+s.tv_nsec*1e-9;

    //File wird gescannt um die array gr��e rauszufinden
    FILE *matrixFile;
    matrixFile =fopen("Matrices.txt","r");
    if(matrixFile== NULL) // wenn das File nicht gefunden wird wird ein Fehler geworfen
    {
        perror("File not Found");
        return EXIT_FAILURE;;
    }
    while(1) //das File wird solange gescannt bis es zu ende ist um die Gr��e der Beiden Arrays festzustellen
    {
        if(feof(matrixFile))
        {
            break;
        }
        char temp[255];
        fscanf(matrixFile,"%s",temp);

        if(!zweitematrix)
        {
            elements1++;
            if(!zeilenende)
            {
                width1++;
            }
            if(strcmp(temp,",")==0)
            {
                elements1--;
                zeilenende=1;
                height1++;
            }
            if( !strcmp(temp,"0")==0 && !strcmp(temp,",")==0&& !strcmp(temp,";")==0)
            {
                arraysize1++;
            }
            if(strcmp(temp,";")==0)
            {

                zweitematrix=1;
                zeilenende=0;
            }
        }
        else
        {
            elements2++;
            if(!zeilenende)
            {
                width2++;
            }
            if(strcmp(temp,",")==0)
            {
                elements2--;
                zeilenende=1;
                height2++;
            }
            if( !strcmp(temp,"0")==0&& !strcmp(temp,",")==0&& !strcmp(temp,";")==0)
            {
                arraysize2++;
            }


           // printf("temp:%s\n",temp);
            //printf("size:%d\n",elements1);
        }

    }

    fclose(matrixFile); //das FIle wird Geschlossen
    height1++;  //die werte passend gemacht
    width1--;
    elements1--;
    height2++;
    width2--;



    if(width1!=height2) //�berpr�fung, ob die Richtigen Dimensionen passen
    {
        printf("Matrices don't have the right sizes.");
        return EXIT_FAILURE;
    }
    //matrix1 wird mit werten bef�llt
    int mat1rowentries[height1];
    int index=0;
    int entries=0;
    matrixFile =fopen("Matrices.txt","r"); //das File wird erneut ge�ffnet, damit man wieder am Anfang ist

    float matrix1[arraysize1*3+1];
    int matrixcounter=0;
    for(int i =0; i < elements1; i++) //Matrix1 wird mit Werten bef�llt
    {
        char temp[10];
        fscanf(matrixFile,"%s",temp);
        if(strcmp(temp,",")==0) //neue Zeile
        {
            i--;
            mat1rowentries[index]=entries;
            entries=0;
            index +=1;
        }
        else
        {
            if(!strcmp(temp,"0")==0&&!strcmp(temp,",")==0&&!strcmp(temp,";")==0)
            {

                if(atof(temp)==0)  //wenn temp keine Zahl ist wird das Programm abgebrochen, da die Matrix falsch ist
                {
                    printf("Matrix not in the right format.");
                    return EXIT_FAILURE;
                }
                entries+=1;

                matrix1[matrixcounter]=(float)(i/width1);
                matrixcounter++;

                matrix1[matrixcounter]=(float)(i%width1);
                matrixcounter++;

                matrix1[matrixcounter]=  (float)atof(temp);

                matrixcounter++;
            }
        }
    }
    mat1rowentries[index]=entries;
    entries=0;
    //matrix2 wird mit werten bef�llt
    float matrix2 [arraysize2*3+1];
    int mat2rowentries[height2];
    matrixcounter=0;
    index=0;
    for(int l =0; l < elements2; l++)//Matrix1 wird mit Werten bef�llt
    {
        char temp[255];
        fscanf(matrixFile,"%s",temp);


        if(strcmp(temp,";")==0) //neue Matrix
        {
            l--;
        }
        if(strcmp(temp,",")==0) //neue Zeile
        {
            l--;
            mat2rowentries[index]=entries;
            entries=0;
            index +=1;
        }
        else
        {
            if(!strcmp(temp,"0")==0&&!strcmp(temp,",")==0&&!strcmp(temp,";")==0)
            {
                if(atof(temp)==0) //wenn temp keine Zahl ist wird das Programm abgebrochen, da die Matrix falsch ist
                {
                    printf("Matrix not in the right format.");
                    return EXIT_FAILURE;
                }
                entries+=1;

                matrix2[matrixcounter]=(float)(l/width2);

                matrixcounter++;

                matrix2[matrixcounter]=(float)(l%width2);

                matrixcounter++;

                matrix2[matrixcounter]=  (float)atof(temp);


                matrixcounter++;



            }
        }
    }
    mat2rowentries[index]=entries;
    entries=0;
    fclose(matrixFile);
//das File wird geschlossen
/*
        printf("mat1elements:%d\n",elements1);
        printf("mat1width:%d\n",width1);
        printf("mat1height:%d\n",height1);
        printf("mat1size:%d\n",arraysize1);
        for(int i=0; i<arraysize1*3; i++)
        {
            printf("mat1:%f\n",matrix1[i]);
        }
        for(int i=0; i<height1; i++)
        {
            printf("mat1rowentries:%d\n",mat1rowentries[i]);
        }
        printf("mat2elements:%d\n",elements2);
        printf("mat2width:%d\n",width2);
        printf("mat2height:%d\n",height2);
        printf("mat2size:%d\n",arraysize2);
        if(matrix2[2]==3.1f)
        {
            printf("Yes");
        }
        for(int i=0; i<arraysize2*3; i++)
        {
            printf("mat2:%f\n",matrix2[i]);
        }
        for(int i=0; i<height2; i++)
        {
            printf("mat2rowentries:%d\n",mat2rowentries[i]);
        }
*/

    if(multype==2) //wenn JDs Multiplikation
    {
        //das Permutationsarray  von Matrix1 wird erstellt
        int perm1[height1+1];
        int max=0;
        int x=0;
        int vorhanden=0;
          for(int a=0; a<height1; a++)
        {
            perm1[a]=-1;
        }

        for(int k=0; k<height1; k++) //f�r jede Reihe
        {
            for(int i=0; i<height1; i++)
            {
                vorhanden=0;
                for(int l=0; l< height1; l++)
                {
                    if(i==perm1[l])
                    {
                        vorhanden=1;
                    }
                }
                if(vorhanden==0)
                {
                  //  printf("matrow:%d",max);
                    if(mat1rowentries[i]>=max)
                    {
                        max= mat1rowentries[i];
                        perm1[x]=i;

                    }
                }
            }
            x +=1;
            max =0;
        }

        //Das PermutationsArray von matrix2 wird erstellt
        int perm2[height2+1];
        int maX=0;
        int y=0;
        vorhanden=0;
          for(int a=0; a<height2; a++)
        {
            perm2[a]=-1;
        }
        for(int k=0; k<height2; k++)
        {
            for(int i=0; i<height2; i++)
            {
                vorhanden=0;
                for(int l=0; l< height2; l++)
                {
                    if(i==perm2[l])
                    {
                        vorhanden=1;
                    }
                }
                if(vorhanden==0)
                {
                    if(mat2rowentries[i]>=maX)
                    {
                        maX= mat2rowentries[i];
                        perm2[y]=i;

                    }
                }
            }
            y +=1;
            maX =0;
        }

        //ein Hilfsarray, das angibt, wie viele zeilen es mit 1,2,3,... eintr�gen es in der Matrix1 gibt wird erstellt
        int rows1size=mat1rowentries[perm1[0]];
        int rows1[rows1size];
        maxnumb=rows1size;
        for(int a=0; a<rows1size; a++)
        {
            rows1[a]=0;
        }
        for(int i =0; i<rows1size; i++)
        {
            for(int l=0; l< height1; l++)
            {
                if(mat1rowentries[l]>= maxnumb)
                {
                    rows1[i] = rows1[i]+1;
                }
            }
            maxnumb--;
        }
        //ein Hilfsarray, das angibt, wie viele zeilen es mit 1,2,3,... eintr�gen es in der Matrix2 gibt wird erstellt
        int row2size=mat2rowentries[perm2[0]];
        int rows2[row2size];
        maxnumb=row2size;
        for(int a=0; a<row2size; a++)
        {
            rows2[a]=0;
        }
        for(int i =0; i<row2size; i++)
        {
            for(int l=0; l< height2; l++)
            {
                if(mat2rowentries[l]>= maxnumb)
                {
                    rows2[i] = rows2[i]+1;
                }
            }
            maxnumb--;
        }
// das Jdag und colInd array wird f�r matrix 1 bef�llt
        float jdag1[arraysize1+1];
        int colInd1[arraysize1+1];
        int startpos=0;
        for(int i =0; i < height1; i++) //f�r jede Reihe
        {
            int pos=0;
            for(int k=0; k< height1; k++) //suche pos in perm
            {
                if(perm1[k]==i)
                {
                    pos=k;
                }
            }
            for(int l=0; l < mat1rowentries[i]; l++) //f�r jeden entry in der Reihe
            {
                if(l>0)
                {
                    jdag1[pos+rows1[rows1size-l]]= matrix1[startpos+2+3*l];
                    colInd1[pos+rows1[rows1size-l]]=(int) matrix1[startpos+1+3*l]+1;
                    pos=pos+rows1[rows1size-l];
                }
                else
                {
                    jdag1[pos]= matrix1[startpos+2+3*l];
                    colInd1[pos]= matrix1[startpos+1+3*l]+1;
                }
            }
            startpos= startpos+3*mat1rowentries[i];// neue startpos
        }
        // das Jdag und colInd array wird f�r matrix2 bef�llt
        float jdag2[arraysize2+1];
        int colInd2[arraysize2+1];
        startpos=0;
        for(int i =0; i < height2; i++) //f�r jede Reihe
        {
            int pos=0;
            for(int k=0; k< height2; k++) //suche pos in perm
            {
                if(perm2[k]==i)
                {
                    pos=k;
                }
            }
            for(int l=0; l < mat2rowentries[i]; l++) //f�r jeden entry in der Reihe
            {
                if(l>0)
                {
                    jdag2[pos+rows2[row2size-l]]= matrix2[startpos+2+3*l];
                    colInd2[pos+rows2[row2size-l]]= (int)matrix2[startpos+1+3*l]+1;
                    pos=pos+rows2[row2size-l];
                }
                else
                {
                    jdag2[pos]= matrix2[startpos+2+3*l];
                    colInd2[pos]=(int) matrix2[startpos+1+3*l]+1;
                }
            }
            startpos= startpos+3*mat2rowentries[i];// neue startpos
        }
        //das ptr array wird f�r matrix 1 erstellt
        int jdPtr1[rows1size+1];
        jdPtr1[0]=1;
        for(int i=1; i< rows1size; i++)
        {
            jdPtr1[i]=jdPtr1[i-1]+rows1[rows1size-i];
        }
        //das ptr array wird f�r matrix 2 erstellt
        int jdPtr2[row2size+1];
        jdPtr2[0]=1;

        //das Gesamtarray f�r colInd,perm und jdPtr wird erstellt
        /*int matrix1rest[arraysize1+height1+rows1size+3];
               int start =0;
               for(int i=0; i< arraysize1; i++)
               {
                   matrix1rest[i]=colInd1[i];
               }
               matrix1rest[arraysize1]=0;
               start = arraysize1+1;
               for(int i=0; i< height1; i++)
               {
                   matrix1rest[i+start]=perm1[i];
               }
               matrix1rest[arraysize1+height1+1]=0;
               start = arraysize1+height1+2;
               for(int i=0; i<rows1size ; i++)
               {
                   matrix1rest[i+start]=jdPtr1[i];
               }

               matrix1rest[arraysize1+height1+rows1size+2]=0;

               //das Gesamtarray f�r colInd,perm und jdPtr wird erstellt
               int matrix2rest[arraysize2+height2+row2size+3];
               start =0;
               for(int i=0; i< arraysize2; i++)
               {
                   matrix2rest[i]=colInd2[i];
               }
               matrix2rest[arraysize2]=0;
               start = arraysize2+1;
               for(int i=0; i< height2; i++)
               {
                   matrix2rest[i+start]=perm2[i];
               }
               matrix2rest[arraysize2+height2+1]=0;
               start = arraysize2+height2+2;
               for(int i=0; i<row2size ; i++)
               {
                   matrix2rest[i+start]=jdPtr2[i];
               }



               matrix1rest[arraysize1+height1+rows1size+2]=0;
        */
         jdPtr2[row2size]=0;  //an den arrays wird am ende eine null drangeh�ngt
         jdag2[arraysize2]=0;
         colInd2[arraysize2]=0;
         perm2[height2]=0;

         jdPtr1[rows1size]=0;
         jdag1[arraysize1]=0;
         colInd1[arraysize1]=0;
         perm1[height1]=0;

        float resultJdag[width2*height1+1]; //die Resultarrays werden erstellt
        int resultCol[width2*height1+1];
        int resultPerm[height1+1];
        int resultPtr[width2+1];


		for (int a = 0; a < height2; a++)
		{
			perm2[a] = perm2[a] + 1;
		}

		for (int i = 0; i < width2*height1 + 1; i++) //resultcol und resultJdag wird mit Nullen bef�llt
		{
			resultJdag[i] = 0.0f;
			resultCol[i] = 0;
		}
		for (int i = 0; i < height1 + 1; i++) //Test
		{
			resultPerm[i] = 0;
		}
		for (int i = 0; i < width2 + 1; i++) //Test
		{
			resultPtr[i] = 0;
		}
		resultPtr[0] = width2; //Übergabe ans resultPtr die größe vom resultPrtarray

		for (int i = 1; i < row2size; i++)
		{
			jdPtr2[i] = jdPtr2[i - 1] + rows2[row2size - i];
		}


		   struct timespec sJds,eJds; // das Zeitmessen wird gestartet
    clock_gettime(CLOCK_MONOTONIC,&sJds);
    double startJds= sJds.tv_sec+sJds.tv_nsec*1e-9;

        matr_mult_jds(jdag1,colInd1,perm1,jdPtr1,jdag2,colInd2,perm2,jdPtr2,resultJdag,resultCol,resultPerm,resultPtr); //die Funktion wird aufgerufen
			 clock_gettime(CLOCK_MONOTONIC,&eJds); //Ende der Zeitmessung
   double endJds = eJds.tv_sec+eJds.tv_nsec*1e-9;
   multiplikationTime = endJds-startJds;
     clock_gettime(CLOCK_MONOTONIC,&e); //Ende der Zeitmessung
		//matr_mult_jds(ajdag1, acolInd1, aperm1, ajdPtr1, ajdag2, acolInd2, aperm2, ajdPtr2, resultJdag, resultCol, resultPerm, resultPtr);


		//Ausgabe der Matrizen A & B

		printf("jdag1:\n");
		for (int i = 0; i < arraysize1 + 1; i++)
		{
			printf("%.0f; ", jdag1[i]);
		}
		printf("\ncol1:\n");
		for (int i = 0; i < arraysize1 + 1; i++)
		{
			printf("%d; ", colInd1[i]);
		}
		printf("\nperm1:\n");
		for (int i = 0; i < height1 + 1; i++)
		{
			printf("%d; ", perm1[i]);
		}
		printf("\nPtr1:\n");
		for (int i = 0; i < sizeof(rows1) / sizeof(int) + 1; i++)
		{
			printf("%d; ", jdPtr1[i]);
		}
		printf("\n");
		printf("\njdag2:\n");
		for (int i = 0; i < arraysize2 + 1; i++)
		{
			printf("%.0f; ", jdag2[i]);
		}
		printf("\ncol2:\n");
		for (int i = 0; i < arraysize2 + 1; i++)
		{
			printf("%d; ", colInd2[i]);
		}
		printf("\nperm2:\n");
		for (int i = 0; i < height2 + 1; i++)
		{
			printf("%d; ", perm2[i]);
		}
		printf("\nPtr2:\n");
		for (int i = 0; i < sizeof(rows2) / sizeof(int) + 1; i++)
		{
			printf("%d; ", jdPtr2[i]);
		}
		printf("\n");
		printf("\n");


		for (int i = 0; i < width2*height1 + 1; i++)
		{
			printf("%.0f; ", resultJdag[i]);
		}
		printf("\n");
		for (int i = 0; i < width2*height1 + 1; i++)
		{
			printf("%d; ", resultCol[i]);
		}
		printf("\n\n");
		for (int i = 0; i < height1 + 1; i++)
		{
			printf("%d; ", resultPerm[i]);
		}
		printf("\n");
		for (int i = 0; i < width2+1; i++)
		{
			printf("%d; ", resultPtr[i]);
		}
		printf("\nThatsallfolks\n");


    }
    else
    {
        float result[width2*height1+1]; //noch unsicher wie gro�
        result[width2*height1]=0;
        matrix1[arraysize1*3]=0;
        matrix2[arraysize1*3]=0;
          struct timespec sCoos,eCoos; // das Zeitmessen wird gestartet
    clock_gettime(CLOCK_MONOTONIC,&sCoos);
    double startCoos= sCoos.tv_sec+sCoos.tv_nsec*1e-9;
       // matr_mult_coos(matrix1,matrix2,result); //die Funktion wird aufgerufen
    clock_gettime(CLOCK_MONOTONIC,&eCoos); //Ende der Zeitmessung
   double endCoos = eCoos.tv_sec+eCoos.tv_nsec*1e-9;
    multiplikationTime = endCoos-startCoos;
  clock_gettime(CLOCK_MONOTONIC,&e); //Ende der Zeitmessung
    }

   double end = e.tv_sec+e.tv_nsec*1e-9;
   double time = end-start;
    printf("Took %f seconds to execute the whole programm\n",time);
    printf("And the multiplication took %f seconds to execute\n",multiplikationTime);
    return EXIT_SUCCESS;

}


