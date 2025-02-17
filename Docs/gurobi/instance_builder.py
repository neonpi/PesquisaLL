def build_instance(file_name):
    print(f"BUILDING INSTANCE {file_name.split("/")[len(file_name.split("/"))-1]}")
    instance_file = open(file_name,"r")
    
    lines = [line.strip() for line in instance_file.readlines()]
    current_line = 0
    
    #Salvando quantidade de clientes e quantidade e lockers
    qty_customer = int(lines[current_line].split(" ")[0])
    qty_locker = int(lines[current_line].split(" ")[1])
    
    current_line += 1
    
    max_vehicle = lines[current_line].split(" ")[0]
    vehicle_capacity = lines[current_line].split(" ")[1]
    
    current_line += 1
    
    demmands = list()
    ## Salvando demmands de clients
    for demmand in range(current_line,qty_customer+2):
        demmands.append(lines[demmand])
    
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
        'type' : lines[current_line].split(" ")[5]
    }
    nodes['D']=depot
    current_line += 1
    
    customers = dict()
    customer_count = 1
    for i in range(current_line, current_line + qty_customer):
        customer = {
        'label' : f"C{customer_count}",
        'x' : float(lines[i].split(" ")[0]),
        'y' : float(lines[i].split(" ")[1]),
        'earliest' : float(lines[i].split(" ")[2]),
        'latest' : float(lines[current_line].split(" ")[3]),
        'service_time' : float(lines[i].split(" ")[4]),
        'demand': demmands[customer_count-1],
        'type' : lines[i].split(" ")[5],
        }
        customer_count+=1
        customer[customer['label']]=customer
        nodes[customer['label']]=customer
    
    current_line += qty_customer
    
    lockers = list()
    locker_count = 1
    for i in range(current_line,current_line+qty_locker):
        locker = {
        'label' : f"P{locker_count}",
        'x' : float(lines[i].split(" ")[0]),
        'y' : float(lines[i].split(" ")[1]),
        'earliest' : float(lines[i].split(" ")[2]),
        'latest' : float(lines[current_line].split(" ")[3]),
        'service_time' : float(lines[i].split(" ")[4]),
        'demand': 0,
        'type' : lines[i].split(" ")[5],
        }
        locker_count+=1
        lockers.append(locker)
        nodes[locker['label']]=locker
    
    costs = dict()
    for i,node_i in enumerate(nodes):
         current_node_i = nodes[node_i]
         current_node_i['index'] = i
         for j,node_j in enumerate(nodes):
             current_node_j = nodes[node_j]
             distance = ((current_node_i['x']-current_node_j['x'])**2 + (current_node_i['y']-current_node_j['y'])**2)**(1/2)
             costs[f"c_{current_node_i['label']}{current_node_j['label']}"] = distance
                
    print(f"Qty customer:{qty_customer}")
    print(f"Qty locker:{qty_locker}")
    print(f"Max vehicles:{max_vehicle}")
    print(f"Vehicle capacity:{vehicle_capacity}")
    print(f"BUILD FINISHED")
    return {
        'qty_customer': qty_customer,
        'qty_locker': qty_locker,
        'qty_nodes': qty_locker,
        'max_vehicle': max_vehicle,
        'vehicle_capacity': vehicle_capacity,
        'nodes': nodes,
        'customers': customers,
        'lockers': lockers,
        'depot': depot,
        'costs': costs
    }
