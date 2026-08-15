import json

with open('data/stage5.json', 'r') as f:
    d = json.load(f)

for w in d['waves']:
    for b in w['batches']:
        layout = b['layout']['type']
        mov = b['movement']['type']
        
        # Validations
        if layout == 'SWEEP_TO_GRID' and mov != 'WAYPOINT':
            print(f"Wave {w['wave_id']} Batch {b['batch_id']}: SWEEP_TO_GRID needs WAYPOINT but got {mov}")
        if layout == 'INTERSECTING_V' and mov != 'WAYPOINT':
            print(f"Wave {w['wave_id']} Batch {b['batch_id']}: INTERSECTING_V needs WAYPOINT but got {mov}")
        if layout == 'RANDOM_RAIN' and mov != 'WAYPOINT':
            print(f"Wave {w['wave_id']} Batch {b['batch_id']}: RANDOM_RAIN needs WAYPOINT but got {mov}")
