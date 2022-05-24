import subprocess
import sys
import os
from tokenize import Double
import numpy as np
#FNULL = open(os.devnull, 'w')    #use this if you want to suppress output to stdout from the subprocess
m = np.empty([3,10])

for i in range(9):
    k = pow(2,i)
    for j in range(3):
        n = pow(10, j+7)
        for l in range(10):
            args = "C:\\Users\\acrda\\Projetos\\ufrj\\SistDist\\T2\\a.exe " + str(n) + " " + str(k)
            m[j][i] += float(subprocess.check_output(args).decode())/10
            #print(subprocess.check_output(args).decode())

print(m)
np.savetxt("data.csv", m, delimiter=",")