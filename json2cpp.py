import json

with open("input.json", "r") as f:
    data = json.load(f)

for key, value in data.items():
    print(f'{{"{key}", {{{", ".join(map(str, value))}}}}},')