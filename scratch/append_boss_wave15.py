import json
import os

file_path = "data/stage5.json"

with open(file_path, 'r', encoding='utf-8') as f:
    data = json.load(f)

# Remove existing wave 15 if any
data['waves'] = [w for w in data['waves'] if w['wave_id'] != 15]

new_wave = {
    "wave_id": 15,
    "batches": [
        {
            "batch_id": 1,
            "visual_id": 12,
            "role": "BOSS",
            "count": 1,
            "layout": {
                "type": "TARGETED_PLAYER",
                "start_y": -150.0
            },
            "movement": {
                "type": "NONE"
            },
            "stats": {
                "hp": 2000,
                "damage": 50,
                "armor": 30,
                "speed": 100,
                "egg_rate": 2.0,
                "score": 1500,
                "scale": 1.0
            }
        }
    ]
}

data['waves'].append(new_wave)

with open(file_path, 'w', encoding='utf-8') as f:
    json.dump(data, f, indent=2, ensure_ascii=False)

print(f"Successfully appended Wave 15 with VoidChickenBoss to {file_path}")
