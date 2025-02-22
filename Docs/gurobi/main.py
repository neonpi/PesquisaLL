import vrppl as rp

instances = open('instances.txt','r')

for instance in instances:
    
    if not '#' in instance:
        instance = instance.strip()
        print(f"Running instance {instance.split('/')[-1]}")
        rp.run_all(instance)
        

instances.close()