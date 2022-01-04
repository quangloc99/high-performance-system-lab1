import sys
import os
import csv
from collections import defaultdict
import matplotlib.pyplot as plt

plt.rcParams["figure.figsize"] = (15,10)

render_result_dir = "./rendered-results"
if not os.path.exists(render_result_dir):
    os.mkdir(render_result_dir)
    
def render_data_from_csv(filename):
    print(filename)
    category = filename.replace(".csv", "")
    plt.title(category)
    with open(f"./result/{filename}", "r") as f:
        groups = defaultdict(list)
        for row in csv.DictReader(f):
            for name, item in row.items():
                groups[name].append(float(item))
        ns = list(int(i) for i in groups['n'])
        groups.pop('n')
        for name, data in groups.items():
            # ls = 'solid' if 'timsort' in name else (0, (0.5, 0.75)) if 'treesort' in name else 'dashed' 
            # color = ( 
                # 'tab:red' if '1' in name 
                # else 'tab:green' if '2' in name 
                # else 'tab:orange' if '3' in name 
                # else 'tab:blue' if '4' in name 
                # else 'pink' if 'stable_sort' in name 
                # else 'cyan') 
            plt.plot(ns, data, label=name)
            
        plt.legend()
        plt.xlabel("n")
        plt.ylabel("ms")
        plt.savefig(f"{render_result_dir}/{category}.png", dpi=200)
        plt.clf()
        # plt.show() 

# render_data_from_csv("medium-range-random.csv") 

def exclude(name):
    return "one-big-n" in name

def render_all_from(dir):
    for filename in os.listdir(dir):
        if exclude(filename):
            continue
        render_data_from_csv(filename)
        
render_all_from("./result") 
        
        
        
