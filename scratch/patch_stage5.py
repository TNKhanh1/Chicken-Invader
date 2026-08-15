import json

def get_stats(visual_id, role="NORMAL"):
    if role == "TANK":
        return {"hp": 1200, "damage": 40, "armor": 10, "speed": 80, "egg_rate": 2.0, "score": 100}
    if role == "SWARM":
        return {"hp": 100, "damage": 20, "armor": 0, "speed": 150, "egg_rate": 0.0, "score": 15}
    if role == "ASTEROID":
        return {"hp": 500, "damage": 50, "armor": 0, "speed": 400, "egg_rate": 0.0, "score": 5}
    
    # NORMAL
    if visual_id == 5:
        # Gà 05 bắn đạn đỏ
        return {"hp": 250, "damage": 30, "armor": 5, "speed": 120, "egg_rate": 2.5, "score": 30}
    elif visual_id == 4:
        return {"hp": 220, "damage": 25, "armor": 0, "speed": 100, "egg_rate": 2.8, "score": 25}
    elif visual_id == 1:
        return {"hp": 150, "damage": 20, "armor": 0, "speed": 100, "egg_rate": 3.0, "score": 10}
    elif visual_id == 2:
        return {"hp": 180, "damage": 25, "armor": 0, "speed": 130, "egg_rate": 2.5, "score": 20}
    elif visual_id == 3:
        return {"hp": 300, "damage": 30, "armor": 5, "speed": 90, "egg_rate": 2.0, "score": 40}
    
    return {"hp": 200, "damage": 20, "armor": 0, "speed": 100, "egg_rate": 3.0, "score": 10}

stage5 = {
  "stage": 5,
  "background": "assets/backgrounds/bg5.png",
  "music": "assets/music/stage5.mp3",
  "waves": [
    {
      "wave_id": 1,
      "batches": [
        {
          "batch_id": 1,
          "visual_id": 5,
          "role": "NORMAL",
          "count": 8,
          "layout": {
            "type": "V_SHAPE",
            "layers": 1,
            "layer_spacing": 0.0,
            "spacing_x": 100.0,
            "spacing_y": 70.0,
            "start_y": -200.0,
            "target_base_y": 120.0
          },
          "movement": {
            "type": "HORIZONTAL_BOUNCE",
            "drift": 150.0
          },
          "stats": get_stats(5)
        },
        {
          "batch_id": 2,
          "visual_id": 5,
          "role": "NORMAL",
          "count": 8,
          "layout": {
            "type": "GRID",
            "rows": 1,
            "cols": 8,
            "spacing_x": 100.0,
            "spacing_y": 0.0,
            "start_y": -100.0,
            "target_base_y": 100.0
          },
          "movement": {
            "type": "HORIZONTAL_BOUNCE",
            "drift": 50.0
          },
          "stats": get_stats(5)
        },
        {
          "batch_id": 2,
          "visual_id": 1,
          "role": "NORMAL",
          "count": 8,
          "layout": {
            "type": "GRID",
            "rows": 1,
            "cols": 8,
            "spacing_x": 100.0,
            "spacing_y": 0.0,
            "start_y": -150.0,
            "target_base_y": 180.0
          },
          "movement": {
            "type": "HORIZONTAL_BOUNCE",
            "drift": 50.0
          },
          "stats": get_stats(1)
        }
      ]
    },
    {
      "wave_id": 2,
      "batches": [
        {
          "batch_id": 1,
          "visual_id": 2,
          "role": "NORMAL",
          "count": 16,
          "layout": {
            "type": "SWEEP_TO_GRID",
            "rows": 2,
            "cols_per_side": 4,
            "spacing_x": 100.0,
            "spacing_y": 80.0,
            "start_y": -150.0,
            "target_base_y": 100.0
          },
          "movement": {
            "type": "HORIZONTAL_BOUNCE",
            "drift": 80.0
          },
          "stats": get_stats(2)
        },
        {
          "batch_id": 2,
          "visual_id": 5,
          "role": "NORMAL",
          "count": 6,
          "layout": {
            "type": "RING",
            "radius": 150.0,
            "center_y": 200.0,
            "start_y": -300.0
          },
          "movement": {
            "type": "SINE_ZIGZAG",
            "amplitude": 120.0,
            "frequency": 2.0
          },
          "stats": get_stats(5)
        },
        {
          "batch_id": 3,
          "visual_id": 5,
          "role": "NORMAL",
          "count": 8,
          "layout": {
            "type": "GRID",
            "rows": 1,
            "cols": 8,
            "spacing_x": 100.0,
            "spacing_y": 0.0,
            "start_y": -100.0,
            "target_base_y": 150.0
          },
          "movement": {
            "type": "STRAIGHT"
          },
          "stats": get_stats(5)
        },
        {
          "batch_id": 3,
          "visual_id": 2,
          "role": "NORMAL",
          "count": 7,
          "layout": {
            "type": "GRID",
            "rows": 1,
            "cols": 7,
            "spacing_x": 110.0,
            "spacing_y": 0.0,
            "start_y": -200.0,
            "target_base_y": 80.0
          },
          "movement": {
            "type": "STRAIGHT"
          },
          "stats": get_stats(2)
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
            "type": "TARGETED_PLAYER",
            "start_y": -100.0,
            "asteroid_variant": 1,
            "spawn_delay": 0.4
          },
          "movement": {
            "type": "METEOR_DIVE"
          },
          "stats": get_stats(1, "ASTEROID")
        },
        {
          "batch_id": 2,
          "visual_id": 5,
          "role": "NORMAL",
          "count": 10,
          "layout": {
            "type": "GRID",
            "rows": 1,
            "cols": 10,
            "spacing_x": 90.0,
            "spacing_y": 0.0,
            "start_y": -100.0,
            "target_base_y": 120.0
          },
          "movement": {
            "type": "HORIZONTAL_BOUNCE",
            "drift": 30.0
          },
          "stats": get_stats(5)
        }
      ]
    },
    {
      "wave_id": 4,
      "batches": [
        {
          "batch_id": 1,
          "visual_id": 4,
          "role": "NORMAL",
          "count": 10,
          "layout": {
            "type": "RING",
            "radius": 220.0,
            "center_y": 150.0,
            "start_y": -200.0
          },
          "movement": {
            "type": "WAYPOINT"
          },
          "stats": get_stats(4)
        },
        {
          "batch_id": 1,
          "visual_id": 5,
          "role": "NORMAL",
          "count": 5,
          "layout": {
            "type": "V_SHAPE",
            "layers": 1,
            "layer_spacing": 0.0,
            "spacing_x": 120.0,
            "spacing_y": 60.0,
            "start_y": -300.0,
            "target_base_y": 80.0
          },
          "movement": {
            "type": "HORIZONTAL_BOUNCE",
            "drift": 200.0
          },
          "stats": get_stats(5)
        },
        {
          "batch_id": 2,
          "visual_id": 3,
          "role": "TANK",
          "count": 4,
          "layout": {
            "type": "GRID",
            "rows": 1,
            "cols": 4,
            "spacing_x": 200.0,
            "spacing_y": 0.0,
            "start_y": -200.0,
            "target_base_y": 150.0
          },
          "movement": {
            "type": "HORIZONTAL_BOUNCE",
            "drift": 100.0
          },
          "stats": get_stats(3, "TANK")
        },
        {
          "batch_id": 3,
          "visual_id": 5,
          "role": "NORMAL",
          "count": 15,
          "layout": {
            "type": "RANDOM_RAIN",
            "duration": 5.0,
            "start_y": -150.0
          },
          "movement": {
            "type": "WAYPOINT"
          },
          "stats": get_stats(5)
        }
      ]
    }
  ]
}

with open('data/stage5.json', 'w') as f:
    json.dump(stage5, f, indent=2)
print("Generated data/stage5.json successfully.")
