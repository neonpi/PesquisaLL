import streamlit as st
import plotly.graph_objects as go
import networkx as nx
import graph as gph
import data as dt
import pandas as pd

# st.write("OLA")
instances = dt.build_instances_vrppl()

#Controllers
st.sidebar.header("Parameters")

instance_selection = st.sidebar.selectbox(label="Instance", options = [instance.name for instance in instances])
selected_instance = [instance for instance in instances if instance.name == instance_selection][0]
in_debug_mode = st.sidebar.toggle("Debug mode")
show_all_edges = st.sidebar.toggle("Show all edges")
show_locker_edges = st.sidebar.toggle("Show locker edges")
show_path_edges = st.sidebar.toggle("Show patch edges")
show_path_string = st.sidebar.toggle("Show path string")
node_size = st.sidebar.slider(label="Node size", min_value=1, max_value=50, value=15)


#Graph
G = nx.MultiGraph()

#Nodes
G.add_nodes_from([(node.id, {'pos': (node.x,node.y), 'node_data': node}) for node in selected_instance.nodes])

#Edges
for index_i, node_i in enumerate(selected_instance.nodes):
    for index_j, node_j in enumerate(selected_instance.nodes):
        G.add_edge(node_i.id, node_j.id, weight = selected_instance.distances[index_i][index_j])

node_trace = gph.build_nodes(G,node_size)
all_edge_trace = gph.build_all_edges(G,show_all_edges)
locker_edge_trace = gph.build_locker_edges(G,selected_instance,show_locker_edges)
paths_array_strings_runs = dt.build_path_strings(selected_instance,in_debug_mode)
path_edges_trace = []
selected_run = int(open('selected_run','r').read())
if(len(paths_array_strings_runs)>0):
    paths_array_strings = paths_array_strings_runs[selected_run]

    path_edges_to_show = []
    if show_path_edges:
        path_edges_to_show = st.multiselect("Routes to show:",[path[0] for path in paths_array_strings],[path[0] for path in paths_array_strings])

    path_edges_trace = gph.build_path_edges_trace(G,selected_instance,show_path_edges,[path[1] for path in paths_array_strings if path[0] in path_edges_to_show])

fig = gph.build_graph([node_trace,all_edge_trace, locker_edge_trace] + path_edges_trace)
if path_edges_trace == [] and show_path_edges:
    st.write("Path file not found")

st.plotly_chart(fig)

if show_path_string:
    st.write("Routes:")
    for route in paths_array_strings:
            st.write(f"{route[0]}: {route[1]}")

distances = pd.DataFrame(selected_instance.distances, columns = [node.id for node in selected_instance.nodes])
distances.index = [node.id for node in selected_instance.nodes]
distances_formatted = distances.map(lambda x: f"{x:.2f}" if isinstance(x, (int, float)) else x)

tab_inst, tab_stats = st.tabs(["Instance data", "Stats"])
with tab_inst:
    st.write("Distance Matrix")
    st.table(distances_formatted)
    load_demands = pd.DataFrame([[node.demand for node in selected_instance.nodes]], columns=[node.id for node in selected_instance.nodes])
    st.write("Load Demmand")
    st.table(load_demands)
    time_demands = pd.DataFrame([[node.service_time for node in selected_instance.nodes]], columns=[node.id for node in selected_instance.nodes])
    st.write("Time Demmand")
    st.table(time_demands)
    time_window = pd.DataFrame([[(node.ready_time,node.due_time) for node in selected_instance.nodes]], columns=[node.id for node in selected_instance.nodes])
    st.write("Time Window")
    st.table(time_window)
with tab_stats:
    stats = dt.build_stats(selected_instance.name,in_debug_mode)
    stats['cost'] = stats['cost'].round(3)
    stats['time'] = stats['time'].round(3)
    selected_row = st.dataframe(stats.style.highlight_min(axis=0), width=500, height=1100, selection_mode="single-row",on_select="rerun")
    selected_run = selected_row.selection.rows[0]
    selected_run_file = open('selected_run','w')
    selected_run_file.write(f"{selected_run}")