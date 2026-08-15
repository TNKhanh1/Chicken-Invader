import json

def get_stats(visual_id, role="NORMAL"):
    if role == "TANK":
        return {"hp": 1200, "damage": 40, "armor": 10, "speed": 80, "egg_rate": 2.0, "score": 100}
    if role == "ASTEROID":
        return {"hp": 500, "damage": 50, "armor": 0, "speed": 400, "egg_rate": 0.0, "score": 5}
    
    if visual_id == 6:
        # Gà 06 bắn đạn 3 tia đỏ
        return {"hp": 300, "damage": 35, "armor": 5, "speed": 110, "egg_rate": 2.5, "score": 40}
    elif visual_id == 5:
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

new_waves = [
    {
      "wave_id": 5,
      "batches": [
        {
          "batch_id": 1,
          "visual_id": 4,
          "role": "NORMAL",
          "count": 10,
          "layout": {
            "type": "INTERSECTING_V",
            "spacing_x": 100.0,
            "spacing_y": 80.0,
            "target_base_y": 150.0
          },
          "movement": {
            "type": "WAYPOINT"
          },
          "stats": get_stats(4)
        },
        {
          "batch_id": 2,
          "visual_id": 5,
          "role": "NORMAL",
          "count": 16,
          "layout": {
            "type": "SWEEP_TO_GRID",
            "rows": 2,
            "cols_per_side": 4,
            "spacing_x": 100.0,
            "spacing_y": 80.0,
            "target_base_y": 100.0
          },
          "movement": {
            "type": "WAYPOINT"
          },
          "stats": get_stats(5)
        }
      ]
    },
    {
      "wave_id": 6,
      "batches": [
        {
          "batch_id": 1,
          "visual_id": 3,
          "role": "TANK",
          "count": 8,
          "layout": {
            "type": "GRID",
            "rows": 1,
            "cols": 8,
            "spacing_x": 120.0,
            "spacing_y": 0.0,
            "start_y": -100.0,
            "target_base_y": 100.0
          },
          "movement": {
            "type": "HORIZONTAL_BOUNCE",
            "drift": 100.0
          },
          "stats": get_stats(3, "TANK")
        },
        {
          "batch_id": 1,
          "visual_id": 2,
          "role": "NORMAL",
          "count": 8,
          "layout": {
            "type": "GRID",
            "rows": 1,
            "cols": 8,
            "spacing_x": 120.0,
            "spacing_y": 0.0,
            "start_y": -200.0,
            "target_base_y": 180.0
          },
          "movement": {
            "type": "HORIZONTAL_BOUNCE",
            "drift": 100.0
          },
          "stats": get_stats(2)
        },
        {
          "batch_id": 2,
          "visual_id": 5,
          "role": "NORMAL",
          "count": 12,
          "layout": {
            "type": "RING",
            "radius": 250.0,
            "center_y": 300.0,
            "start_y": -300.0
          },
          "movement": {
            "type": "WAYPOINT"
          },
          "stats": get_stats(5)
        }
      ]
    },
    {
      "wave_id": 7,
      "batches": [
        {
          "batch_id": 1,
          "visual_id": 6,
          "role": "NORMAL",
          "count": 5,
          "layout": {
            "type": "V_SHAPE",
            "layers": 1,
            "layer_spacing": 0.0,
            "spacing_x": 150.0,
            "spacing_y": 80.0,
            "start_y": -200.0,
            "target_base_y": 100.0
          },
          "movement": {
            "type": "HORIZONTAL_BOUNCE",
            "drift": 150.0
          },
          "stats": get_stats(6)
        },
        {
          "batch_id": 2,
          "visual_id": 1,
          "role": "NORMAL",
          "count": 15,
          "layout": {
            "type": "RANDOM_RAIN",
            "duration": 5.0,
            "start_y": -100.0
          },
          "movement": {
            "type": "STRAIGHT",
            "wrap_around": False
          },
          "stats": get_stats(1)
        }
      ]
    },
    {
      "wave_id": 8,
      "batches": [
        {
          "batch_id": 1,
          "visual_id": 5,
          "role": "NORMAL",
          "count": 4,
          "layout": {
            "type": "GRID",
            "rows": 1,
            "cols": 4,
            "spacing_x": 200.0,
            "spacing_y": 0.0,
            "start_y": -100.0,
            "target_base_y": 120.0
          },
          "movement": {
            "type": "HORIZONTAL_BOUNCE",
            "drift": 80.0
          },
          "stats": get_stats(5)
        },
        {
          "batch_id": 1,
          "visual_id": 6,
          "role": "NORMAL",
          "count": 4,
          "layout": {
            "type": "GRID",
            "rows": 1,
            "cols": 4,
            "spacing_x": 200.0,
            "spacing_y": 0.0,
            "start_y": -100.0,
            "target_base_y": 120.0
          },
          "movement": {
            "type": "HORIZONTAL_BOUNCE",
            "drift": 80.0
          },
          "stats": get_stats(6)
        },
        {
          "batch_id": 2,
          "visual_id": 1,
          "role": "ASTEROID",
          "count": 15,
          "layout": {
            "type": "TARGETED_PLAYER",
            "start_y": -100.0,
            "asteroid_variant": 1,
            "spawn_delay": 0.3
          },
          "movement": {
            "type": "METEOR_DIVE"
          },
          "stats": get_stats(1, "ASTEROID")
        }
      ]
    },
    {
      "wave_id": 9,
      "batches": [
        {
          "batch_id": 1,
          "visual_id": 6,
          "role": "NORMAL",
          "count": 16,
          "layout": {
            "type": "SWEEP_TO_GRID",
            "rows": 2,
            "cols_per_side": 4,
            "spacing_x": 120.0,
            "spacing_y": 100.0,
            "target_base_y": 100.0
          },
          "movement": {
            "type": "WAYPOINT"
          },
          "stats": get_stats(6)
        },
        {
          "batch_id": 2,
          "visual_id": 3,
          "role": "TANK",
          "count": 10,
          "layout": {
            "type": "RING",
            "radius": 200.0,
            "center_y": 250.0,
            "start_y": -300.0
          },
          "movement": {
            "type": "WAYPOINT"
          },
          "stats": get_stats(3, "TANK")
        }
      ]
    }
]

with open('data/stage5.json', 'r') as f:
    stage5 = json.load(f)

# Remove any existing waves 5-9 to avoid duplication during development
stage5['waves'] = [w for w in stage5['waves'] if w['wave_id'] < 5]

# Add new waves
stage5['waves'].extend(new_waves)

# To interleave Gà 05 and Gà 06 in Wave 8 Batch 1 properly:
# Since we have two grids, they will just stack if we use the same target_base_y and spacing.
# To interleave them:
# Grid 1: cols=4, spacing_x=240, start_x offset? The FormationBuilder automatically centers grids.
# So a grid of 4 with spacing 240 has width 3*240 = 720. 
# A second grid of 4 with spacing 240 also has width 720 and will completely overlap!
# We can't interleave grids easily unless we do some manual math or rely on spacing.
# I'll just keep them on different rows.
for b in stage5['waves'][-2]['batches']: # Wave 8
    if b['batch_id'] == 1 and b['visual_id'] == 6:
        b['layout']['target_base_y'] = 200.0 # Move Gà 06 down to row 2

with open('data/stage5.json', 'w') as f:
    json.dump(stage5, f, indent=2)

print("Appended Waves 5-9 to data/stage5.json successfully.")
