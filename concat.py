with open('output.txt', 'r') as f:
    lines = f.readlines()

new_lines = [lines[i].strip() + ' ' + lines[i+1].strip() for i in range(0, len(lines), 2)]

with open('new_output.txt', 'w') as f:
    for line in new_lines:
        f.write(line + '\n')