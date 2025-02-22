import gurobipy as gp
import utils as ut
import timeit as tm
def run_all(instance):

    qty_vehicles = 1
    m = None
    best_m = None
    has_empty_vehicle = False
    while(m is None or m.status == gp.GRB.INFEASIBLE or not has_empty_vehicle):
        
        m, time, has_empty_vehicle = run_exp(instance, qty_vehicles)

        if not has_empty_vehicle:
            best_m = m
        print(f" - {qty_vehicles} - cost: {'Infeasible' if m.status == gp.GRB.INFEASIBLE else round(m.ObjVal,2)} - time: {round(time,2)}s" )
        qty_vehicles += 1

        #TODO tirar isso aqui depois. É pra priorizar instâncias mais rapidas
        if qty_vehicles == 2 and m.status == gp.GRB.INFEASIBLE:
            break

    print(f"Best cost found")


def run_exp(instance, qty_vehicles):
    instance = ut.build_instance(instance)

    #-------------------------------------------------------------#

    v_capacity = int(instance['vehicle_capacity'])
    #Rótulos
    nodes = list(instance['nodes'].keys())
    lockers = [node for node in nodes if instance['nodes'][node]['type'] == 4]
    customers = list(instance['customers'].keys())
    customers_hc = [customer for customer in customers if instance['nodes'][customer]['type'] == 1]
    customers_lc = [customer for customer in customers if instance['nodes'][customer]['type'] == 2]
    customers_hlc = [customer for customer in customers if instance['nodes'][customer]['type'] == 3]
    depot = [node for node in nodes if instance['nodes'][node]['type'] == 0][0]


    # print(edges)

    #Veiculos
    vehicles = list()
    for i in range(qty_vehicles):
        vehicles.append(f"v_{i}")

    # ut.print_labels(nodes, lockers, customers, customers_hc, customers_lc, customers_hlc, vehicles, depot)

    #-------------------------------------------------------------#

    #Criando dicts

    #Nos
    nodes_dict = instance['nodes']

    #Custos
    costs = instance['costs']

    #Demandas, tempo de serviços e janelas de tempo
    demands = dict()
    service_time = dict()
    tw_a = dict()
    tw_b = dict()
    for node in nodes_dict:
        demands[node] = nodes_dict[node]['demand']
        service_time[node] = nodes_dict[node]['service_time']
        tw_a[node] = nodes_dict[node]['earliest']
        tw_b[node] = nodes_dict[node]['latest']
        
    #Atribuições de locker
    locker_attrib = dict()

    for locker in lockers:
        for customer in customers:
            if(customer in nodes_dict[locker]['customers']):
                locker_attrib[customer,locker] = 1
            else:
                locker_attrib[customer,locker] = 0

    #-------------------------------------------------------------#

    #Criando modelo
    m = gp.Model()
    m.setParam(gp.GRB.Param.OutputFlag,0)

    #Criando Variaveis
    x_ijk = m.addVars(nodes,nodes, vehicles, vtype=gp.GRB.BINARY)
    h_i = m.addVars(customers, vtype = gp.GRB.BINARY)
    l_i = m.addVars(customers, vtype = gp.GRB.BINARY)
    y_ij = m.addVars(customers,lockers, vtype = gp.GRB.BINARY)
    psi_jk = m.addVars(lockers,vehicles, vtype = gp.GRB.CONTINUOUS)
    delta_k = m.addVars(vehicles, vtype = gp.GRB.CONTINUOUS)
    mu_ik = m.addVars(nodes,vehicles, vtype = gp.GRB.CONTINUOUS)
    vl_k = m.addVars(vehicles, vtype = gp.GRB.CONTINUOUS)


    #-------------------------------------------------------------#

    M = 1000000

    #Função objetivo
    m.setObjective(
        gp.quicksum(costs[node_i,node_j] * x_ijk[node_i,node_j,v] for node_i in nodes for node_j in nodes for v in vehicles)
    )
    #Restrições

    # #Cliente hc apenas em casa
    c2 = m.addConstrs(
        gp.quicksum(x_ijk[customer_i,node_j,v_k] for node_j in nodes for v_k in vehicles) == 1 for customer_i in customers_hc
    )

    #Não visitar cliente lc
    c3 = m.addConstrs(
        gp.quicksum(x_ijk[customer_i,node_j,v_k] for node_j in nodes for v_k in vehicles) <= h_i[customer_i] for customer_i in customers_hlc
    )

    #Não visitar cliente se for customer_lc
    c4 = m.addConstrs(
        gp.quicksum(x_ijk[customer_i,node_j,v_k] for node_j in nodes for v_k in vehicles) == 0 for customer_i in customers_lc
    )

    #Sempre sair do deposito TODO: mudei de customers para customers + lockers
    c5 = m.addConstrs(
        gp.quicksum(x_ijk[depot,customer_j,v_k] for customer_j in customers + lockers) <= 1 for v_k in vehicles
    )

    #Dinâmica de fluxo
    c6 = m.addConstrs(
        gp.quicksum(x_ijk[node_i,node_j,v_k] for node_i in nodes if node_i != node_j) ==
        gp.quicksum(x_ijk[node_j,node_i,v_k] for node_i in nodes if node_j != node_i) for node_j in nodes for v_k in vehicles  
        
    )

    #Sempre chiegar no deposito TODO: mudei de customers para customers + lockers
    c7 = m.addConstrs(
        gp.quicksum(x_ijk[customer_i,depot,v_k] for customer_i in customers + lockers) <= 1 for v_k in vehicles
    )


    #Limitação de capacidade do locker
    c8 =  m.addConstrs(
        gp.quicksum(demands[customer_j] * x_ijk[node_i,customer_j,v_k] for node_i in nodes for customer_j in customers if node_i != customer_j)
        + gp.quicksum(psi_jk[locker_m,v_k] * x_ijk[node_p,locker_m,v_k]  for node_p in nodes for locker_m in lockers if node_p != locker_m)
        <= v_capacity
        for v_k in vehicles
    )

    #Tempo pra sair do deposito TODO testar com ==
    c9 =  m.addConstrs(
        delta_k[v_k] + costs[depot,node_i] - mu_ik[node_i,v_k] <= M*(1-x_ijk[depot,node_i,v_k])
        for v_k in vehicles for node_i in customers + lockers
    ) 

    #Tempo entre nós
    c10 =  m.addConstrs(
        mu_ik[node_i,v_k] + service_time[node_i] + costs[node_i,node_j] - mu_ik[node_j,v_k] <= M*(1-x_ijk[node_i,node_j,v_k])
        for v_k in vehicles for node_i in customers + lockers for node_j in customers + lockers
    )

    #Tempo para o ultimo cliente
    c11 =  m.addConstrs(
        mu_ik[node_i,v_k] - vl_k[v_k] <= M*(1-x_ijk[node_i,depot,v_k])
        for v_k in vehicles for node_i in customers + lockers
    )

    #Time window
    c12a =  m.addConstrs(
        tw_a[customer_j] * x_ijk[node_i,customer_j,v_k] <=
        mu_ik[customer_j,v_k] * x_ijk[node_i,customer_j, v_k]
        for v_k in vehicles for customer_j in customers for node_i in nodes
    )
    c12b =  m.addConstrs(
        mu_ik[customer_j,v_k] * x_ijk[node_i,customer_j, v_k] <= 
        tw_b[customer_j] * x_ijk[node_i,customer_j,v_k] 
        for v_k in vehicles for customer_j in customers for node_i in nodes
    )
    #Ou casa ou locker vai atender o N_HLC
    c13 =  m.addConstrs(
        h_i[customer_i] + l_i[customer_i] == 1 for customer_i in customers
    )

    #h_i sempre 1 pra cliente hc
    c14 =  m.addConstrs(
        h_i[customer_i] == 1 for customer_i in customers_hc
    )

    #l_i sempre 1 pra cliente lc
    c15 =  m.addConstrs(
        l_i[customer_i] == 1 for customer_i in customers_lc
    )

    # \sum y_ij = l_i 
    c16 =  m.addConstrs(
        gp.quicksum(y_ij[customer_i,locker_j] for locker_j in lockers) == l_i[customer_i] for customer_i in customers
    )

    #Atribuição de lockers, atribui o locker certo
    c17 =  m.addConstrs(
        y_ij[customer_i,locker_j] <= locker_attrib[customer_i,locker_j] for customer_i in customers for locker_j in lockers
    )

    # Garante que a quantidade de items deixados no locker seja igual a quantidade de loads cujo l_i == 1
    c18 =  m.addConstrs(
        (gp.quicksum(psi_jk[locker_j, v_k] for v_k in vehicles) == gp.quicksum(y_ij[customer_i,locker_j] * demands[customer_i] for customer_i in customers_lc + customers_hlc))
        for locker_j in lockers
    )

    #---#
    # Garante que o locker seja visitadoe seja visitado se l_j = 1
    c20 =  m.addConstrs(
        gp.quicksum(x_ijk[node_i,locker_j,v_k] for node_i in nodes) * M >= psi_jk[locker_j,v_k]
        for locker_j in lockers for v_k in vehicles
    )

    # Garante que o cliente seja visitado se h_j = 1
    c21 =  m.addConstrs(
        gp.quicksum(x_ijk[node_i,customer_j,v] for node_i in nodes for v in vehicles) >= h_i[customer_j] for customer_j in customers
    )

    # --- #

    # Domínio PSI_jk
    c27 =  m.addConstrs(
        psi_jk[node_j,v] >= 0 for node_j in lockers for v in vehicles
    )

    c29 =  m.addConstrs(
        delta_k[v] >= 0 for v in vehicles
    )
    # Domínio vl_k
    c30 =  m.addConstrs(
        vl_k[v] >= 0 for v in vehicles
    )

    time = tm.default_timer()
    m.optimize()
    time = tm.default_timer() - time
    has_empty_vehicle = False

    if m.Status != gp.GRB.INFEASIBLE:
        for v in vehicles:
            v_is_empty = True
            for node_i in nodes:
                for node_j in nodes:
                    if round(x_ijk[node_i,node_j,v].X) == 1:
                        v_is_empty = False
            if v_is_empty:
                has_empty_vehicle = True
                break
                
            ut.print_h_i_file(x_ijk,h_i,l_i, y_ij, psi_jk, delta_k, vl_k, customers, lockers, locker_attrib, vehicles, qty_vehicles, instance,m,nodes, time)    
        
    return m, time, has_empty_vehicle
