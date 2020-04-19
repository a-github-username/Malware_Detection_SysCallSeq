import pickle
import sys

print("connected")
model = pickle.load(open("model.pickle.dat", "rb"))
count_vect = pickle.load(open("countvec.pickle.dat", "rb"))
while True:
    file = open('data.txt', 'r')
    data = [file.read().replace("-1", "")]
    if len(data[0]) > 1:
        print("len:", len(data[0]))
        print(data[0])
    file.close()
    if len(data[0]) != 1 and len(data[0]) != 0:
        print("Classifying System Call Sequence...")
        pred = model.predict(count_vect.transform(data))
        pred = int(float(pred[0]))
        with open('data.txt', 'w') as file:
            file.write(str(pred))
            file.close()
        print("Prediction:", str(pred))
