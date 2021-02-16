with open('results/es_plus_2.cars') as file:
    x = set()
    file.readline()
    for line in file.readlines():
        x.add(line)
    print(len(x))
    for i in x:
        print('{',i,'},')