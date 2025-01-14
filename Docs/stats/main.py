import streamlit as st

pg=st.navigation([st.Page("page_general_result.py", title="General Result"),
                  st.Page("page_instance_result.py", title="Instance Result")])
pg.run()