import json
import matplotlib.pyplot as plt
import statistics

f = open("handshake_error_BA.txt", "r")

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

dict2={}
dict2["flow"]=[]

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
print()
print(dict1['flow'])


for i in range(len(dict0['flow'])):
    list_flow0.append(i)
for i in range(len(dict1['flow'])):
    list_flow1.append(i)

for i in list_flow0:
    elem=dict0['flow'][i]-dict1['flow'][i]
    dict2['flow'].append(elem)

print(dict2['flow'])
mean_a=statistics.mean(dict2['flow'])
st_dev_a = statistics.pstdev(dict2['flow'])

print()
print(mean_a,st_dev_a)
print()


print(len(list_flow0))
print(len(list_flow1))


