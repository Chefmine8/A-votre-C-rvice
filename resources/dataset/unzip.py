

import os

os.system("rm -rf data/ data2/")
os.system("unzip -q standard-ocr-dataset.zip")

for directory in ["data", "data2"]:
    for data in ["testing_data", "training_data"]:
        for digit in range(10):
            os.system(f"rm -rf {directory}/{data}/{chr(ord('0') + digit)}")
        for letter in range(26):
            print(chr(ord('A')+letter))
            for (dirpath, dirnames, filesnames) in os.walk(f"{directory}/{data}/{chr(ord('A')+letter)}"):
                for i in range(len(filesnames)):
                    os.system(f"mv {directory}/{data}/{chr(ord('A') + letter)}/{filesnames[i]} {directory}/{data}/{chr(ord('A') + letter)}/{str(i)}.png")



