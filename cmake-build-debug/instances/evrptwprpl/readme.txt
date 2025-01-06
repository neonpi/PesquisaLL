StringID
Type: type of node - 'd': depot, 'f': charging stations, 'c1' customer type 1, 'c2' customer type 2, 'c3' customer type 3, 'p' parcel lockers 
(x,y): xy coordination
demand
(ReadyTime, DueDate) Time window
ServiceTime


assigment of PLs: 1 if parcel locker i is assigned to customer j (i is index of column, j is index of row) and 0 otherwise
for instances with up to 15 customers, only one PL is generated in the data. therefore, the assigment of PLs is skipped.

col1(P0) col2(P1) col3(P2) ....
row1(C1) row2(C2) row3(C3) ...
Note that, all values are zero for 'c1'



param Q := EVBatterycapacity
param CV := EVLoadCapacity
param r := 1
param g := 3.39
param V := 1 