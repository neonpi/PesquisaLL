import json
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
    
    def to_dict(self):
        for node in self.nodes:
            if node.type == 'p':
                print([locker.to_dict() for locker in node.locker_customers])
        # print([node.to_dict() for node in self.nodes])
        obj = {
            "name": self.name,
            "nodes": [node.to_dict() for node in self.nodes],
            "distances": self.distances 
        }
        return obj
    
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
    
    def to_dict(self):
        return {
            "id" : self.id,
            "type" : self.type,
            "x" : self.x,
            "y" : self.y,
            "demand" : self.demand,
            "ready_time" : self.ready_time,
            "due_time" : self.due_time,
            "service_time" : self.service_time,
            "locker_customer" : [node.id for node in self.locker_customers],
            "assigned_locker" : self.assigned_locker.id if isinstance(self.assigned_locker,Node) else ""
        }