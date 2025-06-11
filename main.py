import numpy as np
words = []
with open("allowed_words.txt") as f:
    words = f.read().splitlines()

def lessThan(A,B):
    for i in range(5):
        if ord(A[i]) < ord(B[i]):
            return True
        elif ord(A[i]) > ord(B[i]):
            return False
    print("A,B equal!", A, B)
    return -1
            

def binSearch(word):
    upper = len(words)
    lower = 0
    n = 0
    while n <= 2 * np.log2(len(words)):
        n += 1
        test = (upper + lower) // 2
        if words[test] == word:
            return test
        elif lessThan(words[test],word):
            lower = test + 1
        else:
            upper = test
        if upper == lower:
            return -1
    
def getColours(answer,guess):
    rAnswer = list(answer)
    rGuess = list(guess)
    greens = set()
    yellows = set()
    for i in range(5):
        if rAnswer[i] == rGuess[i]:
            greens.add(i)
            rAnswer[i] = '.'
            rGuess[i] = '.'
    for i in range(5):
        if rGuess[i] == '.':
            continue
        for j in range(5):
            if rGuess[i] == rAnswer[j]:
                rGuess[i] = '.'
                rAnswer[j] = '.'
                yellows.add(i)
    colours = [-1 for i in range(5)]
    for i in range(5):
        if i in greens:
            colours[i] = 2
        elif i in yellows:
            colours[i] = 1
        else:
            colours[i] = 0
    return colours

        

checkWords = []
checkDatas = []
def getInfo():
    global checkWords
    global checkDatas
    checkWords = []
    checkDatas = []
    while True:
        while True:
            word = input("Word: ").lower()
            if word == "q":
                return False
            elif word == 'y':
                return True
            elif len(word) != 5:
                print("Invalid length")
            elif binSearch(word) == -1:
                print("Word not in list")
            else:
                checkWords.append(word)
                break

        while True:
            data = input("Data: ")
            if len(data) != 5:
                print("Data wrong length")
                continue
            for d in data:
                if not int(d) in [0,1,2]:
                    print("Invalid data")
                    break
            else:
                checkDatas.append([int(d) for d in data])
                break


    

def giveList():
    global checkWords
    global checkDatas
    possibilities = set(words)
    newpossibilities = set()
    for i in range(len(checkWords)):
        checkWord = checkWords[i]
        checkData = checkDatas[i]
        for word in possibilities:
            if getColours(word,checkWord) == checkData:
                newpossibilities.add(word)
        possibilities = newpossibilities
        newpossibilities = set()
    for word in possibilities:
        print(word)

if __name__ == "__main__":
    print("0 = Grey, 1 = Yellow, 2 = Green")
    print("q to quit, y to list possible words")
    while getInfo():
        giveList()
