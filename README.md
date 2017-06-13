# EBCD-Punch-Card-Decoder
Reads a file with 80 x 13 rows [0,1] imitating punch card data. Decodes the data into ASCII based on the rules of EBCD punch cards.

EBCD Format:
```
 -     1  2  3  4  5  6  7  8  9 8-2 8-3 8-4 8-5 8-6 8-7
 -     1  2  3  4  5  6  7  8  9  :   #   @   ‘   =   “
 Y  &  A  B  C  D  E  F  G  H  I  [   .   <   (   +   !
 X  -  J  K  L  M  N  O  P  Q  R  ]   $   *   )   ;   ^
 0  0  /  S  T  U  V  W  X  Y  Z  \   ,   %   _   >   ?```
