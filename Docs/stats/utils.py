import streamlit as st
def def_environment():
    st.sidebar.header("Parameters")

def select_instance(instance_name):

    st.session_state['instance'] = [instance for instance in st.session_state['instances'] if instance.name == instance_name][0]