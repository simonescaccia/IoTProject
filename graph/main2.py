import json
import matplotlib.pyplot as plt

f = open("handshake.txt", "r")

dict0={}
dict1={}
list_flow0=[]
list_flow1=[]

dict0['flow']=[]
dict0['quantity']=[]

dict1['flow']=[]
dict1['quantity']=[]
dict1['diff']=[]
dict1['leak']=[]

for line in f:
    js = json.loads(line)
    if js['id']=='0':
        dict0['flow'].append(float(js['flow source']))
        dict0['quantity'].append(float(js['quantity']))
    else:
        dict1['flow'].append(float(js['flow_son']))
        dict1['diff'].append(float(js['flow_diff']))
        dict1['leak'].append(float(js['quantity leak']))


print(dict0['flow'])
print(dict0['quantity'])
print()
print(dict1['flow'])
print(dict1['diff'])
print(dict1['leak'])

for i in range(len(dict0['quantity'])):
    list_flow0.append(i)

for i in range(len(dict1['leak'])):
    list_flow1.append(i)

# plotting the line 1 points
plt.plot(list_flow0, dict0['quantity'], label = "Total Water")
plt.plot(list_flow1, dict1['leak'], label = "Total Leakage")
# naming the x axis
plt.xlabel('Sampling times')
# naming the y axis
plt.ylabel('L - water quantity')
# giving a title to my graph
plt.title('Comparison quantity')

# show a legend on the plot
plt.legend()

# function to show the plot
plt.show()

f.close()
