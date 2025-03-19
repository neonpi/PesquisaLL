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