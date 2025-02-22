from IPython.display import Markdown,display
import math

#Imprimindo variável h_i e l_i
def print_h_i(h_i,l_i, y_ij, psi_jk, delta_k, vl_k, customers, lockers, locker_attrib, vehicles):

    
    display(Markdown("$h_i$ - Cliente $i$ foi visitado em casa"))
    display(Markdown("$l_i$ - Cliente $i$ foi visitado no locker"))
    display(Markdown("$y_{ij}$ - Cliente $i$ foi atendido no locker $j$"))
    display(Markdown(r"$\ell_{ij} $ - Cliente $i$ está atribuido ao locker $j$"))
    
    table = f"|var|{'|'.join(customer for customer in customers)}|\n |---|---{'---'.join('|' for customer in customers)}\n"
    line_h_i = "|$h_i$"
    line_l_i = "|$l_i$"
    line_y_ij = "|$y_{ij}$"
    line_l_ij = r"|$\ell_{ij}$"
    for i in customers:
        line_h_i += f"|{round(h_i[i].X)}"
        line_l_i += f"|{round(l_i[i].X)}"
        
        if round(l_i[i].X) == 1:
            for j in lockers:
                if round(y_ij[i,j].X) == 1:
                    line_y_ij += f"|{j}"             
        else:
            line_y_ij += f"|-"    

        des_locker = "-"
        for j in lockers:
            if locker_attrib[i,j]:
                des_locker = j
        line_l_ij += f"| {des_locker}"
            
        
    line_h_i+="\n"
    line_l_i+="\n"
    line_y_ij+="\n"
    line_l_ij+="\n"
        
        
    display(Markdown(f"{table}{line_h_i}{line_l_i}{line_y_ij}{line_l_ij}"))

    
    display(Markdown(r"$\psi_{jk} $ - Quantidade de pacotes deixados no locker $j$ pelo veículo $k$"))
    
    table = r"|$\psi_{jk}$|"+f"{'|'.join(locker for locker in lockers)}|\n |---|---{'---'.join('|' for locker in lockers)}\n"
    lines_psi_jk = ""
    
    for k in vehicles:
        lines_psi_jk += f"|${k}$"
        for j in lockers:
            lines_psi_jk+= f"| {psi_jk[j,k].X}"
        
        lines_psi_jk+="\n"
    display(Markdown(f"{table}{lines_psi_jk}"))
    
    table_delta = ""
    line_delta = ""
    table_vl = ""
    line_vl = ""
    for v in vehicles:
        table_delta += r"| $\delta_{"+v+"}$ "
        table_vl += r"| $\mathcal{v}_{"+v+"}$ "
        line_delta+= f"| {delta_k[v].X}"
        line_vl+= f"| {vl_k[v].X}"
    table_delta+=f"\n |--{'----'.join('|' for v in vehicles)}\n"
    table_vl+=f"\n |--{'----'.join('|' for v in vehicles)}\n"
    line_delta+="\n"
    line_vl+="\n"


    # for v in vehicles:

    display(Markdown(r"$\delta_{k} $ - Momento de saida do veículo $k$ do depósito"))
    display(Markdown(f"{table_delta}{line_delta}"))
    
    
    display(Markdown(r"$\mathcal{v}_{k} $ - Momento em que o veículo $k$ visitou o último cliente"))
    display(Markdown(f"{table_vl}{line_vl}"))

    # table = ""
    # for i in list(vl_k.keys()):
    #     table += "| $\mathcal{v}_{"+i+"}$ "
    # table+=f"\n |--{'----'.join('|' for v in vehicles)}\n"


    # for i in vehicles:
    #     table+= f"| {_k[i].X}"
    # table+="\n"
    # display(Markdown(table))
    
def build_instance(file_name):
    print(f"BUILDING INSTANCE {file_name.split("/")[len(file_name.split("/"))-1]}")
    instance_file = open(file_name,"r")
    
    lines = [line.strip() for line in instance_file.readlines()]
    current_line = 0
    
    #Salvando quantidade de clientes e quantidade e lockers
    qty_customer = int(lines[current_line].split(" ")[0])
    qty_locker = int(lines[current_line].split(" ")[1])
    
    current_line += 1
    
    max_vehicle = int(lines[current_line].split(" ")[0])
    vehicle_capacity = float(lines[current_line].split(" ")[1])
    
    current_line += 1
    
    demands = list()
    ## Salvando demands de clients
    for demand in range(current_line,qty_customer+2):
        demands.append(float(lines[demand]))
    
    current_line += qty_customer
    
    #Preenchendo nos
    nodes = dict()
    
    #Deposito
    depot = {
        'label': "D",
        'x' : float(lines[current_line].split(" ")[0]),
        'y' : float(lines[current_line].split(" ")[1]),
        'earliest' : float(lines[current_line].split(" ")[2]),
        'latest' : float(lines[current_line].split(" ")[3]),
        'service_time' : float(lines[current_line].split(" ")[4]),
        'demand': 0,
        'type' : int(lines[current_line].split(" ")[5])
    }
    nodes['D']=depot
    current_line += 1

    #Clientes
    customers = dict()
    customer_count = 1
    for i in range(current_line, current_line + qty_customer):
        customer = {
        'label' : f"C{customer_count-1}",
        'x' : float(lines[i].split(" ")[0]),
        'y' : float(lines[i].split(" ")[1]),
        'earliest' : float(lines[i].split(" ")[2]),
        'latest' : float(lines[i].split(" ")[3]),
        'service_time' : float(lines[i].split(" ")[4]),
        'demand': demands[customer_count-1],
        'type' : int(lines[i].split(" ")[5])
        }
        customer_count+=1
        customers[customer['label']]=customer
        nodes[customer['label']]=customer
    current_line += qty_customer
    #Lockers
    lockers = dict()
    locker_count = 1
    for i in range(current_line,current_line+qty_locker):
        locker = {
        'label' : f"P{locker_count-1}",
        'x' : float(lines[i].split(" ")[0]),
        'y' : float(lines[i].split(" ")[1]),
        'earliest' : float(lines[i].split(" ")[2]),
        'latest' : float(lines[current_line].split(" ")[3]),
        'service_time' : float(lines[i].split(" ")[4]),
        'demand': 0,
        'customers': list(),
        'type' : int(lines[i].split(" ")[5])
        }
        locker_count+=1
        lockers[locker['label']] = locker
        nodes[locker['label']]=locker
    current_line += qty_locker

    #Atribuição de lockers
    current_customer = 0
    for i in range(current_line, current_line+qty_customer):
        try:
            customer = customers[list(customers.keys())[current_customer]]
            
            locker_index = lines[i].split(" ").index("1")
            locker = lockers[list(lockers.keys())[locker_index]]
            customer['locker'] = locker['label']
            locker['customers'].append(customer['label'])
        except ValueError:    
            customer['locker'] = ""
        current_customer += 1
    current_line += qty_customer
    
    #Matriz de Distancia
    costs = dict()
    for i,node_i in enumerate(nodes):
         current_node_i = nodes[node_i]
         current_node_i['index'] = i
         for j,node_j in enumerate(nodes):
             current_node_j = nodes[node_j]
             distance = ((current_node_i['x']-current_node_j['x'])**2 + (current_node_i['y']-current_node_j['y'])**2)**(1/2)
             costs[current_node_i['label'],current_node_j['label']] = math.trunc(distance * 10) / 10
    print(f"Qty customer:{qty_customer}")
    print(f"Qty locker:{qty_locker}")
    print(f"Max vehicles:{max_vehicle}")
    print(f"Vehicle capacity:{vehicle_capacity}")
    print(f"BUILD FINISHED")
    print(sum(demands))
    return {
        'qty_customer': qty_customer,
        'qty_locker': qty_locker,
        'qty_nodes': qty_locker,
        'total_demands': sum(demands),
        'min_vehicles': round(sum(demands)/vehicle_capacity),
        'max_vehicle': max_vehicle,
        'vehicle_capacity': vehicle_capacity,
        'nodes': nodes,
        'customers': customers,
        'lockers': lockers,
        'depot': depot,
        'costs': costs
    }

def print_labels(nodes, lockers, customers, customers_hc, customers_lc, customers_hlc, vehicles, depot):
    print(f"Nodes: {nodes}")
    print(f"Depot: {depot}")
    print(f"Lockers: {lockers}")
    print(f"Customers: {customers}")
    print(f"Customers HC : {customers_hc}")
    print(f"Customers LC: {customers_lc}")
    print(f"Customers HLC: {customers_hlc}")
    print(f"Vehicles: {vehicles}")