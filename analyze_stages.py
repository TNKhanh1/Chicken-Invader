import json
import os

def analyze_stages():
    findings = []
    
    stage_expected_hp = {
        1: {'NORMAL': (50, 200), 'TANK': (200, 600), 'ASTEROID': (100, 800), 'BOSS': (3000, 10000)},
        2: {'NORMAL': (100, 300), 'TANK': (300, 800), 'ASTEROID': (100, 1000), 'BOSS': (8000, 45000)},
        3: {'NORMAL': (50, 200)}, # waterfall stage
        4: {'NORMAL': (200, 500), 'TANK': (1000, 2500), 'ASTEROID': (500, 2000), 'BOSS': (15000, 35000)},
        5: {'NORMAL': (100, 600), 'TANK': (1000, 3000), 'ASTEROID': (500, 1500), 'BOSS': (18000, 50000)},
        6: {'BOSS': (8000, 50000)}
    }

    for stage_num in range(1, 7):
        filepath = f"data/stage{stage_num}.json"
        if not os.path.exists(filepath):
            continue
            
        with open(filepath, 'r') as f:
            data = json.load(f)
            
        stage_findings = []
        expected = stage_expected_hp.get(stage_num, {})
        
        for w in data.get('waves', []):
            wid = w.get('wave_id')
            for b_idx, b in enumerate(w.get('batches', [])):
                role = b.get('role', 'NORMAL')
                stats = b.get('stats', {})
                hp = stats.get('hp', 0)
                dmg = stats.get('damage', 0)
                count = b.get('count', 1)
                
                # Check 1: HP bounds
                if role in expected:
                    min_hp, max_hp = expected[role]
                    if hp > max_hp:
                        stage_findings.append(f"Wave {wid}, Batch {b_idx+1} ({role}): HP too high ({hp}), expected max {max_hp}.")
                    elif hp < min_hp:
                        stage_findings.append(f"Wave {wid}, Batch {b_idx+1} ({role}): HP too low ({hp}), expected min {min_hp}.")
                
                # Check 2: Damage abnormalities
                if dmg > 100:
                    stage_findings.append(f"Wave {wid}, Batch {b_idx+1} ({role}): DMG extreme ({dmg}).")
                
                # Check 3: Abnormal Asteroid Counts
                if role == 'ASTEROID' and count > 5:
                    stage_findings.append(f"Wave {wid}, Batch {b_idx+1} ({role}): High spawn count ({count}).")
                    
                # Check 4: Bosses with multiple counts
                if role == 'BOSS' and count > 1:
                    stage_findings.append(f"Wave {wid}, Batch {b_idx+1} ({role}): {count} Bosses spawned simultaneously! (HP={hp}).")
        
        if stage_findings:
            findings.append(f"=== STAGE {stage_num} ===")
            findings.extend(stage_findings)
            
    return findings

if __name__ == "__main__":
    results = analyze_stages()
    with open('report.txt', 'w', encoding='utf-8') as f:
        if results:
            f.write("\n".join(results))
        else:
            f.write("No anomalies found.")
