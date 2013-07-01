#!/usr/bin/env python3.2
"""
Graphic Network Monitoring tool
"""

__author__ = """Alejandro Alcalde (algui91@gmail.com)"""

try:
        import matplotlib.pyplot as plt
except:
        raise

import networkx as nx
from networkx.algorithms import bipartite
import gnm

def pretty(d, indent=0):
        tam = len(d)
        print tam
        #G=nx.cycle_graph(tam)
        #pos=nx.spring_layout(G,iterations=200)
        #labels={}
        #edges=[]
        #for i in range(len(d)):
        #    labels[i] = '*' if d[i]['Dir.Remota.'] == '0.0.0.0' else d[i]['Dir.Remota.']
        #    edges.append((i,)
        #nx.draw(G,pos,node_color=range(tam),node_size=8000,cmap=plt.cm.Blues, labels=labels)
        #plt.savefig("node_colormap.png") # save as png
        #plt.show() # display
        #nx.draw_networkx_labels(G,labels,font_size=16)
        #plt.axis('off')
        #plt.savefig("grafo.png") # save as png
        #plt.show() # display
        #for i in range(len(d)):
        #        print(d[i])
        print d

pretty(gnm.helloC())
