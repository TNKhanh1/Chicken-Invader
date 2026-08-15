import json

with open('data/stage4.json', 'r') as f:
    d = json.load(f)

for w in d['waves']:
    for b in w['batches']:
        layout = b['layout']['type']
        mov = b['movement']['type']
        
        if layout == 'RING':
            print(f"Wave {w['wave_id']} Batch {b['batch_id']}: RING has mov {mov}")
