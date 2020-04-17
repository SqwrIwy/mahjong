import os
import random
import json

os.system("cp a.cpp p0.cpp")
os.system("cp a.cpp p1.cpp")
os.system("cp a.cpp p2.cpp")
os.system("cp a.cpp p3.cpp")
os.system("g++ p0.cpp -o p0 -O2 -std=c++11")
os.system("g++ p1.cpp -o p1 -O2 -std=c++11")
os.system("g++ p2.cpp -o p2 -O2 -std=c++11")
os.system("g++ p3.cpp -o p3 -O2 -std=c++11")

a = [0]
b = {}
b["0"] = [0]
b["1"] = [0]
b["2"] = [0]
b["3"] = [0]
c = [[], [], [], []]
d = [[], [], [], []]

initdata = {}
initdata["srand"] = random.randint(0, 1000000000)
initdata["quan"] = 0

tmp = []
for i in range(1, 10):
    for j in range(4):
        tmp.append("W" + str(i))
        tmp.append("B" + str(i))
        tmp.append("T" + str(i))
        if i <= 4:
            tmp.append("F" + str(i))
        if i <= 3:
            tmp.append("J" + str(i))
    if i <= 8:
        tmp.append("H" + str(i))
random.shuffle(tmp)

initdata["walltiles"] = " ".join(tmp)

for i in range(1, 10000):
    print("turn " + str(i))
    t = {}
    t["log"] = []
    for j in range(1, i):
        t["log"].append(a[j])
        t["log"].append({"0":b["0"][j], "1":b["1"][j], "2":b["2"][j], "3":b["3"][j]})
    t["initdata"] = initdata
    f = open("tmp1", "w")
    json.dump(t, f)
    f.close()
    os.system("./judge < tmp1 > tmp2")
    f = open("tmp2", "r")
    t = json.load(f)
    f.close()
    a.append(t)

    if t["command"] == "request":
        for j in range(4):
            c[j].append(t["content"][str(j)])
            f = open("tmp1", "w")
            json.dump({"requests" : c[j], "responses" : d[j]}, f)
            f.close()
            os.system("./p{} < tmp1 > tmp2".format(j))
            f = open("tmp2", "r")
            s = json.load(f)
            f.close()
            b[str(j)].append(s)
            d[j].append(s["response"])
    else:
        print("end")
        print(t["content"]["0"])
        print(t["content"]["1"])
        print(t["content"]["2"])
        print(t["content"]["3"])
        quit()
