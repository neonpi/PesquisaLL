import pandas as pd
file_names = ["result_25.csv","result_50.csv","result_100.csv"]
miscelaneous = open("Output/miscelaneous",'w')
qtd_gap=[0,0,0,0]
avg_gaps=[0,0,0,0]
qtd_time_ig=[0,0,0,0]
avg_times_ig=[0,0,0,0]
avg_times_g=[0,0,0,0]
avg_times_sa=[0,0,0,0]

for file_name in file_names:

    file = pd.read_csv(file_name)

    file_output = open(f"Output/{file_name}",'w')
    file_output.close()
    file_output = open(f"Output/{file_name}",'a')

    for i in range(len(file)):
        output = f"\\textbf{'{'}{file.loc[i]['instance']}{'}'} & "
        best_cost = f"{min(round(file.loc[i]['g_best_cost'],3),round(file.loc[i]['sa_best_cost'],3),round(file.loc[i]['ig_best_cost'],3)):.3f}"
    
        if (f"{round(file.loc[i]['g_best_cost'],3):.3f}" == best_cost):
            output += f"\\textbf{'{'}{file.loc[i]['g_best_cost']:.3f}{'}'} & "
        else:
            output += f"{file.loc[i]['g_best_cost']:.3f} & "

        output +=  f"{file.loc[i]['g_n_v']} & {file.loc[i]['g_avg_time']:.3f} & {file.loc[i]['sa_avg_cost']:.3f} & "

        if (f"{round(file.loc[i]['sa_best_cost'],3):.3f}" == best_cost):
            output += f"\\textbf{'{'}{file.loc[i]['sa_best_cost']:.3f}{'}'} & "
        else:
            output += f"{file.loc[i]['sa_best_cost']:.3f} & "

        output +=  f"{file.loc[i]['sa_n_v']} & {file.loc[i]['sa_avg_time']:.3f} & {file.loc[i]['ig_avg_cost']:.3f} & "
        
        if (f"{round(file.loc[i]['ig_best_cost'],3):.3f}" == best_cost):
            output += f"\\textbf{'{'}{file.loc[i]['ig_best_cost']:.3f}{'}'} & "
        else:
            output += f"{file.loc[i]['ig_best_cost']:.3f} & "
    
        output +=  f"{file.loc[i]['ig_n_v']} & {file.loc[i]['ig_avg_time']:.3f} & "
        
        if (file.loc[i]['gap'] <= 0):
            output += f"\\textbf{'{'}{file.loc[i]['gap']:.3f}{'}'} \\\\\n"
            if(file_name == "result_25.csv"):
                qtd_gap[0]+=1
            elif (file_name == "result_50.csv"):
                qtd_gap[1]+=1
            else:
                qtd_gap[2]+=1

            qtd_gap[3]+=1
        else:
            output += f"{file.loc[i]['gap']:.3f} \\\\\n"

        if (file.loc[i]['ig_avg_time'] == min(file.loc[i]['sa_avg_time'],file.loc[i]['g_avg_time'],file.loc[i]['ig_avg_time'])):
            # output += f"\\textbf{'{'}{file.loc[i]['ig_avg_time']:.3f}{'}'} \\\\\n"
            if(file_name == "result_25.csv"):
                qtd_time_ig[0]+=1
            elif (file_name == "result_50.csv"):
                qtd_time_ig[1]+=1
            else:
                qtd_time_ig[2]+=1

            qtd_time_ig[3]+=1


        file_output.write(output)

    output = f"\\toprule \\textbf{'{'}Média{'}'} & \\textbf{'{'}{file['g_best_cost'].mean():.3f}{'}'} & & \\textbf{'{'}{file['g_avg_time'].mean():.3f}{'}'} & \\textbf{'{'}{file['sa_avg_cost'].mean():.3f}{'}'} & \\textbf{'{'}{file['sa_best_cost'].mean():.3f}{'}'} & & \\textbf{'{'}{file['sa_avg_time'].mean():.3f}{'}'} & \\textbf{'{'}{file['ig_avg_cost'].mean():.3f}{'}'} & \\textbf{'{'}{file['ig_best_cost'].mean():.3f}{'}'} & & \\textbf{'{'}{file['ig_avg_time'].mean():.3f}{'}'} & \\textbf{'{'}{file['gap'].mean():.3f}{'}'}\% \\\\\n \\toprule"
    
    if(file_name == "result_25.csv"):
        avg_gaps[0]=f"{file['gap'].mean():.3f}"
        avg_times_ig[0]=f"{file['ig_avg_time'].mean():.3f}"
        avg_times_g[0]=f"{file['g_avg_time'].mean():.3f}"
        avg_times_sa[0]=f"{file['sa_avg_time'].mean():.3f}"
    elif (file_name == "result_50.csv"):
        avg_gaps[1]=f"{file['gap'].mean():.3f}"
        avg_times_ig[1]=f"{file['ig_avg_time'].mean():.3f}"
        avg_times_g[1]=f"{file['g_avg_time'].mean():.3f}"
        avg_times_sa[1]=f"{file['sa_avg_time'].mean():.3f}"
    else:
        avg_gaps[2]=f"{file['gap'].mean():.3f}"
        avg_times_ig[2]=f"{file['ig_avg_time'].mean():.3f}"
        avg_times_g[2]=f"{file['g_avg_time'].mean():.3f}"
        avg_times_sa[3]=f"{file['sa_avg_time'].mean():.3f}"
        
    file_output.write(output)
    file_output.close()



everyone = pd.read_csv("everyone")

avg_gaps[3]=f"{everyone['gap'].mean():.3f}"
avg_times_g[3]=f"{everyone['g_avg_time'].mean():.3f}"
avg_times_sa[3]=f"{everyone['sa_avg_time'].mean():.3f}"
avg_times_ig[3]=f"{everyone['ig_avg_time'].mean():.3f}"

output = f"Avg. $\gap & {avg_gaps[0]} & {avg_gaps[1]} & {avg_gaps[2]} & \\textbf{'{'}{avg_gaps[3]}{'}'}\\\\\n"
miscelaneous.write(output)

output = f"$\#gap\leq 0$ & {qtd_gap[0]} & {qtd_gap[1]} & {qtd_gap[2]} & \\textbf{'{'}{qtd_gap[3]}{'}'}\\\\\n"
miscelaneous.write(output)

output = f"$\#t_{'{'}ig{'}'} \leq t_{'{'}g,sa{'}'}$ (s) & G & {qtd_time_ig[0]} & {qtd_time_ig[1]} & {qtd_time_ig[2]} & \\textbf{'{'}{qtd_time_ig[3]}{'}'}\\\\\n"
miscelaneous.write(output)

output = f"tempo medio g (s) & G & {avg_times_g[0]} & {avg_times_g[1]} & {avg_times_g[2]} & \\textbf{'{'}{avg_times_g[3]}{'}'}\\\\\n"
miscelaneous.write(output)

output = f"tempo medio sa (s) & G & {avg_times_sa[0]} & {avg_times_sa[1]} & {avg_times_sa[2]} & \\textbf{'{'}{avg_times_sa[3]}{'}'}\\\\\n"
miscelaneous.write(output)

output = f"tempo medio ig (s) & G & {avg_times_ig[0]} & {avg_times_ig[1]} & {avg_times_ig[2]} & \\textbf{'{'}{avg_times_ig[3]}{'}'}\\\\\n"
miscelaneous.write(output)



miscelaneous.close()