import json

def check_bosses():
    for stage_num in [1, 2, 4, 5, 6]:
        with open(f'data/stage{stage_num}.json') as f:
            data = json.load(f)
        print(f'=== Stage {stage_num} ===')
        for w in data['waves']:
            for b in w['batches']:
                role = b.get('role', '')
                st = b['stats']
                if role == 'BOSS':
                    hp = st['hp']
                    score = st['score']
                    wid = w['wave_id']
                    exp = score * 0.5
                    print(f'  Wave {wid} BOSS: HP={hp}, score={score}, EXP={exp}')
                elif role == 'TANK':
                    hp = st['hp']
                    score = st['score']
                    wid = w['wave_id']
                    count = b['count']
                    total_exp = score * count * 0.5
                    print(f'  Wave {wid} TANK x{count}: HP={hp}, score={score}, total_EXP={total_exp}')

check_bosses()

print()
# Tim maxExp va mult sao cho:
# Stage 1 -> Lv3-4
# Stage 2 (sau boss) -> Lv6-7
# Stage 3 -> Lv7-8
# Stage 4 -> Lv8-9
# Stage 5 -> Lv10
print('=== EXP tuning simulation ===')

# Score boss va enemy giu nguyen, chi thay doi maxExp va mult
# Tinh lai EXP thuc te tung stage
# Stage 1: da co so lieu 1686.5 kill EXP
# Stage 2: W1-4 enemy + W5 boss (score=5000->2500 EXP) + W6-9 + W10 2x boss
# Neu boss score giu nguyen, thu maxExp va mult khac

# Thich hop nhat: dung multiplier cao hon, maxExp cao hon
candidates = [
    (120, 1.2, 12),
    (150, 1.2, 10),
    (120, 1.25, 10),
]

stage_exp = {
    1: 1686.5,
    2: 3000 + 5000*0.5 + 2*5000*0.5,  # enemy + W5boss + W10 2x boss
    3: 800,
    4: 2500 + 5000*0.5,
    5: 3000 + 3000*0.5 + 5000*0.5,   # enemy + midboss (score 3000) + final (score 5000)
}

for (start, mult, meat) in candidates:
    maxE = start
    total_needed = 0
    for i in range(9):
        total_needed += maxE
        maxE *= mult
    print(f'maxExp={start}, x{mult}, meat={meat} (total EXP to Lv10: {round(total_needed,0)}):')
    
    cumul = 0
    for sname in [1,2,3,4,5]:
        cumul += stage_exp[sname] + 15*meat  # 15 meats per stage estimate
        maxE3 = start
        rem3 = cumul
        lv3 = 1
        while rem3 >= maxE3 and lv3 < 10:
            rem3 -= maxE3
            maxE3 *= mult
            lv3 += 1
        print(f'  After Stage {sname}: Lv{lv3} ({round(rem3,1)} EXP into next level)')
    print()
