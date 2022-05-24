from email import header
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv(r"C:\Users\acrda\Projetos\ufrj\SistDist\data.csv", header=None)
data = pd.DataFrame(data)
print(data)

data = data.drop(data.columns[9], axis=1)
data = data.set_axis([ "0", "1", "2", "3", "4", "5", "6", "7", "8"], axis=1)
data = data.set_axis([ "N = 10^7", "N = 10^8", "N = 10^9"], axis=0)
for index, row in data.iterrows():
    plt.plot(row, label = row.name)
    
plt.legend()
plt.title("Gráfico de performance de soma por threads")
plt.xlabel("Número de threads ($2^x$ threads)")
plt.ylabel("Tempo de execução (segundos)")
plt.show()