import re
t = open('debug/wave/live.vcd', encoding='ascii').read()
i = t.find('$enddefinitions')
hdr, body = t[:i], t[i:]

path, sym_map = [], {}
pat = re.compile(r'\$scope module (\S+) \$end|\$upscope \$end|\$var wire (\d+) (\S+) \$(\S+) \$end')
for m in pat.finditer(hdr):
    if m.group(1):
        path.append(m.group(1))
    elif m.group(0).startswith('$ups'):
        path.pop()
    else:
        sym_map[m.group(3)] = '/'.join(path + [m.group(4)])

print('symbols:')
for s, p in sym_map.items():
    print(' ', s, '->', p)

events, last_ts = {}, None
for line in body.splitlines():
    if line.startswith('#'):
        last_ts = int(line[1:])
    else:
        mm = re.match(r'([01])(\S+)\r?$', line)
        if mm and last_ts is not None:
            events.setdefault(mm.group(2), []).append((last_ts, mm.group(1)))

for s, evs in events.items():
    highs, open_t = [], None
    for ts, v in evs:
        if v == '1' and open_t is None:
            open_t = ts
        elif v == '0' and open_t is not None:
            highs.append(ts - open_t)
            open_t = None
    if open_t is not None:
        highs.append(-1)
    gaps = [evs[k + 1][0] - evs[k][0] for k in range(len(evs) - 1)]
    print(f'{sym_map.get(s, s)}: {len(evs)} edges, {len(highs)} windows')
    print('   window lens[:10]:', highs[:10])
    print('   begin ts:', evs[0][0], ' edge-to-edge gaps[:8]:', gaps[:8])
