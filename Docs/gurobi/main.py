import vrppl as rp

instances = open('instances.txt','r')

for instance in instances:
    
    if not '#' in instance:
        instance = instance.strip()
        rp.run_all(instance)
        

instances.close()