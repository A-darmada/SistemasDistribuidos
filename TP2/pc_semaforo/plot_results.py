import matplotlib.pyplot as plt

def readFromFile(a):
	 f = open(a, "r")
	 timeTaken = float(f.readline())
	 f.close()
	 return timeTaken

filenames = list()
threadsNumber = [("1", "1"),("1", "2"),("1", "4"),("1", "8"),("2", "1"),("4", "1"),("8", "1")]
memoryPositions = ["1","2","4","16","32"]

for threads in threadsNumber:
	for memory in memoryPositions:
		filenames.append(memory+"_"+"_".join(threads))
    	    
resultado = dict()
for el in filenames:
	resultado[el] = readFromFile("./medias/"+el+".csv")

xLabels = list()
for threads in threadsNumber:
	xLabels.append(",".join(threads))

plotColors = { '1':'r', '2':'g', '4':'b', '16':'k', '32':'y' }

for memory in memoryPositions:
	values = list()
	for threads in threadsNumber:
		values.append(resultado[memory+"_"+"_".join(threads)])
	plt.plot(xLabels, values, plotColors[memory])

plt.xlabel("Np,Nc")
plt.ylabel("Tempo de execução (s)")
plt.legend(plotColors, title="Memória")
plt.savefig("result.png")
