10 LET A = 0 - 9
20 LET B = 0 - 9
30 PRINT A ; "," ; B ; " is " ; 
40 PRINT A + B ; " " ; A - B ; " " ; A * B ;
50 IF B = 0 THEN PRINT " _" ;
51 IF B <> 0 THEN PRINT " " ; A / B ;
60 IF A <= B THEN PRINT " _" ;
61 IF A > B THEN PRINT " " ; A % B ; 
70 PRINT " "
80 LET B = B + 1
90 IF B < 10 THEN GOTO 30
100 LET A = A + 1
110 IF A < 10 THEN GOTO 20
120 END
