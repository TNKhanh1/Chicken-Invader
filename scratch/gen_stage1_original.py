import json
import os

def gen_original_stage1():
    stage_data = {
        "stage_id": 1,
        "name": "Stage 1",
        "waves": [
            {
                "wave_id": 1,
                "batches": [
                    {
                        "batch_id": 1,
                        "visual_id": 1,
                        "role": "NORMAL",
                        "count": 10,
                        "layout": {
                            "type": "HORIZONTAL_SWEEP",
                            "dir": -1,
                            "spacing_x": 150.0,
                            "spacing_y": 100.0,
                            "rows": 2,
                            "start_y": 150.0
                        },
                        "movement": {
                            "type": "HORIZONTAL_SWEEP",
                            "dir": -1.0
                        },
                        "stats": { "hp": 100, "damage": 20, "armor": 0, "speed": 100, "egg_rate": 3.0, "score": 10 }
                    },
                    {
                        "batch_id": 2,
                        "visual_id": 1,
                        "role": "NORMAL",
                        "count": 15,
                        "layout": {
                            "type": "HORIZONTAL_SWEEP",
                            "dir": 1,
                            "spacing_x": 150.0,
                            "spacing_y": 100.0,
                            "rows": 3,
                            "start_y": 150.0
                        },
                        "movement": {
                            "type": "HORIZONTAL_SWEEP",
                            "dir": 1.0
                        },
                        "stats": { "hp": 100, "damage": 20, "armor": 0, "speed": 100, "egg_rate": 3.0, "score": 10 }
                    },
                    {
                        "batch_id": 3,
                        "visual_id": 1,
                        "role": "NORMAL",
                        "count": 15,
                        "layout": {
                            "type": "GRID",
                            "rows": 3,
                            "cols": 5,
                            "spacing_x": 150.0,
                            "spacing_y": 100.0,
                            "start_y": -300.0,
                            "target_base_y": 100.0
                        },
                        "movement": {
                            "type": "HORIZONTAL_BOUNCE",
                            "drift": 300.0
                        },
                        "stats": { "hp": 100, "damage": 20, "armor": 0, "speed": 100, "egg_rate": 3.0, "score": 10 }
                    }
                ]
            },
            {
                "wave_id": 2,
                "batches": [
                    {
                        "batch_id": 1,
                        "visual_id": 1,
                        "role": "NORMAL",
                        "count": 15,
                        "layout": {
                            "type": "GRID_ZIGZAG",
                            "cols": 5,
                            "spacing_x": 250.0,
                            "spacing_y": 150.0,
                            "start_x": 150.0,
                            "start_y": -100.0
                        },
                        "movement": {
                            "type": "VERTICAL_ZIGZAG"
                        },
                        "stats": { "hp": 100, "damage": 20, "armor": 0, "speed": 100, "egg_rate": 3.0, "score": 10 }
                    },
                    {
                        "batch_id": 2,
                        "visual_id": 2,
                        "role": "SWARM",
                        "count": 10,
                        "layout": {
                            "type": "DIAGONAL",
                            "spacing_x": 120.0,
                            "spacing_y": 50.0,
                            "start_x": 200.0,
                            "start_y": -100.0
                        },
                        "movement": {
                            "type": "SINE_ZIGZAG"
                        },
                        "stats": { "hp": 50, "damage": 10, "armor": 0, "speed": 120, "egg_rate": 10.0, "score": 15 }
                    },
                    {
                        "batch_id": 3,
                        "visual_id": 1,
                        "role": "MIXED",
                        "count": 15,
                        "layout": {
                            "type": "GRID",
                            "rows": 3,
                            "cols": 5,
                            "spacing_x": 200.0,
                            "spacing_y": 150.0,
                            "start_y": -200.0,
                            "target_base_y": -200.0
                        },
                        "movement": {
                            "type": "VERTICAL_ZIGZAG"
                        },
                        "stats": { "hp": 100, "damage": 20, "armor": 0, "speed": 100, "egg_rate": 3.0, "score": 10 }
                    }
                ]
            },
            {
                "wave_id": 3,
                "batches": [
                    {
                        "batch_id": 1,
                        "visual_id": 1,
                        "role": "ASTEROID",
                        "count": 20,
                        "layout": {
                            "type": "RANDOM_RAIN",
                            "duration": 10.0,
                            "start_y": -100.0,
                            "asteroid_variant": 1
                        },
                        "movement": {
                            "type": "METEOR_DIVE"
                        },
                        "stats": { "hp": 500, "damage": 50, "armor": 0, "speed": 300, "egg_rate": 0.0, "score": 5 }
                    },
                    {
                        "batch_id": 2,
                        "visual_id": 5,
                        "role": "TANK",
                        "count": 5,
                        "layout": {
                            "type": "LINEAR",
                            "start_x": 200.0,
                            "spacing_x": 300.0,
                            "start_y": -100.0
                        },
                        "movement": {
                            "type": "STRAIGHT"
                        },
                        "stats": { "hp": 1000, "damage": 50, "armor": 10, "speed": 80, "egg_rate": 1.5, "score": 100 }
                    },
                    {
                        "batch_id": 3,
                        "visual_id": 1,
                        "role": "ASTEROID_FLAME_TANK_MIX",
                        "count": 13,
                        "layout": {
                            "type": "WAVE_3_3_SPECIAL"
                        },
                        "movement": {
                            "type": "SPECIAL"
                        },
                        "stats": { "hp": 500, "damage": 50, "armor": 0, "speed": 300, "egg_rate": 0.0, "score": 5 }
                    }
                ]
            },
            {
                "wave_id": 4,
                "batches": [
                    {
                        "batch_id": 1,
                        "visual_id": 1,
                        "role": "MIXED",
                        "count": 20,
                        "layout": {
                            "type": "ALTERNATING_SWEEP"
                        },
                        "movement": {
                            "type": "ALTERNATING_SWEEP"
                        },
                        "stats": { "hp": 100, "damage": 20, "armor": 0, "speed": 100, "egg_rate": 3.0, "score": 10 }
                    },
                    {
                        "batch_id": 2,
                        "visual_id": 5,
                        "role": "TANK",
                        "count": 8,
                        "layout": {
                            "type": "LINEAR",
                            "start_x": 200.0,
                            "spacing_x": 170.0,
                            "start_y": -100.0
                        },
                        "movement": {
                            "type": "HORIZONTAL_BOUNCE",
                            "drift": 100.0
                        },
                        "stats": { "hp": 1000, "damage": 50, "armor": 10, "speed": 80, "egg_rate": 1.5, "score": 100 }
                    },
                    {
                        "batch_id": 3,
                        "visual_id": 1,
                        "role": "MIXED",
                        "count": 20,
                        "layout": {
                            "type": "RANDOM_X_RAIN",
                            "start_y": -100.0,
                            "spacing_y": 100.0
                        },
                        "movement": {
                            "type": "MIXED_RAIN"
                        },
                        "stats": { "hp": 100, "damage": 20, "armor": 0, "speed": 100, "egg_rate": 3.0, "score": 10 }
                    }
                ]
            }
        ]
    }

    base_dir = r"e:\ChickenInvader_local\ChickenInvader"
    json_path = os.path.join(base_dir, "data", "stage1.json")
    with open(json_path, "w", encoding="utf-8") as f:
        json.dump(stage_data, f, indent=4)
        
    print("Regenerated stage1.json to match original exactly.")

if __name__ == "__main__":
    gen_original_stage1()
