import json
import os

data_dir = r"e:\ChickenInvader_local\ChickenInvader\data"

with open(os.path.join(data_dir, "stage5.json"), "r") as f:
    s5 = json.load(f)

d5 = [
    # W1
    [{"hp":280, "damage":32, "armor":5, "egg_rate":2.5, "score":30},
     {"hp":280, "damage":32, "armor":5, "egg_rate":2.5, "score":30},
     {"hp":160, "damage":22, "armor":0, "egg_rate":3.0, "score":12}],
    # W2
    [{"hp":200, "damage":28, "armor":0, "egg_rate":2.5, "score":22},
     {"hp":280, "damage":32, "armor":5, "egg_rate":2.5, "score":32},
     {"hp":280, "damage":32, "armor":5, "egg_rate":2.5, "score":32}],
    # W3
    [{"hp":600, "damage":55, "armor":0, "egg_rate":0.0, "score":5},
     {"hp":280, "damage":32, "armor":5, "egg_rate":2.5, "score":32}],
    # W4
    [{"hp":260, "damage":28, "armor":0, "egg_rate":2.8, "score":28},
     {"hp":280, "damage":32, "armor":5, "egg_rate":2.5, "score":32},
     {"hp":1500, "damage":45, "armor":12, "egg_rate":2.0, "score":120},
     {"hp":280, "damage":32, "armor":5, "egg_rate":2.5, "score":32}],
    # W5
    [{"hp":260, "damage":28, "armor":0, "egg_rate":2.8, "score":28},
     {"hp":280, "damage":32, "armor":5, "egg_rate":2.5, "score":32}],
    # W6
    [{"hp":1800, "damage":48, "armor":12, "egg_rate":2.0, "score":120},
     {"hp":200, "damage":28, "armor":0, "egg_rate":2.5, "score":22},
     {"hp":300, "damage":34, "armor":5, "egg_rate":2.5, "score":35}],
    # W7
    [{"hp":350, "damage":38, "armor":8, "egg_rate":2.5, "score":45},
     {"hp":170, "damage":24, "armor":0, "egg_rate":3.0, "score":12}],
    # W8
    [{"hp":300, "damage":34, "armor":5, "egg_rate":2.5, "score":35},
     {"hp":360, "damage":40, "armor":8, "egg_rate":2.5, "score":48},
     {"hp":600, "damage":55, "armor":0, "egg_rate":0.0, "score":5}],
    # W9
    [{"hp":360, "damage":40, "armor":8, "egg_rate":2.5, "score":45},
     {"hp":2000, "damage":50, "armor":15, "egg_rate":1.8, "score":150}],
    # W10
    [{"hp":18000, "damage":55, "armor":20, "egg_rate":0.0, "score":3000}],
    # W11
    [{"hp":380, "damage":42, "armor":8, "egg_rate":2.5, "score":45},
     {"hp":100, "damage":30, "armor":0, "egg_rate":8.0, "score":25}],
    # W12
    [{"hp":2200, "damage":55, "armor":18, "egg_rate":1.5, "score":150},
     {"hp":380, "damage":42, "armor":8, "egg_rate":2.5, "score":45}],
    # W13
    [{"hp":400, "damage":45, "armor":10, "egg_rate":2.5, "score":50},
     {"hp":2200, "damage":55, "armor":18, "egg_rate":1.5, "score":150}],
    # W14
    [{"hp":100, "damage":30, "armor":0, "egg_rate":8.0, "score":22},
     {"hp":2200, "damage":55, "armor":18, "egg_rate":1.5, "score":150}],
    # W15
    [{"hp":35000, "damage":70, "armor":35, "egg_rate":0.0, "score":5000}],
]

for w_idx, w_data in enumerate(d5):
    batches = s5["waves"][w_idx]["batches"]
    for b_idx, b_stats in enumerate(w_data):
        batches[b_idx]["stats"].update(b_stats)

with open(os.path.join(data_dir, "stage5.json"), "w") as f:
    json.dump(s5, f, indent=2)


with open(os.path.join(data_dir, "stage6.json"), "r") as f:
    s6 = json.load(f)

d6 = [
    {"hp": 9000, "damage": 50, "armor": 24, "speed": 130, "score": 1000},
    {"hp": 13000, "damage": 48, "armor": 28, "speed": 130, "score": 1000},
    {"hp": 18000, "damage": 68, "armor": 24, "speed": 140, "score": 1500},
    {"hp": 24000, "damage": 72, "armor": 34, "speed": 110, "score": 2000},
    {"hp": 32000, "damage": 88, "armor": 40, "speed": 140, "score": 3000},
]

for w_idx, w_stats in enumerate(d6):
    s6["waves"][w_idx]["batches"][0]["stats"].update(w_stats)

with open(os.path.join(data_dir, "stage6.json"), "w") as f:
    json.dump(s6, f, indent=2)

print("Finished scripts for 5-6")
