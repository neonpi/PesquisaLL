import pandas as pd
import streamlit as st
import numpy as np
import data as dt
import json
instances = st.session_state['instances']
debug = st.session_state['debug']

# stats = [[instance.name,dt.build_stats(instance.name,debug)] for instance in instances ]
instance_results, overall_df,summary_result = dt.build_stats(debug)

st.title("Summary results")
st.write(f"Literature Avg. Time: {summary_result['Literature avg time']:.3f} s")
st.write(f"Avg. Time: {summary_result['Avg time']:.3f} s")
st.write(f"Literature Avg. Best Cost: {summary_result['Literature avg best cost']:.2f}")
st.write(f"Avg Best Cost: {summary_result['Avg best cost']:.2f}")
st.write(f"Avg GAP: {summary_result['Avg GAP']:.4f} %")
st.write(f"Best GAP: {summary_result['Best GAP']:.4f} %")
st.write(f"Worst GAP: {summary_result['Worst GAP']:.4f} %")

st.title("Overall results")

st.dataframe(overall_df, height=6000, width=600, column_config={
    'Lit. Cost': st.column_config.NumberColumn(format="%.1f"),
    'Lit. Avg Time': st.column_config.NumberColumn(format="%.3f"),
    'Avg Time': st.column_config.NumberColumn(format="%.3f"),
    'Avg Cost': st.column_config.NumberColumn(format="%.1f"),
    'Best Cost': st.column_config.NumberColumn(format="%.1f"),
    'Gap': st.column_config.NumberColumn(format="%.3f")
})


# for i in range(len(instances)):
#     if instances[i].name in [stat[0] for stat in stats]:
#         # inst_data = np.array([instances[i].name, np.mean(stats[i][1]['time'].to_numpy()).round(5),np.mean(stats[i][1]['cost'].to_numpy()).round(5),(np.array(stats[i][1]['time'])).min(),(np.array(stats[i][1]['cost']).min()).round(3)])
#     else:
#         inst_data = [instances[i].name,0,0,0,0]

#     stats_data.append(inst_data)

# df = pd.DataFrame(np.array(stats_data),columns = [ "Instance","Avg Time", "Avg Cost", "Best Time", "Best Cost"])
# st.dataframe(df, width=700, height=6000)