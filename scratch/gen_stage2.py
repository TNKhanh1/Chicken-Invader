import json

stage = {
    "stage_id": 2,
    "waves": []
}

# Wave 1: Grid (HORIZONTAL_BOUNCE)
w1 = {
    "wave_id": 1,
    "batches": [
        {
            "batch_id": 1,
            "visual_id": 2,
            "role": 1, # NORMAL
            "stats": {
                "hp": 2,
                "score": 100
            },
            "layout": {
                "type": "GRID",
                "rows": 4,
                "cols": 5,
                "spacing_x": 120.0,
                "spacing_y": 100.0,
                "start_y": 100.0
            },
            "movement": {
                "type": "HORIZONTAL_BOUNCE",
                "speed_x": 100.0,
                "bounds_margin": 100.0
            },
            "egg_drop_chance": 0.05
        }
    ]
}

# Wave 2: Sweep to Grid (SWEEP_TO_GRID)
w2 = {
    "wave_id": 2,
    "batches": [
        {
            "batch_id": 1,
            "visual_id": 2,
            "role": 1,
            "stats": {
                "hp": 3, # slightly higher health
                "score": 150
            },
            "layout": {
                "type": "SWEEP_TO_GRID",
                "rows": 4,
                "cols_per_side": 2,
                "spacing_x": 150.0,
                "spacing_y": 120.0,
                "start_y": 150.0
            },
            "movement": {
                "type": "SWEEP_TO_GRID",
                "speed_x": 300.0,
                "speed_y": 200.0
            },
            "egg_drop_chance": 0.05
        }
    ]
}

# Wave 3: Meteor Rain (RANDOM_RAIN) using asteroid_variant 2
w3 = {
    "wave_id": 3,
    "batches": [
        {
            "batch_id": 1,
            "visual_id": 1, # Asteroid uses its own texture, visual_id 1 is fine or ignored
            "role": 2, # ASTEROID
            "stats": {
                "hp": 100,
                "score": 200
            },
            "layout": {
                "type": "RANDOM_RAIN",
                "count": 40,
                "asteroid_variant": 2 # FLAME ASTEROID
            },
            "movement": {
                "type": "STRAIGHT",
                "speed_y": 350.0,
                "direction": {"x": 0.0, "y": 1.0}
            },
            "egg_drop_chance": 0.0
        }
    ]
}

# Wave 4: 3-layer V-Shape
w4 = {
    "wave_id": 4,
    "batches": [
        {
            "batch_id": 1,
            "visual_id": 2,
            "role": 1,
            "stats": {
                "hp": 4,
                "score": 200
            },
            "layout": {
                "type": "V_SHAPE",
                "layers": 3,
                "spacing_x": 120.0,
                "spacing_y": 90.0,
                "layer_spacing": 100.0,
                "start_y": -300.0
            },
            "movement": {
                "type": "VERTICAL_ZIGZAG", # Let's use vertical zigzag to drop down and hover
                "speed_y": 150.0,
                "drift_y": 0.0, # Will hover once it reaches target
                "amplitude": 30.0,
                "frequency": 2.0
            },
            "egg_drop_chance": 0.1 # Aggressive egg dropping
        }
    ]
}

stage["waves"] = [w1, w2, w3, w4]

with open('data/stage2.json', 'w') as f:
    json.dump(stage, f, indent=4)
