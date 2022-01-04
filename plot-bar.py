import sys
import os
import csv
from collections import defaultdict
import matplotlib.pyplot as plt

plt.rcParams["figure.figsize"] = (15,10)

render_result_dir = "./rendered-results"
if not os.path.exists(render_result_dir):
    os.mkdir(render_result_dir)
    
plt.ylim(0, 2500)
xs = list(range(1, 5))

def render_data_from_csv(filename):
    print(filename)
    category = filename.replace(".csv", "")
    with open(f"./result/{filename}", "r") as f:
        groups = defaultdict(list)
        for row in csv.DictReader(f):
            for name, item in row.items():
                groups[name].append(float(item))
        ns = list(int(i) for i in groups['n'])
        groups.pop('n')
        
        ys = list(sum(vals) / len(vals) for k, vals in groups.items())
        print(xs)
        print(ys)
        plt.bar(xs, ys, label=category)
        plt.xticks(xs, groups.keys())

render_data_from_csv("./num-threads-1-one-big-n-random.csv") 
render_data_from_csv("./num-threads-2-one-big-n-random.csv") 
render_data_from_csv("./num-threads-3-one-big-n-random.csv") 
render_data_from_csv("./num-threads-4-one-big-n-random.csv") 
plt.legend()
plt.xlabel("algorithm")
plt.ylabel("t (ms)")
plt.savefig(f"{render_result_dir}/num-thread-changes.png", dpi=200) 
# plt.show() 

# def render_all_from(dir): 
    # for filename in os.listdir(dir): 
        # render_data_from_csv(filename) 
        
# render_all_from("./result")  

