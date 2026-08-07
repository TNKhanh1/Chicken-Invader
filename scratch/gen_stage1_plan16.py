import json
import os

def gen_stage1_plan16():
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
                            "drift": 0.0
                        },
                        "stats": { "hp": 100, "damage": 20, "armor": 0, "speed": 100, "egg_rate": 3.0, "score": 10 }
                    },
                    {
                        "batch_id": 2,
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
                    },
                    {
                        "batch_id": 3,
                        "visual_id": 1,
                        "role": "NORMAL",
                        "count": 8,
                        "layout": {
                            "type": "SWEEP_TO_GRID",
                            "rows": 2,
                            "cols_per_side": 4,
                            "spacing_x": 200.0,
                            "spacing_y": 100.0,
                            "target_base_y": 100.0
                        },
                        "movement": {
                            "type": "WAYPOINT"
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
                        "count": 11,
                        "layout": {
                            "type": "V_SHAPE",
                            "layers": 2,
                            "layer_spacing": 150.0,
                            "spacing_x": 100.0,
                            "spacing_y": 80.0,
                            "start_y": -100.0,
                            "target_base_y": 150.0
                        },
                        "movement": {
                            "type": "HORIZONTAL_BOUNCE",
                            "drift": 0.0
                        },
                        "stats": { "hp": 100, "damage": 20, "armor": 0, "speed": 100, "egg_rate": 3.0, "score": 10 }
                    },
                    {
                        "batch_id": 2,
                        "visual_id": 2,
                        "role": "SWARM",
                        "count": 10,
                        "layout": {
                            "type": "INTERSECTING_V",
                            "spacing_x": 80.0,
                            "spacing_y": 80.0,
                            "target_base_y": 100.0
                        },
                        "movement": {
                            "type": "WAYPOINT"
                        },
                        "stats": { "hp": 50, "damage": 10, "armor": 0, "speed": 120, "egg_rate": 10.0, "score": 15 }
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
                        "count": 30,
                        "layout": {
                            "type": "RANDOM_RAIN",
                            "duration": 15.0,
                            "start_y": -100.0
                        },
                        "movement": {
                            "type": "METEOR_DIVE"
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
                        "role": "NORMAL",
                        "count": 15,
                        "layout": {
                            "type": "V_SHAPE",
                            "layers": 1,
                            "layer_spacing": 0.0,
                            "spacing_x": 100.0,
                            "spacing_y": 80.0,
                            "start_y": -100.0,
                            "target_base_y": 150.0
                        },
                        "movement": {
                            "type": "HORIZONTAL_BOUNCE",
                            "drift": 0.0
                        },
                        "stats": { "hp": 100, "damage": 20, "armor": 0, "speed": 100, "egg_rate": 3.0, "score": 10 }
                    },
                    {
                        "batch_id": 2,
                        "visual_id": 5,
                        "role": "TANK",
                        "count": 5,
                        "layout": {
                            "type": "GRID",
                            "rows": 1,
                            "cols": 5,
                            "spacing_x": 250.0,
                            "spacing_y": 0.0,
                            "start_y": -100.0,
                            "target_base_y": 150.0
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
                        "role": "ASTEROID",
                        "count": 1,
                        "layout": {
                            "type": "TARGETED_PLAYER",
                            "start_y": -100.0,
                            "asteroid_variant": 2,
                            "spawn_delay": 3.0
                        },
                        "movement": {
                            "type": "METEOR_DIVE"
                        },
                        "stats": { "hp": 500, "damage": 50, "armor": 0, "speed": 350, "egg_rate": 0.0, "score": 5 }
                    },
                    {
                        "batch_id": 4,
                        "visual_id": 1,
                        "role": "ASTEROID",
                        "count": 1,
                        "layout": {
                            "type": "TARGETED_PLAYER",
                            "start_y": -100.0,
                            "asteroid_variant": 2,
                            "spawn_delay": 6.0
                        },
                        "movement": {
                            "type": "METEOR_DIVE"
                        },
                        "stats": { "hp": 500, "damage": 50, "armor": 0, "speed": 350, "egg_rate": 0.0, "score": 5 }
                    },
                    {
                        "batch_id": 5,
                        "visual_id": 1,
                        "role": "ASTEROID",
                        "count": 1,
                        "layout": {
                            "type": "TARGETED_PLAYER",
                            "start_y": -100.0,
                            "asteroid_variant": 2,
                            "spawn_delay": 9.0
                        },
                        "movement": {
                            "type": "METEOR_DIVE"
                        },
                        "stats": { "hp": 500, "damage": 50, "armor": 0, "speed": 350, "egg_rate": 0.0, "score": 5 }
                    }
                ]
            }
        ]
    }

    base_dir = r"e:\ChickenInvader_local\ChickenInvader"
    json_path = os.path.join(base_dir, "data", "stage1.json")
    with open(json_path, "w", encoding="utf-8") as f:
        json.dump(stage_data, f, indent=4)
        
    print("Regenerated stage1.json for plan16.")

if __name__ == "__main__":
    gen_stage1_plan16()
