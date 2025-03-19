import pandas as pd
import streamlit as st
import numpy as np
import data as dt

instances = st.session_state['instances']

instances[0]

in_debug_mode = st.sidebar.toggle("Debug mode")

selected_instance = [instance for instance in instances if instance.name == instance_selection][0]
# stats = [[instance.name,dt.build_stats(instance.name,in_debug_mode)] for instance in instances ]
# st.write(stats)
# stats_data = []
# print(stats)
# for i in range(len(instances)):
#     if instances[i].name in [stat[0] for stat in stats]:
#         print(instances[i].name)
#         # inst_data = np.array([instances[i].name, np.mean(stats[i][1]['time'].to_numpy()).round(5),np.mean(stats[i][1]['cost'].to_numpy()).round(5),(np.array(stats[i][1]['time'])).min(),(np.array(stats[i][1]['cost']).min()).round(3)])
#     else:
#         inst_data = [instances[i].name,0,0,0,0]

#     stats_data.append(inst_data)

# df = pd.DataFrame(np.array(stats_data),columns = [ "Instance","Avg Time", "Avg Cost", "Best Time", "Best Cost"])
# st.dataframe(df, width=700, height=6000)