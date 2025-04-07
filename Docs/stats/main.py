import streamlit as st
import data as dt
import utils as ut
import json

st.session_state['instances'] = dt.build_instances()

instances = st.session_state['instances']
selected_instance = st.sidebar.selectbox("Instance",[instance.name for instance in instances],0)
ut.select_instance(selected_instance)

st.session_state['debug'] = st.sidebar.toggle("Debug mode")

# st.session_state['instance']
# st.session_state['instances'] = dt.build_instances()


# pg=st.navigation([
#                   st.Page("page_general_result.py", title="General Result"),
#                   st.Page("page_instance_result.py", title="Instance Result")])

pg=st.navigation([
                  st.Page("page_instance_result.py", title="Instance Result")])
pg.run()

