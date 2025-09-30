import os

os.system("rm -rf data/")
os.system("rm -rf data2/")

os.system("unzip -q standard-ocr-dataset.zip")


step = 0
last = 0
def print_progress():
    global step
    global last
    size = 50
    progress = int(step/32833 *100)
    if progress != last:
        
        os.system("clear")

        print('[', end='')
        for i in range(size):
            if i < progress/100 * size:
                print('-', end='')
            else:
                print(' ', end='')
        print(f'] {progress}%')
        last = progress


for directory in ["data", "data2"]:
    for data in ["testing_data", "training_data"]:
        for digit in range(10):
            os.system(f"rm -rf {directory}/{data}/{chr(ord('0') + digit)}")
            step+= 1
            print_progress()

        for letter in range(26):
            for (dirpath, dirnames, filesnames) in os.walk(f"{directory}/{data}/{chr(ord('A')+letter)}"):
                for i in range(len(filesnames)):
                    os.system(f"mv {directory}/{data}/{chr(ord('A') + letter)}/{filesnames[i]} {directory}/{data}/{chr(ord('A') + letter)}/{str(i)}.png")
                    step+=1
                    print_progress()





