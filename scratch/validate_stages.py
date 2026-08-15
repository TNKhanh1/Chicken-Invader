import json
import glob
import os

def check_stages():
    stages = ['data/stage1.json', 'data/stage2.json', 'data/stage3.json', 'data/stage4.json']
    
    valid_roles = ["NORMAL", "SWARM", "ASTEROID", "MINIBOSS", "BOSS", "TANK"]
    valid_layouts = ["GRID", "V_SHAPE", "CIRCLE", "RANDOM_RAIN", "TARGETED_PLAYER", "SWEEP_TO_GRID", "INTERSECTING_V"]
    valid_movements = ["STATIONARY", "HORIZONTAL_BOUNCE", "WAYPOINT", "METEOR_DIVE", "SINE_ZIGZAG", "VERTICAL_ZIGZAG", "SPIRAL", "HORIZONTAL_SWEEP", "STRAIGHT"]

    for path in stages:
        if not os.path.exists(path):
            print(f"Missing {path}")
            continue
            
        with open(path, 'r') as f:
            try:
                data = json.load(f)
            except Exception as e:
                print(f"[CRITICAL] Failed to parse JSON in {path}: {e}")
                continue
        
        print(f"=== Checking {path} (Stage {data.get('stage_id')}) ===")
        
        waves = data.get("waves", [])
        if not waves:
            print(f"[WARNING] No waves found in {path}")
            
        for wave in waves:
            w_id = wave.get("wave_id")
            batches = wave.get("batches", [])
            for batch in batches:
                b_id = batch.get("batch_id")
                prefix = f"Wave {w_id}, Batch {b_id}:"
                
                # Check Visual ID
                vid = batch.get("visual_id", 0)
                if vid < 1 or vid > 13:
                    print(f"{prefix} [BUG] Invalid visual_id {vid} (should be 1-13)")
                
                # Check Role
                role = batch.get("role", "")
                if role not in valid_roles:
                    print(f"{prefix} [BUG] Invalid role '{role}'")
                    
                # Check count
                count = batch.get("count", 0)
                if count <= 0:
                    print(f"{prefix} [BUG] Count is {count}")
                    
                # Check layout
                layout = batch.get("layout", {})
                l_type = layout.get("type", "")
                if l_type not in valid_layouts:
                    print(f"{prefix} [BUG] Invalid layout type '{l_type}'")
                    
                # Layout specific checks
                if l_type == "GRID":
                    rows = layout.get("rows", 0)
                    cols = layout.get("cols", 0)
                    if rows * cols < count:
                        print(f"{prefix} [BUG] GRID layout rows*cols ({rows*cols}) < count ({count})")
                
                # Check movement
                mov = batch.get("movement", {})
                m_type = mov.get("type", "")
                if m_type not in valid_movements:
                    print(f"{prefix} [BUG] Invalid movement type '{m_type}'")
                    
                # Missing Waypoints logic (WaveManager might crash if WAYPOINT movement but no layout supports it properly or if WaypointMovement expects Waypoints vector but it's not set correctly. Note: FormationBuilder handles waypoints, but we need to ensure it generates them).

                # Boss specific checks
                if role == "BOSS":
                    if count != 1:
                        print(f"{prefix} [BUG] Boss count is {count} (should be 1)")

if __name__ == '__main__':
    check_stages()
