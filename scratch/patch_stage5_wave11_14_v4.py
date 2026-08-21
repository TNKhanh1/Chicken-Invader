import json

file_path = "data/stage5.json"

with open(file_path, 'r', encoding='utf-8') as f:
    data = json.load(f)

# Remove existing waves 11 to 14
data['waves'] = [w for w in data['waves'] if w['wave_id'] < 11]

new_waves = [
    {
        "wave_id": 11,
        "batches": [
            {
                "batch_id": 1,
                "visual_id": [1, 2],
                "role": "NORMAL",
                "count": 12,
                "layout": {
                    "type": "GRID",
                    "rows": 3,
                    "cols": 4,
                    "spacing_x": 150.0,
                    "spacing_y": 80.0,
                    "start_y": -300.0,
                    "target_base_y": 100.0
                },
                "movement": {
                    "type": "HORIZONTAL_BOUNCE",
                    "drift": 200.0
                },
                "stats": {
                    "hp": 100,
                    "damage": 38,
                    "armor": 6,
                    "speed": 110,
                    "egg_rate": 2.5,
                    "score": 40
                }
            },
            {
                "batch_id": 2,
                "visual_id": [3, 4],
                "role": "SWARM",
                "count": 5,
                "layout": {
                    "type": "V_SHAPE",
                    "layers": 2,
                    "layer_spacing": 100.0,
                    "spacing_x": 120.0,
                    "spacing_y": 80.0,
                    "start_y": -300.0,
                    "target_base_y": 350.0
                },
                "movement": {
                    "type": "HORIZONTAL_BOUNCE",
                    "drift": 250.0
                },
                "stats": {
                    "hp": 50,
                    "damage": 25,
                    "armor": 0,
                    "speed": 160,
                    "egg_rate": 3.5,
                    "score": 20
                }
            }
        ]
    },
    {
        "wave_id": 12,
        "batches": [
            {
                "batch_id": 1,
                "visual_id": [5, 6],
                "role": "TANK",
                "count": 10,
                "layout": {
                    "type": "INTERSECTING_V",
                    "spacing_x": 120.0,
                    "spacing_y": 100.0,
                    "target_base_y": 100.0
                },
                "movement": {
                    "type": "WAYPOINT"
                },
                "stats": {
                    "hp": 200,
                    "damage": 45,
                    "armor": 15,
                    "speed": 80,
                    "egg_rate": 2.0,
                    "score": 100
                }
            },
            {
                "batch_id": 2,
                "visual_id": [1, 3],
                "role": "NORMAL",
                "count": 16,
                "layout": {
                    "type": "SWEEP_TO_GRID",
                    "rows": 2,
                    "cols_per_side": 4,
                    "spacing_x": 120.0,
                    "spacing_y": 100.0,
                    "target_base_y": 280.0
                },
                "movement": {
                    "type": "WAYPOINT"
                },
                "stats": {
                    "hp": 100,
                    "damage": 38,
                    "armor": 6,
                    "speed": 110,
                    "egg_rate": 2.5,
                    "score": 40
                }
            }
        ]
    },
    {
        "wave_id": 13,
        "batches": [
            {
                "batch_id": 1,
                "visual_id": [2, 4],
                "role": "NORMAL",
                "count": 12,
                "layout": {
                    "type": "GRID",
                    "rows": 2,
                    "cols": 6,
                    "spacing_x": 120.0,
                    "spacing_y": 80.0,
                    "start_y": -300.0,
                    "target_base_y": 80.0
                },
                "movement": {
                    "type": "HORIZONTAL_BOUNCE",
                    "drift": 150.0
                },
                "stats": {
                    "hp": 100,
                    "damage": 38,
                    "armor": 6,
                    "speed": 110,
                    "egg_rate": 2.5,
                    "score": 40
                }
            },
            {
                "batch_id": 2,
                "visual_id": [5, 6],
                "role": "TANK",
                "count": 3,
                "layout": {
                    "type": "V_SHAPE",
                    "layers": 2,
                    "layer_spacing": 120.0,
                    "spacing_x": 150.0,
                    "spacing_y": 100.0,
                    "start_y": -150.0,
                    "target_base_y": 250.0
                },
                "movement": {
                    "type": "HORIZONTAL_BOUNCE",
                    "drift": 180.0
                },
                "stats": {
                    "hp": 200,
                    "damage": 45,
                    "armor": 15,
                    "speed": 80,
                    "egg_rate": 2.0,
                    "score": 100
                }
            }
        ]
    },
    {
        "wave_id": 14,
        "batches": [
            {
                "batch_id": 1,
                "visual_id": [1, 2, 3],
                "role": "SWARM",
                "count": 12,
                "layout": {
                    "type": "SWEEP_TO_GRID",
                    "rows": 2,
                    "cols_per_side": 3,
                    "spacing_x": 120.0,
                    "spacing_y": 80.0,
                    "target_base_y": 100.0
                },
                "movement": {
                    "type": "WAYPOINT"
                },
                "stats": {
                    "hp": 50,
                    "damage": 25,
                    "armor": 0,
                    "speed": 160,
                    "egg_rate": 3.5,
                    "score": 20
                }
            },
            {
                "batch_id": 2,
                "visual_id": [4, 5, 6],
                "role": "TANK",
                "count": 10,
                "layout": {
                    "type": "INTERSECTING_V",
                    "spacing_x": 120.0,
                    "spacing_y": 100.0,
                    "target_base_y": 280.0
                },
                "movement": {
                    "type": "WAYPOINT"
                },
                "stats": {
                    "hp": 200,
                    "damage": 45,
                    "armor": 15,
                    "speed": 80,
                    "egg_rate": 2.0,
                    "score": 100
                }
            }
        ]
    }
]

data['waves'].extend(new_waves)

with open(file_path, 'w', encoding='utf-8') as f:
    json.dump(data, f, indent=2, ensure_ascii=False)

print(f"Successfully appended Waves 11-14 with mixed array visual_id to {file_path}")
