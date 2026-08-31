import json
import os

data_dir = r"e:\ChickenInvader_local\ChickenInvader\data"

# Stage 1
with open(os.path.join(data_dir, "stage1.json"), "r") as f:
    s1 = json.load(f)

# Wave 1
s1["waves"][0]["batches"][0]["stats"].update({"hp": 120, "damage": 22, "armor": 0, "egg_rate": 2.5, "score": 10})
s1["waves"][0]["batches"][1]["stats"].update({"hp": 120, "damage": 22, "armor": 0, "egg_rate": 2.5, "score": 10})
s1["waves"][0]["batches"][2]["stats"].update({"hp": 130, "damage": 22, "armor": 0, "egg_rate": 2.5, "score": 12})
# Wave 2
s1["waves"][1]["batches"][0]["stats"].update({"hp": 130, "damage": 25, "armor": 0, "egg_rate": 2.5, "score": 12})
s1["waves"][1]["batches"][1]["stats"].update({"hp": 65, "damage": 12, "armor": 0, "egg_rate": 8.0, "score": 15})
# Wave 3
s1["waves"][2]["batches"][0]["stats"].update({"hp": 600, "damage": 55, "armor": 0, "egg_rate": 0.0, "score": 5})
s1["waves"][2]["batches"][1]["stats"].update({"hp": 600, "damage": 55, "armor": 0, "egg_rate": 0.0, "score": 5})
# Wave 4
s1["waves"][3]["batches"][0]["stats"].update({"hp": 150, "damage": 25, "armor": 0, "egg_rate": 2.0, "score": 12})
s1["waves"][3]["batches"][1]["stats"].update({"hp": 1200, "damage": 55, "armor": 12, "egg_rate": 1.2, "score": 120})
for i in range(2, 5):
    s1["waves"][3]["batches"][i]["stats"].update({"hp": 600, "damage": 55, "armor": 0, "egg_rate": 0.0, "score": 5})
# Wave 5
s1["waves"][4]["batches"][0]["stats"].update({"hp": 7000, "damage": 45, "armor": 18, "egg_rate": 0.0, "score": 1000})

with open(os.path.join(data_dir, "stage1.json"), "w") as f:
    json.dump(s1, f, indent=2)


# Stage 2
with open(os.path.join(data_dir, "stage2.json"), "r") as f:
    s2 = json.load(f)

s2["waves"][0]["batches"][0]["stats"].update({"hp": 160, "damage": 22, "armor": 0, "egg_rate": 2.5, "score": 15})
s2["waves"][0]["batches"][1]["stats"].update({"hp": 160, "damage": 22, "armor": 0, "egg_rate": 2.0, "score": 15})
s2["waves"][0]["batches"][2]["stats"].update({"hp": 80, "damage": 14, "armor": 0, "egg_rate": 8.0, "score": 10})

s2["waves"][1]["batches"][0]["stats"].update({"hp": 190, "damage": 26, "armor": 0, "egg_rate": 2.0, "score": 20})
s2["waves"][1]["batches"][1]["stats"].update({"hp": 200, "damage": 26, "armor": 0, "egg_rate": 1.8, "score": 20})
s2["waves"][1]["batches"][2]["stats"].update({"hp": 180, "damage": 26, "armor": 0, "egg_rate": 2.0, "score": 20})

s2["waves"][2]["batches"][0]["stats"].update({"hp": 600, "damage": 55, "armor": 0, "egg_rate": 0.0, "score": 5})
s2["waves"][2]["batches"][1]["stats"].update({"hp": 600, "damage": 60, "armor": 0, "egg_rate": 0.0, "score": 5})
s2["waves"][2]["batches"][2]["stats"].update({"hp": 210, "damage": 28, "armor": 0, "egg_rate": 2.0, "score": 22})

s2["waves"][3]["batches"][0]["stats"].update({"hp": 260, "damage": 32, "armor": 5, "egg_rate": 1.5, "score": 30})
s2["waves"][3]["batches"][1]["stats"].update({"hp": 1400, "damage": 55, "armor": 12, "egg_rate": 0.8, "score": 140})
for i in range(2, 5):
    s2["waves"][3]["batches"][i]["stats"].update({"hp": 600, "damage": 60, "armor": 0, "egg_rate": 0.0, "score": 5})
s2["waves"][3]["batches"][5]["stats"].update({"hp": 210, "damage": 28, "armor": 0, "egg_rate": 2.0, "score": 25})

s2["waves"][4]["batches"][0]["stats"].update({"hp": 45000, "damage": 55, "armor": 18, "egg_rate": 0.0, "score": 5000})

s2["waves"][5]["batches"][0]["stats"].update({"hp": 200, "damage": 28, "armor": 0, "egg_rate": 2.0, "score": 20})
s2["waves"][5]["batches"][1]["stats"].update({"hp": 200, "damage": 32, "armor": 0, "egg_rate": 1.5, "score": 25})
s2["waves"][5]["batches"][2]["stats"].update({"hp": 180, "damage": 32, "armor": 0, "egg_rate": 1.0, "score": 25})

s2["waves"][6]["batches"][0]["stats"].update({"hp": 210, "damage": 32, "armor": 0, "egg_rate": 2.0, "score": 25})
s2["waves"][6]["batches"][1]["stats"].update({"hp": 190, "damage": 32, "armor": 0, "egg_rate": 2.0, "score": 25})

s2["waves"][7]["batches"][0]["stats"].update({"hp": 260, "damage": 32, "armor": 0, "egg_rate": 2.5, "score": 30})
s2["waves"][7]["batches"][1]["stats"].update({"hp": 220, "damage": 36, "armor": 0, "egg_rate": 2.5, "score": 30})

s2["waves"][8]["batches"][0]["stats"].update({"hp": 110, "damage": 22, "armor": 0, "egg_rate": 1.0, "score": 15})
s2["waves"][8]["batches"][1]["stats"].update({"hp": 230, "damage": 36, "armor": 0, "egg_rate": 3.5, "score": 40})

s2["waves"][9]["batches"][0]["stats"].update({"hp": 35000, "damage": 55, "armor": 18, "egg_rate": 0.0, "score": 5000})

with open(os.path.join(data_dir, "stage2.json"), "w") as f:
    json.dump(s2, f, indent=2)


# Stage 3
with open(os.path.join(data_dir, "stage3.json"), "r") as f:
    s3 = json.load(f)

s3["waves"][0]["batches"][0]["stats"].update({"hp": 80, "damage": 22, "armor": 0, "speed": 100, "egg_rate": 4.0, "score": 12})
s3["waves"][0]["batches"][0]["layout"]["phases"] = [
    { "spawn_count_threshold": 0,   "hp_multiplier": 1.0, "speed_multiplier": 1.0 },
    { "spawn_count_threshold": 50,  "hp_multiplier": 1.5, "speed_multiplier": 1.15 },
    { "spawn_count_threshold": 100, "hp_multiplier": 2.2, "speed_multiplier": 1.3 },
    { "spawn_count_threshold": 150, "hp_multiplier": 3.2, "speed_multiplier": 1.5 }
]

with open(os.path.join(data_dir, "stage3.json"), "w") as f:
    json.dump(s3, f, indent=2)


# Stage 4
with open(os.path.join(data_dir, "stage4.json"), "r") as f:
    s4 = json.load(f)

# Rule: +20% HP to NORMAL, +15% to TANK, Boss to 25000
for w in s4["waves"]:
    for b in w["batches"]:
        role = b.get("role", "NORMAL")
        st = b["stats"]
        if role == "BOSS":
            st["hp"] = 25000
            st["damage"] = 60
            st["armor"] = 30
        elif role == "ASTEROID":
            st["damage"] += 5
        elif role == "TANK":
            st["hp"] = int(st["hp"] * 1.15)
            st["damage"] += 8
        else: # NORMAL/SWARM
            st["hp"] = int(st["hp"] * 1.20)
            st["damage"] += 4

with open(os.path.join(data_dir, "stage4.json"), "w") as f:
    json.dump(s4, f, indent=2)

print("Finished scripts for 1-4")
