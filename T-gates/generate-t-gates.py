import copy

# create a 6 x 6 matrix will all elements as 0
matrix = [[0 for i in range(6)] for j in range(6)]

# set the diagonal elements to 1
for i in range(6):
    matrix[i][i] = 1

# generate the T-gates
for p in range(5):
    for q in range(6):
        if p == q:
            continue
        temp = copy.deepcopy(matrix)
        temp[p][p] = 2
        temp[p][q] = -2
        temp[q][p] = 2
        temp[q][q] = 2
        if p > q:
            temp[p][q] = 2
            temp[q][p] = -2
        print("T-gate for qubits ", p+1, " and ", q+1, " is: ")
        for i in range(6):
            print(temp[i])
        file = open("T-gates/T"+str(p+1)+str(q+1)+".txt", "w")
        file.write("[")
        for i in range(6):
            file.write("[")
            for j in range(6):
                file.write(str(temp[i][j]))
                if j != 5:
                    file.write(",")
            file.write("]")
        file.write("]")
        file.close()