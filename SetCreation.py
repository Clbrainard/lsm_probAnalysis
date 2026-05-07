def load_matrix(filepath: str) -> list[list[float]]:
    rows = []
    with open(filepath, "r") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            values = line.split("\t", 1)[-1]
            rows.append([float(v) for v in values.split(",")])
    return rows

def save_matrix(matrix: list[list[float]], filepath: str) -> None:
    with open(filepath, "w") as f:
        for row in matrix:
            f.write(",".join(str(v) for v in row) + "\n")


N = 320
P = 1000

moves = [1, 2, 4, 5, 8, 10, 20, 25, 40, 50, 100, 125, 200, 250, 500, 1000]



outputFolder = "TestSet1"
srcMatrix = "GBM1.csv"

GBMmatrix = load_matrix(srcMatrix)


def saveNewMatrix(GBM, outfile, move):
    outputMatrix = []
    dN = int(N / move)
    for i in range(len(GBM)): 
        row = []
        for k in range(move):
            row.append(GBM[i][(k+1) * dN - 1])
        outputMatrix.append(row)
    save_matrix(outputMatrix,outfile)

    
print(len(GBMmatrix[0]))
for move in moves:
    saveNewMatrix(GBMmatrix, "TestSet1\Matrix" + str(move) + ".csv", move)
    
