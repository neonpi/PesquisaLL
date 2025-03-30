import pandas as pd
import math
import models as md
import streamlit as st
import numpy as np
import traceback


def build_runs():
    debug = st.session_state['debug']
    instance = st.session_state['instance']
    path = f"../../output/{instance.name}_stats"
    runs = list()

    current_run = None
    current_run_routes = None
    with open(path,"r") as file:
        lines = [line.strip() for line in file.readlines()]
        for index, line in enumerate(lines):
            if "---" in line:
                current_run = dict()
                current_run_routes = list()
                p_line = line.strip().split(',')
                p_line[0] = p_line[0].split(" ")[1]
                current_run['run'] = int(p_line[0])
                current_run['cost'] = round(float(p_line[1]),2)
                current_run['n_vehicles'] = int(p_line[2])
                current_run['time'] = round(float(p_line[3]),4)
            else:
                if(len(line) <= 1 or index == (len(lines)-1)):
                    current_run['routes'] = current_run_routes
                    runs.append(current_run)
                    current_run = None
                    current_run_routes = None
                elif(index > 0):
                    line = line.split(" ")
                    route = list()

                    for str_node in line:
                        node = dict()
                        if("P" in str_node):
                            node['id'] = str_node.split("(")[0]
                            node['customers'] = str_node.split("(")[1].split(")")[0].split(",")
                        else:
                            node['id'] = str_node
                            node['customers'] = [str_node]
                        route.append(node)
                    
                    current_run_routes.append(route)
                    
    st.session_state['runs'] = runs

    
def build_stats(debug):

    instance_results = dict()
    overall_stats = dict()
    summary_result = dict()
    try:
        instances = st.session_state['instances']

        overall_stats = {
            'avg_cost': np.array([]),
            'best_cost': np.array([]),
            'avg_time': np.array([]),
            'gap': np.array([]),

        }
        
        # lendo resultados da literatura
        with open("lit_sol.txt","r") as lit_file:
            lines = lit_file.readlines()
            overall_stats['lit_best_cost'] = np.array([float(sol.strip().split(",")[0]) for sol in lines])
            overall_stats['lit_avg_time'] = np.array([float(sol.strip().split(",")[1]) for sol in lines])

        
        i_num = 0
        for instance in instances:
            
            if debug:
                file_name= f'../../cmake-build-debug/Output/{instance.name}_stats'
            else:
                file_name= f'../../Output/{instance.name}_stats'
            
            exec_line = []
            
            with open(file_name,"r") as file:
                for line in [l.strip() for l in file.readlines()]:
                    if "---" in line:
                        exec_line.append(line.strip().split(" ")[1].split(","))
    
            runs = np.array([int(line[0]) for line in exec_line])
            costs = np.array([float(line[1]) for line in exec_line])
            vehicles = np.array([int(line[2]) for line in exec_line])
            times = np.array([float(line[3]) for line in exec_line])

            overall_stats['avg_time'] = np.append(overall_stats['avg_time'],np.mean(times))
            overall_stats['avg_cost'] = np.append(overall_stats['avg_cost'],np.mean(costs))
            overall_stats['best_cost'] = np.append(overall_stats['best_cost'],np.min(costs))
            overall_stats['gap'] = np.append(overall_stats['gap'],(np.min(costs) - overall_stats['lit_best_cost'][i_num])/overall_stats['lit_best_cost'][i_num])
            
            i_num+=1

            # print(times)
            data = {
                "Cost": costs,
                "#Vehicle": vehicles,
                "Time": times
            }
            stats = pd.DataFrame(data=data, index = runs)
            stats.index.name = "Run"
            instance_results[instance.name] = stats
        
        overall_stats = {
            "Lit. Cost": overall_stats['lit_best_cost'],
            "Lit. Avg Time": overall_stats['lit_avg_time'],
            "Avg Time": overall_stats['avg_time'],
            "Avg Cost": overall_stats['avg_cost'],
            "Best Cost": overall_stats['best_cost'],
            "Gap": overall_stats['gap']
        }
        overall_df = pd.DataFrame(data = overall_stats,index = [instance.name.strip().split(".")[0] for instance in instances])
        overall_df.index.name = "Instance"

        summary_result = {
            "Literature avg time": np.mean(overall_stats['Lit. Avg Time']),
            "Avg time": np.mean(overall_stats['Avg Time']),
            "Literature avg best cost": np.mean(overall_stats['Lit. Cost']),
            "Avg best cost": np.mean(overall_stats['Best Cost']),
            "Avg GAP": np.mean(overall_stats['Gap']),
            "Best GAP": np.min(overall_stats['Gap'])*100,
            "Worst GAP": np.max(overall_stats['Gap'])*100
        }


    except FileNotFoundError as error:
        print(f"File  not Found")  
        # print(f"File {instance_name}_stats not Found")  
    except Exception as error:
        print(f"Erro {error}")
    finally:
        return instance_results, overall_df, summary_result

def build_path_strings():
    show_path_string = st.session_state['show_path_string']
    
    if(show_path_string):
        runs = st.session_state['runs_to_show']
        for run in runs:
            st.write(f"**Run {run['run']}** :")
            veh_no = 0
            for route in run['routes']:
                line = f"   - V{veh_no} - "
                for node in route:
                    if(len(node['customers']) > 1):
                        line = f"{line} {node['id']}({",".join(node["customers"])})"
                    else:
                        line = f"{line} {node['id']}"
                veh_no += 1
                st.write(line)

def build_instances():
    instance_list = open('../../instances/instances.txt', "r")

    ready_instances = []

    for instance_name in instance_list:
        instance_name = '..\\..\\' + instance_name.split("\n")[0]
        instance_file = open(instance_name,"r")

        new_instance = md.Instance()
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
        node_d0 = md.Node()
        node_d0.id = "D0"
        new_instance.nodes.append(node_d0)
        node_dt = md.Node()
        node_dt.id = "Dt"
        new_instance.nodes.append(node_dt)


        for i in range(n_customers):
            line = instance_file.readline()
            line = line.split("\n")[0]
            new_node = md.Node()
            new_node.demand = line
            new_node.id = "C" + str(i)
            new_instance.nodes.append(new_node)
        
        
        for i in range(n_lockers):
            new_node = md.Node()
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
                    break
            
        ready_instances.append(new_instance)
        #print([node.id for node in new_instance.nodes])
            
        
        

        # for line in instance_file:
        #     print(line)

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
    return md.Node()

def calculate_distances(instances):
    for instance in instances:
        for i in range(len(instance.nodes)):
            line_distance = []
            for j in range(len(instance.nodes)):
                line_distance.append(math.trunc((((instance.nodes[i].x-instance.nodes[j].x)**2 + (instance.nodes[i].y-instance.nodes[j].y)**2)**(1/2)) * 10) / 10)
            instance.distances.append(line_distance)