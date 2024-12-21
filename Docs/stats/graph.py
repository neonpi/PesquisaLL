import plotly.graph_objects as go
import networkx as nx
import data as dt

def build_graph(traces):
    return go.Figure(data = traces,
                layout=go.Layout(
                    showlegend=False,
                    hovermode='closest',
                    margin=dict(b=20,l=5,r=5,t=40),
                    xaxis=dict(showgrid=False, zeroline=False, showticklabels=False),
                    yaxis=dict(showgrid=False, zeroline=False, showticklabels=False)
                ))

def build_all_edges(G, show):
    edge_xs =[]
    edge_ys = []
    for edge in G.edges():
        x0 ,y0 = G.nodes[edge[0]]['pos']
        x1 ,y1 = G.nodes[edge[1]]['pos']
        edge_xs.append(x0)
        edge_xs.append(x1)
        edge_xs.append(None)
        edge_ys.append(y0)
        edge_ys.append(y1)
        edge_ys.append(None)

    trace = go.Scatter(
        x = edge_xs,
        y = edge_ys,
        visible= show,
        zorder=1,
        line = dict(
            width=1.5,
            color='#888',
        ),
        mode='lines',
        hoverinfo='text',
        )
    

    return trace
    

def build_locker_edges(G,instance, show):
    locker_edge_xs =[]
    locker_edge_ys = []

    
    for node_i in instance.nodes:
        if(node_i.assigned_locker!=''):
            G.add_edge(node_i.id, node_i.assigned_locker.id)
            x0 = node_i.x
            y0 = node_i.y
            x1 = node_i.assigned_locker.x
            y1 = node_i.assigned_locker.y
            locker_edge_xs.append(x0)
            locker_edge_xs.append(x1)
            locker_edge_xs.append(None)
            locker_edge_ys.append(y0)
            locker_edge_ys.append(y1)
            locker_edge_ys.append(None)
    return go.Scatter(
            x = locker_edge_xs,
            y = locker_edge_ys,
            visible= show,
            zorder=2,
            line = dict(
                width=5.5,
                color='#888',
            ),
            mode='lines'
        )
def build_path_edges(G,instance,show,debug):
    colors = [
    "#F2C2B7", "#D9E4F5", "#E8D5B5", "#A6C4CF", "#F2D7D5",
    "#B7C9E2", "#E1F5C4", "#DAF7A6", "#C4D79B", "#F5C2E1",
    "#B5EAD5", "#C4A6CF", "#F7DAF5", "#E2B7C9", "#D5F2E8",
    "#CFB5A6", "#A6C4B7", "#9BF7DA", "#F5E2C4", "#D5F2A6"
    ]
    color_i=0
    paths_array = dt.build_paths(instance.name,debug)
    path_graphs = []
    for path_array in paths_array:

        path_edge_xs =[]
        path_edge_ys = []

        for i in range(len(path_array)):
            if(i < (len(path_array)-1)):
                node_i = list(filter(lambda x: x.id == path_array[i], instance.nodes))[0]
                node_j = list(filter(lambda x: x.id == path_array[i+1], instance.nodes))[0]

                G.add_edge(node_i.id, node_j.id)

                x0 = node_i.x
                y0 = node_i.y
                x1 = node_j.x
                y1 = node_j.y
                path_edge_xs.append(x0)
                path_edge_xs.append(x1)
                path_edge_xs.append(None)
                path_edge_ys.append(y0)
                path_edge_ys.append(y1)
                path_edge_ys.append(None)
        
            path_graphs.append( go.Scatter(
                    x = path_edge_xs,
                    y = path_edge_ys,
                    visible= show,
                    zorder=3,
                    line = dict(
                        width=2.5,
                        color=colors[color_i],
                    ),
                    mode='lines'
                )
            )
        color_i+=1

    return path_graphs


def build_nodes(G,node_size):
    node_xs =[]
    node_ys = []
    for node in G.nodes():
        x,y = G.nodes[node]['pos']
        node_xs.append(x)
        node_ys.append(y)
        # st.sidebar.write(G.nodes[node]['node_data'].demand)
    return go.Scatter(
            x = node_xs,
            y = node_ys,
            mode='markers+text',
            hoverinfo='text',
            zorder=4,
            # text= [G.nodes[node]['node_data'].id if (G.nodes[node]['node_data'].id !='Dt' and G.nodes[node]['node_data'].id !='S0') else "" for node in G.nodes()  ],
            text= build_node_text(G),
            textposition='top center',
            textfont= dict(color='#A7C7E7',
                        weight='bold',
                        size=max(10,node_size*0.3)),
            marker = dict(color=[choose_node_color(G.nodes[node]['node_data']) for node in G.nodes()],
                        size=node_size,
                        line_width= 2,
                        line_color='#000',
                        symbol=[choose_node_symbol(G.nodes[node]['node_data']) for node in G.nodes()])
            
        )

def choose_node_symbol(node):
    if node.type == 'd':
        return 'square-x'
    elif node.type == 'p':
        return 'hexagon'
    elif node.type == 'f':
        return 'diamond'
    else:
        return 'circle'

def choose_node_color(node):
    if node.type == 'd' or node.id == "S0":
        return 'silver'
    elif node.type == 'p':
        return '#f4d5c0'
    elif node.type == 'f':
        return '#FFF44F'
    elif node.type == 'c1':
        return '#6495ED'
    elif node.type == 'c2':
        return '#bfffec'
    else:        
        return '#ccff99'

def build_node_text(G):
    nodes = G.nodes()
    texts = []
    for node in nodes:
       if (G.nodes[node]['node_data'].id !='Dt' and G.nodes[node]['node_data'].id !='S0'):
           
           text = nodes[node]['node_data'].id
           
           if (G.nodes[node]['node_data'].type == 'c1'):
               text=text+"(HD)"
           elif (G.nodes[node]['node_data'].type == 'c2'):
               text=text+"(SP)"
           elif (G.nodes[node]['node_data'].type == 'c3'):
               text=text+"(FX)"
           texts.append(text)
       else:
           texts.append("")
    return texts