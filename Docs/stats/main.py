import streamlit as st
import data as dt


instances = dt.build_instances() 
selected_instance = instances[0]

def select_instance(instance):
    selected_instance = instance

# st.sidebar.selectbox("Instance",[instance.name for instance in instances],0, on_change = lambda x: selected_instance = x)

st.session_state['instances'] = dt.build_instances()
instances[0]

pg=st.navigation([
                  st.Page("page_instance_result.py", title="Instance Result")])
pg.run()