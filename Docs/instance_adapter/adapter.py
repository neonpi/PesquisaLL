import pandas as pd
import os
list_file = open('instances/instances.txt','r')

instance_list = list_file.readlines()

list_file.close()

for i in range(len(instance_list)):
    instance_list[i] = instance_list[i].replace('\n','')

for instance in instance_list:
    instance_input_file = pd.read_csv(instance,sep=' ')
    instance = instance.replace('instances\\','')
    
    instance_output_file = None
    try:
        instance_output_file = open(f'output\\{instance}','x')
        instance_output_file.close()
    except FileExistsError:
        os.remove(f'output\\{instance}')
    
    instance_output_file = open(f'output\\{instance}','a')
    instance_output_file.write("StringID Type x y demand ReadyTime DueDate ServiceTime\n")
    #instance_output_file.write(instance_input_file.index)
    for line in instance_input_file.iterrows():
        if(line[1]['Type'] != 'f' and 
           line[1]['Type'] != 'Q'and
           line[1]['Type'] != 'r' and
           line[1]['Type'] != 'g'):
            instance_output_file.write(" ".join(str(element) for element in line[1])+'\n')
            
    instance_output_file.close()
    

