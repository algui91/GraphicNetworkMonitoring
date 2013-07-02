#!/usr/bin/env python3.2
"""
Graphic Network Monitoring tool
"""

__author__ = """Alejandro Alcalde (algui91@gmail.com)"""

try:
        import matplotlib.pyplot as plt
except:
        raise

import json

import networkx as nx
from networkx.algorithms import bipartite

import gnm

def pretty(d, indent=0):
        lTam = len(d['Dir Local.'])
        rTam = len(d['Dir Remota.'])

        #G=nx.cycle_graph(lTam + rTam)
        #pos=nx.spring_layout(G,iterations=200)
        G=nx.path_graph(lTam + rTam)
        pos=nx.circular_layout(G)
        labels={}
        edges=[]
        for i in range(lTam):
            labels[i] = d['Dir Remota.'][i]
        #print json.dumps(labels, sort_keys=True, indent=4)
        for i in range(rTam):
            labels[i + lTam] = d['Dir Local.'][i]
        for i in range(rTam):
            edges.append((i,i+rTam))
        #print json.dumps(labels, sort_keys=True, indent=4)
        nx.draw(G,pos,node_color=range(lTam + rTam),node_size=800,cmap=plt.cm.Blues, labels=labels)
        nx.draw_networkx_edges(G,pos,edgelist=edges,width=1,alpha=0.5,edge_color='r')
        #nx.draw_networkx_labels(G,labels,font_size=16)
        plt.axis('off')
        plt.show() # display
        print json.dumps(d, sort_keys=True, indent=4)

pretty(gnm.helloC())
