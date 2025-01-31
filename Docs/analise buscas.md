- or-opt-1
D0  1   2   3   4   5   6   7   Dt
D0  1   2   |3|   4   5   6   7   Dt
D0  |3|   1   2   4   5   6   7   Dt
D0  1   |3|   2   4   5   6   7   Dt
D0  1   2   |3|   4   5   6   7   Dt
D0  1   2   4   |3|   5   6   7   Dt
D0  1   2   4   5   |3|   6   7   Dt
D0  1   2   4   5   6   |3|   7   Dt
D0  1   2   4   5   6   7   |3|   Dt

D0  1   2   |3|   4   5   6   7   Dt
D0  1   2   4   |3|   5   6   7   Dt

D0  1   2   |3|   4   5   6   7   Dt
D0  |3|   1   2   4   5   6   7   Dt


- or-opt-2
D0  1   2   3   4   5   6   7   8   9   10 Dt
D0  1   |2   3|   4   5   6   7   8   9   10 Dt
D0  1   4   |2   3|   5   6   7   8   9   10 Dt
D0  1   4   5   |2   3|   6   7   8   9   10 Dt
D0  1   4   5   6   |2   3|   7   8   9   10 Dt
D0  1   4   5   6   7   |2   3|   8   9   10 Dt
D0  1   4   5   6   7   8   |2   3|   9   10 Dt
D0  1   4   5   6   7   8   9   |2   3|   10 Dt
D0  1   4   5   6   7   8   9   10  |2   3| Dt

TODO: 
V_0: D0 -> C4(P1) -> C22(P1) -> C5(P1) -> C21(P1) -> C2(P1) -> C10 -> C3(P1) -> C23(P1) -> C9(P1) -> C7(P1) -> C24(P1) -> C20 -> Dt -> 
V_1: D0 -> C13(P0) -> C16(P0) -> C1(P0) -> C17(P0) -> C18 -> C14 -> C15 -> C8 -> Dt -> 
V_2: D0 -> C12 -> C6 -> C11 -> Dt -> 
V_3: D0 -> C0(P1) -> C19 -> Dt ->

- 1: C4(P1) -> C22(P1) -> C5(P1) -> C21(P1) -> C2(P1)

    Não faz sentido aplicar o or-opt se estivermos com o nó e o destino dentro deste bloquinho de locker




V_0: D0 -> C4(P1) -> C22(P1) -> C5(P1) -> C21(P1) -> C2(P1) -> C10 -> C3(P1) -> C23(P1) -> C9(P1) -> C7(P1) -> C24(P1) -> C20 -> Dt -> 
V_1: D0 -> C13(P0) -> C16(P0) -> C1(P0) -> C17(P0) -> C18 -> C14 -> C15 -> C8 -> Dt -> 
V_2: D0 -> C12 -> C6 -> C11 -> Dt -> 
V_3: D0 -> C0(P1) -> C19 -> Dt -> 

V_0: D0 -> C4(P1) -> C22(P1) -> C5(P1) -> C21(P1) -> C2(P1) -> C10 -> C3(P1) -> C23(P1) -> C9(P1) -> C7(P1) -> C24(P1) -> C20 -> Dt -> 
V_0: D0 -> C4(P1) -> C22(P1) -> C5(P1) -> C21(P1) -> C2(P1) -> C10 -> C3(P1) -> C23(P1) -> C9(P1) -> C7(P1) -> C24(P1) -> C20 -> Dt -> 
V_0: D0 -> C4(P1) -> C22(P1) -> C5(P1) -> C21(P1) -> C2(P1) -> C3(P1) -> C23(P1) -> C9(P1) -> C7(P1) -> C24(P1) -> C10 -> C20 -> Dt ->
V_0: D0 -> C4(P1) -> C22(P1) -> C5(P1) -> C21(P1) -> C2(P1) -> C3(P1) -> C23(P1) -> C9(P1) -> C7(P1) -> C24(P1) -> C10 -> C20 -> Dt ->
 C10 C20

 Rotacionar o vetor:

 vector<int> v = {0,1,2,3,4,5,6,7,8,9};
//Direita, blz
v.insert(v.begin()+6,v.begin()+1,v.begin()+3);
v.erase(v.begin()+1,v.begin()+3);
//Esquerda, blz
vector<int> v2 = {0,1,2,3,4,5,6,7,8,9};
v2.insert(v2.begin()+1,v2.begin()+6,v2.begin()+8);
v2.erase(v2.begin()+6+2,v2.begin()+8+2);