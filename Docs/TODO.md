Inserção que causa problema:
C9
V_0: D0 -> C19 -> C24 -> C22(P0) -> C21(P0) -> C20(P0) -> Dt ->
V_0: D0 -> C19 -> C24 -> C9 -> C22(P0) -> C21(P0) -> C20(P0) -> Dt ->

Cada locker pode ser visitado mais de uma vez, inclusive pelo mesmo veículo

$\delta_k \rightarrow$ Tempo em que o veículo k deixa o depósito
$t_{ij} \rightarrow$ tempo de trajeto
$\mu_{ik} \rightarrow$ tempo em que o veículo começa a servir o cliente $i$ $N_{CPL}$
$v_{k} \rightarrow$ é o tempo em que o veículo k visitou o último cliente
$[a_i,b_i] \rightarrow$ Janela de tempo
$h_i \rightarrow$ Cliente recebeu em casa
$l_i \rightarrow$ Cliente recebeu no locker
$y_{ij} \rightarrow$ Se o cliente i está atribuido ao locker j

Este artigo NÃO UTILIZA tempo de folga entre clientes. Apenas no depósito

Não considera o tempo de serviço para validar o time window. O que importa é somente o tempo de chegada.

Instância R103_co_100 possui clientes sem locker. O gurobi nao conseguiu resultados

Pesquisador:
40002022
40003965

Analista:
40002198
40002345