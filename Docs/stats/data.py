import pandas as pd
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
            file = pd.read_csv(f'..\\..\\cmake-build-debug\\Output\\{instance_name}')
        else:
            file = pd.read_csv(f'..\\..\\Output\\{instance_name}')

        file = list(file['route'])
        
        for route in file:
            routes.append(route.split(" "))
    
    except FileNotFoundError as error:
        print(f"File {instance_name} not Found")  

    finally:
        return routes    
        

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
        customer_count = 0
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
            # elif(is_locker_line(line)):
                # if(new_instance.name=="rc208prpl.dat"):
                # customer = find_k_customer(new_instance.nodes,customer_count)
                # customer_count+=1
                # if customer.type != 'c1':
                #     locker = find_k_locker(new_instance.nodes,line[1])
                #     customer.assigned_locker = locker
                #     locker.locker_customers.append(customer)
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
                line_distance.append(((instance.nodes[i].x-instance.nodes[j].x)**2 + (instance.nodes[i].y-instance.nodes[j].y)**2)**(1/2))
            instance.distances.append(line_distance)