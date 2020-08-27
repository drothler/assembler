.global matr_mult_coos

//x0=matrix0
//x1=matrix1
//x2=resultmatrix
//x3= aktueller pointer auf matrix0 tupel
//x4= aktueller pointer auf matrix1 tupel
//x5= resultcounter
//x6 = segment anfang erklärung später
//x7 = segment ende erklärung später
//s1= aktuelles matrix0 tupel
//s2= tupel0, y
//s3= tupel0, value
//s4= tupel1, x
//s5= tupel1, y
//s6= tupel1, value



matr_mult_coos:
   
   

    sub sp, sp, 16 //x19-x30 am Stackpointer zwischenspeichern
    stp x19, x30, [sp] 

    MOV x3, x0
    MOV x4, x1
    MOV x6, x2
    MOV x7, x2
    //LDR 
     
    
    .LMatrix0Loop:

        //Jedes Tupel aus matrix0 wird geladen und auf Kompatibilität mit matrix1 geprüft
        //s20 compare spot
        LDR s1, [x3,8]        //Tupel0 n (x-coord) wird geladen
        FCMP s1, #0.0       
        b.eq .LEnd  
        LDR s1, [x3]         //Sobald 0 erreicht wird -> Ende
        LDR s2, [x3, 4]     //Tupel n y-coord wird geladen
        LDR s3, [x3, 8]     //Tupel n value wird geladen
        MOV x4, x1           // Tupel 1 pointer wird um 12 nach links verschoben, ist also quasi auf matrix1[-3]
        SUB x4, x4, 12
        .LMatrix1Loop:
            ADD x4, x4, 12           // Tupel 1 pointer wird auf das nächste tupel verschoben
            LDR s4, [x4, 8]        //Tupel1 n wird geladen
            FCMP s4, #0.0           // wenn value 0 ist -> Abbruch
            b.eq .LMatrix1End
            LDR s4, [x4]            // x wird geladen

            LDR s5, [x4, 4]     //Tupel1 n y-coord wird geladen
            LDR s6, [x4, 8]     //Tupel1 n value wird geladen
            
            FCMP s2, s4         //tupel0 y != tupel1 x -> nächstes tupel1 element tryen
	    b.LT .LMatrix1End
	    FCMP s2, s4
            b.ne .LMatrix1Loop

            FMUL s7, s3, s6     // values werden multipliziert

            CMP x6, x7          //wenn segment zum einordnen leer ist, muss das element nur eingeordnet werden und der segment pointer x7 um 12 nach unten verschoben werden
            b.eq .LEmptySegment
            
            MOV x8, x6
            .LSegmentLoop:
                LDR s20, [x8, 4]
                FCMP s5, s20
                b.GT .LskipElement      //falls aktuelles result element y coord kleiner als neues element y coord ist, wird es eins nach hinten verschiebt
                FCMP s5, s20
                b.eq  .LAddElement       //falls aktuelles element die gleiche pos hat wie aktuelles result element wird es drauf addiert
		FCMP s5, s20
		b.LT .LOrdnen
		b .LMatrix1Loop
                // hier ist klar, dass das akutelle element in result nach dem aktuellen neuen element kommen müssten
                // also muss es an die jetzige position eingeordnet werden und der rest nach hinten verschoben werden
		.LOrdnen:
                       
                MOV x9, x7          // x9 (loop pointer) wird auf die erste 0 im result aarray verschoben und dann auf das letzte element des letzten tupels
                SUB x9, x9, 12
                .LVerschiebungStart:
                   
                              
                    CMP x9, x8          //wenn der loop pointer und der erste element pointer auf der gleichen position liegen, muss eine letzte Verschiebung vorgenommen werden
                    b.eq .LLetzteVerschiebung
                    LDR s18, [x9]       //value vom n tupel wird geladen
                    STR s18, [x9, 12]   //value wird drei plätze nach unten verschoben
                    LDR s18, [x9, 4]   //y coord vom n tupel wird geladen
                    STR s18, [x9, 16]    //y coord wird drei plätze nach unten verschoben
                    LDR s18, [x9, 8]   //x coord vom n tupel wird geladen
                    STR s18, [x9, 20]    //x coord wird drei plätze nach unten verschoben
                    SUB x9, x9, 12          //loop pointer wird auf die erste position des aktuellen verschobenen tupels verschoben, damit die Schleife checken kann ob wir uns am
                                        // anfang des segments befinden
                    //ADD x7, x7, 12                   
                    b .LVerschiebungStart         
                    
                    .LLetzteVerschiebung:
                        LDR s18, [x9]       //value vom n tupel wird geladen
                        STR s18, [x9, 12]   //value wird drei plätze nach unten verschoben
                        LDR s18, [x9, 4]   //y coord vom n tupel wird geladen
                        STR s18, [x9, 16]    //y coord wird drei plätze nach unten verschoben
                        LDR s18, [x9, 8]   //x coord vom n tupel wird geladen
                        STR s18, [x9, 20]   // x coord vom n tupel wird drei plätze nach unten verschoben
                        STR s1, [x9]        
                        STR s5, [x9, 4]
                        STR s7, [x9, 8]
                        ADD x7, x7, 12
                    b .LMatrix1Loop


                .LAddElement:
                    LDR s17, [x8, 8]
                    FADD s7, s7, s17
                    STR s7, [x8, 8]
                    //MOV x8, 4
                    b .LMatrix1Prep


                .LskipElement:
                    ADD x8, x8, 20          // loop pointer wird auf ncähstes tupel bzw dessen value verschoben
                    LDR s16, [x8]       // value wird geladen
                    FCMP s16, #0.0          // wenn 0
                    b.eq .LNewLast          // neues element kann einfach eingeordnet werden
                    SUB x8, x8, 8           //wenn nicht, wird pointer auf den x value verschoben 
                    b .LSegmentLoop         //Rücksprung auf Segment



                    .LNewLast:
                    SUB x8, x8, 8           // (x8 pointer zeigt noch auf value, Verschiebung auf x pos)
                    STR s1, [x8]        // new x wird gestored
                    ADD x8, x8, 4           // x8 auf y verschoben
                    STR s5, [x8]        // new y wird gstored
                    ADD x8, x8, 4           //x8 auf value verschoben
                    STR s7, [x8]        // new value wird gestored
                    //ADD x8, x8, 4           //x8 zeigt auf das nächste element
                    ADD x7, x7, 12          // x7 segment ende pointer wird auf die 0 nach dem neuen tupel verschoben
                    ADD x5, x5, 1
                    b .LMatrix1Prep

                .LEmptySegment:
                STR s1, [x7]        // new x wird auf result[n+0] gelegt
                STR s5, [x7, 4]     // new y wird auf result[n+1] gelegt
                STR s7, [x7, 8]     // new value wird auf result [n+2] gelegt
                ADD x7, x7, 12      //segment ende pointer wird um 12 nach unten verschoben, zeigt jetzt auf die Null unter dem ersten Element im Segment
                ADD x5, x5, 1

            .LMatrix1Prep:
            //ADD x4, 12
            b .LMatrix1Loop 

    
    
        .LMatrix1End:

        
        LDR s19, [x3, 12]
        ADD x3, x3, 12
        FCMP s19, s1
        
        b.eq .LMatrix0Loop
        MOV x6, x7

        
        b .LMatrix0Loop
    .LEnd:

    MOV     x0, x2
    ldp     x19, x30, [sp] //x19-x30 vom Stackpointer zurckholen
    add     sp, sp, 16
    ret
