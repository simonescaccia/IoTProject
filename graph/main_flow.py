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
#print(dict0['quantity'])
print()
print(dict1['flow'])
#print(dict1['diff'])
#print(dict1['leak'])

for i in range(len(dict0['flow'])):
    list_flow0.append(i)

for i in range(len(dict1['flow'])):
    list_flow1.append(i)

print(len(list_flow0))
print(len(list_flow1))

for elem in dict1['flow']:
    elem = elem - 30*elem/100

# plotting the line 1 points
plt.plot(list_flow0, dict0['flow'], label = "Source")
plt.plot(list_flow1, dict1['flow'], label = "Son")
# naming the x axis
plt.xlabel('Sampling times')
# naming the y axis
plt.ylabel('L/min - water flow rate')
# giving a title to my graph
plt.title('Comparison flow Source and Son')

# show a legend on the plot
plt.legend()

# function to show the plot
plt.show()

f.close()

