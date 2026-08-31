import json

with open('data/stage1.json') as f:
    s1 = json.load(f)

print('--- Stage 1 Wave 4 ---')
for i, b in enumerate(s1['waves'][3]['batches']):
    print(f"Batch {i+1}: Role={b.get('role', 'NORMAL')}, HP={b['stats']['hp']}, Count={b.get('count', 1)}")

with open('data/stage2.json') as f:
    s2 = json.load(f)

print('\n--- Stage 2 Wave 4 ---')
for i, b in enumerate(s2['waves'][3]['batches']):
    print(f"Batch {i+1}: Role={b.get('role', 'NORMAL')}, HP={b['stats']['hp']}, Count={b.get('count', 1)}")
