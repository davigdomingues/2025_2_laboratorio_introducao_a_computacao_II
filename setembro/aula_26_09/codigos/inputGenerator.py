import random

def fileGenerator(max_middle_value=5.0, method=1):
    lines = []
    colors = [
        "amarelo",
        "azul",
        "branco",
        "rosa",
        "preto",
        "verde",
        "vermelho"
    ]
    files = [
        "best.in",
        "average.in",
        "worst.in"
    ]

    # Generate inputs
    for color in colors:
        current_middle = 0.1
        while current_middle <= max_middle_value:
            for i in range(1, 11):
                lines.append(f"{color} {current_middle:.1f} {11-i}")
            
            current_middle += 0.1
            current_middle = round(current_middle, 1)

    # Write files
    with open(files[0], 'w') as f:
        f.write(f"{len(lines)}\n")
        
        for line in lines:
            f.write(line + "\n")
        
        f.write(f"{method}\n")

    with open(files[1], 'w') as f:
        f.write(f"{len(lines)}\n")
        
        for i in range(len(lines)):
            f.write(random.choice(lines) + "\n")
        
        f.write(f"{method}\n")

    with open(files[2], 'w') as f:
        f.write(f"{len(lines)}\n")
        
        for i in range(len(lines)):
            f.write(lines[-i -1] + "\n")
        
        f.write(f"{method}\n")
    
    print(f"Generated {len(lines)} entries")

if __name__ == "__main__":
    # fileGenerator(0.1,4)
    # fileGenerator(1.0,4) 
    # fileGenerator(10,4)
    fileGenerator(100,4)
