import pandas as pd
import math
class Instance:
    def __init__(self):
        self.name=""
        self.nodes = []
        self.distances = []
    def __str__(self):
        instance_str = self.name + " - "
        for node in self.nodes:
            instance_str = instance_str + node.__str__()+" "
        return instance_str
class Node:
    def __init__(self):
        self.id = ''
        self.type = ''
        self.x = 0
        self.y = 0
        self.demand = 0
        self.ready_time = 0
        self.due_time = 0
        self.service_time = 0
        self.locker_customers = []
        self.assigned_locker = ''
    
    def __str__(self):
        return "ID:" + self.id

def build_paths(instance_name, debug):
    
    routes = []

    try:

        file = None

        if debug:
            file = pd.read_csv(f'..\\..\\cmake-build-debug\\Output\\{instance_name}_routes')
        else:
            file = pd.read_csv(f'..\\..\\Output\\{instance_name}_routes')

        file = list(file['route'])

        run = []

        for route in file:
            if "---" in route:
               routes.append(run)
               run = []
            else: 
                run.append(route.split(" "))
    
    except FileNotFoundError as error:
        print(f"File {instance_name} not Found")  

    finally:
        return routes    
    
def build_stats(instance_name, debug):

    try:

        stats = None

        if debug:
            stats = pd.read_csv(f'..\\..\\cmake-build-debug\\Output\\{instance_name}_stats',index_col="run")
        else:
            stats = pd.read_csv(f'..\\..\\Output\\{instance_name}_stats',index_col="run")
    
    except FileNotFoundError as error:
        print(f"File {instance_name}_stats not Found")  

    finally:
        return stats    

def build_path_strings(selected_instance,in_debug_mode):
    paths_array_strings = build_paths(selected_instance.name,in_debug_mode)

    for run in paths_array_strings:
        i = 0
        for path_i in range(len(run)):
            run[path_i] = [f"V{i}",run[path_i]]
            i+=1            
    return paths_array_strings

def build_instances_vrppl():
    instance_list = open('..\\..\\instances\\instances_vrppl.txt',"r")

    ready_instances = []

    for instance_name in instance_list:
        instance_name = '..\\..\\' + instance_name.split("\n")[0]
        instance_file = open(instance_name,"r")

        new_instance = Instance()
        new_instance.name = instance_name.split("\\")[len(instance_name.split("\\"))-1]

        #Numero de clientes e lockers
        line = instance_file.readline()
        line = line.split(" ")
        n_customers = int(line[0])
        n_lockers = int(line[1])

        #Numero de veículos e capacidade dos veiculos
        line = instance_file.readline()
        line = line.split(" ")
        n_vehicle = int(line[0])
        load_capacity = float(line[1])
        
        ## Definindo nós

        # Ids e demandas
        node_d0 = Node()
        node_d0.id = "D0"
        new_instance.nodes.append(node_d0)
        node_dt = Node()
        node_dt.id = "Dt"
        new_instance.nodes.append(node_dt)


        for i in range(n_customers):
            line = instance_file.readline()
            line = line.split("\n")[0]
            new_node = Node()
            new_node.demand = line
            new_node.id = "C" + str(i)
            new_instance.nodes.append(new_node)
        
        
        for i in range(n_lockers):
            new_node = Node()
            new_node.demand = 0
            new_node.id = "P" + str(i)
            new_instance.nodes.append(new_node)
        

        line = instance_file.readline()
        line = line.split("\n")[0].split(" ")
        new_instance.nodes[0].type = 'd'
        new_instance.nodes[0].x = float(line[0])
        new_instance.nodes[0].y = float(line[1])
        new_instance.nodes[0].ready_time = float(line[2])
        new_instance.nodes[0].due_time = float(line[3])
        new_instance.nodes[0].service_time = float(line[4])
        new_instance.nodes[1].type = 'd'
        new_instance.nodes[1].x = float(line[0])
        new_instance.nodes[1].y = float(line[1])
        new_instance.nodes[1].ready_time = float(line[2])
        new_instance.nodes[1].due_time = float(line[3])
        new_instance.nodes[1].service_time = float(line[4])

        for i in range(2,len(new_instance.nodes)):
            line = instance_file.readline()
            line = line.split("\n")[0].split(" ")
            new_instance.nodes[i].x = float(line[0])
            new_instance.nodes[i].y = float(line[1])
            new_instance.nodes[i].ready_time = float(line[2])
            new_instance.nodes[i].due_time = float(line[3])
            new_instance.nodes[i].service_time = float(line[4])

            match int(line[5]):
                case 1:    
                    new_instance.nodes[i].type = 'c1'
                case 2:    
                    new_instance.nodes[i].type = 'c2'
                case 3:    
                    new_instance.nodes[i].type = 'c3'
                case 4:    
                    new_instance.nodes[i].type = 'p'


            # print(line)
            # print(new_instance.nodes[i].x, new_instance.nodes[i].y, new_instance.nodes[i].ready_time, new_instance.nodes[i].due_time, new_instance.nodes[i].service_time, new_instance.nodes[i].type)
        
        #Definindo locker
        for i in range(2,n_customers+2):
            line = instance_file.readline()
            line = line.split("\n")[0].split(" ")

            for j in range(len(line)):
                if int(line[j]) == 1:
                    locker = new_instance.nodes[n_customers + 2 + j]
                    customer = new_instance.nodes[i]
                    customer.assigned_locker = locker
                    locker.locker_customers.append(customer)
            
        ready_instances.append(new_instance)
        #print([node.id for node in new_instance.nodes])
            
        
        

        # for line in instance_file:
        #     print(line)

    calculate_distances(ready_instances)
    return ready_instances

def build_instances_evrptwprpl():
    file = pd.read_csv('..\\..\\instances\\instances_evrptwprpl.txt',names=['file_name'])
    file = list(file['file_name'])
    instances = ['..\\..\\' + instance for instance in file]
    ready_instances = []

    for instance in instances:
        file_instance = pd.read_csv(instance, sep=' ')
        new_instance = Instance()
        new_instance.name = instance.split('\\')[5]
        locker_count = 0
        for line in file_instance.iterrows():
            if(is_node(line)):
                new_node = Node()
                new_node.id = line[1]['StringID']
                new_node.type = line[1]['Type']
                new_node.x = int(line[1]['x'])
                new_node.y = int(line[1]['y'])
                new_node.demand = int(line[1]['demand'])
                new_node.ready_time = int(line[1]['ReadyTime'])
                new_node.due_time = int(line[1]['DueDate'])
                new_node.service_time = int(line[1]['ServiceTime'])
                new_instance.nodes.append(new_node)
                if(new_node.type == 'p'):
                    locker_count+=1
            elif (is_param(line)):
                if locker_count==1:
                    locker = find_k_locker(new_instance.nodes,['1'])
                    for node in new_instance.nodes:
                        if(node.type == 'c2' or node.type == 'c3'):
                            node.assigned_locker = locker
                            locker.locker_customers.append(node)
                


        ready_instances.append(new_instance) 
    calculate_distances(ready_instances)
    return ready_instances

def build_instances():
    file = pd.read_csv('..\\..\\instances\\instances.txt',names=['file_name'])
    file = list(file['file_name'])
    instances = ['..\\..\\' + instance for instance in file]

    ready_instances = []

    for instance in instances:
        file_instance = pd.read_csv(instance, sep=' ')
        new_instance = Instance()
        new_instance.name = instance.split('\\')[4]
        locker_count = 0
        for line in file_instance.iterrows():
            if(is_node(line)):
                new_node = Node()
                new_node.id = line[1]['StringID']
                new_node.type = line[1]['Type']
                new_node.x = int(line[1]['x'])
                new_node.y = int(line[1]['y'])
                new_node.demand = int(line[1]['demand'])
                new_node.ready_time = int(line[1]['ReadyTime'])
                new_node.due_time = int(line[1]['DueDate'])
                new_node.service_time = int(line[1]['ServiceTime'])
                new_instance.nodes.append(new_node)
                if(new_node.type == 'p'):
                    locker_count+=1
            elif (is_param(line)):
                if locker_count==1:
                    locker = find_k_locker(new_instance.nodes,['1'])
                    for node in new_instance.nodes:
                        if(node.type == 'c2' or node.type == 'c3'):
                            node.assigned_locker = locker
                            locker.locker_customers.append(node)
                


        ready_instances.append(new_instance) 
    calculate_distances(ready_instances)
    return ready_instances

def is_node(line):
    return line[1]['StringID'] != 'param' and line[1]['StringID'] != '0' and line[1]['StringID'] != '1'
def is_locker_line(line):
    return line[1]['StringID'] == '0' or line[1]['StringID'] == '1'
def is_param(line):
    return line[1]['StringID'] == 'param'

    
def find_k_customer(nodes, k):
    c = 0
    for node in nodes:
        if node.type == 'c1' or node.type == 'c2' or node.type == 'c3':
            if c == k:
                return node
            else:
                c+=1
    print("ERRO Customer")
    return None
                
def find_k_locker(nodes, line):
    k=0
    for value in line:
        if (value == '1'):
            break
        else:
            k+=1
    c = 0
    for node in nodes:
        if node.type == 'p':
            if c == k:
                return node
            else:
                c+=1
    return Node()

def calculate_distances(instances):
    for instance in instances:
        for i in range(len(instance.nodes)):
            line_distance = []
            for j in range(len(instance.nodes)):
                line_distance.append(math.trunc((((instance.nodes[i].x-instance.nodes[j].x)**2 + (instance.nodes[i].y-instance.nodes[j].y)**2)**(1/2)) * 10) / 10)
            instance.distances.append(line_distance)