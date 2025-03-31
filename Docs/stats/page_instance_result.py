import streamlit as st
import plotly.graph_objects as go
import networkx as nx
import graph as gph
import data as dt
import pandas as pd

dt.build_runs()
instance = st.session_state['instance']
runs = st.session_state['runs']
# st.write(st.session_state['runs'])

#Controllers
show_all_edges = st.sidebar.toggle("Show all edges")
show_locker_edges = st.sidebar.toggle("Show locker edges")
show_path_edges = st.sidebar.toggle("Show patch edges")
show_path_string = st.sidebar.toggle("Show path string")
node_size = st.sidebar.slider(label="Node size", min_value=1, max_value=50, value=15)
node_label_color = st.sidebar.selectbox(label="Node label color",options=["Black", "Gold"])
runs_to_show = st.multiselect("Run to show:",[run for run in runs], format_func = lambda run: f"r {run['run']}, c: {run['cost']}, #v: {run['n_vehicles']}")

st.session_state['runs_to_show'] = runs_to_show
st.session_state['show_path_string'] = show_path_string
st.session_state['show_all_edges'] = show_all_edges
st.session_state['show_locker_edges'] = show_locker_edges
st.session_state['show_path_edges'] = show_path_edges
st.session_state['node_label_color'] = node_label_color
st.session_state['node_size'] = node_size

dt.build_path_strings()
gph.build_graph()

node_trace = gph.build_nodes()
all_edge_trace = gph.build_all_edges()
locker_edge_trace = gph.build_locker_edges()
path_edges_trace = gph.build_path_edges_trace()

fig = gph.build_fig([node_trace,all_edge_trace, locker_edge_trace] + path_edges_trace)

st.plotly_chart(fig)


distances = pd.DataFrame(instance.distances, columns = [node.id for node in instance.nodes])
distances.index = [node.id for node in instance.nodes]
distances_formatted = distances.map(lambda x: f"{x:.2f}" if isinstance(x, (int, float)) else x)

tab_inst, tab_stats = st.tabs(["Instance data", "Stats"])
with tab_inst:
    node_a = st.selectbox("Node a", [node.id for node in instance.nodes])
    node_b = st.selectbox("Node b", [node.id for node in instance.nodes])
    st.write(f"Distance: {distances.loc[node_a,node_b]}")
    st.write("Distance Matrix")
    st.table(distances_formatted)
    load_demands = pd.DataFrame([[node.demand for node in instance.nodes]], columns=[node.id for node in instance.nodes])
    st.write("Load Demmand")
    st.table(load_demands)
    time_demands = pd.DataFrame([[node.service_time for node in instance.nodes]], columns=[node.id for node in instance.nodes])
    st.write("Time Demmand")
    st.table(time_demands)
    time_window = pd.DataFrame([[(node.ready_time,node.due_time) for node in instance.nodes]], columns=[node.id for node in instance.nodes])
    st.write("Time Window")
    st.table(time_window)
with tab_stats:
    stats = dt.build_stats(instance.name,in_debug_mode)
    stats['cost'] = stats['cost'].round(3)
    stats['time'] = stats['time'].round(3)
    selected_row = st.dataframe(stats.style.highlight_min(axis=0), width=500, height=1100, selection_mode="single-row",on_select="rerun")
    selected_run = selected_row.selection.rows[0]
    selected_run_file = open('selected_run','w')
    selected_run_file.write(f"{selected_run}")