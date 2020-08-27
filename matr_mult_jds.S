.global matr_mult_jds
matr_mult_jds:

//Bemerkung: wenn als Kommentar steht x... freigegeben bedeutet dies, dass der Wert überschrieben werden darf,
//nicht dass er auf xzr gesetzt wird

//x0 pointer JDA
//x1 pointer colIndA
//x2 pointer PermA
//x3 pointer JDPtrA

//x4 pointer JDB
//x5 pointer colIndB
//x6 pointer PermB
//x7 pointer JDPtrB

//Addressen x8 - x11 von Stackpointer holen
ldr x8, [sp, 0]
ldr x9, [sp, 8]
ldr x10, [sp, 16]
ldr x11, [sp, 24]

sub sp, sp, 16 //x19-x30 am Stackpointer zwischenspeichern
stp x19, x30, [sp] 

//x8 pointer JDC
//x9 pointer colIndC
//x10 pointer PermC
//x11 pointer JDPtrC

//Abteil 1, Erstellung JDC, colIndC, PermC, JDPtrC

mov w17, wzr //Current Index JDA, mainloopcounter
mov w20, wzr //JDA-PointerCounter
mov w21, wzr //JDA-PointerCompare, zum Vergleich ob Pointer erreicht wurde

//Größe Intervalle von JDC bestimmen
mov w16, 1
ldr w12, [x3, xzr, lsl 2]
ldr w13, [x3, x16, lsl 2]
mov w18, wzr //Größe Interval JDC

cbz w13, .Lonejdp
sub w18, w13, w12
b .Lmainloop

.Lonejdp: //bestimmt größe von Intervall falls nur eines vorhanden
add w18, w18, 1
mov w13, w18
ldr w14, [x1, x13, lsl 2]
cbnz w14, .Lonejdp

//x12, x13, x14 freigegeben

.Lmainloop: //iteriert über JDA
add w13, w21, 1
ldr w14, [x3, x20, lsl 2] //momentaner JDAPointer
cmp w14, w13
b.ne .Lcontinue //falls JDAPointer gleich x17 + 1, dann x17 auf 0 setzen, Counter erhöhen
mov w17, wzr
add w20, w20, 1
//w13, w14 freigeben

.Lcontinue:
ldr s1, [x0], 4 //momentaner JD von JDA, Pointer wird verschoben
ldr w12, [x1], 4//Index v. momentanem JDA, Pointer wird verschoben
cbz w12, .Lpermccopy //Index von Matrix A gleich 0, über Matrix fertig iteriert

mov w13, wzr //Counter für PermB(Stelle)

.Lrowpermb: //findet Reihe von B in PermB, Wert muss enthalten sein da: Anzahl Spalten A = Anzahl Zeilen B = Größe PermB
ldr w14, [x6, x13, lsl 2] //Wert von PermB an Stelle des Counters
add w13, w13, 1
cmp w14, w12
b.ne .Lrowpermb
sub w13, w13, 1 //IndexB um 1 kleiner als gefundene Stelle

//x12, x14 wieder freigegeben

mov w12, wzr //JDBPointer Counter
.LjdB: //berechnet ob sich ein zu multiplizierender Wert in den Intervallen befindet
add w14, w12, 1 //JDBPointer +1
ldr w16, [x7, x12, lsl 2] //ladet Pointer an Stelle Counter
cbz w16, .Lincreasecounter //falls der Pointer an Stelle Counter = 0 ist JDB fertig und kein Wert mehr vorhanden
ldr w15, [x7, x14, lsl 2] //ladet Pointer an Stelle Counter+1
//x14 wieder freigegeben
mov w14, wzr //Llastinterval counter(falls notwendig)
cbz w15, .Llastinterval //falls nicht 0 dann kein Vergleich möglich da hinterstes Intervall
sub w15, w15, w16
b .Lintervaldet

.Llastinterval: //berechnet Länge des hintersten Intervalls
add w19, w16, w14 //lastPointer + Counter
add w14, w14, 1
ldr w19, [x5, x19, lsl 2]
mov w15, w14
cbnz w19, .Llastinterval

//x14, x19 freigegeben

.Lintervaldet://überprüft ob sich ein Wert im jeweiligen Intervall befindet
sub w15, w15, 1
cmp w15, w13 //w15 = Anzahl Werte im Intervall, x13 = PermB-Counter
b.lt .Lincreasecounter //falls Differenz JD-Pointer kleiner als Counter-PermB, dann kein Wert verfügbar, und kein passender Wert kann folgen
//w15 freigegeben

//Multiplikation von Werten
add w14, w13, w16
//x16 freigegeben
sub w14, w14, 1 //Index von JD-B
ldr s2, [x4, x14, lsl 2] //zu multiplizierenden JD-B
ldr w14, [x5, x14, lsl 2] //Index v. zu multiplizierenden JD-B
fmul s4, s1, s2 //Ergebnis Multiplikation in s4

mov w15, wzr //Intervalloopcounter
.LinsertLoop:
//w18 größe Intervalle JDC
mul w16, w15, w18 
add w16, w16, w17 //möglicher index JD-C
ldr w19, [x9, x16, lsl 2] //momentaner Index an Stelle
add w15, w15, 1
cbz w19, .Linsert //falls Index momentan 0(leer), Wert einfügen
cmp w19, w14  //falls Index momentan gleich, Wert einfügen
b.ne .LinsertLoop

.Linsert:
ldr s3, [x8, x16, lsl 2]  //bisheriger Wert
fadd s5, s4, s3
str s5, [x8, x16, lsl 2]   //neuer Wert
str w14, [x9, x16, lsl 2]
add w12, w12, 1 //JDB-Pointer Counter erhöhen
b .LjdB

.Lincreasecounter:
add w17, w17, 1
add w21, w21, 1
b .Lmainloop

//x12 - x17 freigegeben

.Lpermccopy: //kopiert PermA zu PermC, x12 counter
ldr w13, [x2, x12, lsl 2]
str w13, [x10, x12, lsl 2]
add w12, w12, 1
cbnz w13, .Lpermccopy

ldr w16, [x11, xzr, lsl 2] //Größe PointerArray
.Ljdpointerc: //setzt jdpointerc array, x13 counter
mul w14, w18, w13
add w14, w14, 1
ldr w15, [x7, x13, lsl 2]
str w14, [x11, x13, lsl 2]
add w13, w13, 1
cmp w13, w16
b.lt .Ljdpointerc


//Abteil 2 Verschieben von 0-en
//alle x > 11 außer x18 freigegeben 

add w14, w14, w18 //w14, vorher Wert hinterster Pointer
sub w14, w14, 1 //w14 Stelle von abschließender 0 am Ende von JDC
mov w13, 1 //CurrIntervalCounter+1
mov w15, wzr //CleanupCounter
mov w20, wzr //zerocounter/swapcounter(in .Lswapping)
mov w21, wzr //pushcounter/JDCPointer(in .Lswapping)
.Lcleanup:
cmp w15, w14 //Falls Ende von Array erreicht dann fertig
b.eq .Ldone
ldr w17, [x11, x13, lsl 2] //Start nächstes Intervall
cbnz w17, .Lendpointer //falls w17=0  1.Stelle nächstes Intervall = Stelle abschließende Null
mov w17, w14
add w17, w17, 1

.Lendpointer:
sub w17, w17, 1 //Stelle 1. Wert nächstes Intervall
cmp w17, w15
b.eq .LnextInterval //nächstes Intervall erreicht
ldr w16, [x9, x15, lsl 2]
mov w19, w15
cbz w16, .Lzerofound //falls Wert 0, 0 gefunden
add w15, w15, 1
b .Lcleanup

.Lzerofound: //w19, searchIndex
add w20, w20, 1 //zählt die Anzahl 0-en
add w19, w19, 1
cmp w19, w17
b.eq .Lnovalues //Intervall fertig abgesucht, keine Werte !0 gefunden
ldr w16, [x9, x19, lsl 2] 
cbnz w16, .Lvaluefound //falls !0 Perm ändern, sonst weiter
b .Lzerofound

//x16 freigegeben
.Lnovalues: //Berechnung von w12 = Index letzer Wert nächstes Intervall
cmp w19, w14 //falls w19 = w14 nur mehr Nullen bis Array Ende, Funktion kann beendet werden
b.eq .Ldone
add w12, w13, 1
ldr w16, [x11, x12, lsl 2]
sub w12, w16, 2
cbnz w16, .Lnovalues2 //falls w16 = 0 nächstes Intervall = letztes
sub w12, w14, 1 //falls letztes Intervall w12 = letzter Index anders berechnen

//x16 freigegeben
.Lnovalues2: //Verschiebung Intervall
mov w19, w15
add w19, w19, w21 //w19 cleanupcounter + pushcounter
add w22, w17, w21 //w22 Start nächstes Intervall(w17) + pushcounter
ldr w16, [x9, x22, lsl 2] //ladet Index von vorheriger Stelle
str w16, [x9, x19, lsl 2] //setzt Index an neue Stelle
ldr s1, [x8, x22, lsl 2] 
str s1, [x8, x19, lsl 2]
mov w16, wzr
fmov s1, wzr
str w16, [x8, x22, lsl 2] //setzt 0 an vorherige Stelle
str s1, [x9, x22, lsl 2]
add w21, w21, 1 //erhöhe pushCounter
cmp w12, w22 //falls w22 = Index letzter Wert nächstes Intervall, weiter
b.ne .Lnovalues2

//x16 freigegeben
//Intervall fertig verschoben, Pointer anpassen!
ldr w16, [x11, x13, lsl 2] //bisheriger Pointer
sub w16, w16, w20
str w16, [x11, x13, lsl 2] //neuer Pointer
mov w20, wzr
mov w21, wzr
b .Lcleanup

.Lvaluefound: //Wert an Stelle w19 gefunden, tausche mit gefundener 0 Stelle w15(alle Intervalle)
sub w16, w13, 1
ldr w16, [x11, x16, lsl 2] 
sub w16, w16, 1 //Index Anfang momentanes Intervall
sub w19, w19, w16 //w19 = Index1
sub w16, w15, w16 //w16 = Index2
//tausche PermC an Index1 & Index2
ldr w12, [x10, x19, lsl 2]
ldr w17, [x10, x16, lsl 2]
str w17, [x10, x19, lsl 2]
str w12, [x10, x16, lsl 2]
mov w20, wzr

.Lswapping: //w20 = swapcounter
ldr w21, [x11, x20, lsl 2] //w21 = JDPointerC[swapcounter]
cbz w21, .Lswappingdone //sobald JDPointerC = 0, kein Intervall mehr zu swappen
sub w21, w21, 1
add w22, w19, w21 //Index Tauschwert 1
add w23, w16, w21 //Index Tauschwert 2
//colIndC swapping
ldr w12, [x9, x22, lsl 2]
ldr w17, [x9, x23, lsl 2]
str w17, [x9, x22, lsl 2]
str w12, [x9, x23, lsl 2]
//JDC swapping
ldr s1, [x8, x22, lsl 2]
ldr s2, [x8, x23, lsl 2]
str s2, [x8, x22, lsl 2]
str s1, [x8, x23, lsl 2]
add w20, w20, 1
b .Lswapping

.Lswappingdone:
mov w20, wzr //reset swapcounter
b .Lcleanup

.LnextInterval:
add w13, w13, 1 //increase CurrIntervalCounter+1
b .Lcleanup

.Ldone:

ldp     x19, x30, [sp] //x19-x30 vom Stackpointer zurückholen
add     sp, sp, 16

	ret