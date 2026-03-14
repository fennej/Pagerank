import matplotlib.pyplot as plt
import numpy as np


"variables pour la premier partie "
alpha=[]
time=[]

i=0

f = open("resultats.txt", "r")
next(f) # skip the header line
next(f) # skip the second line
    
for line in f:

    if(line.strip() == ""):
        next(f) # skip the empty line
        break

    result=line.split(",")
    alpha.append(float(result[0]))
    time.append(float(result[3]))



plt.plot(alpha, time)
plt.xlabel('alpha')
plt.ylabel('time')
plt.title('Plot of alpha vs time')
plt.show()



"variables pour la deuxieme partie"

time=[]
alpha=[]
epsolon=[]


next(f) # skip the second line

for line in f:

    if(line.strip() == ""):
        break

    result=line.split(",")
    alpha.append(float(result[0]))
    epsolon.append(float(result[1]))
    time.append(float(result[3]))


ax=plt.axes(projection='3d')

ax.scatter(alpha, np.log10(epsolon), time)
ax.set_xlabel('alpha')
ax.set_ylabel('log10(epsilon)')
ax.set_zlabel('time')
plt.title('Alpha & Epsilon vs Time')
plt.show()

next(f) # skip the second line



"variable pour la variaption epsilon"
epsolon=[]
time=[]

for line in f:

    if(line.strip() == ""):
        break

    result=line.split(",")
    epsolon.append(float(result[1]))
    time.append(float(result[3]))



plt.plot(np.log10(epsolon), time)
plt.xlabel('log10(epsilon)')
plt.ylabel('time')
plt.title('Plot of log10(epsilon) vs time')
plt.show()



f.close()

